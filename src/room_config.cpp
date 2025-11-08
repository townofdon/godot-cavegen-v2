#include "room_config.h"

#include "godot_cpp/core/error_macros.hpp"

VARIANT_ENUM_CAST(RoomConfig::TileState);

void RoomConfig::_bind_methods() {
	ADD_GROUP("Debug", "debug__");

	ClassDB::bind_method(D_METHOD("get_ShowNoise"), &RoomConfig::GetShowNoise);
	ClassDB::bind_method(D_METHOD("set_ShowNoise", "p_ShowNoise"), &RoomConfig::SetShowNoise);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_noise"), "set_ShowNoise", "get_ShowNoise");

	ClassDB::bind_method(D_METHOD("get_ShowBorder"), &RoomConfig::GetShowBorder);
	ClassDB::bind_method(D_METHOD("set_ShowBorder", "p_ShowBorder"), &RoomConfig::SetShowBorder);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_border"), "set_ShowBorder", "get_ShowBorder");

	ClassDB::bind_method(D_METHOD("get_ShowFloor"), &RoomConfig::GetShowFloor);
	ClassDB::bind_method(D_METHOD("set_ShowFloor", "p_ShowFloor"), &RoomConfig::SetShowFloor);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_floor"), "set_ShowFloor", "get_ShowFloor");

	ClassDB::bind_method(D_METHOD("get_ShowOuterWalls"), &RoomConfig::GetShowOuterWalls);
	ClassDB::bind_method(D_METHOD("set_ShowOuterWalls", "p_ShowOuterWalls"), &RoomConfig::SetShowOuterWalls);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_outer_walls"), "set_ShowOuterWalls", "get_ShowOuterWalls");

	ADD_GROUP("Room Noise", "room_noise__");

	ClassDB::bind_method(D_METHOD("get_Normalize"), &RoomConfig::GetNormalize);
	ClassDB::bind_method(D_METHOD("set_Normalize", "p_Normalize"), &RoomConfig::SetNormalize);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_noise__normalize"), "set_Normalize", "get_Normalize");

	ClassDB::bind_method(D_METHOD("get_IsoValue"), &RoomConfig::GetIsoValue);
	ClassDB::bind_method(D_METHOD("set_IsoValue", "p_IsoValue"), &RoomConfig::SetIsoValue);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__iso_value", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_IsoValue", "get_IsoValue");

	ClassDB::bind_method(D_METHOD("get_NoiseFloor"), &RoomConfig::GetNoiseFloor);
	ClassDB::bind_method(D_METHOD("set_NoiseFloor", "p_NoiseFloor"), &RoomConfig::SetNoiseFloor);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__noise_floor", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_NoiseFloor", "get_NoiseFloor");

	ClassDB::bind_method(D_METHOD("get_NoiseCeil"), &RoomConfig::GetNoiseCeil);
	ClassDB::bind_method(D_METHOD("set_NoiseCeil", "p_NoiseCeil"), &RoomConfig::SetNoiseCeil);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__noise_ceil", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_NoiseCeil", "get_NoiseCeil");

	ClassDB::bind_method(D_METHOD("get_Curve"), &RoomConfig::GetCurve);
	ClassDB::bind_method(D_METHOD("set_Curve", "p_Curve"), &RoomConfig::SetCurve);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__curve", PROPERTY_HINT_RANGE, "0,2,0.001"), "set_Curve", "get_Curve");

	ClassDB::bind_method(D_METHOD("get_TiltY"), &RoomConfig::GetTiltY);
	ClassDB::bind_method(D_METHOD("set_TiltY", "p_TiltY"), &RoomConfig::SetTiltY);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__tilt_y", PROPERTY_HINT_RANGE, "0,2,0.001"), "set_TiltY", "get_TiltY");

	ClassDB::bind_method(D_METHOD("get_TiltX"), &RoomConfig::GetTiltX);
	ClassDB::bind_method(D_METHOD("set_TiltX", "p_TiltX"), &RoomConfig::SetTiltX);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__tilt_x", PROPERTY_HINT_RANGE, "0,2,0.001"), "set_TiltX", "get_TiltX");

	ClassDB::bind_method(D_METHOD("get_TiltZ"), &RoomConfig::GetTiltZ);
	ClassDB::bind_method(D_METHOD("set_TiltZ", "p_TiltZ"), &RoomConfig::SetTiltZ);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__tilt_z", PROPERTY_HINT_RANGE, "0,2,0.001"), "set_TiltZ", "get_TiltZ");

	ClassDB::bind_method(D_METHOD("get_OffsetY"), &RoomConfig::GetOffsetY);
	ClassDB::bind_method(D_METHOD("set_OffsetY", "p_OffsetY"), &RoomConfig::SetOffsetY);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__offset_y", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_OffsetY", "get_OffsetY");

	ClassDB::bind_method(D_METHOD("get_Smoothing"), &RoomConfig::GetSmoothing);
	ClassDB::bind_method(D_METHOD("set_Smoothing", "p_Smoothing"), &RoomConfig::SetSmoothing);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__smooth_crystalize", PROPERTY_HINT_RANGE, "-10,10,0.01"), "set_Smoothing", "get_Smoothing");

	ClassDB::bind_method(D_METHOD("get_FalloffAboveCeiling"), &RoomConfig::GetFalloffAboveCeiling);
	ClassDB::bind_method(D_METHOD("set_FalloffAboveCeiling", "p_FalloffAboveCeiling"), &RoomConfig::SetFalloffAboveCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__falloff_above_ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_FalloffAboveCeiling", "get_FalloffAboveCeiling");

	ClassDB::bind_method(D_METHOD("get_FalloffNearBorder"), &RoomConfig::GetFalloffNearBorder);
	ClassDB::bind_method(D_METHOD("set_FalloffNearBorder", "p_FalloffNearBorder"), &RoomConfig::SetFalloffNearBorder);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__falloff_near_border", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_FalloffNearBorder", "get_FalloffNearBorder");

	ClassDB::bind_method(D_METHOD("get_Interpolate"), &RoomConfig::GetInterpolate);
	ClassDB::bind_method(D_METHOD("set_Interpolate", "p_Interpolate"), &RoomConfig::SetInterpolate);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__interpolate", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_Interpolate", "get_Interpolate");

	ClassDB::bind_method(D_METHOD("get_ActiveYSmoothing"), &RoomConfig::GetActiveYSmoothing);
	ClassDB::bind_method(D_METHOD("set_ActiveYSmoothing", "p_ActiveYSmoothing"), &RoomConfig::SetActiveYSmoothing);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__active_y_smoothing", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_ActiveYSmoothing", "get_ActiveYSmoothing");

	ClassDB::bind_method(D_METHOD("get_FloorLevel"), &RoomConfig::GetFloorLevel);
	ClassDB::bind_method(D_METHOD("set_FloorLevel", "p_FloorLevel"), &RoomConfig::SetFloorLevel);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__floor_level", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_FloorLevel", "get_FloorLevel");

	ClassDB::bind_method(D_METHOD("get_RemoveOrphans"), &RoomConfig::GetRemoveOrphans);
	ClassDB::bind_method(D_METHOD("set_RemoveOrphans", "p_RemoveOrphans"), &RoomConfig::SetRemoveOrphans);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_noise__remove_orphans"), "set_RemoveOrphans", "get_RemoveOrphans");

	ADD_GROUP("Room Border", "room_border__");

	ClassDB::bind_method(D_METHOD("get_UseBorderNoise"), &RoomConfig::GetUseBorderNoise);
	ClassDB::bind_method(D_METHOD("set_UseBorderNoise", "p_UseBorderNoise"), &RoomConfig::SetUseBorderNoise);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_border__use_border_noise"), "set_UseBorderNoise", "get_UseBorderNoise");

	ClassDB::bind_method(D_METHOD("get_NormalizeBorder"), &RoomConfig::GetNormalizeBorder);
	ClassDB::bind_method(D_METHOD("set_NormalizeBorder", "p_NormalizeBorder"), &RoomConfig::SetNormalizeBorder);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_border__normalize_border_noise"), "set_NormalizeBorder", "get_NormalizeBorder");

	ClassDB::bind_method(D_METHOD("get_BorderSize"), &RoomConfig::GetBorderSize);
	ClassDB::bind_method(D_METHOD("set_BorderSize", "p_BorderSize"), &RoomConfig::SetBorderSize);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "room_border__border_size", PROPERTY_HINT_RANGE, "0,10,"), "set_BorderSize", "get_BorderSize");

	ClassDB::bind_method(D_METHOD("get_BorderNoiseIsoValue"), &RoomConfig::GetBorderNoiseIsoValue);
	ClassDB::bind_method(D_METHOD("set_BorderNoiseIsoValue", "p_BorderNoiseIsoValue"), &RoomConfig::SetBorderNoiseIsoValue);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__border_noise_strength", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_BorderNoiseIsoValue", "get_BorderNoiseIsoValue");

	ClassDB::bind_method(D_METHOD("get_SmoothBorderNoise"), &RoomConfig::GetSmoothBorderNoise);
	ClassDB::bind_method(D_METHOD("set_SmoothBorderNoise", "p_SmoothBorderNoise"), &RoomConfig::SetSmoothBorderNoise);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__smooth_border_noise", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_SmoothBorderNoise", "get_SmoothBorderNoise");

	ClassDB::bind_method(D_METHOD("get_BorderTilt"), &RoomConfig::GetBorderTilt);
	ClassDB::bind_method(D_METHOD("set_BorderTilt", "p_BorderTilt"), &RoomConfig::SetBorderTilt);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__border_tilt", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_BorderTilt", "get_BorderTilt");

	ADD_GROUP("Tile Apply", "tile_apply__");

	ClassDB::bind_method(D_METHOD("get_TileStrength"), &RoomConfig::GetTileStrength);
	ClassDB::bind_method(D_METHOD("set_TileStrength", "p_TileStrength"), &RoomConfig::SetTileStrength);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_strength", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_TileStrength", "get_TileStrength");

	ClassDB::bind_method(D_METHOD("get_TileSmoothing"), &RoomConfig::GetTileSmoothing);
	ClassDB::bind_method(D_METHOD("set_TileSmoothing", "p_TileSmoothing"), &RoomConfig::SetTileSmoothing);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_smoothing", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileSmoothing", "get_TileSmoothing");

	ClassDB::bind_method(D_METHOD("get_TileCeiling"), &RoomConfig::GetTileCeiling);
	ClassDB::bind_method(D_METHOD("set_TileCeiling", "p_TileCeiling"), &RoomConfig::SetTileCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileCeiling", "get_TileCeiling");

	ClassDB::bind_method(D_METHOD("get_TileCeilingFalloff"), &RoomConfig::GetTileCeilingFalloff);
	ClassDB::bind_method(D_METHOD("set_TileCeilingFalloff", "p_TileCeilingFalloff"), &RoomConfig::SetTileCeilingFalloff);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_ceil_blend", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileCeilingFalloff", "get_TileCeilingFalloff");

	ClassDB::bind_method(D_METHOD("get_TileFloor"), &RoomConfig::GetTileFloor);
	ClassDB::bind_method(D_METHOD("set_TileFloor", "p_TileFloor"), &RoomConfig::SetTileFloor);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_floor", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileFloor", "get_TileFloor");

	ClassDB::bind_method(D_METHOD("get_TileFloorFalloff"), &RoomConfig::GetTileFloorFalloff);
	ClassDB::bind_method(D_METHOD("set_TileFloorFalloff", "p_TileFloorFalloff"), &RoomConfig::SetTileFloorFalloff);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_floor_blend", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileFloorFalloff", "get_TileFloorFalloff");

	ClassDB::bind_method(D_METHOD("get_TileEraseSize"), &RoomConfig::GetTileEraseSize);
	ClassDB::bind_method(D_METHOD("set_TileEraseSize", "p_TileEraseSize"), &RoomConfig::SetTileEraseSize);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_erase_size", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileEraseSize", "get_TileEraseSize");

	ADD_GROUP("Neighbors", "neighbors__");

	ClassDB::bind_method(D_METHOD("get_NeighborBlend"), &RoomConfig::GetNeighborBlend);
	ClassDB::bind_method(D_METHOD("set_NeighborBlend", "p_NeighborBlend"), &RoomConfig::SetNeighborBlend);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "neighbors__neighbor_blend", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_NeighborBlend", "get_NeighborBlend");

	ADD_GROUP("Internal", "internal__");

	ClassDB::bind_method(D_METHOD("get_dirty"), &RoomConfig::GetDirty);
	ClassDB::bind_method(D_METHOD("set_dirty", "p_dirty"), &RoomConfig::SetDirty);

	ClassDB::bind_method(D_METHOD("get_room_idx"), &RoomConfig::GetRoomIdx);
	ClassDB::bind_method(D_METHOD("set_room_idx", "p_room_idx"), &RoomConfig::SetRoomIdx);

	ClassDB::bind_method(D_METHOD("get_GridPosition"), &RoomConfig::GetGridPosition);
	ClassDB::bind_method(D_METHOD("set_GridPosition", "p_GridPosition"), &RoomConfig::SetGridPosition);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "internal__grid_position"), "set_GridPosition", "get_GridPosition");

	ClassDB::bind_method(D_METHOD("get_node_up"), &RoomConfig::GetNodeUp);
	ClassDB::bind_method(D_METHOD("set_node_up", "p_room"), &RoomConfig::SetNodeUp);

	ClassDB::bind_method(D_METHOD("get_node_down"), &RoomConfig::GetNodeDown);
	ClassDB::bind_method(D_METHOD("set_node_down", "p_room"), &RoomConfig::SetNodeDown);

	ClassDB::bind_method(D_METHOD("get_node_left"), &RoomConfig::GetNodeLeft);
	ClassDB::bind_method(D_METHOD("set_node_left", "p_room"), &RoomConfig::SetNodeLeft);

	ClassDB::bind_method(D_METHOD("get_node_right"), &RoomConfig::GetNodeRight);
	ClassDB::bind_method(D_METHOD("set_node_right", "p_room"), &RoomConfig::SetNodeRight);

	ADD_SIGNAL(MethodInfo("on_changed"));
	ADD_SIGNAL(MethodInfo("dirtied"));

	ClassDB::bind_method(D_METHOD("notify_changed"), &RoomConfig::NotifyChanged);

	//
	// Tilemap Data
	//

	ADD_GROUP("Tilemap Data", "tilemap__");

	ClassDB::bind_method(D_METHOD("get_TilesExport"), &RoomConfig::GetTilesExport);
	ClassDB::bind_method(D_METHOD("set_TilesExport", "p_tiles"), &RoomConfig::SetTilesExport);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "tilemap__tiles"), "set_TilesExport", "get_TilesExport");

	ClassDB::bind_method(D_METHOD("init_tiles", "num_cells_2d"), &RoomConfig::InitTiles);
	ClassDB::bind_method(D_METHOD("init_tiles_for_new_room", "num_cells_2d", "dir", "border_mask"), &RoomConfig::InitTilesForNewRoom);
	ClassDB::bind_method(D_METHOD("set_tile", "num_cells_2d", "coords", "tile"), &RoomConfig::SetTile);
	ClassDB::bind_method(D_METHOD("get_tile_at", "num_cells_2d", "coords"), &RoomConfig::GetTileAt);
	ClassDB::bind_method(D_METHOD("get_num_tiles"), &RoomConfig::GetNumTiles);

	BIND_ENUM_CONSTANT(TILE_STATE_UNSET);
	BIND_ENUM_CONSTANT(TILE_STATE_FILLED);
	BIND_ENUM_CONSTANT(TILE_STATE_EMPTY);
}

