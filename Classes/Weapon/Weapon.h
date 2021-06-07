#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "cocos2d.h"

USING_NS_CC;

class Weapon : public Sprite {
public:
    static Weapon* create(const std::string& filename)
    {
        Weapon* sprite = new (std::nothrow) Weapon();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

	CC_SYNTHESIZE(float, m_speed, WeaponSpeed);
	CC_SYNTHESIZE(float, m_attack, WeaponAttack);
	CC_SYNTHESIZE(bool, m_state, WeaponState);
	CC_SYNTHESIZE(int, m_type, WeaponType);

	void weaponInit(float speed, float attack, int type, int state);
};
#endif;