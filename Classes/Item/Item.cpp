#include "Item/Item.h"

USING_NS_CC;

bool Item::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	m_item_frame_cache = SpriteFrameCache::getInstance();
	m_item_frame_cache->addSpriteFramesWithFile("item.plist", "item.png");

	return true;
}

void Weapon::weaponInit(int type, bool state) {
	//0.5f + rand_0_1(), random(0, 5),
	float sp[4] = { 1.5f,1.0f,0.8f,1.0f };
	m_speed = sp[type];
	float fsp[4] = { 0.15f,0.1f,0.08f,0.1f };
	m_fire_speed = fsp[type];
	float at[4] = { 4.0f,9.0f,12.0f,15.0f };
	m_attack = at[type];
	m_type = type;
	m_state = state;
}

bool Bandage::init()
{
	if (!Item::init())
	{
		return false;
	}

	auto frame = m_item_frame_cache->getSpriteFrameByName("bandage.png");
	this->initWithSpriteFrame(frame);

	char name[] = "bandage";
	setItemName(name);
	setRecoverHP(random(10, 80));    //random recovery from 10 to 80

	return true;
}


bool Ammunition::init()
{
	if (!Item::init())
	{
		return false;
	}

	auto frame = m_item_frame_cache->getSpriteFrameByName("ammunition.png");
	this->initWithSpriteFrame(frame);

	char name[] = "ammunition";
	setItemName(name);
	setAddBullet(random(20, 90));    //add random bullets from 20 to 90

	return true;
}
