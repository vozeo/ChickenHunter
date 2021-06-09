#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "Weapon/Bullet.h"
#include "Weapon/Weapon.h"
#include "MapLayer.h"
#include "StateLayer.h"
#include <map>

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class MapLayer : public cocos2d::Layer
{
private:
    TMXTiledMap* map;
    TMXLayer* meta;

	float mapHeight, mapWidth;

    Character* hunter;

    std::map<EventKeyboard::KeyCode, bool> keyMap;

	std::array <Bullet*, 30> bullets;
	std::array <Weapon*, 20> weapons;

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

	void initBullet();
	void initWeapon();
    
    virtual void update(float fDelta);
    void registerKeyboardEvent();
	void registerTouchEvent();

	static float calRotation(float bulletX, float bulletY);

	void showEffect(Vec2 pos);
};

#endif
