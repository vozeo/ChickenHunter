#ifndef __ROOM_SCENE_H__
#define __ROOM_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"
#include "Network/Server.h"
#include "Network/Client.h"
#include "Scene/WarningLayer.h"

#include <cstring>

USING_NS_CC;

class Room : public cocos2d::Scene {
    const int m_player_number = MAX_CONNECTIONS;
    MenuItemFont *start_game;
    bool started = false;
    std::vector<Label *> player_label;

public:
    Room();

    static cocos2d::Scene *createScene(bool isServer);

    static Room *create(bool is_server) {
        Room *pRet = new(std::nothrow) Room();
        if (pRet && pRet->init(is_server)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            return nullptr;
        }
    }

    virtual bool init(bool is_server);

    virtual void update(float fDelta);
};

#endif
