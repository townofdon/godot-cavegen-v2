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

	// delete the copy constructor to avoid accidental assignment by value
	RoomConfig(const RoomConfig &) = delete;

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
	float TiltY;
	float TiltX;
	float TiltZ;
	float OffsetY;
	float Smoothing;
	float FalloffAboveCeiling;
	float FalloffNearBorder;
	float Interpolate;
	float ActiveYSmoothing;
	float FloorLevel;
	bool RemoveOrphans;
	// room border
	bool UseBorderNoise;
	bool NormalizeBorder;
	int BorderSize;
	float BorderNoiseIsoValue;
	float SmoothBorderNoise;
	float BorderTilt;
	int BorderGapSpread;
	// tiles
	float TileStrength;
	float TileSmoothing;
	float TileFloor;
	float TileFloorFalloff;
	float TileCeiling;
	float TileCeilingFalloff;
	// neighbors
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
	float GetTiltY();
	float GetTiltX();
	float GetTiltZ();
	float GetOffsetY();
	float GetSmoothing();
	float GetFalloffAboveCeiling();
	float GetFalloffNearBorder();
	float GetActiveYSmoothing();
	float GetFloorLevel();
	float GetInterpolate();
	bool GetRemoveOrphans();
	bool GetUseBorderNoise();
	bool GetNormalizeBorder();
	int GetBorderSize();
	float GetBorderNoiseIsoValue();
	float GetSmoothBorderNoise();
	float GetBorderTilt();
	float GetBorderGapSpread();
	float GetTileStrength();
	float GetTileSmoothing();
	float GetTileFloor();
	float GetTileFloorFalloff();
	float GetTileCeiling();
	float GetTileCeilingFalloff();
	float GetNeighborBlend();

	void SetShowNoise(bool p_ShowNoise);
	void SetShowBorder(bool p_ShowBorder);
	void SetShowFloor(bool p_ShowFloor);
	void SetShowOuterWalls(bool p_ShowOuterWalls);
	void SetNormalize(bool p_Normalize);
	void SetIsoValue(float p_IsoValue);
	void SetNoiseFloor(float p_NoiseFloor);
	void SetNoiseCeil(float p_NoiseCeil);
	void SetCurve(float p_Curve);
	void SetTiltY(float p_TiltY);
	void SetTiltX(float p_TiltX);
	void SetTiltZ(float p_TiltZ);
	void SetOffsetY(float p_OffsetY);
	void SetSmoothing(float p_Smoothing);
	void SetFalloffAboveCeiling(float p_FalloffAboveCeiling);
	void SetFalloffNearBorder(float p_FalloffNearBorder);
	void SetActiveYSmoothing(float p_ActiveYSmoothing);
	void SetFloorLevel(float p_FloorLevel);
	void SetInterpolate(float p_Interpolate);
	void SetRemoveOrphans(bool p_RemoveOrphans);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetNormalizeBorder(bool p_NormalizeBorder);
	void SetBorderSize(int p_BorderSize);
	void SetBorderNoiseIsoValue(float p_BorderNoiseIsoValue);
	void SetSmoothBorderNoise(float p_SmoothBorderNoise);
	void SetBorderTilt(float p_BorderTilt);
	void SetBorderGapSpread(float p_BorderGapSpread);
	void SetTileStrength(float p_TileStrength);
	void SetTileSmoothing(float p_TileSmoothing);
	void SetTileFloor(float p_TileFloor);
	void SetTileFloorFalloff(float p_TileFloorFalloff);
	void SetTileCeiling(float p_TileCeiling);
	void SetTileCeilingFalloff(float p_TileCeilingFalloff);
	void SetNeighborBlend(float p_NeighborBlend);

	int GetPrecedence();
	void SetPrecedence(int p_precedence);

	void NotifyChanged();

	//
	// Internal
	//
	struct RoomNodes {
		Ref<RoomConfig> up;
		Ref<RoomConfig> down;
		Ref<RoomConfig> left;
		Ref<RoomConfig> right;
	};
	int precedence;
	RoomNodes nodes;
	bool ValidateSetNode(const Ref<RoomConfig> &p_room, Ref<RoomConfig> &compare);
	void SetNodeUp(const Ref<RoomConfig> &p_room);
	void SetNodeDown(const Ref<RoomConfig> &p_room);
	void SetNodeLeft(const Ref<RoomConfig> &p_room);
	void SetNodeRight(const Ref<RoomConfig> &p_room);
	Ref<RoomConfig> GetNodeUp();
	Ref<RoomConfig> GetNodeDown();
	Ref<RoomConfig> GetNodeLeft();
	Ref<RoomConfig> GetNodeRight();

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

	// processed room noise data
	float noiseSamples[MAX_NOISE_NODES];

	// unprocessed noise data to be shared with other rooms for mixing purposes
	float rawSamples[MAX_NOISE_NODES];

protected:
	int tiles[MAX_NOISE_NODES_2D];
};
