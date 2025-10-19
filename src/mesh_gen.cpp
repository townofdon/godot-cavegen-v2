#include "mesh_gen.h"
#include "easing.hpp"
#include "flood_fill_3d.hpp"

#include "constants.h"
#include "sizing_data.hpp"
#include <chrono>
#include <iostream>
#include <string>

void MeshGen::_bind_methods() {
	ClassDB::bind_method(D_METHOD("generate", "global_cfg", "room_cfg", "noise", "border_noise"), &MeshGen::generate);
}

float *MeshGen::time_processNoise = new float[100]{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

float *MeshGen::time_marchCubes = new float[100]{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static inline float *noiseBuffer = new float[MAX_NOISE_NODES];
static inline bool *floodFillScreen = new bool[MAX_NOISE_NODES];

void MeshGen::generate(GlobalConfig *p_global_cfg, RoomConfig *p_room_cfg, Noise *p_noise, Noise *p_border_noise) {
	ERR_FAIL_NULL_EDMSG(p_global_cfg, "null p_global_cfg");
	ERR_FAIL_NULL_EDMSG(p_room_cfg, "null p_room_cfg");
	ERR_FAIL_NULL_EDMSG(p_noise, "null p_noise");
	ERR_FAIL_NULL_EDMSG(p_border_noise, "null p_border_noise");
	if (p_global_cfg->CellSize <= 0) {
		UtilityFunctions::printerr("cell_size must be greater than zero");
		return;
	}
	if (p_global_cfg->RoomWidth <= 0 || p_global_cfg->RoomHeight <= 0 || p_global_cfg->RoomDepth <= 0) {
		auto x = String(std::to_string(p_global_cfg->RoomWidth).c_str());
		auto y = String(std::to_string(p_global_cfg->RoomHeight).c_str());
		auto z = String(std::to_string(p_global_cfg->RoomDepth).c_str());
		UtilityFunctions::printerr("invalid room dimensions: (" + x + "," + y + "," + z + ")");
		return;
	}

	// setup context
	struct Vector3i numCells;
	GlobalConfig cfg = *p_global_cfg;
	RoomConfig room = *p_room_cfg;
	SizingData sizing = cfg.GetSizingData();
	numCells = sizing.numCells;
	int *tiles = room.GetTiles();
	struct MG::Context::Config ctxCfg = {
		// global
		cfg.RoomWidth,
		cfg.RoomHeight,
		cfg.RoomDepth,
		sizing.cellSize, // actual cell-size might not reflect value from config
		cfg.Ceiling,
		cfg.ActivePlaneOffset,
		// debug
		room.ShowNoise,
		room.ShowBorder,
		room.ShowOuterWalls,
		// noise
		room.Normalize,
		room.IsoValue,
		room.NoiseFloor,
		room.NoiseCeil,
		room.Curve,
		room.Tilt,
		room.FalloffAboveCeiling,
		room.Interpolate,
		room.ActiveYSmoothing,
		room.RemoveOrphans,
		// border
		room.UseBorderNoise,
		room.BorderSize,
		room.BorderNoiseIsoValue,
		room.SmoothBorderNoise,
		room.FalloffNearBorder,
		room.BorderGapSpread,
		// tiles
		room.TileStrength,
		room.TileSmoothing,
		room.TileFloor,
		room.TileFloorFalloff,
		room.TileCeiling,
		room.TileCeilingFalloff,
	};
	struct MG::Context ctx = {
		ctxCfg,
		*p_noise,
		*p_border_noise,
		sizing.numCells,
		tiles,
	};

	//
	// process noise
	//
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		process_noise(ctx, room.GetNoise());
		auto t1 = std::chrono::high_resolution_clock::now();
		// benchmark
		float millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
		auto dmillis = String(std::to_string(millis).c_str());
		auto dnodes = String(std::to_string(numCells.x * numCells.y * numCells.z).c_str());
		float minv = INFINITY;
		float maxv = -INFINITY;
		float total = 0.0f;
		int count = 0;
		for (int i = 99; i >= 0; i--) {
			if (i <= 0) {
				time_processNoise[0] = millis;
			} else {
				time_processNoise[i] = time_processNoise[i - 1];
			}
			if (time_processNoise[i] > 0) {
				total += time_processNoise[i];
				count++;
				if (time_processNoise[i] > maxv) {
					maxv = time_processNoise[i];
				}
				if (time_processNoise[i] < minv) {
					minv = time_processNoise[i];
				}
			}
		}
		if (count > 0) {
			float avg = total / (float)count;
			auto dmin = String(std::to_string(minv).c_str());
			auto dmax = String(std::to_string(maxv).c_str());
			auto davg = String(std::to_string(avg).c_str());
			UtilityFunctions::print("process_noise() took " + dmillis + "ms for " + dnodes + " nodes, avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}

	//
	// march cubes
	//
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		march_cubes(ctx, room.GetNoise());
		auto t1 = std::chrono::high_resolution_clock::now();
		// benchmark
		float millis = (float)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / 1000.0;
		auto dmillis = String(std::to_string(millis).c_str());
		float minv = INFINITY;
		float maxv = -INFINITY;
		float total = 0.0f;
		int count = 0;
		for (int i = 99; i >= 0; i--) {
			if (i <= 0) {
				time_marchCubes[0] = millis;
			} else {
				time_marchCubes[i] = time_marchCubes[i - 1];
			}
			if (time_marchCubes[i] > 0) {
				total += time_marchCubes[i];
				count++;
				if (time_marchCubes[i] > maxv) {
					maxv = time_marchCubes[i];
				}
				if (time_marchCubes[i] < minv) {
					minv = time_marchCubes[i];
				}
			}
		}
		if (count > 0) {
			float avg = total / (float)count;
			auto dmin = String(std::to_string(minv).c_str());
			auto dmax = String(std::to_string(maxv).c_str());
			auto davg = String(std::to_string(avg).c_str());
			// UtilityFunctions::print("march_cubes() took " + dmillis + "ms, avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}
}

//
// process noise
//
#pragma region process_noise
void MeshGen::process_noise(MG::Context ctx, float noiseSamples[]) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	auto noise = ctx.noise;
	float minV = INFINITY;
	float maxV = -INFINITY;
	float cellSize = ctx.cfg.CellSize;

	//
	// base noise
	//
	// - first pass - initialize && sample all noise values in grid
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = MG::NoiseIndex(ctx, x, y, z);
				noiseBuffer[i] = 0.0f;
				noiseSamples[i] = 0.0f;
				if (cfg.ShowNoise) {
					// typically, noise output is in range [-1,1]
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
	// - second pass - normalize noise values, apply mods
	if (cfg.ShowNoise) {
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					// normalize
					int i = MG::NoiseIndex(ctx, x, y, z);
					float val;
					if (cfg.Normalize) {
						val = inverse_lerpf(minV, maxV, noiseSamples[i]);
					} else {
						val = inverse_lerpf(-1, 1, noiseSamples[i]);
					}
					val = clamp01(val);
					val = lerpf(cfg.NoiseFloor, cfg.NoiseCeil, val);
					// apply noise curve
					float valEaseIn = Easing::InCubic(val);
					float valEaseOut = Easing::OutCubic(val);
					val = lerpf(valEaseIn, val, clamp01(cfg.Curve));
					val = lerpf(val, valEaseOut, clamp01(cfg.Curve - 1));
					// apply falloff above ceiling
					float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					zeroValue = lerpf(0.0f, zeroValue, cfg.FalloffAboveCeiling); // flattens noise at ceiling as falloff approaches zero
					val = lerpf(val, zeroValue, Easing::InCubic(MG::GetAboveCeilAmount(ctx, y, cfg.FalloffAboveCeiling)));
					// apply tilt
					float yPct = MG::GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * lerpf(0.0f, 1.0f, yPct);
					float valTiltBottom = val * lerpf(1.0f, 0.0f, yPct);
					val = lerpf(valTiltTop, val, clamp01(cfg.Tilt));
					val = lerpf(val, valTiltBottom, clamp01(cfg.Tilt - 1));
					noiseSamples[i] = val;
				}
			}
		}
	}
	// - third pass - apply bounds, borders
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = MG::NoiseIndex(ctx, x, y, z);
				float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
				float val = noiseSamples[i];
				if (MG::IsAtBoundaryY(ctx, y)) {
					// apply y bounds
					val = zeroValue;
				} else if (MG::IsAtBoundaryXZ(ctx, x, z) && (cfg.ShowOuterWalls || !MG::IsBelowCeiling(ctx, y))) {
					// apply xz bounds
					val = zeroValue;
				} else if (y <= 1) {
					// apply floor
					val = maxf(val, cfg.IsoValue + 0.1f);
				} else if (
					MG::IsAtBorder(ctx, x, y, z) && (MG::IsAtBorderEdge(ctx, x, y, z) || !cfg.UseBorderNoise) &&
					MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_UNSET) {
					// apply border
					if (MG::IsBelowCeiling(ctx, y) && cfg.ShowBorder) {
						val = maxf(val, cfg.IsoValue + 0.1f);
						// un-comment the following to remove noise contribution to border:
						// val = cfg.IsoValue + 0.1f;
						// subtract from border if close to an empty border tile
						float activeY = (float)MG::GetActivePlaneY(ctx) - 6;
						float ceiling = (float)MG::GetCeiling(ctx);
						int dist = MG::GetDistanceToEmptyBorderTile(ctx, x, z, cfg.BorderGapSpread);
						float distPct = Easing::InQuad(clamp01(dist / (float)cfg.BorderGapSpread));
						float yPct = clamp01(inverse_lerpf(lerpf(activeY, ceiling - CMP_EPSILON, distPct), ceiling, y));
						val = lerpf(val, 0, yPct);
					} else {
						val = zeroValue;
					}
				} else if (!MG::IsBelowCeiling(ctx, y) && cfg.FalloffNearBorder > 0) {
					// apply falloff to noise above ceil && near border
					int borderSize = cfg.UseBorderNoise ? 1 : cfg.BorderSize;
					float dist = MG::DistFromBorder(ctx, x, y, z, borderSize);
					float size = minf(ctx.numCells.x, ctx.numCells.z) - cfg.BorderSize * 2;
					float distPct = dist / maxf(1, size * cfg.FalloffNearBorder);
					float t = MG::GetAboveCeilAmount(ctx, y, distPct) * (1 - clamp01(distPct));
					val = lerpf(val, zeroValue, t);
				}
				noiseSamples[i] = val;
			}
		}
	}
	// - fourth pass - adjust areas around action y plane to improve readability
	{
		int activeY = MG::GetActivePlaneY(ctx);
		float activeYSmoothing = cfg.ActiveYSmoothing;
		if (activeY > 1 && activeY + 1 < numCells.y - 1) {
			for (size_t z = 0; z < numCells.z; z++) {
				for (size_t x = 0; x < numCells.x; x++) {
					int y = activeY - 1;
					// clang-format off
					noiseBuffer[MG::NoiseIndex(ctx, x, y, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(1, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(0, 0.33333333f, activeYSmoothing)
					);
					noiseBuffer[MG::NoiseIndex(ctx, x, y + 1, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(1, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(0, 0.33333333f, activeYSmoothing)
					);
					noiseBuffer[MG::NoiseIndex(ctx, x, y + 2, z)] = (
						noiseSamples[MG::NoiseIndex(ctx, x, y + 0, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 1, z)] * lerpf(0, 0.33333333f, activeYSmoothing) +
						noiseSamples[MG::NoiseIndex(ctx, x, y + 2, z)] * lerpf(1, 0.33333333f, activeYSmoothing)
					);
					// clang-format on
				}
			}
			for (size_t z = 0; z < numCells.z; z++) {
				for (size_t y = maxi(activeY - 1, 1); y <= activeY + 1 && y < numCells.y - 1; y++) {
					for (size_t x = 0; x < numCells.x; x++) {
						int i = MG::NoiseIndex(ctx, x, y, z);
						noiseSamples[i] = noiseBuffer[i];
					}
				}
			}
		}
	}

	//
	// apply border noise
	//
	if (cfg.ShowBorder && cfg.UseBorderNoise && cfg.BorderSize > 1) {
		auto ceiling = GetCeiling(ctx);
		// - first pass - sample and normalize border noise on x walls
		//   step 0 => sample noise at x=0
		//   step 1 => normalize at x=0
		//   step 2 => sample noise at x=max
		//   step 3 => normalize at x=max
		for (int step = 0; step < 4; step++) {
			if (step == 0 || step == 2) {
				minV = INFINITY;
				maxV = -INFINITY;
			}
			for (int z = 0; z < numCells.z; z++) {
				for (int y = 1; y < numCells.y && y <= ceiling; y++) {
					int x = 0;
					if (step >= 2) {
						x = numCells.x - 1;
					}
					int i = NoiseIndex(ctx, x, y, z);
					if (step == 0 || step == 2) {
						float val = ctx.borderNoise.get_noise_3d(x, y, z);
						noiseBuffer[i] = val;
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
					} else {
						float val = clamp01(inverse_lerpf(minV, maxV, noiseBuffer[i]));
						if (MG::GetBorderTile(ctx, x, z) == RoomConfig::TILE_STATE_EMPTY) [[unlikely]] {
							val = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
		// - second pass - sample and normalize border noise on z walls
		//   step 0 => sample noise at z=0
		//   step 1 => normalize at z=0
		//   step 2 => sample noise at z=max
		//   step 3 => normalize at z=max
		for (int step = 0; step < 4; step++) {
			if (step == 0 || step == 2) {
				minV = INFINITY;
				maxV = -INFINITY;
			}
			for (int y = 1; y < numCells.y && y <= ceiling; y++) {
				for (int x = 0; x < numCells.x; x++) {
					int z = 0;
					if (step >= 2) {
						z = numCells.z - 1;
					}
					int i = NoiseIndex(ctx, x, y, z);
					if (step == 0 || step == 2) {
						float val = ctx.borderNoise.get_noise_3d(x, y, z);
						noiseBuffer[i] = val;
						if (val < minV) {
							minV = val;
						}
						if (val > maxV) {
							maxV = val;
						}
					} else {
						float val = clamp01(inverse_lerpf(minV, maxV, noiseBuffer[i]));
						if (MG::GetBorderTile(ctx, x, z) == RoomConfig::TILE_STATE_EMPTY) [[unlikely]] {
							val = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
						}
						noiseBuffer[i] = val;
					}
				}
			}
		}
		// - third pass: apply noise to border points
		// side: x=0
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 2; y < numCells.y && y <= ceiling; y++) {
				for (int x = 2; x < cfg.BorderSize + 1; x++) {
					if (MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, 0, y, z)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, 0, y + j, z + k)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (x - 1) / (float)cfg.BorderSize;
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		// side: z=max
		for (int z = numCells.z - 3; z > numCells.z - cfg.BorderSize - 2; z--) {
			for (int y = 2; y < numCells.y && y <= ceiling; y++) {
				for (int x = 1; x < numCells.x - 1; x++) {
					if (MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, x, y, numCells.z - 1)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, x + j, y + k, numCells.z - 1)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (numCells.z - 2 - z) / (float)(cfg.BorderSize - 1);
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		// side: x=max
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 2; y < numCells.y && y <= ceiling; y++) {
				for (int x = numCells.x - 3; x > numCells.x - cfg.BorderSize - 2; x--) {
					if (MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, numCells.x - 1, y, z)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					if (cfg.SmoothBorderNoise > 0) {
						for (int j = -1; j <= 1; j++) {
							for (int k = -1; k <= 1; k++) {
								if (j == 0 && k == 0)
									continue;
								kernel += noiseBuffer[NoiseIndex(ctx, numCells.x - 1, y + j, z + k)] * 0.125f;
							}
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (numCells.x - 2 - x) / (float)(cfg.BorderSize - 1);
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
		// side: z=0
		for (int z = 2; z < cfg.BorderSize + 1; z++) {
			for (int x = 1; x < numCells.x - 1; x++) {
				for (int y = 2; y < numCells.y && y <= ceiling; y++) {
					if (MG::GetBorderTile(ctx, x, z) != RoomConfig::TILE_STATE_FILLED) {
						continue;
					}
					float n0 = noiseBuffer[NoiseIndex(ctx, x, y, 0)];
					// use surrounding cells for avg smoothing
					float kernel = 0.0f;
					for (int j = -1; j <= 1; j++) {
						for (int k = -1; k <= 1; k++) {
							if (j == 0 && k == 0)
								continue;
							kernel += noiseBuffer[NoiseIndex(ctx, x + j, y + k, 0)] * 0.125f;
						}
					}
					float val = n0 * lerpf(1.0f, 0.1f, cfg.SmoothBorderNoise) + kernel * lerpf(0.0f, 0.9f, cfg.SmoothBorderNoise);
					int i = NoiseIndex(ctx, x, y, z);
					float t = (z - 1) / (float)cfg.BorderSize;
					float strength = lerpf(1.0f, cfg.BorderNoiseIsoValue + 0.001f, t);
					noiseSamples[i] = maxf(noiseSamples[i], val * strength);
				}
			}
		}
	}

	//
	// apply tile data
	//
	{
		int activeY = MG::GetActivePlaneY(ctx);
		int ceiling = round(MG::GetCeiling(ctx));
		float tSmooth = cfg.TileSmoothing; // range [0,1]
		float vfloor = cfg.TileFloor; // range [0,1]
		float vfloorFalloff = lerpf(vfloor - CMP_EPSILON, 0, cfg.TileFloorFalloff);
		float vceil = cfg.TileCeiling; // range [0,1]
		float vceilFalloff = lerpf(vceil - CMP_EPSILON, 0, cfg.TileCeilingFalloff);
		// border tiles are already accounted for
		for (int z = 2; z < numCells.z - 2; z++) {
			for (int y = 2; y < numCells.y - 2; y++) {
				for (int x = 2; x < numCells.x - 2; x++) {
					int i = MG::NoiseIndex(ctx, x, y, z);
					auto defaultTile = MG::GetTile(ctx, x, z);
					// calculate tile kernel, where 0 => empty, 1 => unset, 2 => filled
					float kernel = 0.0f;
					for (int j = -1; j <= 1; j++) {
						for (int k = -1; k <= 1; k++) {
							bool isBorderTile = MG::IsBorderTile(ctx, x + j, z + k);
							int tile = isBorderTile ? defaultTile : MG::GetTile(ctx, x + j, z + k, defaultTile);
							float val = 0;
							if (tile == RoomConfig::TILE_STATE_UNSET) {
								val = 1;
							} else if (tile == RoomConfig::TILE_STATE_FILLED) {
								val = 2;
							}
							// 1/9 = 0.1111111111
							kernel += val * lerpf(int(j == 0 && k == 0), 0.1111111111f, tSmooth);
						}
					}
					// calculate y thresholds
					// 0   => 0
					// 0.5 => activeY
					// 1   => ceiling
					float t0 = clamp01(inverse_lerpf(0, activeY, y)) * 0.5f;
					float t1 = clamp01(inverse_lerpf(activeY, ceiling + CMP_EPSILON, y)) * 0.5f;
					float t = clamp01(t0 + t1);
					float t_floor = clamp01(inverse_lerpf(vfloorFalloff, vfloor, t));
					float t_ceil = clamp01(inverse_lerpf(vceil, vceilFalloff, t));
					float targ_unset = noiseSamples[i];
					// calc empty noise target
					float targ_empty = 0.0f;
					targ_empty = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					targ_empty = lerpf(noiseSamples[i], targ_empty, clamp01(cfg.TileStrength));
					targ_empty = lerpf(targ_empty, 0.0f, clamp01(cfg.TileStrength - 1));
					targ_empty = lerpf(noiseSamples[i], targ_empty, t_floor);
					// calc filled noise target
					float targ_filled = 0.0f;
					float zero = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					// as tile strength approaches 2, reduce noise above ceil
					float fallback = lerpf(noiseSamples[i], zero, clamp01(cfg.TileStrength - 1));
					targ_filled = maxf(noiseSamples[i], cfg.IsoValue + 0.1f);
					targ_filled = lerpf(noiseSamples[i], targ_filled, clamp01(cfg.TileStrength));
					targ_filled = lerpf(targ_filled, 1.0f, clamp01(cfg.TileStrength - 1));
					targ_filled = lerpf(fallback, targ_filled, t_ceil);
					// aggregate targets based on kernel results
					float targ;
					targ = lerpf(targ_empty, targ_unset, clamp01(kernel));
					targ = lerpf(targ, targ_filled, clamp01(kernel - 1));
					noiseSamples[i] = targ;
				}
			}
		}
	}

	//
	// remove orphans / islands - flood fill
	//
	if (cfg.RemoveOrphans) {
		// pick likely start point candidate, at the middle of the grid - this will always be ON when a border is present
		Vector3i floodStart = Vector3i((numCells.x - 1) / 2, 1, (numCells.z - 1) / 2);
		bool foundCandidate = MG::IsPointActive(ctx, noiseSamples, floodStart.x, floodStart.y, floodStart.z);
		// initialize flood screen and set floodStart if not yet found
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 1; y < numCells.y - 1; y++) {
				for (int x = 1; x < numCells.x - 1; x++) {
					if (!foundCandidate && MG::IsPointActive(ctx, noiseSamples, x, y, z)) {
						floodStart = Vector3i(x, y, z);
						foundCandidate = true;
					}
					floodFillScreen[MG::NoiseIndex(ctx, x, y, z)] = MG::IsPointActive(ctx, noiseSamples, x, y, z);
				}
			}
		}
		// flood fill from start, setting true nodes to false - remaining true nodes will be zeroed.
		flood_fill_3d(ctx, floodFillScreen, floodStart, true, false);
		for (int z = 1; z < numCells.z - 1; z++) {
			for (int y = 1; y < numCells.y - 1; y++) {
				for (int x = 1; x < numCells.x - 1; x++) {
					int i = MG::NoiseIndex(ctx, x, y, z);
					if (floodFillScreen[i]) {
						float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
						noiseSamples[i] = zeroValue;
					}
				}
			}
		}
	}
}
#pragma endregion // process_noise

//
// march cubes
//
#pragma region march_cubes
void MeshGen::march_cubes(MG::Context ctx, float noiseSamples[]) {
	auto ref = get_mesh();
	auto ptr = *(ref);
	ArrayMesh *meshPtr = Object::cast_to<ArrayMesh>(ptr);
	if (meshPtr == nullptr) {
		UtilityFunctions::push_warning("mesh cannot be null");
		return;
	}
	ArrayMesh mesh = *meshPtr;
	mesh.clear_surfaces();

	auto surface_array = godot::Array();
	surface_array.resize(Mesh::ARRAY_MAX);

	auto verts = PackedVector3Array();
	auto uvs = PackedVector2Array();
	auto normals = PackedVector3Array();
	auto indices = PackedInt32Array(); // vertex indices, in groups of 3 to form triangles

	Vector3 points[3];
	auto numCells = ctx.numCells;
	int vertexIndex = 0;

	for (int z = 0; z < numCells.z - 1; z++) {
		for (int y = 0; y < numCells.y - 1; y++) {
			for (int x = 0; x < numCells.x - 1; x++) {
				int pointIndex = 0;
				int triIdx = MG::GetTriangulation(ctx, noiseSamples, x, y, z);
				auto uv = Vector2(
					(x + 1) / (float)numCells.x,
					maxf(
						(y + 1) / (float)numCells.y,
						(z + 1) / (float)numCells.z));
				// iterate over triangulations
				for (size_t i = 0; i < 15; i++) {
					// get triangulation by row_index + col_index
					auto edgeIdx = TRIANGULATIONS[triIdx * 15 + i];
					if (edgeIdx < 0) {
						break;
					}
					// var (p0, p1) = Constants.EDGES[edgeIdx];
					int p0 = EDGES[edgeIdx * 2 + 0];
					int p1 = EDGES[edgeIdx * 2 + 1];
					// var (x0, y0, z0) = Constants.POINTS[p0];
					int x0 = POINTS[p0 * 3 + 0];
					int y0 = POINTS[p0 * 3 + 1];
					int z0 = POINTS[p0 * 3 + 2];
					// var (x1, y1, z1) = Constants.POINTS[p1];
					int x1 = POINTS[p1 * 3 + 0];
					int y1 = POINTS[p1 * 3 + 1];
					int z1 = POINTS[p1 * 3 + 2];
					auto a = Vector3i(x + x0, y + y0, z + z0);
					auto b = Vector3i(x + x1, y + y1, z + z1);
					auto p = MG::InterpolateMeshPoints(ctx, noiseSamples, a, b);
					p = MG::ClampMeshBorderPoint(ctx, p);
					points[pointIndex] = Vector3(p.x, p.y, p.z);
					pointIndex++;
					if (pointIndex == 3) {
						pointIndex = 0;
						auto p1 = points[0];
						auto p2 = points[1];
						auto p3 = points[2];
						auto normal = -(p2 - p1).cross(p3 - p1).normalized();
						for (size_t i = 0; i < 3; i++) {
							auto point = points[i];
							auto x = point.x * ctx.cfg.CellSize;
							auto y = point.y * ctx.cfg.CellSize;
							auto z = point.z * ctx.cfg.CellSize;
							auto vert = get_global_position() + Vector3(x, y, z);
							verts.append(vert);
							uvs.append(uv);
							normals.append(normal);
							indices.append(vertexIndex);
							vertexIndex++;
						}
					}
				}
			}
		}
	}

	surface_array[Mesh::ARRAY_VERTEX] = verts;
	surface_array[Mesh::ARRAY_TEX_UV] = uvs;
	surface_array[Mesh::ARRAY_NORMAL] = normals;
	surface_array[Mesh::ARRAY_INDEX] = indices;
	mesh.add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
}
#pragma endregion // march_cubes
