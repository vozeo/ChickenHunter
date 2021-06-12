#pragma once
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "character/Character.h"
#include "StateLayer.h"
#include "Item/Item.h"
#include "Weapon/Bullet.h"
#include <map>
#include <ctime>

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class MapLayer : public cocos2d::Layer
{
private:
    TMXTiledMap* map;
    TMXLayer* meta;

	float mapHeight, mapWidth;
	float tileHeight, tileWidth;

    Character* hunter;

	//add enemies
	
	const int m_enemy_number = 9;
	const int m_weapon_number = 20;
	const int m_bandage_number = 40;
	const int m_ammunition_number = 40;
	clock_t t1 = clock(), t2;

	//add items
	std::vector<Character*> m_enemy;
	std::vector<Weapon*> weapons;
	std::vector<Bandage*> m_bandage;
	std::vector<Ammunition*> m_ammunition;
	
	std::array <Bullet*, 60> bullets;

	CC_SYNTHESIZE(float*, m_volume, Volume);

public:
	static MapLayer* create(Character* gameHunter)
	{
		MapLayer* pRet = new(std::nothrow) MapLayer();
		if (pRet && pRet->init(gameHunter))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
    static cocos2d::Layer* createScene(Character* gameHunter);
    virtual bool init(Character* gameHunter);
    
    virtual void update(float fDelta);
    void registerKeyboardEvent();
	void registerTouchEvent();

	static float calRotation(float bulletX, float bulletY);

	void initBullet();
	void initWeapon();

	void initSetEnemy();
	void initSetItem();

	template <class T> void setRandPos(T* elem);
	template <class T> void initItem(std::vector<T*> &items, int number);

	void judgePick(Character* character);

	void showEffect(Vec2 pos);

	void enemyFire(float delt);
	void Fire(float dt);

	void showEffect(Vec2 pos);
};

#endif
