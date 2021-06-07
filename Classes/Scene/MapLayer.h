#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "character/Character.h"
#include "StateLayer.h"
#include "Item/Item.h"
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
	std::vector< Character*> m_enemy;      
	static int m_enemy_number;

	//add items
	std::vector< Bandage*> m_bandage;
	std::vector< Ammunition*> m_ammunition;

    std::map<EventKeyboard::KeyCode, bool> keyMap;

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

	void initSetEnemy();
	template <class T> void setRandPos(T ele);

	void initSetItem();



};

#endif
