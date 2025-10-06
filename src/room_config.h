#pragma once

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
	bool Interpolate;
	bool RemoveOrphans;
	// room border
	bool UseBorderNoise;
	int BorderSize;
	float SmoothBorderNoise;
	int FalloffNearBorder;

	bool GetShowNoise();
	bool GetShowBorder();
	bool GetShowOuterWalls();
	float GetIsoValue();
	float GetCurve();
	float GetTilt();
	float GetFalloffAboveCeiling();
	bool GetInterpolate();
	bool GetRemoveOrphans();
	bool GetUseBorderNoise();
	int GetBorderSize();
	float GetSmoothBorderNoise();
	int GetFalloffNearBorder();

	void SetShowNoise(bool p_ShowNoise);
	void SetShowBorder(bool p_ShowBorder);
	void SetShowOuterWalls(bool p_ShowOuterWalls);
	void SetIsoValue(float p_IsoValue);
	void SetCurve(float p_Curve);
	void SetTilt(float p_Tilt);
	void SetFalloffAboveCeiling(float p_FalloffAboveCeiling);
	void SetInterpolate(bool p_Interpolate);
	void SetRemoveOrphans(bool p_RemoveOrphans);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetBorderSize(int p_BorderSize);
	void SetSmoothBorderNoise(float p_SmoothBorderNoise);
	void SetFalloffNearBorder(int p_FalloffNearBorder);
};
