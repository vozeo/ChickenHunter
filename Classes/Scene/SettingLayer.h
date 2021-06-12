#pragma once
#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocos2d::ui;

#define winSize Director::getInstance()->getWinSize()

class SettingLayer : public cocos2d::Layer
{
    Slider* volumeSlider;
    float* m_volume;
public:
    virtual bool init();
    CREATE_FUNC(SettingLayer);

    void settingInit(float* volume);
    void closeCallback(cocos2d::Ref* pSender);
};

#endif