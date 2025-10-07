#include "mesh_gen.h"
#include "easing.hpp"

#include "constants.h"
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

void MeshGen::generate(GlobalConfig *p_global_cfg, RoomConfig *p_room_cfg, Noise *p_noise, Noise *p_border_noise) {
	if (p_global_cfg == nullptr) {
		UtilityFunctions::printerr("global_cfg cannot be null");
		return;
	}
	if (p_room_cfg == nullptr) {
		UtilityFunctions::printerr("room_cfg cannot be null");
		return;
	}
	if (p_noise == nullptr) {
		UtilityFunctions::printerr("noise cannot be null");
		return;
	}
	if (p_border_noise == nullptr) {
		UtilityFunctions::printerr("border_noise cannot be null");
		return;
	}
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

	GlobalConfig cfg = *p_global_cfg;
	RoomConfig room = *p_room_cfg;
	struct Vector3i numCells;
	float cellSize = cfg.CellSize - 0.5f;
	do {
		cellSize = cellSize + 0.5f;
		numCells.x = (int)floor(cfg.RoomWidth / cellSize);
		numCells.y = (int)floor(cfg.RoomHeight / cellSize);
		numCells.z = (int)floor(cfg.RoomDepth / cellSize);
	} while (numCells.x * numCells.y * numCells.z > MAX_NOISE_NODES);

	// setup context
	struct MG::Context::Config ctxCfg = {
		// global
		cfg.RoomWidth,
		cfg.RoomHeight,
		cfg.RoomDepth,
		cellSize, // actual cell-size might not reflect value from config
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
	struct MG::Context ctx = {
		ctxCfg,
		*p_noise,
		*p_border_noise,
		numCells,
	};

	// initialize noiseSamples
	// float noiseSamples[numCells.x * numCells.y * numCells.z];
	float *noiseSamples = new float[numCells.x * numCells.y * numCells.z];

	//
	// process noise
	//
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		process_noise(ctx, noiseSamples);
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
		march_cubes(ctx, noiseSamples);
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
			UtilityFunctions::print("march_cubes() took " + dmillis + "ms, avg=" + davg + ",min=" + dmin + ",max=" + dmax);
		}
	}

	delete[] noiseSamples;
}

