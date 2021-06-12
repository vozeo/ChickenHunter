#pragma once
#ifndef __EXIT_SCENE_H__
#define __EXIT_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class ExitLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    CREATE_FUNC(ExitLayer);

    void closeCallback(cocos2d::Ref* pSender);
};

#endif
