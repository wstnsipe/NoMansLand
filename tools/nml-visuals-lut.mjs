#!/usr/bin/env node
// Generates NML_Visuals colour-grading LUTs (ADR 0007) from readable JSON specs.
//
// Output: 256x16 RGB PNG strip, 16 slices of 16x16 (x = r + 16*b, y = g), named *_LUT.png so Workbench
// imports it with the engine's TextureLut profile. Register the PNG in Workbench once; it writes the .meta.
// Workbench must be running when a PNG is (re)written: it rebuilds the runtime .edds on file change.
//
// The engine's ColorGrading effect looks the LUT up with LINEAR scene values and treats the result as linear
// (verified on game 1.8.0.13: a display-space LUT lifted blacks ~5x). Specs are written in display (sRGB)
// terms; buildLut converts linear lattice -> sRGB -> spec transform -> linear.
//
// Usage: node tools/nml-visuals-lut.mjs <spec.json> <out_LUT.png>
//        node tools/nml-visuals-lut.mjs --all   (every spec in tools/nml-visuals/luts -> NML_Visuals textures)
//
// Spec (all optional; omitted = identity):
//   curve:        [[in, out], ...] tone curve points in 0..1, applied per channel (monotone cubic)
//   whiteBalance: [r, g, b] channel multipliers applied after the curve
//   saturation:   global saturation multiplier
//   hueBands:     [{ center, width, hueShift, saturation }] hue in degrees; smooth falloff over width
//   clip:         output at or above this value becomes 1 (hard highlight clip), e.g. 0.97

import fs from "node:fs";
import path from "node:path";
import zlib from "node:zlib";
import { fileURLToPath } from "node:url";

const N = 16;
const repo = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");

function monotoneCurve(points) {
	if (!points || points.length < 2) return (x) => x;
	const p = [...points].sort((a, b) => a[0] - b[0]);
	const n = p.length, d = [], m = [];
	for (let i = 0; i < n - 1; i++) d.push((p[i + 1][1] - p[i][1]) / (p[i + 1][0] - p[i][0]));
	m.push(d[0]);
	for (let i = 1; i < n - 1; i++) m.push(d[i - 1] * d[i] <= 0 ? 0 : (d[i - 1] + d[i]) / 2);
	m.push(d[n - 2]);
	return (x) => {
		if (x <= p[0][0]) return p[0][1];
		if (x >= p[n - 1][0]) return p[n - 1][1];
		let i = 0;
		while (x > p[i + 1][0]) i++;
		const h = p[i + 1][0] - p[i][0], t = (x - p[i][0]) / h;
		const t2 = t * t, t3 = t2 * t;
		return (2 * t3 - 3 * t2 + 1) * p[i][1] + (t3 - 2 * t2 + t) * h * m[i] + (-2 * t3 + 3 * t2) * p[i + 1][1] + (t3 - t2) * h * m[i + 1];
	};
}

function rgbToHsl(r, g, b) {
	const mx = Math.max(r, g, b), mn = Math.min(r, g, b), l = (mx + mn) / 2;
	if (mx === mn) return [0, 0, l];
	const d = mx - mn, s = l > 0.5 ? d / (2 - mx - mn) : d / (mx + mn);
	const h = mx === r ? (g - b) / d + (g < b ? 6 : 0) : mx === g ? (b - r) / d + 2 : (r - g) / d + 4;
	return [h * 60, s, l];
}

function hslToRgb(h, s, l) {
	if (s === 0) return [l, l, l];
	const q = l < 0.5 ? l * (1 + s) : l + s - l * s, p = 2 * l - q;
	const f = (t) => { t = ((t % 1) + 1) % 1; return t < 1 / 6 ? p + (q - p) * 6 * t : t < 0.5 ? q : t < 2 / 3 ? p + (q - p) * (2 / 3 - t) * 6 : p; };
	return [f(h / 360 + 1 / 3), f(h / 360), f(h / 360 - 1 / 3)];
}

function hueWeight(h, center, width) {
	let dist = Math.abs(h - center) % 360;
	if (dist > 180) dist = 360 - dist;
	if (dist >= width) return 0;
	const t = dist / width;
	return 1 - t * t * (3 - 2 * t);
}

