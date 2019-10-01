#ifndef GENERATORS_H_
#define GENERATORS_H_

#include "hkbbehaviorgraph.h"
#include "hkbmodifiergenerator.h"
#include "hkbmanualselectorgenerator.h"
#include "hkbbehaviorreferencegenerator.h"

#include "statemachine\hkbstatemachine.h"
#include "statemachine\hkbstatemachinestateinfo.h"
#include "statemachine\hkbstatemachineeventpropertyarray.h"
#include "statemachine\hkbstatemachinetransitioninfoarray.h"

#include "blender\hkbblendergenerator.h"
#include "blender\hkbblendergeneratorchild.h"
#include "blender\hkbposematchinggenerator.h"

#include "clip\hkbclipgenerator.h"
#include "clip\hkbcliptriggerarray.h"

#include "bs\bsistatetagginggenerator.h"
#include "bs\bsoffsetanimationgenerator.h"
#include "bs\bssynchronizedclipgenerator.h"
#include "bs\bscyclicblendtransitiongenerator.h"

#include "bs\bone\bsboneswitchgenerator.h"
#include "bs\bone\bsboneswitchgeneratorbonedata.h"

#endif
