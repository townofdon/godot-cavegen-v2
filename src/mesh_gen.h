#pragma once

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/noise.hpp"

#include "global_config.h"
#include "room_config.h"

using namespace godot;

class MeshGen : public MeshInstance3D {
	GDCLASS(MeshGen, MeshInstance3D)

protected:
	static void _bind_methods();

public:
	MeshGen() = default;
	~MeshGen() override = default;

	struct Context {
		struct Config {
			// global
			float RoomWidth;
			float RoomHeight;
			float RoomDepth;
			float CellSize;
			float Ceiling;
			float ActivePlaneOffset;
			// debug
			bool ShowNoise;
			bool ShowBorder;
			bool ShowOuterWalls;
			// room noise
			float IsoValue;
			float Curve;
			float Tilt;
			float FalloffAboveCeiling;
			bool Interpolate;
			bool RemoveOrphans;
			// room border
			bool UseBorderNoise;
			int BorderSize;
			float SmoothBorderNoise;
			int FalloffNearBorder;
		};
		Config cfg;
		Noise noise;
		Noise borderNoise;
		Vector3i numCells;
	};

	void generate(GlobalConfig *global_cfg, RoomConfig *room_cfg, Noise *noise, Noise *border_noise);

	static float *time_processNoise;

private:
	void process_noise(MeshGen::Context ctx, float noiseSamples[]);
	int NoiseIndex(MeshGen::Context ctx, int x, int y, int z);
	float GetAboveCeilAmount(MeshGen::Context ctx, int y);
	float GetAboveCeilAmount2(MeshGen::Context ctx, int y);
	float GetCeiling(MeshGen::Context ctx);
	float GetFloorToCeilAmount(MeshGen::Context ctx, int y);
};

// math functions
inline float minf(float a, float b) {
	return std::min(a, b);
}

inline float maxf(float a, float b) {
	return std::max(a, b);
}

inline float clamp(float x, float minv, float maxv) {
	return std::max(minv, std::min(x, maxv));
}

inline float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

inline float inverse_lerp(float a, float b, float p_value) {
	if (a == b) [[unlikely]] {
		return 0.0f;
	} else [[likely]] {
		return (p_value - a) / (b - a);
	}
}
