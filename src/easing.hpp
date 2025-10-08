#ifndef EASING_HPP
#define EASING_HPP

#include <algorithm>
#include <cmath>

// https://easings.net/

namespace Easing {

inline constexpr float Linear(float x) {
	return x;
}

inline constexpr float InQuad(float x) {
	return x * x;
}

inline constexpr float OutQuad(float x) {
	return 1.0 - (1.0 - x) * (1.0 - x);
}

inline float InOutQuad(float x) {
	return x < 0.5
		? 2.0 * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 2.0) / 2.0;
}

inline constexpr float InCubic(float x) {
	return x * x * x;
}

inline constexpr float OutCubic(float x) {
	auto x0 = 1.0f - x;
	return 1.0f - (x0 * x0 * x0);
	// return 1.0 - std::pow(1.0 - x, 3.0);
}

inline float InOutCubic(float x) {
	return x < 0.5
		? 4.0 * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
}

inline constexpr float InQuart(float x) {
	return x * x * x * x;
}

inline float OutQuart(float x) {
	return 1.0 - std::pow(1.0 - x, 4.0);
}

inline float InOutQuart(float x) {
	return x < 0.5
		? 8.0 * x * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 4.0) / 2.0;
}

inline constexpr float InQuint(float x) {
	return x * x * x * x * x;
}

inline float OutQuint(float x) {
	return 1.0 - std::pow(1.0 - x, 5.0);
}

inline float InOutQuint(float x) {
	return x < 0.5
		? 16.0 * x * x * x * x * x
		: 1.0 - std::pow(-2.0 * x + 2.0, 5.0) / 2.0;
}

inline constexpr float InExpo(float x) {
	return x == 0.0 ? 0.0 : std::pow(2.0, 10.0 * x - 10.0);
}

inline constexpr float OutExpo(float x) {
	return x == 1.0 ? 1.0 : 1.0 - std::pow(2.0, -10.0 * x);
}

inline float InOutExpo(float x) {
	if (x == 0.0)
		return 0.0;
	if (x == 1.0)
		return 1.0;

	return x < 0.5
		? std::pow(2.0, 20.0 * x - 10.0) / 2.0
		: (2.0 - std::pow(2.0, -20.0 * x + 10.0)) / 2.0;
}

inline constexpr float InBack(float x, float backAmount) {
	return (backAmount + 1.0) * x * x * x - backAmount * x * x;
}

inline float OutBack(float x, float backAmount) {
	return 1.0 + (backAmount + 1.0) * std::pow(x - 1.0, 3.0) + backAmount * std::pow(x - 1.0, 2.0);
}

inline float InOutBack(float x, float backAmount, float stabilize) {
	float bs = backAmount * stabilize;
	if (x < 0.5) {
		return (std::pow(2.0 * x, 2.0) * ((bs + 1.0) * 2.0 * x - bs)) / 2.0;
	} else {
		return (std::pow(2.0 * x - 2.0, 2.0) * ((bs + 1.0) * (2.0 * x - 2.0) + bs) + 2.0) / 2.0;
	}
}

}; //namespace Easing

#endif // EASING_HPP
