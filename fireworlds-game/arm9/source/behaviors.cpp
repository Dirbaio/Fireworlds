#include "behaviors.h"

bool isBehaviorSolid(int beh)
{
	return beh < BEH_LAVA || beh > BEH_WATERDOWN;
}
