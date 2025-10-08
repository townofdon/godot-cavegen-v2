#include "room_config.h"

void RoomConfig::_bind_methods() {
	ADD_GROUP("Debug", "debug__");

	ClassDB::bind_method(D_METHOD("get_ShowNoise"), &RoomConfig::GetShowNoise);
	ClassDB::bind_method(D_METHOD("set_ShowNoise", "p_ShowNoise"), &RoomConfig::SetShowNoise);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_noise"), "set_ShowNoise", "get_ShowNoise");

	ClassDB::bind_method(D_METHOD("get_ShowBorder"), &RoomConfig::GetShowBorder);
	ClassDB::bind_method(D_METHOD("set_ShowBorder", "p_ShowBorder"), &RoomConfig::SetShowBorder);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_border"), "set_ShowBorder", "get_ShowBorder");

	ClassDB::bind_method(D_METHOD("get_ShowOuterWalls"), &RoomConfig::GetShowOuterWalls);
	ClassDB::bind_method(D_METHOD("set_ShowOuterWalls", "p_ShowOuterWalls"), &RoomConfig::SetShowOuterWalls);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug__show_outer_walls"), "set_ShowOuterWalls", "get_ShowOuterWalls");

	ADD_GROUP("Room Noise", "room_noise__");

	ClassDB::bind_method(D_METHOD("get_IsoValue"), &RoomConfig::GetIsoValue);
	ClassDB::bind_method(D_METHOD("set_IsoValue", "p_IsoValue"), &RoomConfig::SetIsoValue);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_noise__iso_value", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_IsoValue", "get_IsoValue");

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
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "room_noise__interpolate"), "set_Interpolate", "get_Interpolate");

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

	ClassDB::bind_method(D_METHOD("get_SmoothBorderNoise"), &RoomConfig::GetSmoothBorderNoise);
	ClassDB::bind_method(D_METHOD("set_SmoothBorderNoise", "p_SmoothBorderNoise"), &RoomConfig::SetSmoothBorderNoise);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__smooth_border_noise", PROPERTY_HINT_RANGE, "0,1,0.01"), "set_SmoothBorderNoise", "get_SmoothBorderNoise");

	ClassDB::bind_method(D_METHOD("get_FalloffNearBorder"), &RoomConfig::GetFalloffNearBorder);
	ClassDB::bind_method(D_METHOD("set_FalloffNearBorder", "p_FalloffNearBorder"), &RoomConfig::SetFalloffNearBorder);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "room_border__falloff_near_border", PROPERTY_HINT_RANGE, "0,2,0.01"), "set_FalloffNearBorder", "get_FalloffNearBorder");

	ADD_SIGNAL(MethodInfo("on_changed"));
}

RoomConfig::RoomConfig() {
	// Initialize variables
	ShowNoise = true;
	ShowBorder = true;
	ShowOuterWalls = true;
	IsoValue = 0.5f;
	Curve = 1.0f;
	Tilt = 1.0f;
	FalloffAboveCeiling = 0.5f;
	Interpolate = true;
	RemoveOrphans = true;
	UseBorderNoise = false;
	BorderSize = 1;
	SmoothBorderNoise = 0.5f;
	FalloffNearBorder = 0.2f;
}

RoomConfig::~RoomConfig() {
	// Add your cleanup here.
}

bool RoomConfig::GetShowNoise() {
	return ShowNoise;
}
bool RoomConfig::GetShowBorder() {
	return ShowBorder;
}
bool RoomConfig::GetShowOuterWalls() {
	return ShowOuterWalls;
}
float RoomConfig::GetIsoValue() {
	return IsoValue;
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
bool RoomConfig::GetInterpolate() {
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
float RoomConfig::GetSmoothBorderNoise() {
	return SmoothBorderNoise;
}
float RoomConfig::GetFalloffNearBorder() {
	return FalloffNearBorder;
}

void RoomConfig::SetShowNoise(bool p_ShowNoise) {
	ShowNoise = p_ShowNoise;
	emit_signal("on_changed");
}
void RoomConfig::SetShowBorder(bool p_ShowBorder) {
	ShowBorder = p_ShowBorder;
	emit_signal("on_changed");
}
void RoomConfig::SetShowOuterWalls(bool p_ShowOuterWalls) {
	ShowOuterWalls = p_ShowOuterWalls;
	emit_signal("on_changed");
}
void RoomConfig::SetIsoValue(float p_IsoValue) {
	IsoValue = p_IsoValue;
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
void RoomConfig::SetInterpolate(bool p_Interpolate) {
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
void RoomConfig::SetSmoothBorderNoise(float p_SmoothBorderNoise) {
	SmoothBorderNoise = p_SmoothBorderNoise;
	emit_signal("on_changed");
}
void RoomConfig::SetFalloffNearBorder(float p_FalloffNearBorder) {
	FalloffNearBorder = p_FalloffNearBorder;
	emit_signal("on_changed");
}
