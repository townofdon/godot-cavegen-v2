#include "easing.h"
#include <algorithm>
#include <cmath>

// see: https://easings.net/

// double Easing::Clamp(double value, double min, double max) {
// 	return std::max(min, std::min(max, value));
// }

// double Easing::InverseLerpRaw(double a, double b, double value) {
// 	if (a != b)
// 		return Clamp((value - a) / (b - a), 0.0, 1.0);
// 	else
// 		return 0.0;
// }

// double Easing::LerpRaw(double a, double b, double t) {
// 	return a + (b - a) * t;
// }

// double Easing::Remap(double value, double min, double max, double (*easingFunc)(double)) {
// 	return LerpRaw(min, max, easingFunc(InverseLerpRaw(min, max, value)));
// }

// double Easing::InverseLerp(double min, double max, double value, double (*easingFunc)(double)) {
// 	return easingFunc(InverseLerpRaw(min, max, value));
// }

// double Easing::Lerp(double min, double max, double value, double (*easingFunc)(double)) {
// 	return LerpRaw(min, max, easingFunc(Clamp(value, 0.0, 1.0)));
// }

double Easing::Linear(double x) {
	return x;
}

double Easing::InQuad(double x) {
	return x * x;
}

double Easing::OutQuad(double x) {
	return 1.0 - (1.0 - x) * (1.0 - x);
}

double Easing::InOutQuad(double x) {
	return x < 0.5
			? 2.0 * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 2.0) / 2.0;
}

double Easing::InCubic(double x) {
	return x * x * x;
}

double Easing::OutCubic(double x) {
	return 1.0 - std::pow(1.0 - x, 3.0);
}

double Easing::InOutCubic(double x) {
	return x < 0.5
			? 4.0 * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
}

double Easing::InQuart(double x) {
	return x * x * x * x;
}

double Easing::OutQuart(double x) {
	return 1.0 - std::pow(1.0 - x, 4.0);
}

double Easing::InOutQuart(double x) {
	return x < 0.5
			? 8.0 * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 4.0) / 2.0;
}

double Easing::InQuint(double x) {
	return x * x * x * x * x;
}

double Easing::OutQuint(double x) {
	return 1.0 - std::pow(1.0 - x, 5.0);
}

double Easing::InOutQuint(double x) {
	return x < 0.5
			? 16.0 * x * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 5.0) / 2.0;
}

double Easing::InExpo(double x) {
	return x == 0.0 ? 0.0 : std::pow(2.0, 10.0 * x - 10.0);
}

double Easing::OutExpo(double x) {
	return x == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * x);
}

double Easing::InOutExpo(double x) {
	if (x == 0.0)
		return 0.0;
	if (x == 1.0)
		return 1.0;

	return x < 0.5
			? std::pow(2.0, 20.0 * x - 10.0) / 2.0
			: (2.0 - std::pow(2.0, -20.0 * x + 10.0)) / 2.0;
}

double Easing::InBack(double x, double backAmount) {
	return (backAmount + 1.0) * x * x * x - backAmount * x * x;
}

double Easing::OutBack(double x, double backAmount) {
	return 1.0 + (backAmount + 1.0) * std::pow(x - 1.0, 3.0) + backAmount * std::pow(x - 1.0, 2.0);
}

double Easing::InOutBack(double x, double backAmount, double stabilize) {
	double bs = backAmount * stabilize;
	if (x < 0.5) {
		return (std::pow(2.0 * x, 2.0) * ((bs + 1.0) * 2.0 * x - bs)) / 2.0;
	} else {
		return (std::pow(2.0 * x - 2.0, 2.0) * ((bs + 1.0) * (2.0 * x - 2.0) + bs) + 2.0) / 2.0;
	}
}
