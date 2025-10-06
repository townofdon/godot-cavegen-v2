#ifndef EASING_H
#define EASING_H

namespace Easing {
// Core functions
float Remap(float value, float min, float max, float (*easingFunc)(float));
float InverseLerp(float min, float max, float value, float (*easingFunc)(float));
float Lerp(float min, float max, float value, float (*easingFunc)(float));

// Basic easing functions
float Linear(float x);
float InQuad(float x);
float OutQuad(float x);
float InOutQuad(float x);
float InCubic(float x);
float OutCubic(float x);
float InOutCubic(float x);
float InQuart(float x);
float OutQuart(float x);
float InOutQuart(float x);
float InQuint(float x);
float OutQuint(float x);
float InOutQuint(float x);
float InExpo(float x);
float OutExpo(float x);
float InOutExpo(float x);
float InBack(float x, float backAmount = 1.70158f);
float OutBack(float x, float backAmount = 1.70158f);
float InOutBack(float x, float backAmount = 1.70158f, float stabilize = 1.525f);
} //namespace Easing

#endif // EASING_H