RoomConfig::RoomConfig() {
	// Initialize variables
	ShowNoise = true;
	ShowBorder = true;
	ShowOuterWalls = true;
	ShowFloor = true;
	Normalize = false;
	IsoValue = 0.5f;
	NoiseFloor = 0;
	NoiseCeil = 1;
	Curve = 1.0f;
	TiltY = 1.0f;
	TiltX = 1.0f;
	TiltZ = 1.0f;
	OffsetY = 0.5f;
	Smoothing = 0;
	FalloffAboveCeiling = 0.5f;
	FalloffNearBorder = 0.2f;
	Interpolate = 1.0f;
	ActiveYSmoothing = 0.5f;
	FloorLevel = 0;
	RemoveOrphans = true;
	UseBorderNoise = false;
	NormalizeBorder = false;
	BorderSize = 1;
	BorderNoiseIsoValue = 0.5f;
	SmoothBorderNoise = 0.5f;
	BorderTilt = 0.5f;
	TileStrength = 1.0f;
	TileSmoothing = 0.0f;
	TileCeiling = 0.6f;
	TileCeilingFalloff = 0.2f;
	TileFloor = 0.4f;
	TileFloorFalloff = 0.2f;
	NeighborBlend = 0.25f;
	// initialize tiles
	for (size_t i = 0; i < MAX_NOISE_NODES_2D; i++) {
		tiles[i] = 0;
	}
	InitTiles(Vector2i(30, 30));
	numTiles = 0;
	// initialize internal vars
	roomIdx = 0;
	GridPosition = Vector2i(0, 0);
}

