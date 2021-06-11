#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "MapLayer.h"
#include "StateLayer.h"
#include "ExitLayer.h"
#include "SettingLayer.h"

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class Game : public cocos2d::Scene
{
private:
    int backgroundAudioID;

    EventListenerMouse* mouseListener;
    Sprite* m_cursor;

    MapLayer* map;

    State* stateUI;

    Character* hunter;

    CC_SYNTHESIZE(float*, m_volume, Volume);

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Game);

    void initMouse();
    virtual void update(float dt);

    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuSettingCallback(cocos2d::Ref* pSender);
};

#endif
