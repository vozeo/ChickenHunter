#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "character/Character.h"
#include <map>

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class Game : public cocos2d::Scene
{
private:
    TMXTiledMap* map;
    TMXLayer* meta;

    ui::LoadingBar* blood_bar;
    Label* blood_label;
    Label* survivor_label;

    Character* hunter;

    MenuItem* gun[4][2];

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    virtual void initState();
    virtual void initGun();
    virtual void update(float fDelta);
    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(Game);

    void registerKeyboardEvent();

private:
    std::map<EventKeyboard::KeyCode, bool> keyMap;

    void updateHunterInfo();

    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};

#endif
