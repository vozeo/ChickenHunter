#include "Item/Item.h"

USING_NS_CC;

int Bandage::number = 5;
int Ammunition::number = 5;

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

bool Bandage::init()
{
	if (!Item::init())
	{
		return false;
	}

	auto frame = m_item_frame_cache->getSpriteFrameByName("bandage.png");
	this->initWithSpriteFrame(frame);
	setItemName("bandage");
	setRecoverHP(10 + rand() % 8 * 10);    //random recovery from 10 to 80

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
	setAddBullet(20 + rand() % 8 * 10);    //add random bullets from 20 to 90

	return true;
}
