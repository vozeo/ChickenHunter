#ifndef __ROOM_SCENE_H__
#define __ROOM_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class Room : public cocos2d::Scene
{
    const int m_playerNumber = 10;
    std::vector<Label*> playerLabel;
    CC_SYNTHESIZE(float, m_volume, Volume);
public:
    static cocos2d::Scene* createScene(bool isServer);
	virtual bool init(bool isServer);
	static Room* create(bool isServer)
	{
		Room* pRet = new(std::nothrow) Room();
		if (pRet && pRet->init(isServer))
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
};

#endif
