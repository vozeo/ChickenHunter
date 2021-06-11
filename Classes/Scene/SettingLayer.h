#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class SettingLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    CREATE_FUNC(SettingLayer);

    void closeCallback(cocos2d::Ref* pSender);
};

#endif