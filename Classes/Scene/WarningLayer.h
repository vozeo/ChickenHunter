#pragma once
#ifndef __WARNING_SCENE_H__
#define __WARNING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Const.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define winSize Director::getInstance()->getWinSize()

class WarningLayer : public cocos2d::Layer {
public:
    WarningLayer();

    virtual bool init(std::string warning_text);

    static WarningLayer *create(std::string warning_text) {
        WarningLayer *pRet = new(std::nothrow) WarningLayer();
        if (pRet && pRet->init(warning_text)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            return nullptr;
        }
    }

    void closeCallback(cocos2d::Ref *pSender);
};

#endif