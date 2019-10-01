#include "blendcurve.h"

using namespace std;

string getBlendCurve(blendcurve cur)
{
	switch (cur)
	{
	case BLEND_CURVE_SMOOTH: return "BLEND_CURVE_SMOOTH";
	case BLEND_CURVE_LINEAR: return "BLEND_CURVE_LINEAR";
	case BLEND_CURVE_LINEAR_TO_SMOOTH: return "BLEND_CURVE_LINEAR_TO_SMOOTH";
	case BLEND_CURVE_SMOOTH_TO_LINEAR: return "BLEND_CURVE_SMOOTH_TO_LINEAR";
	default: return "BLEND_CURVE_SMOOTH";
	}
}