// note - this mutates noiseSamples
void MeshGen::process_noise(MG::Context ctx, float noiseSamples[]) {
	auto cfg = ctx.cfg;
	auto numCells = ctx.numCells;
	auto noise = ctx.noise;
	float *noiseBuffer = new float[numCells.x * numCells.y * numCells.z];
	float minV = INFINITY;
	float maxV = -INFINITY;
	float cellSize = ctx.cfg.CellSize;
	// first pass - initialize && sample all noise values in grid
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = MG::NoiseIndex(ctx, x, y, z);
				noiseBuffer[i] = 0.0f;
				noiseSamples[i] = 0.0f;
				if (cfg.ShowNoise) {
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
	if (cfg.ShowNoise) {
		for (size_t z = 0; z < numCells.z; z++) {
			for (size_t y = 0; y < numCells.y; y++) {
				for (size_t x = 0; x < numCells.x; x++) {
					// normalize
					int i = MG::NoiseIndex(ctx, x, y, z);
					float val;
					val = inverse_lerp(minV, maxV, noiseSamples[i]);
					val = clamp(val, 0.0f, 1.0f);
					// apply noise curve
					float valEaseIn = Easing::InCubic(val);
					float valEaseOut = Easing::OutCubic(val);
					val = lerp(valEaseIn, val, clamp(cfg.Curve, 0.0f, 1.0f));
					val = lerp(val, valEaseOut, clamp(cfg.Curve - 1, 0.0f, 1.0f));
					// apply falloff above ceiling
					float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					zeroValue = lerp(0.0f, zeroValue, cfg.FalloffAboveCeiling);
					val = lerp(val, zeroValue, MG::GetAboveCeilAmount(ctx, y));
					// apply tilt
					float yPct = MG::GetFloorToCeilAmount(ctx, y);
					float valTiltTop = val * lerp(0.0f, 1.0f, yPct);
					float valTiltBottom = val * lerp(1.0f, 0.0f, yPct);
					val = lerp(valTiltTop, val, clamp(cfg.Tilt, 0.0f, 1.0f));
					val = lerp(val, valTiltBottom, clamp(cfg.Tilt - 1, 0.0f, 1.0f));
					noiseSamples[i] = val;
				}
			}
		}
	}
	// third pass - apply bounds, borders
	for (size_t z = 0; z < numCells.z; z++) {
		for (size_t y = 0; y < numCells.y; y++) {
			for (size_t x = 0; x < numCells.x; x++) {
				int i = MG::NoiseIndex(ctx, x, y, z);
				// apply bounds
				if (MG::IsAtBoundaryXZ(ctx, x, z) && cfg.ShowOuterWalls || MG::IsAtBoundaryY(ctx, y)) {
					noiseSamples[i] = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					continue;
				}
				// apply border
				if (MG::IsAtBorder(ctx, x, y, z) && (!cfg.UseBorderNoise || MG::IsAtBorderEdge(ctx, x, y, z))) {
					noiseSamples[i] = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					if (MG::IsBelowCeiling(ctx, y) && cfg.ShowBorder) {
						noiseSamples[i] = maxf(noiseSamples[i], cfg.IsoValue + 0.1f);
					}
					continue;
				}
				// apply falloff to noise above ceil && close to border
				if (!MG::IsBelowCeiling(ctx, y) && cfg.FalloffNearBorder > 0) {
					float dist = MG::DistFromBorder(ctx, x, y, z);
					float t = inverse_lerp(0.0f, (float)cfg.FalloffNearBorder, dist - 1) * (1 - MG::GetAboveCeilAmount(ctx, y));
					float zeroValue = minf(noiseSamples[i], cfg.IsoValue - 0.1f);
					noiseSamples[i] = lerp(zeroValue, noiseSamples[i], clamp(t, 0, 1));
				}
			}
		}
	}

	delete[] noiseBuffer;
}

void MeshGen::march_cubes(MG::Context ctx, float noiseSamples[]) {
	auto ref = get_mesh();
	auto ptr = *(ref);
	ImmediateMesh *meshPtr = Object::cast_to<ImmediateMesh>(ptr);
	if (meshPtr == nullptr) {
		UtilityFunctions::printerr("mesh cannot be null");
		return;
	}
	ImmediateMesh mesh = *meshPtr;
	mesh.clear_surfaces();
	mesh.surface_begin(Mesh::PrimitiveType::PRIMITIVE_TRIANGLES);
	Vector3 points[3];
	auto numCells = ctx.numCells;

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
					points[pointIndex] = Vector3(p.x, p.y, p.z);
					pointIndex++;
					if (pointIndex == 3) {
						add_triangle_to_mesh(ctx, mesh, points, uv);
						pointIndex = 0;
					}
				}
			}
		}
	}

	mesh.surface_end();
}

void MeshGen::add_triangle_to_mesh(MG::Context ctx, ImmediateMesh mesh, Vector3 points[], Vector2 uv) {
	auto p1 = points[0];
	auto p2 = points[1];
	auto p3 = points[2];
	auto normal = -(p2 - p1).cross(p3 - p1).normalized();
	for (size_t i = 0; i < 3; i++) {
		auto point = points[i];
		auto x = point.x * ctx.cfg.CellSize;
		auto y = point.y * ctx.cfg.CellSize;
		auto z = point.z * ctx.cfg.CellSize;
		mesh.surface_set_uv(uv);
		mesh.surface_set_normal(normal);
		mesh.surface_add_vertex(get_global_position() + Vector3(x, y, z));
	}
}
