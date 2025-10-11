#pragma once

#include "constants.h"
#include "godot_cpp/classes/resource.hpp"

using namespace godot;

class RoomConfig : public Resource {
	GDCLASS(RoomConfig, Resource)

protected:
	static void _bind_methods();

public:
	RoomConfig();
	~RoomConfig();

	// debug
	bool ShowNoise;
	bool ShowBorder;
	bool ShowOuterWalls;
	// room noise
	float IsoValue;
	float Curve;
	float Tilt;
	float FalloffAboveCeiling;
	float Interpolate;
	bool RemoveOrphans;
	// room border
	bool UseBorderNoise;
	int BorderSize;
	float BorderNoiseIsoValue;
	float SmoothBorderNoise;
	float FalloffNearBorder;

	bool GetShowNoise();
	bool GetShowBorder();
	bool GetShowOuterWalls();
	float GetIsoValue();
	float GetCurve();
	float GetTilt();
	float GetFalloffAboveCeiling();
	float GetInterpolate();
	bool GetRemoveOrphans();
	bool GetUseBorderNoise();
	int GetBorderSize();
	float GetBorderNoiseIsoValue();
	float GetSmoothBorderNoise();
	float GetFalloffNearBorder();

	void SetShowNoise(bool p_ShowNoise);
	void SetShowBorder(bool p_ShowBorder);
	void SetShowOuterWalls(bool p_ShowOuterWalls);
	void SetIsoValue(float p_IsoValue);
	void SetCurve(float p_Curve);
	void SetTilt(float p_Tilt);
	void SetFalloffAboveCeiling(float p_FalloffAboveCeiling);
	void SetInterpolate(float p_Interpolate);
	void SetRemoveOrphans(bool p_RemoveOrphans);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetBorderSize(int p_BorderSize);
	void SetBorderNoiseIsoValue(float p_BorderNoiseIsoValue);
	void SetSmoothBorderNoise(float p_SmoothBorderNoise);
	void SetFalloffNearBorder(float p_FalloffNearBorder);

	//
	// Tilemap Data
	//
	enum TileState {
		TILE_STATE_UNSET, // unset - will honor noise value at node
		TILE_STATE_EMPTY,
		TILE_STATE_FILLED,
	};
	void SetTile(Vector2i numCells2d, Vector2i coords, int tile);
	// void SetTiles(PackedInt32Array tiles);

protected:
	int tiles[MAX_NOISE_NODES_2D];
};
