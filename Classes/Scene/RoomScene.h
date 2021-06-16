#ifndef __ROOM_SCENE_H__
#define __ROOM_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"
#include "Network/Server.h"
#include "Network/Client.h"

USING_NS_CC;

class Room : public cocos2d::Scene
{
    const int m_playerNumber = MAX_CONNECTIONS;
	MenuItemFont* startGame;
	bool started = false;
    std::vector<Label*> playerLabel;
    CC_SYNTHESIZE(float, m_volume, Volume);
public:
    static cocos2d::Scene* createScene(bool isServer);
	virtual bool init(bool isServer);
	virtual void update(float fDelta);
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
			return nullptr;
		}
	}
};

#endif
