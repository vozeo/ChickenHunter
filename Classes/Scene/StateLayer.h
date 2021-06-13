#pragma once
#ifndef __STATE_SCENE_H__
#define __STATE_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "MapLayer.h"

#include <map>
#include <chrono>
using namespace std::chrono;

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class State : public cocos2d::Layer
{
private:
    ui::LoadingBar* blood_bar;
    Label* blood_label;
    Label* survivor_label;
	Label* time_label;
	Label* bullet_label;

    Character* hunter;

	Vector<MenuItem*> guns;
    MenuItem* gun[5][2];

	Menu* gunMenu;

	system_clock::time_point startTime;
	 
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

	virtual void update(float fDelta);

	int getTime();

};

#endif
