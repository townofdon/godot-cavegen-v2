#include "global_config.h"

void GlobalConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_RoomWidth"), &GlobalConfig::GetRoomWidth);
	ClassDB::bind_method(D_METHOD("set_RoomWidth", "p_RoomWidth"), &GlobalConfig::SetRoomWidth);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_width", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomWidth", "get_RoomWidth");

	ClassDB::bind_method(D_METHOD("get_RoomHeight"), &GlobalConfig::GetRoomHeight);
	ClassDB::bind_method(D_METHOD("set_RoomHeight", "p_RoomHeight"), &GlobalConfig::SetRoomHeight);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_height", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomHeight", "get_RoomHeight");

	ClassDB::bind_method(D_METHOD("get_RoomDepth"), &GlobalConfig::GetRoomDepth);
	ClassDB::bind_method(D_METHOD("set_RoomDepth", "p_RoomDepth"), &GlobalConfig::SetRoomDepth);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_depth", PROPERTY_HINT_RANGE, "5,50,"), "set_RoomDepth", "get_RoomDepth");

	ClassDB::bind_method(D_METHOD("get_CellSize"), &GlobalConfig::GetCellSize);
	ClassDB::bind_method(D_METHOD("set_CellSize", "p_CellSize"), &GlobalConfig::SetCellSize);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cell_size", PROPERTY_HINT_RANGE, "0.25,5.0,0.25"), "set_CellSize", "get_CellSize");

	ClassDB::bind_method(D_METHOD("get_Ceiling"), &GlobalConfig::GetCeiling);
	ClassDB::bind_method(D_METHOD("set_Ceiling", "p_Ceiling"), &GlobalConfig::SetCeiling);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "ceiling", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_Ceiling", "get_Ceiling");

	ClassDB::bind_method(D_METHOD("get_ActivePlaneOffset"), &GlobalConfig::GetActivePlaneOffset);
	ClassDB::bind_method(D_METHOD("set_ActivePlaneOffset", "p_ActivePlaneOffset"), &GlobalConfig::SetActivePlaneOffset);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "active_plane_offset", PROPERTY_HINT_RANGE, "0,50,0.01"), "set_ActivePlaneOffset", "get_ActivePlaneOffset");

	ADD_SIGNAL(MethodInfo("on_changed"));
	// ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
}

GlobalConfig::GlobalConfig() {
	// Initialize any variables here.
	RoomWidth = 30.0;
	RoomHeight = 30.0;
	RoomDepth = 30.0;
	CellSize = 1.0;
	Ceiling = 0.75;
	ActivePlaneOffset = 10;
}

GlobalConfig::~GlobalConfig() {
	// Add your cleanup here.
}

double GlobalConfig::GetRoomWidth() {
	return RoomWidth;
}

void GlobalConfig::SetRoomWidth(double p_RoomWidth) {
	RoomWidth = p_RoomWidth;
	emit_signal("on_changed");
}

double GlobalConfig::GetRoomHeight() {
	return RoomHeight;
}

void GlobalConfig::SetRoomHeight(double p_RoomHeight) {
	RoomHeight = p_RoomHeight;
	emit_signal("on_changed");
}

double GlobalConfig::GetRoomDepth() {
	return RoomDepth;
}

void GlobalConfig::SetRoomDepth(double p_RoomDepth) {
	RoomDepth = p_RoomDepth;
	emit_signal("on_changed");
}

double GlobalConfig::GetCellSize() {
	return CellSize;
}

void GlobalConfig::SetCellSize(double p_CellSize) {
	CellSize = p_CellSize;
	emit_signal("on_changed");
}

double GlobalConfig::GetCeiling() {
	return Ceiling;
}

void GlobalConfig::SetCeiling(double p_Ceiling) {
	Ceiling = p_Ceiling;
	emit_signal("on_changed");
}

double GlobalConfig::GetActivePlaneOffset() {
	return ActivePlaneOffset;
}

void GlobalConfig::SetActivePlaneOffset(double p_ActivePlaneOffset) {
	ActivePlaneOffset = p_ActivePlaneOffset;
	emit_signal("on_changed");
}
