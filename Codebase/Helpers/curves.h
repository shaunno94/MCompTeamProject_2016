#pragma once

/** @ingroup Helpers
*  @{
*/

////http://www.cplusplus.com/forum/beginner/63025/
////http://people.sc.fsu.edu/~jburkardt/cpp_src/hermite_cubic/hermite_cubic.html
////https://mrl.nyu.edu/~perlin/courses/spring2008/splines1.html
static float bezierCurve(float start, float control, float end, float progress)
{
	return ((1 - progress) * (1 - progress) * start) +
	       (2 * (1 - progress) * progress * control) +
	       (progress * progress * end);
}

static float bezierCurve(float start, float control1, float control2, float end, float progress)
{
	float oneMinusProgress = 1.0f - progress;
	float progressPow2 = progress * progress;
	float oneMinusProgressPow2 = oneMinusProgress * oneMinusProgress;

	return ((oneMinusProgressPow2 * oneMinusProgress) * start) +
	       ((3.0f * oneMinusProgressPow2 * progress) * control1) +
	       ((3.0f * oneMinusProgress * progressPow2) * control2) +
	       ((progressPow2 * progress) * end);
};

static float bezierCurveDerivative(float start, float control1, float end, float progress)
{
	return (2 * (1 - progress) * (control1 - start)) +
	       (2 * progress * (end - control1));
};

static float bezierCurveDerivative(float start, float control1, float control2, float end, float progress)
{
	return (3 * (1 - progress) * (1 - progress) * (control1 - start)) +
	       (6 * (1 - progress) * progress * (control2 - control1)) +
	       (3 * progress * progress * (end - control2));
};


static float hermiteCurve(float start, float control1, float control2, float end, float progress)
{
	float progressPow2 = progress * progress;
	float progressPow3MinusPow2 = progressPow2 * progress - progressPow2;

	return ((progressPow3MinusPow2 - progress + 1.0f) * start) +
	       ((progressPow3MinusPow2 - progressPow2 + progress) * control1) +
	       (progressPow3MinusPow2 * control2) +
	       ((-3.0f * progressPow3MinusPow2 + progressPow2) * end);
}

static float hermiteCurveUsingTangents(float start, float tangent1, float tangent2, float end, float progress)
{
	float progressPow2 = progress * progress;
	float progressPow3MinusPow2 = (progressPow2 * progress) - progressPow2;

	return start * (2.0f * progressPow3MinusPow2 - progressPow2 + 1) +
	       tangent1 * (progressPow3MinusPow2 - progressPow2 + progress) +
	       tangent2 * (progressPow3MinusPow2)+
	       end * (-2.0f * progressPow3MinusPow2 + progressPow2);
}

static float hermiteCurveDerivativeUsingTangents(float start, float tangent1, float tangent2, float end, float progress)
{
	float threeProgressPow2 = 3.0f * (progress * progress);

	return start * (2.0f * threeProgressPow2 - (6 * progress)) +
	       tangent1 * (threeProgressPow2 - (4 * progress) + 1) +
	       tangent2 * (threeProgressPow2 - (2 * progress)) +
	       end * (-2.0f * threeProgressPow2 + (6 * progress));
}


/** @} */