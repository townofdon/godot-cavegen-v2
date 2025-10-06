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
	double IsoValue;
	double Curve;
	double Tilt;
	double FalloffAboveCeiling;
	bool Interpolate;
	bool RemoveOrphans;
	// room border
	bool UseBorderNoise;
	int BorderSize;
	double SmoothBorderNoise;
	int FalloffNearBorder;

	bool GetShowNoise();
	bool GetShowBorder();
	bool GetShowOuterWalls();
	double GetIsoValue();
	double GetCurve();
	double GetTilt();
	double GetFalloffAboveCeiling();
	bool GetInterpolate();
	bool GetRemoveOrphans();
	bool GetUseBorderNoise();
	int GetBorderSize();
	double GetSmoothBorderNoise();
	int GetFalloffNearBorder();

	void SetShowNoise(bool p_ShowNoise);
	void SetShowBorder(bool p_ShowBorder);
	void SetShowOuterWalls(bool p_ShowOuterWalls);
	void SetIsoValue(double p_IsoValue);
	void SetCurve(double p_Curve);
	void SetTilt(double p_Tilt);
	void SetFalloffAboveCeiling(double p_FalloffAboveCeiling);
	void SetInterpolate(bool p_Interpolate);
	void SetRemoveOrphans(bool p_RemoveOrphans);
	void SetUseBorderNoise(bool p_UseBorderNoise);
	void SetBorderSize(int p_BorderSize);
	void SetSmoothBorderNoise(double p_SmoothBorderNoise);
	void SetFalloffNearBorder(int p_FalloffNearBorder);
};
