#ifndef _ITEM_H_
#define _ITEM_H_

#include "cocos2d.h"

USING_NS_CC;

class Item : public Sprite {
protected:
	CC_SYNTHESIZE(char*, m_name, ItemName);
	CC_SYNTHESIZE(char*, m_content, ItemContent);
	CC_SYNTHESIZE(SpriteFrameCache*, m_item_frame_cache, ItemFrameCache);

public:
	/*virtual void work(Character* hunter)
	{
		return;
	}*/
	virtual bool init();
};

class Weapon : public Item {
public:

	static Weapon* create()
	{
		Weapon* weapon = new (std::nothrow) Weapon();
		auto weaponType = random(0, 3);
		if (weapon && weapon->initWithFile(StringUtils::format("images/weapon_%d.png", weaponType))) {
			weapon->autorelease();
			weapon->weaponInit(0.5f + rand_0_1(), 0.5f + rand_0_1(), weaponType, true);
			return weapon;
		}
		CC_SAFE_DELETE(weapon);
		return nullptr;
	}

	CC_SYNTHESIZE(float, m_speed, WeaponSpeed);
	CC_SYNTHESIZE(float, m_attack, WeaponAttack);
	CC_SYNTHESIZE(int, m_type, WeaponType);
	CC_SYNTHESIZE(bool, m_state, WeaponState);

	void weaponInit(float speed, float attack, int type, bool state);
};

class Bandage : public Item {
private:
	CC_SYNTHESIZE(int, m_recover_HP, RecoverHP);
public:
	//virtual void work(Character* hunter);
	virtual bool init();
	CREATE_FUNC(Bandage);

};

class Ammunition : public Item {
private:
	CC_SYNTHESIZE(int, m_add_bullet, AddBullet);
public:
	//virtual void work(Character* hunter);
	virtual bool init();
	CREATE_FUNC(Ammunition);
};


#endif // !_ITEM_H_