RoomConfig::~RoomConfig() {
	// Add your cleanup here, however, the destructor is called quite often (??), so be wary.
}

bool RoomConfig::GetShowNoise() {
	return ShowNoise;
}
bool RoomConfig::GetShowBorder() {
	return ShowBorder;
}
bool RoomConfig::GetShowFloor() {
	return ShowFloor;
}
bool RoomConfig::GetShowOuterWalls() {
	return ShowOuterWalls;
}
bool RoomConfig::GetNormalize() {
	return Normalize;
}
float RoomConfig::GetIsoValue() {
	return IsoValue;
}
float RoomConfig::GetNoiseFloor() {
	return NoiseFloor;
}
float RoomConfig::GetNoiseCeil() {
	return NoiseCeil;
}
float RoomConfig::GetCurve() {
	return Curve;
}
float RoomConfig::GetTiltY() {
	return TiltY;
}
float RoomConfig::GetTiltX() {
	return TiltX;
}
float RoomConfig::GetTiltZ() {
	return TiltZ;
}
float RoomConfig::GetOffsetY() {
	return OffsetY;
}
float RoomConfig::GetSmoothing() {
	return Smoothing;
}
float RoomConfig::GetFalloffAboveCeiling() {
	return FalloffAboveCeiling;
}
float RoomConfig::GetFalloffNearBorder() {
	return FalloffNearBorder;
}
float RoomConfig::GetActiveYSmoothing() {
	return ActiveYSmoothing;
}
float RoomConfig::GetFloorLevel() {
	return FloorLevel;
}
float RoomConfig::GetInterpolate() {
	return Interpolate;
}
bool RoomConfig::GetRemoveOrphans() {
	return RemoveOrphans;
}
bool RoomConfig::GetUseBorderNoise() {
	return UseBorderNoise;
}
bool RoomConfig::GetNormalizeBorder() {
	return NormalizeBorder;
}
int RoomConfig::GetBorderSize() {
	return BorderSize;
}
float RoomConfig::GetBorderNoiseIsoValue() {
	return BorderNoiseIsoValue;
}
float RoomConfig::GetSmoothBorderNoise() {
	return SmoothBorderNoise;
}
float RoomConfig::GetBorderTilt() {
	return BorderTilt;
}
float RoomConfig::GetTileStrength() {
	return TileStrength;
}
float RoomConfig::GetTileSmoothing() {
	return TileSmoothing;
}
float RoomConfig::GetTileCeiling() {
	return TileCeiling;
}
float RoomConfig::GetTileCeilingFalloff() {
	return TileCeilingFalloff;
}
float RoomConfig::GetTileFloor() {
	return TileFloor;
}
float RoomConfig::GetTileFloorFalloff() {
	return TileFloorFalloff;
}
float RoomConfig::GetTileEraseSize() {
	return TileEraseSize;
}
float RoomConfig::GetNeighborBlend() {
	return NeighborBlend;
}

