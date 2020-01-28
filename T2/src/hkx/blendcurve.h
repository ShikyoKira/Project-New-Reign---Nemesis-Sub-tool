#ifndef BLENDCURVE_H_
#define BLENDCURVE_H_

#include <string>

#include "src/utilities/hkMap.h"

enum blendcurve
{
	BLEND_CURVE_SMOOTH,
	BLEND_CURVE_LINEAR,
	BLEND_CURVE_LINEAR_TO_SMOOTH,
	BLEND_CURVE_SMOOTH_TO_LINEAR
};

std::string getBlendCurve(blendcurve cur);

extern hkMap<std::string, blendcurve> curveMap;

#endif
