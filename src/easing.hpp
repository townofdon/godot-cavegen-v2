#ifndef EASING_HPP
#define EASING_HPP

#include <algorithm>
#include <cmath>

// https://easings.net/

namespace Easing {

inline constexpr double Linear(double x) {
	return x;
}

inline constexpr double InQuad(double x) {
	return x * x;
}

inline constexpr double OutQuad(double x) {
	return 1.0 - (1.0 - x) * (1.0 - x);
}

inline double InOutQuad(double x) {
	return x < 0.5
		? 2.0 * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 2.0) / 2.0;
}

inline constexpr double InCubic(double x) {
	return x * x * x;
}

inline constexpr double OutCubic(double x) {
	return 1.0 - std::pow(1.0 - x, 3.0);
}

inline double InOutCubic(double x) {
	return x < 0.5
		? 4.0 * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
}

inline constexpr double InQuart(double x) {
	return x * x * x * x;
}

inline constexpr double OutQuart(double x) {
	return 1.0 - std::pow(1.0 - x, 4.0);
}

inline double InOutQuart(double x) {
	return x < 0.5
		? 8.0 * x * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 4.0) / 2.0;
}

inline constexpr double InQuint(double x) {
	return x * x * x * x * x;
}

inline constexpr double OutQuint(double x) {
	return 1.0 - std::pow(1.0 - x, 5.0);
}

inline double InOutQuint(double x) {
	return x < 0.5
		? 16.0 * x * x * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 5.0) / 2.0;
}

inline constexpr double InExpo(double x) {
	return x == 0.0 ? 0.0 : std::pow(2.0, 10.0 * x - 10.0);
}

inline constexpr double OutExpo(double x) {
	return x == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * x);
}

inline double InOutExpo(double x) {
	if (x == 0.0)
		return 0.0;
	if (x == 1.0)
		return 1.0;

	return x < 0.5
		? std::pow(2.0, 20.0 * x - 10.0) / 2.0
		: (2.0 - std::pow(2.0, -20.0 * x + 10.0)) / 2.0;
}

inline constexpr double InBack(double x, double backAmount) {
	return (backAmount + 1.0) * x * x * x - backAmount * x * x;
}

inline constexpr double OutBack(double x, double backAmount) {
	return 1.0 + (backAmount + 1.0) * std::pow(x - 1.0, 3.0) + backAmount * std::pow(x - 1.0, 2.0);
}

inline double InOutBack(double x, double backAmount, double stabilize) {
	double bs = backAmount * stabilize;
	if (x < 0.5) {
		return (std::pow(2.0 * x, 2.0) * ((bs + 1.0) * 2.0 * x - bs)) / 2.0;
	} else {
		return (std::pow(2.0 * x - 2.0, 2.0) * ((bs + 1.0) * (2.0 * x - 2.0) + bs) + 2.0) / 2.0;
	}
}

}; //namespace Easing

#endif // EASING_HPP
