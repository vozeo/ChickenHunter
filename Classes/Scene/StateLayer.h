#ifndef __STATE_SCENE_H__
#define __STATE_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "character/Character.h"
#include "MapLayer.h"
#include <map>

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class State : public cocos2d::Layer
{
private:
    ui::LoadingBar* blood_bar;
    Label* blood_label;
    Label* survivor_label;

    Character* hunter;

    MenuItem* gun[4][2];

public:
	static State* create(Character* gameHunter)
	{
		State* pRet = new(std::nothrow) State();
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
    virtual void initState();
    virtual void initGun();
    void updateHunterInfo();
    void menuCloseCallback(cocos2d::Ref* pSender);
};

#endif
