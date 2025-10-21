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

	// internal
	int precedence;

	// debug
	bool ShowNoise;
	bool ShowBorder;
	bool ShowFloor;
	bool ShowOuterWalls;
	// room noise
	bool Normalize;
	float IsoValue;
	float NoiseFloor;
	float NoiseCeil;
	float Curve;
	float Tilt;
	float FalloffAboveCeiling;
	float Interpolate;
	float ActiveYSmoothing;
	bool RemoveOrphans;
	// room border
	bool UseBorderNoise;
	int BorderSize;
	float BorderNoiseIsoValue;
	float SmoothBorderNoise;
	float FalloffNearBorder;
	int BorderGapSpread;
	// tiles
	float TileStrength;
	float TileSmoothing;
	float TileFloor;
	float TileFloorFalloff;
	float TileCeiling;
	float TileCeilingFalloff;
	// neighbors
	// TODO: ADD GETTER,SETTER
	float NeighborBlend;

	bool GetShowNoise();
	bool GetShowBorder();
	bool GetShowFloor();
	bool GetShowOuterWalls();
	bool GetNormalize();
	float GetIsoValue();
	float GetNoiseFloor();
	float GetNoiseCeil();
	float GetCurve();
	float GetTilt();
	float GetFalloffAboveCeiling();
	float GetActiveYSmoothing();
	float GetInterpolate();
	bool GetRemoveOrphans();
	bool GetUseBorderNoise();
	int GetBorderSize();
	float GetBorderNoiseIsoValue();
	float GetSmoothBorderNoise();
	float GetFalloffNearBorder();
	float GetBorderGapSpread();
	float GetTileStrength();
	float GetTileSmoothing();
	float GetTileFloor();
	float GetTileFloorFalloff();
	float GetTileCeiling();
	float GetTileCeilingFalloff();

	void SetShowNoise(bool p_ShowNoise);
	void SetShowBorder(bool p_ShowBorder);
	void SetShowFloor(bool p_ShowFloor);
	void SetShowOuterWalls(bool p_ShowOuterWalls);
	void SetNormalize(bool p_Normalize);
	void SetIsoValue(float p_IsoValue);
	void SetNoiseFloor(float p_NoiseFloor);
	void SetNoiseCeil(float p_NoiseCeil);
	void SetCurve(float p_Curve);
	void SetTilt(float p_Tilt);
	void SetFalloffAboveCeiling(float p_FalloffAboveCeiling);
	void SetActiveYSmoothing(float p_ActiveYSmoothing);
	void SetInterpolate(float p_Interpolate);
	void SetRemoveOrphans(bool p_RemoveOrphans);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetBorderSize(int p_BorderSize);
	void SetBorderNoiseIsoValue(float p_BorderNoiseIsoValue);
	void SetSmoothBorderNoise(float p_SmoothBorderNoise);
	void SetFalloffNearBorder(float p_FalloffNearBorder);
	void SetBorderGapSpread(float p_BorderGapSpread);
	void SetTileStrength(float p_TileStrength);
	void SetTileSmoothing(float p_TileSmoothing);
	void SetTileFloor(float p_TileFloor);
	void SetTileFloorFalloff(float p_TileFloorFalloff);
	void SetTileCeiling(float p_TileCeiling);
	void SetTileCeilingFalloff(float p_TileCeilingFalloff);

	int GetPrecedence();
	void SetPrecedence(int p_precedence);

	void NotifyChanged();

	//
	// Neighbors
	//
	struct RoomNodes {
		RoomConfig *up;
		RoomConfig *down;
		RoomConfig *left;
		RoomConfig *right;
	};
	enum NeighborDir {
		UP,
		DOWN,
		LEFT,
		RIGHT,
	};
	void SetNeighbor(RoomConfig *room, NeighborDir dir);
	RoomNodes nodes;

	//
	// Tilemap Data
	//
	enum TileState {
		TILE_STATE_UNSET, // unset - will honor noise value at node
		TILE_STATE_EMPTY,
		TILE_STATE_FILLED,
		_TILE_STATE_MAX_,
	};
	void SetTile(Vector2i numCells2d, Vector2i coords, int tile);
	int *GetTiles();

	//
	// Noise
	//
	float rawSamples[MAX_NOISE_NODES];
	float noiseSamples[MAX_NOISE_NODES];

protected:
	int tiles[MAX_NOISE_NODES_2D];
};