export function transform(spec, rgb) {
	const curve = monotoneCurve(spec.curve);
	let [r, g, b] = rgb.map(curve);
	const wb = spec.whiteBalance || [1, 1, 1];
	r *= wb[0]; g *= wb[1]; b *= wb[2];
	let [h, s, l] = rgbToHsl(Math.min(r, 1), Math.min(g, 1), Math.min(b, 1));
	s *= spec.saturation ?? 1;
	for (const band of spec.hueBands || []) {
		const w = hueWeight(h, band.center, band.width) * Math.min(1, s * 4); // near-grey pixels are barely shifted
		h += (band.hueShift || 0) * w;
		s *= 1 + ((band.saturation ?? 1) - 1) * w;
	}
	[r, g, b] = hslToRgb(((h % 360) + 360) % 360, Math.max(0, Math.min(1, s)), l);
	const clip = spec.clip ?? 1;
	const c = (v) => Math.max(0, Math.min(1, v >= clip ? 1 : v));
	return [c(r), c(g), c(b)];
}

function crc32(buf) {
	let c = 0xffffffff;
	for (const byte of buf) { c ^= byte; for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1; }
	return (c ^ 0xffffffff) >>> 0;
}

function pngChunk(type, data) {
	const len = Buffer.alloc(4); len.writeUInt32BE(data.length);
	const body = Buffer.concat([Buffer.from(type), data]);
	const crc = Buffer.alloc(4); crc.writeUInt32BE(crc32(body));
	return Buffer.concat([len, body, crc]);
}

const toSrgb = (v) => (v <= 0.0031308 ? v * 12.92 : 1.055 * Math.pow(v, 1 / 2.4) - 0.055);
const toLinear = (v) => (v <= 0.04045 ? v / 12.92 : Math.pow((v + 0.055) / 1.055, 2.4));

export function buildLut(spec) {
	const W = N * N, H = N, stride = W * 3 + 1;
	const raw = Buffer.alloc(stride * H);
	for (let y = 0; y < H; y++) {
		for (let x = 0; x < W; x++) {
			const linearIn = [(x % N) / (N - 1), y / (N - 1), Math.floor(x / N) / (N - 1)];
			const out = transform(spec, linearIn.map(toSrgb)).map(toLinear);
			const o = y * stride + 1 + x * 3;
			raw[o] = Math.round(out[0] * 255); raw[o + 1] = Math.round(out[1] * 255); raw[o + 2] = Math.round(out[2] * 255);
		}
	}
	const ihdr = Buffer.alloc(13);
	ihdr.writeUInt32BE(W, 0); ihdr.writeUInt32BE(H, 4); ihdr[8] = 8; ihdr[9] = 2;
	return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), pngChunk("IHDR", ihdr), pngChunk("IDAT", zlib.deflateSync(raw)), pngChunk("IEND", Buffer.alloc(0))]);
}

function main(argv) {
	if (argv[0] === "--all") {
		const specDir = path.join(repo, "tools", "nml-visuals", "luts");
		const outDir = path.join(repo, "addons", "NML_Visuals", "Textures", "NML", "Visuals");
		fs.mkdirSync(outDir, { recursive: true });
		for (const f of fs.readdirSync(specDir).filter((n) => n.endsWith(".json")).sort()) {
			const out = path.join(outDir, `NML_Visuals_${path.basename(f, ".json")}_LUT.png`);
			fs.writeFileSync(out, buildLut(JSON.parse(fs.readFileSync(path.join(specDir, f), "utf8"))));
			console.log(`wrote ${path.relative(repo, out)}`);
		}
		return 0;
	}
	if (argv.length !== 2) {
		console.error("Usage: node tools/nml-visuals-lut.mjs <spec.json> <out_LUT.png> | --all");
		return 1;
	}
	fs.writeFileSync(argv[1], buildLut(JSON.parse(fs.readFileSync(argv[0], "utf8"))));
	console.log(`wrote ${argv[1]}`);
	return 0;
}

if (process.argv[1] && path.resolve(process.argv[1]) === fileURLToPath(import.meta.url)) process.exit(main(process.argv.slice(2)));
