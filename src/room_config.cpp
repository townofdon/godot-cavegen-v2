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
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__iso_value", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_IsoValue", "get_IsoValue");

	ClassDB::bind_method(D_METHOD("get_NoiseFloor"), &RoomConfig::GetNoiseFloor);
	ClassDB::bind_method(D_METHOD("set_NoiseFloor", "p_NoiseFloor"), &RoomConfig::SetNoiseFloor);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__noise_floor", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_NoiseFloor", "get_NoiseFloor");

	ClassDB::bind_method(D_METHOD("get_NoiseCeil"), &RoomConfig::GetNoiseCeil);
	ClassDB::bind_method(D_METHOD("set_NoiseCeil", "p_NoiseCeil"), &RoomConfig::SetNoiseCeil);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__noise_ceil", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_NoiseCeil", "get_NoiseCeil");

	ClassDB::bind_method(D_METHOD("get_Curve"), &RoomConfig::GetCurve);
	ClassDB::bind_method(D_METHOD("set_Curve", "p_Curve"), &RoomConfig::SetCurve);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__curve", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_Curve", "get_Curve");

	ClassDB::bind_method(D_METHOD("get_Tilt"), &RoomConfig::GetTilt);
	ClassDB::bind_method(D_METHOD("set_Tilt", "p_Tilt"), &RoomConfig::SetTilt);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__tilt", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_Tilt", "get_Tilt");

	ClassDB::bind_method(D_METHOD("get_FalloffAboveCeiling"), &RoomConfig::GetFalloffAboveCeiling);
	ClassDB::bind_method(D_METHOD("set_FalloffAboveCeiling", "p_FalloffAboveCeiling"), &RoomConfig::SetFalloffAboveCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__falloff_above_ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_FalloffAboveCeiling", "get_FalloffAboveCeiling");

	ClassDB::bind_method(D_METHOD("get_Interpolate"), &RoomConfig::GetInterpolate);
	ClassDB::bind_method(D_METHOD("set_Interpolate", "p_Interpolate"), &RoomConfig::SetInterpolate);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__interpolate", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_Interpolate", "get_Interpolate");

	ClassDB::bind_method(D_METHOD("get_ActiveYSmoothing"), &RoomConfig::GetActiveYSmoothing);
	ClassDB::bind_method(D_METHOD("set_ActiveYSmoothing", "p_ActiveYSmoothing"), &RoomConfig::SetActiveYSmoothing);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__active_y_smoothing", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_ActiveYSmoothing", "get_ActiveYSmoothing");

	ClassDB::bind_method(D_METHOD("get_RemoveOrphans"), &RoomConfig::GetRemoveOrphans);
	ClassDB::bind_method(D_METHOD("set_RemoveOrphans", "p_RemoveOrphans"), &RoomConfig::SetRemoveOrphans);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_noise__remove_orphans"), "set_RemoveOrphans", "get_RemoveOrphans");

	ADD_GROUP("Room Border", "room_border__");

	ClassDB::bind_method(D_METHOD("get_UseBorderNoise"), &RoomConfig::GetUseBorderNoise);
	ClassDB::bind_method(D_METHOD("set_UseBorderNoise", "p_UseBorderNoise"), &RoomConfig::SetUseBorderNoise);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_border__use_border_noise"), "set_UseBorderNoise", "get_UseBorderNoise");

	ClassDB::bind_method(D_METHOD("get_BorderSize"), &RoomConfig::GetBorderSize);
	ClassDB::bind_method(D_METHOD("set_BorderSize", "p_BorderSize"), &RoomConfig::SetBorderSize);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "room_border__border_size", PROPERTY_HINT_RANGE, "0,10,"), "set_BorderSize", "get_BorderSize");

	ClassDB::bind_method(D_METHOD("get_BorderNoiseIsoValue"), &RoomConfig::GetBorderNoiseIsoValue);
	ClassDB::bind_method(D_METHOD("set_BorderNoiseIsoValue", "p_BorderNoiseIsoValue"), &RoomConfig::SetBorderNoiseIsoValue);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__border_noise_strength", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_BorderNoiseIsoValue", "get_BorderNoiseIsoValue");

	ClassDB::bind_method(D_METHOD("get_SmoothBorderNoise"), &RoomConfig::GetSmoothBorderNoise);
	ClassDB::bind_method(D_METHOD("set_SmoothBorderNoise", "p_SmoothBorderNoise"), &RoomConfig::SetSmoothBorderNoise);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__smooth_border_noise", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_SmoothBorderNoise", "get_SmoothBorderNoise");

	ClassDB::bind_method(D_METHOD("get_FalloffNearBorder"), &RoomConfig::GetFalloffNearBorder);
	ClassDB::bind_method(D_METHOD("set_FalloffNearBorder", "p_FalloffNearBorder"), &RoomConfig::SetFalloffNearBorder);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__falloff_near_border", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_FalloffNearBorder", "get_FalloffNearBorder");

	ClassDB::bind_method(D_METHOD("get_BorderGapSpread"), &RoomConfig::GetBorderGapSpread);
	ClassDB::bind_method(D_METHOD("set_BorderGapSpread", "p_BorderGapSpread"), &RoomConfig::SetBorderGapSpread);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "room_border__border_gap_spread", PROPERTY_HINT_RANGE, "0,10,"), "set_BorderGapSpread", "get_BorderGapSpread");

	ADD_GROUP("Tile Apply", "tile_apply__");

	ClassDB::bind_method(D_METHOD("get_TileStrength"), &RoomConfig::GetTileStrength);
	ClassDB::bind_method(D_METHOD("set_TileStrength", "p_TileStrength"), &RoomConfig::SetTileStrength);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_strength", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_TileStrength", "get_TileStrength");

	ClassDB::bind_method(D_METHOD("get_TileSmoothing"), &RoomConfig::GetTileSmoothing);
	ClassDB::bind_method(D_METHOD("set_TileSmoothing", "p_TileSmoothing"), &RoomConfig::SetTileSmoothing);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_smoothing", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileSmoothing", "get_TileSmoothing");

	ClassDB::bind_method(D_METHOD("get_TileFloor"), &RoomConfig::GetTileFloor);
	ClassDB::bind_method(D_METHOD("set_TileFloor", "p_TileFloor"), &RoomConfig::SetTileFloor);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_floor", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileFloor", "get_TileFloor");

	ClassDB::bind_method(D_METHOD("get_TileFloorFalloff"), &RoomConfig::GetTileFloorFalloff);
	ClassDB::bind_method(D_METHOD("set_TileFloorFalloff", "p_TileFloorFalloff"), &RoomConfig::SetTileFloorFalloff);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_floor_falloff", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileFloorFalloff", "get_TileFloorFalloff");

	ClassDB::bind_method(D_METHOD("get_TileCeiling"), &RoomConfig::GetTileCeiling);
	ClassDB::bind_method(D_METHOD("set_TileCeiling", "p_TileCeiling"), &RoomConfig::SetTileCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileCeiling", "get_TileCeiling");

	ClassDB::bind_method(D_METHOD("get_TileCeilingFalloff"), &RoomConfig::GetTileCeilingFalloff);
	ClassDB::bind_method(D_METHOD("set_TileCeilingFalloff", "p_TileCeilingFalloff"), &RoomConfig::SetTileCeilingFalloff);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tile_apply__tile_ceil_falloff", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_TileCeilingFalloff", "get_TileCeilingFalloff");

	ADD_GROUP("Neighbors", "neighbors__");

	ClassDB::bind_method(D_METHOD("get_NeighborBlend"), &RoomConfig::GetNeighborBlend);
	ClassDB::bind_method(D_METHOD("set_NeighborBlend", "p_NeighborBlend"), &RoomConfig::SetNeighborBlend);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "neighbors__neighbor_blend", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_NeighborBlend", "get_NeighborBlend");

	ADD_GROUP("Internal", "internal__");

	ClassDB::bind_method(D_METHOD("get_Precedence"), &RoomConfig::GetPrecedence);
	ClassDB::bind_method(D_METHOD("set_Precedence", "p_Precedence"), &RoomConfig::SetPrecedence);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "internal__precedence"), "set_Precedence", "get_Precedence");

	ClassDB::bind_method(D_METHOD("get_NodeUp"), &RoomConfig::GetNodeUp);
	ClassDB::bind_method(D_METHOD("set_NodeUp", "p_room"), &RoomConfig::SetNodeUp);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "internal__node_up", PROPERTY_HINT_RESOURCE_TYPE, "RoomConfig"), "set_NodeUp", "get_NodeUp");

	ClassDB::bind_method(D_METHOD("get_NodeDown"), &RoomConfig::GetNodeDown);
	ClassDB::bind_method(D_METHOD("set_NodeDown", "p_room"), &RoomConfig::SetNodeDown);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "internal__node_down", PROPERTY_HINT_RESOURCE_TYPE, "RoomConfig"), "set_NodeDown", "get_NodeDown");

	ClassDB::bind_method(D_METHOD("get_NodeLeft"), &RoomConfig::GetNodeLeft);
	ClassDB::bind_method(D_METHOD("set_NodeLeft", "p_room"), &RoomConfig::SetNodeLeft);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "internal__node_left", PROPERTY_HINT_RESOURCE_TYPE, "RoomConfig"), "set_NodeLeft", "get_NodeLeft");

	ClassDB::bind_method(D_METHOD("get_NodeRight"), &RoomConfig::GetNodeRight);
	ClassDB::bind_method(D_METHOD("set_NodeRight", "p_room"), &RoomConfig::SetNodeRight);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "internal__node_right", PROPERTY_HINT_RESOURCE_TYPE, "RoomConfig"), "set_NodeRight", "get_NodeRight");

	ADD_SIGNAL(MethodInfo("on_changed"));

	ClassDB::bind_method(D_METHOD("notify_changed"), &RoomConfig::NotifyChanged);

	//
	// Tilemap Data
	//
	ClassDB::bind_method(D_METHOD("set_tile", "num_cells", "coords", "tile"), &RoomConfig::SetTile);
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
	Tilt = 1.0f;
	FalloffAboveCeiling = 0.5f;
	Interpolate = 1.0f;
	ActiveYSmoothing = 0.5f;
	RemoveOrphans = true;
	UseBorderNoise = false;
	BorderSize = 1;
	BorderNoiseIsoValue = 0.5f;
	SmoothBorderNoise = 0.5f;
	FalloffNearBorder = 0.2f;
	BorderGapSpread = 2;
	TileStrength = 1.0f;
	TileSmoothing = 0.0f;
	TileFloor = 0.4f;
	TileCeiling = 0.6f;
	TileFloorFalloff = 0.2f;
	TileCeilingFalloff = 0.2f;
	NeighborBlend = 0.25f;
	// initialize tiles
	for (size_t i = 0; i < MAX_NOISE_NODES_2D; i++) {
		tiles[i] = 0;
	}
	// initialize internal vars
	precedence = 0;
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
float RoomConfig::GetTilt() {
	return Tilt;
}
float RoomConfig::GetFalloffAboveCeiling() {
	return FalloffAboveCeiling;
}
float RoomConfig::GetActiveYSmoothing() {
	return ActiveYSmoothing;
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
int RoomConfig::GetBorderSize() {
	return BorderSize;
}
float RoomConfig::GetBorderNoiseIsoValue() {
	return BorderNoiseIsoValue;
}
float RoomConfig::GetSmoothBorderNoise() {
	return SmoothBorderNoise;
}
float RoomConfig::GetFalloffNearBorder() {
	return FalloffNearBorder;
}
float RoomConfig::GetBorderGapSpread() {
	return BorderGapSpread;
}
float RoomConfig::GetTileStrength() {
	return TileStrength;
}
float RoomConfig::GetTileSmoothing() {
	return TileSmoothing;
}
float RoomConfig::GetTileFloor() {
	return TileFloor;
}
float RoomConfig::GetTileFloorFalloff() {
	return TileFloorFalloff;
}
float RoomConfig::GetTileCeiling() {
	return TileCeiling;
}
float RoomConfig::GetTileCeilingFalloff() {
	return TileCeilingFalloff;
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
void RoomConfig::SetTilt(float p_Tilt) {
	Tilt = p_Tilt;
	emit_signal("on_changed");
}
void RoomConfig::SetFalloffAboveCeiling(float p_FalloffAboveCeiling) {
	FalloffAboveCeiling = p_FalloffAboveCeiling;
	emit_signal("on_changed");
}
void RoomConfig::SetActiveYSmoothing(float p_ActiveYSmoothing) {
	ActiveYSmoothing = p_ActiveYSmoothing;
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
void RoomConfig::SetFalloffNearBorder(float p_FalloffNearBorder) {
	FalloffNearBorder = p_FalloffNearBorder;
	emit_signal("on_changed");
}
void RoomConfig::SetBorderGapSpread(float p_BorderGapSpread) {
	BorderGapSpread = p_BorderGapSpread;
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
void RoomConfig::SetTileFloor(float p_TileFloor) {
	TileFloor = p_TileFloor;
	emit_signal("on_changed");
}
void RoomConfig::SetTileFloorFalloff(float p_TileFloorFalloff) {
	TileFloorFalloff = p_TileFloorFalloff;
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
void RoomConfig::SetTile(Vector2i numCells2d, Vector2i coords, int tile) {
	int x = coords.x;
	int y = coords.y;
	int i = x + y * numCells2d.x;

	// // debug
	// if (i < 0 || i >= (MAX_NOISE_NODES_2D)) {
	// 	auto ax = String(std::to_string(x).c_str());
	// 	auto ay = String(std::to_string(y).c_str());
	// 	auto bx = String(std::to_string(numCells2d.x).c_str());
	// 	auto by = String(std::to_string(numCells2d.y).c_str());
	// 	UtilityFunctions::printerr("tile index out of bounds: (" + ax + "," + ay + "), numCells2d((" + bx + "," + by + ")");
	// 	return;
	// }
	// if (tile < 0 || tile > RoomConfig::TileState::TILE_STATE_FILLED) {
	// 	auto d_tile = String(std::to_string(tile).c_str());
	// 	UtilityFunctions::printerr("invalid tile: " + d_tile);
	// 	return;
	// }
	// auto dtile = String(std::to_string(tile).c_str());
	// auto di = String(std::to_string(i).c_str());
	// auto dx = String(std::to_string(x).c_str());
	// auto dy = String(std::to_string(y).c_str());
	// UtilityFunctions::print("set tile " + dtile + " at index " + di + "(" + dx + "," + dy + ")");

	ERR_FAIL_INDEX_EDMSG(i, MAX_NOISE_NODES_2D, "tile index out of bounds");
	ERR_FAIL_INDEX_EDMSG(tile, RoomConfig::TileState::_TILE_STATE_MAX_, "invalid tile");

	tiles[i] = tile;
}
int *RoomConfig::GetTiles() {
	return tiles;
}

//
// INTERNAL
//
int RoomConfig::GetPrecedence() {
	return precedence;
}
void RoomConfig::SetPrecedence(int p_precedence) {
	precedence = p_precedence;
}
bool RoomConfig::ValidateSetNode(const Ref<RoomConfig> &p_room, Ref<RoomConfig> &compare) {
	if (!p_room.is_valid()) {
		return true;
	}
	if (!compare.is_valid()) {
		return true;
	}
	String name = p_room->get_name();
	if (name.is_empty()) {
		name = p_room->get_path();
	}
	ERR_FAIL_COND_V_EDMSG(p_room->get_rid() == compare->get_rid(), false, "node already in use: " + name);
	return true;
}
void RoomConfig::SetNodeUp(const Ref<RoomConfig> &p_room) {
	nodes.up = p_room;
}
void RoomConfig::SetNodeDown(const Ref<RoomConfig> &p_room) {
	nodes.down = p_room;
}
void RoomConfig::SetNodeLeft(const Ref<RoomConfig> &p_room) {
	nodes.left = p_room;
}
void RoomConfig::SetNodeRight(const Ref<RoomConfig> &p_room) {
	nodes.right = p_room;
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
