#ifndef HKBEVENTPROPERTY_H_
#define HKBEVENTPROPERTY_H_

#include <memory>
#include "src\hkx\hkbstringeventpayload.h"
#include "src\utilities\eventinfopack.h"

struct eventproperty
{
	EventId id;
	std::shared_ptr<hkbstringeventpayload> payload;

	eventproperty() {}
	eventproperty(int n_id) : id(n_id) {}
};

#endif
