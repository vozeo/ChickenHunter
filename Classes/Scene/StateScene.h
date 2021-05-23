#ifndef __STATE_SCENE_H__
#define __STATE_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class State : public cocos2d::Layer
{
public:
    static cocos2d::Layer* createLayer();
    virtual bool init();
    CREATE_FUNC(State);
};

#endif
