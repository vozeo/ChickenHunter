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
    Slider *volumeSlider;

public:
    WarningLayer();

    virtual bool init(std::string warningText);

    static WarningLayer *create(std::string warningText) {
        WarningLayer *pRet = new(std::nothrow) WarningLayer();
        if (pRet && pRet->init(warningText)) {
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