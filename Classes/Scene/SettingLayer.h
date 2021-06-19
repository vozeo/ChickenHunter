#pragma once
#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Const.h"
#include "Character/Character.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define winSize Director::getInstance()->getWinSize()

class SettingLayer : public cocos2d::Layer {
    Slider *volume_slider;

public:
    SettingLayer();

    virtual bool init(Character *hunter);

    static SettingLayer *create(Character *hunter = nullptr) {
        SettingLayer *pRet = new(std::nothrow) SettingLayer();
        if (pRet && pRet->init(hunter)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    void closeCallback(cocos2d::Ref *pSender);

};

#endif