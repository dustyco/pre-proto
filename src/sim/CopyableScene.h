#pragma once


#include <stdint.h>


// Structure for representing objects the ClientGame and Net need to
// render or broadcast updates for their player observers (to be combined
// with terrain)
//  -render/physics objects
//  -cameras/microphones
//  -sounds
//  -vguis
class CopyableScene {
public:
	uint64_t id;
	
//	CopyableScene& operator= (const CopyableScene& rhs);
	
private:
	
};
