#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"
#include "Scene/ServerScene.h"
#include "Const.h"

USING_NS_CC;

class Start : public cocos2d::Scene {
    int startAudioID = -1;
public:
    static cocos2d::Scene *createScene();

    virtual bool init();

    CREATE_FUNC(Start);

    virtual void update(float dt);
};

#endif