void RoomConfig::SetShowNoise(bool p_ShowNoise) {
	ShowNoise = p_ShowNoise;
	emit_signal("on_changed");
}
void RoomConfig::SetShowBorder(bool p_ShowBorder) {
	ShowBorder = p_ShowBorder;
	emit_signal("on_changed");
}
void RoomConfig::SetShowFloor(bool p_ShowFloor) {
	ShowFloor = p_ShowFloor;
	emit_signal("on_changed");
}
void RoomConfig::SetShowOuterWalls(bool p_ShowOuterWalls) {
	ShowOuterWalls = p_ShowOuterWalls;
	emit_signal("on_changed");
}
void RoomConfig::SetNormalize(bool p_Normalize) {
	Normalize = p_Normalize;
	emit_signal("on_changed");
}
void RoomConfig::SetIsoValue(float p_IsoValue) {
	IsoValue = p_IsoValue;
	emit_signal("on_changed");
}
void RoomConfig::SetNoiseFloor(float p_NoiseFloor) {
	NoiseFloor = p_NoiseFloor;
	emit_signal("on_changed");
}
void RoomConfig::SetNoiseCeil(float p_NoiseCeil) {
	NoiseCeil = p_NoiseCeil;
	emit_signal("on_changed");
}
void RoomConfig::SetCurve(float p_Curve) {
	Curve = p_Curve;
	emit_signal("on_changed");
}
void RoomConfig::SetTiltY(float p_TiltY) {
	TiltY = p_TiltY;
	emit_signal("on_changed");
}
void RoomConfig::SetTiltX(float p_TiltX) {
	TiltX = p_TiltX;
	emit_signal("on_changed");
}
void RoomConfig::SetTiltZ(float p_TiltZ) {
	TiltZ = p_TiltZ;
	emit_signal("on_changed");
}
void RoomConfig::SetOffsetY(float p_OffsetY) {
	OffsetY = p_OffsetY;
	emit_signal("on_changed");
}
void RoomConfig::SetSmoothing(float p_Smoothing) {
	Smoothing = p_Smoothing;
	emit_signal("on_changed");
}
void RoomConfig::SetFalloffAboveCeiling(float p_FalloffAboveCeiling) {
	FalloffAboveCeiling = p_FalloffAboveCeiling;
	emit_signal("on_changed");
}
void RoomConfig::SetFalloffNearBorder(float p_FalloffNearBorder) {
	FalloffNearBorder = p_FalloffNearBorder;
	emit_signal("on_changed");
}
void RoomConfig::SetActiveYSmoothing(float p_ActiveYSmoothing) {
	ActiveYSmoothing = p_ActiveYSmoothing;
	emit_signal("on_changed");
}
void RoomConfig::SetFloorLevel(float p_FloorLevel) {
	FloorLevel = p_FloorLevel;
	emit_signal("on_changed");
}
void RoomConfig::SetInterpolate(float p_Interpolate) {
	Interpolate = p_Interpolate;
	emit_signal("on_changed");
}
void RoomConfig::SetRemoveOrphans(bool p_RemoveOrphans) {
	RemoveOrphans = p_RemoveOrphans;
	emit_signal("on_changed");
}
void RoomConfig::SetUseBorderNoise(bool p_UseBorderNoise) {
	UseBorderNoise = p_UseBorderNoise;
	emit_signal("on_changed");
}
void RoomConfig::SetNormalizeBorder(bool p_NormalizeBorder) {
	NormalizeBorder = p_NormalizeBorder;
	emit_signal("on_changed");
}
void RoomConfig::SetBorderSize(int p_BorderSize) {
	BorderSize = p_BorderSize;
	emit_signal("on_changed");
}
void RoomConfig::SetBorderNoiseIsoValue(float p_BorderNoiseIsoValue) {
	BorderNoiseIsoValue = p_BorderNoiseIsoValue;
	emit_signal("on_changed");
}
void RoomConfig::SetSmoothBorderNoise(float p_SmoothBorderNoise) {
	SmoothBorderNoise = p_SmoothBorderNoise;
	emit_signal("on_changed");
}
void RoomConfig::SetBorderTilt(float p_BorderTilt) {
	BorderTilt = p_BorderTilt;
	emit_signal("on_changed");
}
void RoomConfig::SetTileStrength(float p_TileStrength) {
	TileStrength = p_TileStrength;
	emit_signal("on_changed");
}
void RoomConfig::SetTileSmoothing(float p_TileSmoothing) {
	TileSmoothing = p_TileSmoothing;
	emit_signal("on_changed");
}
void RoomConfig::SetTileCeiling(float p_TileCeiling) {
	TileCeiling = p_TileCeiling;
	emit_signal("on_changed");
}
void RoomConfig::SetTileCeilingFalloff(float p_TileCeilingFalloff) {
	TileCeilingFalloff = p_TileCeilingFalloff;
	emit_signal("on_changed");
}
void RoomConfig::SetTileFloor(float p_TileFloor) {
	TileFloor = p_TileFloor;
	emit_signal("on_changed");
}
void RoomConfig::SetTileFloorFalloff(float p_TileFloorFalloff) {
	TileFloorFalloff = p_TileFloorFalloff;
	emit_signal("on_changed");
}
void RoomConfig::SetTileEraseSize(float p_TileEraseSize) {
	TileEraseSize = p_TileEraseSize;
	emit_signal("on_changed");
}
void RoomConfig::SetNeighborBlend(float p_NeighborBlend) {
	NeighborBlend = p_NeighborBlend;
	emit_signal("on_changed");
}

