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
	float OrphanLevel;
	// room border
	bool UseBorderNoise;
	bool NormalizeBorder;
	int BorderSize;
	float BorderNoiseIsoValue;
	float SmoothBorderNoise;
	float BorderTilt;
	// tiles
	float TileStrength;
	float TileSmoothing;
	float TileCeiling;
	float TileCeilingFalloff;
	float TileFloor;
	float TileFloorFalloff;
	float TileEraseSize;
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
	float GetOrphanLevel();
	bool GetUseBorderNoise();
	bool GetNormalizeBorder();
	int GetBorderSize();
	float GetBorderNoiseIsoValue();
	float GetSmoothBorderNoise();
	float GetBorderTilt();
	float GetTileStrength();
	float GetTileSmoothing();
	float GetTileCeiling();
	float GetTileCeilingFalloff();
	float GetTileFloor();
	float GetTileFloorFalloff();
	float GetTileEraseSize();
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
	void SetOrphanLevel(float p_OrphansLevel);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetNormalizeBorder(bool p_NormalizeBorder);
	void SetBorderSize(int p_BorderSize);
	void SetBorderNoiseIsoValue(float p_BorderNoiseIsoValue);
	void SetSmoothBorderNoise(float p_SmoothBorderNoise);
	void SetBorderTilt(float p_BorderTilt);
	void SetTileStrength(float p_TileStrength);
	void SetTileSmoothing(float p_TileSmoothing);
	void SetTileCeiling(float p_TileCeiling);
	void SetTileCeilingFalloff(float p_TileCeilingFalloff);
	void SetTileFloor(float p_TileFloor);
	void SetTileFloorFalloff(float p_TileFloorFalloff);
	void SetTileEraseSize(float p_TileEraseSize);
	void SetNeighborBlend(float p_NeighborBlend);

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
	bool dirty;
	int roomIdx;
	Vector2i GridPosition;
	RoomNodes nodes;

	bool GetDirty();
	int GetRoomIdx();
	Vector2i GetGridPosition();
	Ref<RoomConfig> GetNodeUp();
	Ref<RoomConfig> GetNodeDown();
	Ref<RoomConfig> GetNodeLeft();
	Ref<RoomConfig> GetNodeRight();
	void SetDirty(bool p_dirty);
	void SetRoomIdx(int p_roomIdx);
	void SetGridPosition(Vector2i p_GridPosition);
	void SetNodeUp(const Ref<RoomConfig> &p_room);
	void SetNodeDown(const Ref<RoomConfig> &p_room);
	void SetNodeLeft(const Ref<RoomConfig> &p_room);
	void SetNodeRight(const Ref<RoomConfig> &p_room);

	//
	// Tilemap Data
	//
	enum TileState {
		TILE_STATE_UNSET, // unset - will honor noise value at node
		TILE_STATE_EMPTY,
		TILE_STATE_FILLED,
		_TILE_STATE_MAX_,
	};
	void InitTiles(Vector2i numCells2d);
	void InitTilesForNewRoom(Vector2i numCells2d, Vector2i dir, int borderMask);
	void SetTile(Vector2i numCells2d, Vector2i coords, int tile);
	int GetTileAt(Vector2i numCells2d, Vector2i coords);
	int GetNumTiles();
	int *GetTiles();
	void SetTilesExport(PackedInt32Array p_tiles);
	PackedInt32Array GetTilesExport();

	//
	// Noise
	//

	// processed room noise data
	float noiseSamples[MAX_NOISE_NODES];

	// unprocessed noise data to be shared with other rooms for mixing purposes
	float rawSamples[MAX_NOISE_NODES];

protected:
	int numTiles;
	int tiles[MAX_NOISE_NODES_2D];
};
