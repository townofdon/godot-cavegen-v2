#include "mesh_gen.h"
#include "easing.h"
#include "godot_cpp/core/math.hpp"

#include <chrono>
#include <iostream>

void MeshGen::_bind_methods() {
	ClassDB::bind_method(D_METHOD("generate", "global_cfg", "room_cfg", "noise", "border_noise"), &MeshGen::generate);
}

void MeshGen::generate(GlobalConfig *p_global_cfg, RoomConfig *p_room_cfg, Noise *p_noise, Noise *p_border_noise) {
	struct MeshGen::Context ctx = build_context(p_global_cfg, p_room_cfg, p_noise, p_border_noise);
	if (ctx.cfg.CellSize <= 0) {
		UtilityFunctions::printerr("cell_size must be greater than zero");
		return;
	}

	// initialize noiseSamples
	Vector3i numCells = ctx.numCells;
	float noiseSamples[numCells.x * numCells.y * numCells.z];

	auto t0 = std::chrono::high_resolution_clock::now();
	process_noise(ctx, noiseSamples);
	auto t1 = std::chrono::high_resolution_clock::now();
	float t0_millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
	UtilityFunctions::print("process_noise() took " + String(std::to_string(t0_millis).c_str()) + "ms");

	// TODO: REMOVE
	UtilityFunctions::print(ctx.cfg.Ceiling);
	UtilityFunctions::print(ctx.cfg.BorderSize);
	UtilityFunctions::print(noiseSamples[0]);
	UtilityFunctions::print(noiseSamples[1]);
	auto count = sizeof(noiseSamples) / sizeof(float);
	UtilityFunctions::print("count=" + String(std::to_string(count).c_str()));
}

MeshGen::Context MeshGen::build_context(GlobalConfig *p_global_cfg, RoomConfig *p_room_cfg, Noise *p_noise, Noise *p_border_noise) {
	GlobalConfig cfg = *p_global_cfg;
	RoomConfig room = *p_room_cfg;
	Vector3i numCells;
	numCells.x = (int)floor(cfg.RoomWidth / cfg.CellSize);
	numCells.y = (int)floor(cfg.RoomHeight / cfg.CellSize);
	numCells.z = (int)floor(cfg.RoomDepth / cfg.CellSize);
	struct MeshGen::Context::Config ctxCfg = {
		// global
		cfg.RoomWidth,
		cfg.RoomHeight,
		cfg.RoomDepth,
		cfg.CellSize,
		cfg.Ceiling,
		cfg.ActivePlaneOffset,
		// debug
		room.ShowNoise,
		room.ShowBorder,
		room.ShowOuterWalls,
		// noise
		room.IsoValue,
		room.Curve,
		room.Tilt,
		room.FalloffAboveCeiling,
		room.Interpolate,
		room.RemoveOrphans,
		// border
		room.UseBorderNoise,
		room.BorderSize,
		room.SmoothBorderNoise,
		room.FalloffNearBorder,
	};
	struct MeshGen::Context ctx = {
		ctxCfg,
		*p_noise,
		*p_border_noise,
		numCells,
	};
	return ctx;
}

void MeshGen::process_noise(MeshGen::Context ctx, float noiseSamples[]) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	auto noise = ctx.noise;
	auto ShowNoise = cfg.ShowNoise;
	float noiseBuffer[numCells.x * numCells.y * numCells.z];
	float minV = INFINITY;
	float maxV = -INFINITY;
	float cellSize = ctx.cfg.CellSize;
	// first pass - initialize && sample all noise values in grid
	for (size_t x = 0; x < numCells.x; x++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t z = 0; z < numCells.z; z++) {
				int i = NoiseIndex(ctx, x, y, z);
				noiseBuffer[i] = 0.0;
				noiseSamples[i] = 0.0;
				if (ShowNoise) {
					float val = noise.get_noise_3d(x * cellSize, y * cellSize, z * cellSize);
					noiseSamples[i] = val;
					if (val < minV) {
						minV = val;
					}
					if (val > maxV) {
						maxV = val;
					}
				}
			}
		}
	}
	// second pass - normalize noise values, apply mods
	if (ShowNoise) {
		for (size_t x = 0; x < numCells.x; x++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t z = 0; z < numCells.z; z++) {
					// normalize
					int i = NoiseIndex(ctx, x, y, z);
					float val;
					val = Math::inverse_lerp(minV, maxV, noiseSamples[i]);
					val = Math::clamp(val, 0.0f, 1.0f);
					// apply noise curve
					float valEaseIn = Easing::InCubic(val);
					float valEaseOut = Easing::OutCubic(val);
					val = Math::lerp(valEaseIn, val, Math::clamp(cfg.Curve, 0.0f, 1.0f));
					val = Math::lerp(val, valEaseOut, Math::clamp(cfg.Curve - 1, 0.0f, 1.0f));
					// apply falloff above ceiling
					float zeroValue = Math::min(noiseSamples[i], cfg.IsoValue - 0.1f);
					zeroValue = Math::lerp(0.0f, zeroValue, cfg.FalloffAboveCeiling);
					val = Math::lerp(val, zeroValue, GetAboveCeilAmount(ctx, y));
					// apply tilt
					float yPct = GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * Math::lerp(0.0f, 1.0f, yPct);
					float valTiltBottom = val * Math::lerp(1.0f, 0.0f, yPct);
					val = Math::lerp(valTiltTop, val, Math::clamp(cfg.Tilt, 0.0f, 1.0f));
					val = Math::lerp(val, valTiltBottom, Math::clamp(cfg.Tilt - 1, 0.0f, 1.0f));
					noiseSamples[i] = val;
				}
			}
		}
	}
}

int MeshGen::NoiseIndex(MeshGen::Context ctx, int x, int y, int z) {
	auto numCells = ctx.numCells;
	int i = x + y * numCells.x + z * numCells.y * numCells.z;
	ERR_FAIL_INDEX_V_MSG(i, numCells.x * numCells.y * numCells.z, 0, "noise index out of bounds");
	return i;
}

float MeshGen::GetAboveCeilAmount(MeshGen::Context ctx, int y) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	if (cfg.Ceiling >= 1) {
		return 0.0;
	}
	float ceiling = GetCeiling(ctx);
	float maxY = numCells.y - 1 - cfg.BorderSize * 2;
	if (Math::is_zero_approx(Math::abs(ceiling - maxY))) {
		return 0.0;
	}
	maxY = Math::lerp(ceiling, maxY, cfg.FalloffAboveCeiling);
	if (y < ceiling) {
		return 0.0;
	}
	if (y >= maxY) {
		return 1.0;
	}
	if (ceiling >= maxY || Math::is_zero_approx(Math::abs(ceiling - maxY))) {
		return 1.0;
	}
	return Math::clamp(Math::inverse_lerp(ceiling, maxY, y), 0.0f, 1.0f);
}

float MeshGen::GetCeiling(MeshGen::Context ctx) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	return Math::max(1.0f, Math::min(cfg.Ceiling * (numCells.y - 1), (float)numCells.y - 2));
}

float MeshGen::GetFloorToCeilAmount(MeshGen::Context ctx, int y) {
	float ceiling = GetCeiling(ctx);
	return (float)y / ceiling;
}
