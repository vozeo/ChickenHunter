#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include "cocos2d.h"
#include "Character/Character.h"
#include "Scene/MapLayer.h"

USING_NS_CC;

class Joystick : public Layer {
private:
    Point m_center_point1;
    Point m_current_point1;

    Sprite *m_back1;
    Sprite *m_button1;

    Point m_center_point2;
    Point m_current_point2;

    Sprite *m_back2;
    Sprite *m_button2;

    Character *hunter;
    std::function<void(MapLayer *, cocos2d::Touch *touch)> touchBegan;
    std::function<void(MapLayer *)> touchEnded;

    Size visible_size;

    float move_distance = 30.0f;

public:
    Joystick();

    MapLayer *map_layer;

    bool init();

    CREATE_FUNC(Joystick);

    bool onTouchBegan(const std::vector<Touch *> &touches, Event *event);

    void onTouchMoved(const std::vector<Touch *> &touches, Event *event);

    void onTouchEnded(const std::vector<Touch *> &touches, Event *event);

    void animate();

    virtual void update(float dt);

    void bindTouch(Character *player, std::function<void(MapLayer *, cocos2d::Touch *touch)> &began,
                   std::function<void(MapLayer *)> &ended);

};

#endif