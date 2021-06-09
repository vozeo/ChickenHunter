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

void Weapon::weaponInit(float speed, float attack, int type, bool state) {
	m_speed = speed;
	m_attack = attack;
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
	setItemName("bandage");
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

	setItemName("ammunition");
	setAddBullet(random(20, 90));    //add random bullets from 20 to 90

	return true;
}
