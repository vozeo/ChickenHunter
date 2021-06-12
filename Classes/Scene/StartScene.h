#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include "cocos2d.h"
#include "Scene/GameScene.h"

USING_NS_CC;

class Start : public cocos2d::Scene
{
    int startAudioID;
    CC_SYNTHESIZE(float, m_volume, Volume);
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Start);

    virtual void update(float dt);
};

#endif
