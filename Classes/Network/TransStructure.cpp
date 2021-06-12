#include "TransStructure.h"
#include <cstring>

void PlayerInformation::clear()
{
	/*
	uid = 0;
	hp = 0;
	m_position_x = 0, m_position_y = 0;
	*/
	memset(this, 0, sizeof(PlayerInformation));
}
