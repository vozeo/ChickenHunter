#ifndef __CHAT_SCENE_H__
#define __CHAT_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "Network/Client.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define winSize Director::getInstance()->getWinSize()

class ChatLayer : public cocos2d::Layer {
    std::vector<Label *> chats;
    TextField *chat_text;
    int chat_number = 0;
public:
    virtual bool init();

    CREATE_FUNC(ChatLayer);

    void showChat(const char *name, const char *text);
};

#endif
