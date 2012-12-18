#pragma once


#include "EntID.h"


class ComponentPhysical {
public:
	void addMovableSolid (EntID id);
	void addMovableGhost (EntID id);
	void addStaticSolid  (EntID id);
	void addStaticGhost  (EntID id);
};
