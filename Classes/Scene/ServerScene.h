#ifndef __SERVER_SCENE_H__
#define __SERVER_SCENE_H__

#include "cocos2d.h"
#include "RoomScene.h"
#include "ui/CocosGUI.h"
#include "Network/Server.h"
#include "Network/Client.h"
#include "Scene/WarningLayer.h"
USING_NS_CC;

class Server : public cocos2d::Scene
{
    CC_SYNTHESIZE(float, m_volume, Volume);
    TextField* addressText;
    TextField* nameText;
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Server);
};

#endif