void RoomConfig::NotifyChanged() {
	emit_signal("on_changed");
}

//
// Tilemap Data
//
void RoomConfig::InitTiles(Vector2i numCells2d) {
	for (size_t y = 0; y < numCells2d.y; y++) {
		for (size_t x = 0; x < numCells2d.x; x++) {
			SetTile(numCells2d, Vector2i(x, y), TILE_STATE_UNSET);
		}
	}
	numTiles = numCells2d.x * numCells2d.y;
}
void RoomConfig::InitTilesForNewRoom(Vector2i numCells2d, Vector2i dir, int borderMask) {
	auto up = Vector2i(0, -1);
	auto down = Vector2i(0, 1);
	auto left = Vector2i(-1, 0);
	auto right = Vector2i(1, 0);
	ERR_FAIL_COND_EDMSG(dir != up && dir != down && dir != left && dir != right, "unacceptable, reclaimer.");
	ERR_FAIL_INDEX_EDMSG(numCells2d.x * numCells2d.y - 1, MAX_NOISE_NODES_2D, "numcells greater than max allowed");
	int temp[MAX_NOISE_NODES_2D];
	int xmax = numCells2d.x - 1;
	int ymax = numCells2d.y - 1;
	bool borderUp = (borderMask & BORDER_MASK_UP) > 0;
	bool borderDown = (borderMask & BORDER_MASK_DOWN) > 0;
	bool borderLeft = (borderMask & BORDER_MASK_LEFT) > 0;
	bool borderRight = (borderMask & BORDER_MASK_RIGHT) > 0;
	for (size_t y = 0; y < numCells2d.y; y++) {
		for (size_t x = 0; x < numCells2d.x; x++) {
			int i = x + y * numCells2d.x;
			if (
				(x == 0 && borderLeft) ||
				(x == xmax && borderRight) ||
				(y == 0 && borderUp) ||
				(y == ymax && borderDown)) {
				if (x == 0 && dir == right) {
					temp[i] = GetTileAt(numCells2d, Vector2i(xmax, y));
				} else if (x == xmax && dir == left) {
					temp[i] = GetTileAt(numCells2d, Vector2i(0, y));
				} else if (y == 0 && dir == down) {
					temp[i] = GetTileAt(numCells2d, Vector2i(x, ymax));
				} else if (y == ymax && dir == up) {
					temp[i] = GetTileAt(numCells2d, Vector2i(x, 0));
				} else {
					temp[i] = TILE_STATE_FILLED;
				}
			} else {
				temp[i] = TILE_STATE_UNSET;
			}
		}
	}
	for (size_t y = 0; y < numCells2d.y; y++) {
		for (size_t x = 0; x < numCells2d.x; x++) {
			int i = x + y * numCells2d.x;
			SetTile(numCells2d, Vector2i(x, y), temp[i]);
		}
	}
	numTiles = numCells2d.x * numCells2d.y;
}
void RoomConfig::SetTile(Vector2i numCells2d, Vector2i coords, int tile) {
	int x = coords.x;
	int y = coords.y;
	int i = x + y * numCells2d.x;

	ERR_FAIL_INDEX_EDMSG(i, MAX_NOISE_NODES_2D, "tile index out of bounds");
	ERR_FAIL_INDEX_EDMSG(tile, RoomConfig::TileState::_TILE_STATE_MAX_, "invalid tile");

	tiles[i] = tile;
	numTiles = numCells2d.x * numCells2d.y;
}
int RoomConfig::GetTileAt(Vector2i numCells2d, Vector2i coords) {
	int x = coords.x;
	int y = coords.y;
	int i = x + y * numCells2d.x;
	ERR_FAIL_INDEX_V_EDMSG(i, MAX_NOISE_NODES_2D, 0, "tile index out of bounds");
	return tiles[i];
}
int RoomConfig::GetNumTiles() {
	return numTiles;
}
int *RoomConfig::GetTiles() {
	return tiles;
}
void RoomConfig::SetTilesExport(PackedInt32Array p_tiles) {
	numTiles = 0;
	int count = p_tiles.size();
	for (size_t i = 0; i < count; i++) {
		int tile = p_tiles.get(i);
		ERR_FAIL_INDEX_EDMSG(i, MAX_NOISE_NODES_2D, "tile index out of bounds");
		ERR_FAIL_INDEX_EDMSG(tile, RoomConfig::TileState::_TILE_STATE_MAX_, "invalid tile");
		tiles[i] = tile;
	}
	numTiles = count;
}
PackedInt32Array RoomConfig::GetTilesExport() {
	auto ret = PackedInt32Array();
	for (size_t i = 0; i < numTiles; i++) {
		ret.append(tiles[i]);
	}
	return ret;
}

