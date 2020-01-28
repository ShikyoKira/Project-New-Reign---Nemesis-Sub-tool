#include "blendcurve.h"

using namespace std;

hkMap<string, blendcurve> curveMap =
{
	{ "BLEND_CURVE_SMOOTH", blendcurve::BLEND_CURVE_SMOOTH },
	{ "BLEND_CURVE_LINEAR", blendcurve::BLEND_CURVE_LINEAR },
	{ "BLEND_CURVE_LINEAR_TO_SMOOTH", blendcurve::BLEND_CURVE_LINEAR_TO_SMOOTH },
	{ "BLEND_CURVE_SMOOTH_TO_LINEAR", blendcurve::BLEND_CURVE_SMOOTH_TO_LINEAR },
};

string getBlendCurve(blendcurve cur)
{
	return curveMap[cur];
}
