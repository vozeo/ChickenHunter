#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "cocos2d.h"

USING_NS_CC;

class Weapon : public Sprite {
public:
	CREATE_FUNC(Weapon);
	virtual bool init();

	CC_SYNTHESIZE(float, m_speed, WeaponSpeed);
	CC_SYNTHESIZE(int, m_attack, WeaponAttack);
	CC_SYNTHESIZE(int, m_state, WeaponState);
	CC_SYNTHESIZE(int, m_type, WeaponType);

	void weaponInit(float speed, int attack, int type, int state);
};
#endif;