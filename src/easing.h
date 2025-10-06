#ifndef EASING_H
#define EASING_H

class Easing {
public:
	// // Core functions
	// static double Remap(double value, double min, double max, double (*easingFunc)(double));
	// static double InverseLerp(double min, double max, double value, double (*easingFunc)(double));
	// static double Lerp(double min, double max, double value, double (*easingFunc)(double));

	// Basic easing functions
	static double Linear(double x);
	static double InQuad(double x);
	static double OutQuad(double x);
	static double InOutQuad(double x);
	static double InCubic(double x);
	static double OutCubic(double x);
	static double InOutCubic(double x);
	static double InQuart(double x);
	static double OutQuart(double x);
	static double InOutQuart(double x);
	static double InQuint(double x);
	static double OutQuint(double x);
	static double InOutQuint(double x);
	static double InExpo(double x);
	static double OutExpo(double x);
	static double InOutExpo(double x);
	static double InBack(double x, double backAmount = 1.70158f);
	static double OutBack(double x, double backAmount = 1.70158f);
	static double InOutBack(double x, double backAmount = 1.70158f, double stabilize = 1.525f);
}; //class Easing

#endif // EASING_H
