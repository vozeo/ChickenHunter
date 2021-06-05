#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "character/Character.h"

USING_NS_CC;

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
    void menuCloseCallback(cocos2d::Ref* pSender);
    CREATE_FUNC(Game);

    void updateHunterInfo();

private:
    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
};

#endif
