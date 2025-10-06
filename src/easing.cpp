#include "easing.h"
#include <algorithm>
#include <cmath>

// see: https://easings.net/

namespace Easing {
static double Clamp(double value, double min, double max) {
	return std::max(min, std::min(max, value));
}

static double InverseLerpRaw(double a, double b, double value) {
	if (a != b)
		return Clamp((value - a) / (b - a), 0.0, 1.0);
	else
		return 0.0;
}

static double LerpRaw(double a, double b, double t) {
	return a + (b - a) * t;
}

double Remap(double value, double min, double max, double (*easingFunc)(double)) {
	return LerpRaw(min, max, easingFunc(InverseLerpRaw(min, max, value)));
}

double InverseLerp(double min, double max, double value, double (*easingFunc)(double)) {
	return easingFunc(InverseLerpRaw(min, max, value));
}

double Lerp(double min, double max, double value, double (*easingFunc)(double)) {
	return LerpRaw(min, max, easingFunc(Clamp(value, 0.0, 1.0)));
}

double Linear(double x) {
	return x;
}

double InQuad(double x) {
	return x * x;
}

double OutQuad(double x) {
	return 1.0 - (1.0 - x) * (1.0 - x);
}

double InOutQuad(double x) {
	return x < 0.5
			? 2.0 * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 2.0) / 2.0;
}

double InCubic(double x) {
	return x * x * x;
}

double OutCubic(double x) {
	return 1.0 - std::pow(1.0 - x, 3.0);
}

double InOutCubic(double x) {
	return x < 0.5
			? 4.0 * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
}

double InQuart(double x) {
	return x * x * x * x;
}

double OutQuart(double x) {
	return 1.0 - std::pow(1.0 - x, 4.0);
}

double InOutQuart(double x) {
	return x < 0.5
			? 8.0 * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 4.0) / 2.0;
}

double InQuint(double x) {
	return x * x * x * x * x;
}

double OutQuint(double x) {
	return 1.0 - std::pow(1.0 - x, 5.0);
}

double InOutQuint(double x) {
	return x < 0.5
			? 16.0 * x * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 5.0) / 2.0;
}

double InExpo(double x) {
	return x == 0.0 ? 0.0 : std::pow(2.0, 10.0 * x - 10.0);
}

double OutExpo(double x) {
	return x == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * x);
}

double InOutExpo(double x) {
	if (x == 0.0)
		return 0.0;
	if (x == 1.0)
		return 1.0;

	return x < 0.5
			? std::pow(2.0, 20.0 * x - 10.0) / 2.0
			: (2.0 - std::pow(2.0, -20.0 * x + 10.0)) / 2.0;
}

double InBack(double x, double backAmount) {
	return (backAmount + 1.0) * x * x * x - backAmount * x * x;
}

double OutBack(double x, double backAmount) {
	return 1.0 + (backAmount + 1.0) * std::pow(x - 1.0, 3.0) + backAmount * std::pow(x - 1.0, 2.0);
}

double InOutBack(double x, double backAmount, double stabilize) {
	double bs = backAmount * stabilize;
	if (x < 0.5) {
		return (std::pow(2.0 * x, 2.0) * ((bs + 1.0) * 2.0 * x - bs)) / 2.0;
	} else {
		return (std::pow(2.0 * x - 2.0, 2.0) * ((bs + 1.0) * (2.0 * x - 2.0) + bs) + 2.0) / 2.0;
	}
}
} //namespace Easing
