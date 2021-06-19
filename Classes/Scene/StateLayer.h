#pragma once
#ifndef __STATE_SCENE_H__
#define __STATE_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "ChatLayer.h"
#include "MapLayer.h"
#include "Network/Client.h"
#include "Joystick/Joystick.h"

#include <map>
#include <chrono>
#include <utility>

using namespace std::chrono;

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class State : public cocos2d::Layer {
private:
    ui::LoadingBar *blood_bar;
    Label *blood_label;
    Label *survivor_label;
    Label *time_label;
    Label *bullet_label;
    Label *grenade_label;

    ChatLayer* chatLayer;
    Sprite* redDot;

    std::vector<Character *> m_enemy;
    Character *hunter;
    int aliveNumber = 0;
    bool gameIsEnd = false;

    std::function<void(MapLayer *, cocos2d::Touch *touch)> touchBegan;
    std::function<void(MapLayer *)> touchEnded;

    Vector<MenuItem *> guns;
    MenuItem *gun[5][2];

    Menu *gunMenu;

    system_clock::time_point startTime;

public:
    State();

    MapLayer *mapLayer;

    static State *create(std::vector<Character *> gameHunter) {
        State *pRet = new(std::nothrow) State();
        if (pRet && pRet->init(gameHunter)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            return nullptr;
        }
    }

    static cocos2d::Layer *createScene(std::vector<Character *> &gameHunter);

    virtual bool init(std::vector<Character *> gameHunter);

    virtual void initState();

    virtual void initGun();

    void touchInit();

    void bindTouch(std::function<void(MapLayer *, cocos2d::Touch *touch)> &began,
                   std::function<void(MapLayer *)> &ended);

    virtual void update(float fDelta);

    int getTime();

};

#endif
