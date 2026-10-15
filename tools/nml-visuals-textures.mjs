#!/usr/bin/env node
// Generates the NML_Visuals textures (ADR 0007), deterministic:
//   NML_Visuals_Grime_UI.png       lens grime overlay: soft smudges towards the edges, near-black brown (never lifts
//                                  deep shadows), alpha = dirt
//   NML_Visuals_GrimeHeavy_UI.png  EXTREME grime on top: denser noise-broken smudges, streaks, wipes, dust specks
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
		return [26, 23, 19, Math.round(Math.min(0.45, alpha[y * W + x] * edge) * 255)];
	});
}

//! Smooth multi-octave value noise in [0, 1] (seeded lattice, smoothstep interpolation): breaks dirt into
//! irregular, non-repeating patches.
function valueNoise(seed, cell, octaves) {
	const rand = rng(seed), size = 64, lattice = Array.from({ length: size * size }, rand);
	const at = (ix, iy) => lattice[(((iy % size) + size) % size) * size + (((ix % size) + size) % size)];
	const smooth = (t) => t * t * (3 - 2 * t);
	return (x, y) => {
		let sum = 0, amp = 1, norm = 0, c = cell;
		for (let o = 0; o < octaves; o++) {
			const fx = x / c, fy = y / c, ix = Math.floor(fx), iy = Math.floor(fy), tx = smooth(fx - ix), ty = smooth(fy - iy);
			const top = at(ix, iy) + (at(ix + 1, iy) - at(ix, iy)) * tx;
			const bottom = at(ix, iy + 1) + (at(ix + 1, iy + 1) - at(ix, iy + 1)) * tx;
			sum += (top + (bottom - top) * ty) * amp;
			norm += amp; amp *= 0.5; c /= 2;
		}
		return sum / norm;
	};
}

//! EXTREME grime (drawn on top of the base grime): denser, noise-broken edge smudges, streaks/drips and wipe
//! marks, dust specks; the centre stays clear so the view remains playable.
export function buildGrimeHeavy() {
	const rand = rng(SEED + 1);
	const alpha = new Float32Array(W * H);
	const noise = valueNoise(SEED + 2, 120, 4);
	const splat = (bx, by, rx, ry, rot, a, falloff) => {
		const r = Math.max(rx, ry) * 2.2, c = Math.cos(rot), s = Math.sin(rot);
		for (let y = Math.max(0, Math.floor(by - r)); y < Math.min(H, by + r); y++) {
			for (let x = Math.max(0, Math.floor(bx - r)); x < Math.min(W, bx + r); x++) {
				const dx = x - bx, dy = y - by;
				const u = (dx * c + dy * s) / rx, v = (-dx * s + dy * c) / ry;
				const d2 = u * u + v * v;
				if (d2 < 4.8) alpha[y * W + x] += a * Math.exp(-d2 * falloff) * (u > 0 ? Math.max(0, 1 - u / 2.2) : 1);
			}
		}
	};
	const edgePoint = (spread) => {
		const ang = rand() * Math.PI * 2, rad = Math.pow(rand(), spread);
		return [W / 2 + Math.cos(ang) * rad * W * 0.64, H / 2 + Math.sin(ang) * rad * H * 0.64];
	};
	// Smudges: many soft blobs of varied size, biased hard towards the edges.
	for (let i = 0; i < 160; i++) {
		const [x, y] = edgePoint(0.25), rx = 10 + rand() * 120;
		splat(x, y, rx, rx * (0.4 + 0.6 * rand()), rand() * Math.PI, 0.08 + rand() * 0.22, 1.6);
	}
	// Streaks: long faint drips (near vertical) and wipe marks (diagonal), fading along their length.
	for (let i = 0; i < 30; i++) {
		const [x, y] = edgePoint(0.2), drip = rand() < 0.6;
		const rot = drip ? Math.PI / 2 + (rand() - 0.5) * 0.5 : (rand() < 0.5 ? 1 : -1) * (0.5 + rand() * 0.5);
		splat(x, y, 60 + rand() * 180, 3 + rand() * 9, rot, 0.06 + rand() * 0.14, 1.2);
	}
	// Dust specks: small, sharper, sparse.
	for (let i = 0; i < 260; i++) {
		const [x, y] = edgePoint(0.3), rx = 1 + rand() * 3;
		splat(x, y, rx, rx, 0, 0.15 + rand() * 0.3, 2.5);
	}
	return rgbaPng(W, H, (x, y) => {
		const ex = (x - W / 2) / (W / 2), ey = (y - H / 2) / (H / 2);
		const edge = Math.min(1, Math.max(0, (Math.sqrt(ex * ex + ey * ey) - 0.32) / 0.5));
		const breakup = 0.35 + 1.0 * noise(x, y); // irregular density, never a uniform film
		const a = Math.min(0.6, alpha[y * W + x] * edge * breakup);
		const tone = Math.round(6 * (noise(x + 500, y + 300) - 0.5)); // slight colour variation in the dirt
		return [20 + tone, 18 + tone, 14 + tone, Math.round(a * 255)];
	});
}

function main() {
	fs.mkdirSync(outDir, { recursive: true });
	for (const [name, build] of [["NML_Visuals_Grime_UI.png", buildGrime], ["NML_Visuals_GrimeHeavy_UI.png", buildGrimeHeavy]]) {
		const out = path.join(outDir, name), png = build();
		// Unchanged files are skipped: committed textures are LFS-lockable and read-only until locked.
		if (fs.existsSync(out) && fs.readFileSync(out).equals(png)) {
			console.log(`unchanged ${path.relative(repo, out)}`);
			continue;
		}

		fs.writeFileSync(out, png);
		console.log(`wrote ${path.relative(repo, out)}`);
	}
}

if (process.argv[1] && path.resolve(process.argv[1]) === fileURLToPath(import.meta.url)) main();
