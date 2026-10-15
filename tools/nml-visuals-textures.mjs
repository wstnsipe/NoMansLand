#!/usr/bin/env node
// Generates the NML_Visuals textures (ADR 0007), deterministic:
//   NML_Visuals_Grime_UI.png     lens grime overlay: soft smudges towards the edges, dark brown-grey, alpha = dirt
// Workbench must be running when the PNGs are (re)written: it rebuilds the runtime .edds on file change.
//
// Usage: node tools/nml-visuals-textures.mjs

import fs from "node:fs";
import path from "node:path";
import zlib from "node:zlib";
import { fileURLToPath } from "node:url";

const repo = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
const W = 1024, H = 576, SEED = 7331;
const outDir = path.join(repo, "addons", "NML_Visuals", "UI", "Textures", "NML", "Visuals");

function rng(seed) {
	let s = seed >>> 0;
	return () => { s = (s * 1664525 + 1013904223) >>> 0; return s / 4294967296; };
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

function rgbaPng(w, h, pixel) {
	const stride = w * 4 + 1, raw = Buffer.alloc(stride * h);
	for (let y = 0; y < h; y++) {
		for (let x = 0; x < w; x++) {
			const o = y * stride + 1 + x * 4, p = pixel(x, y);
			raw[o] = p[0]; raw[o + 1] = p[1]; raw[o + 2] = p[2]; raw[o + 3] = p[3];
		}
	}
	const ihdr = Buffer.alloc(13);
	ihdr.writeUInt32BE(w, 0); ihdr.writeUInt32BE(h, 4); ihdr[8] = 8; ihdr[9] = 6;
	return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), pngChunk("IHDR", ihdr), pngChunk("IDAT", zlib.deflateSync(raw)), pngChunk("IEND", Buffer.alloc(0))]);
}

export function buildGrime() {
	const rand = rng(SEED);
	const alpha = new Float32Array(W * H);
	const blobs = [];
	for (let i = 0; i < 70; i++) {
		// Bias positions towards the frame edges: distance from centre pushed outwards.
		const ang = rand() * Math.PI * 2, rad = Math.pow(rand(), 0.35);
		blobs.push({
			x: W / 2 + Math.cos(ang) * rad * W * 0.62,
			y: H / 2 + Math.sin(ang) * rad * H * 0.62,
			rx: 14 + rand() * 80, ry: 0, rot: rand() * Math.PI,
			a: 0.10 + rand() * 0.28,
		});
	}
	for (const b of blobs) {
		b.ry = b.rx * (0.45 + 0.55 * rand()); // limit stretch: no needle streaks
		const r = Math.max(b.rx, b.ry) * 2.2, c = Math.cos(b.rot), s = Math.sin(b.rot);
		for (let y = Math.max(0, Math.floor(b.y - r)); y < Math.min(H, b.y + r); y++) {
			for (let x = Math.max(0, Math.floor(b.x - r)); x < Math.min(W, b.x + r); x++) {
				const dx = x - b.x, dy = y - b.y;
				const u = (dx * c + dy * s) / b.rx, v = (-dx * s + dy * c) / b.ry;
				const d2 = u * u + v * v;
				if (d2 < 4.8) alpha[y * W + x] += b.a * Math.exp(-d2 * 1.6);
			}
		}
	}
	return rgbaPng(W, H, (x, y) => {
		// Fade dirt towards the centre so the middle of the frame stays clean.
		const ex = (x - W / 2) / (W / 2), ey = (y - H / 2) / (H / 2);
		const edge = Math.min(1, Math.max(0, (Math.sqrt(ex * ex + ey * ey) - 0.35) / 0.55));
		return [58, 53, 44, Math.round(Math.min(0.45, alpha[y * W + x] * edge) * 255)];
	});
}

fs.mkdirSync(outDir, { recursive: true });
for (const [name, build] of [["NML_Visuals_Grime_UI.png", buildGrime]]) {
	const out = path.join(outDir, name), png = build();
	// Unchanged files are skipped: committed textures are LFS-lockable and read-only until locked.
	if (fs.existsSync(out) && fs.readFileSync(out).equals(png)) {
		console.log(`unchanged ${path.relative(repo, out)}`);
		continue;
	}

	fs.writeFileSync(out, png);
	console.log(`wrote ${path.relative(repo, out)}`);
}
