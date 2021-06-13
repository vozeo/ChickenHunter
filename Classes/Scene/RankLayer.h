#pragma once
#ifndef __RANK_SCENE_H__
#define __RANK_SCENE_H__

#include "cocos2d.h"
#include "Character/Character.h"

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class RankLayer : public cocos2d::Layer
{
public:
    virtual bool init();
    CREATE_FUNC(RankLayer);

    void closeCallback(cocos2d::Ref* pSender);
    void rankInit(bool isWin, std::vector<Character*> player);
};

#endif