//
// INTERNAL
//
bool RoomConfig::GetDirty() {
	return dirty;
}
int RoomConfig::GetRoomIdx() {
	return roomIdx;
}
Vector2i RoomConfig::GetGridPosition() {
	return GridPosition;
}
Ref<RoomConfig> RoomConfig::GetNodeUp() {
	return nodes.up;
}
Ref<RoomConfig> RoomConfig::GetNodeDown() {
	return nodes.down;
}
Ref<RoomConfig> RoomConfig::GetNodeLeft() {
	return nodes.left;
}
Ref<RoomConfig> RoomConfig::GetNodeRight() {
	return nodes.right;
}
void RoomConfig::SetDirty(bool p_dirty) {
	dirty = p_dirty;
	if (p_dirty) {
		if (nodes.up.is_valid()) {
			nodes.up->dirty = true;
		}
		if (nodes.down.is_valid()) {
			nodes.down->dirty = true;
		}
		if (nodes.left.is_valid()) {
			nodes.left->dirty = true;
		}
		if (nodes.right.is_valid()) {
			nodes.right->dirty = true;
		}
		emit_signal("dirtied");
	}
}
void RoomConfig::SetRoomIdx(int p_roomIdx) {
	roomIdx = p_roomIdx;
}
void RoomConfig::SetGridPosition(Vector2i p_GridPosition) {
	GridPosition = p_GridPosition;
}
void RoomConfig::SetNodeUp(const Ref<RoomConfig> &p_room) {
	if (p_room != nodes.up) {
		SetDirty(true);
	}
	nodes.up = p_room;
}
void RoomConfig::SetNodeDown(const Ref<RoomConfig> &p_room) {
	if (p_room != nodes.down) {
		SetDirty(true);
	}
	nodes.down = p_room;
}
void RoomConfig::SetNodeLeft(const Ref<RoomConfig> &p_room) {
	if (p_room != nodes.left) {
		SetDirty(true);
	}
	nodes.left = p_room;
}
void RoomConfig::SetNodeRight(const Ref<RoomConfig> &p_room) {
	if (p_room != nodes.right) {
		SetDirty(true);
	}
	nodes.right = p_room;
}
