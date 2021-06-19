#include "Joystick.h"

#include <utility>

Joystick::Joystick() = default;

bool Joystick::init() {
    visible_size = Director::getInstance()->getVisibleSize();

    m_back1 = Sprite::create("images/joyback.png");
    m_back1->setPosition(Vec2(visible_size.width / 6, visible_size.height / 4));
    addChild(m_back1, 1);

    m_button1 = Sprite::create("images/joybutton.png");
    m_button1->setPosition(m_back1->getPosition());
    addChild(m_button1, 2);

    m_center_point1 = m_back1->getPosition();
    m_current_point1 = m_center_point1;

    m_back2 = Sprite::create("images/joyback.png");
    m_back2->setPosition(Vec2(5 * visible_size.width / 6, visible_size.height / 4));
    addChild(m_back2, 1);

    m_button2 = Sprite::create("images/joybutton.png");
    m_button2->setPosition(m_back2->getPosition());
    addChild(m_button2, 2);

    m_center_point2 = m_back2->getPosition();
    m_current_point2 = m_center_point2;

    this->scheduleUpdate();

    auto m_listener = EventListenerTouchAllAtOnce::create();
    m_listener->onTouchesBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
    m_listener->onTouchesMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
    m_listener->onTouchesEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, this);

    return true;
}

void Joystick::update(float dt) {
    m_button1->setPosition(m_current_point1);
    m_button2->setPosition(m_current_point2);
}

void
Joystick::bindTouch(Character *player,
                    std::function<void(MapLayer *, cocos2d::Touch *touch)> &began,
                    std::function<void(MapLayer *)> &ended) {
    hunter = player;
    touchBegan = began;
    touchEnded = ended;
}

bool Joystick::onTouchBegan(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visible_size.width / 2) {
            Vec2 touch_pos = touch->getLocation();
            m_current_point1 = touch_pos;
            animate();
        } else {
            Vec2 touch_pos = touch->getLocation();
            m_current_point2 = touch_pos;
            hunter->m_bullet_location = m_current_point2 - m_center_point2 + visible_size / 2;
            touchBegan(map_layer, touch);
        }
    }
    return true;
}

void Joystick::onTouchMoved(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visible_size.width / 2) {
            Vec2 touch_pos = touch->getLocation();
            m_current_point1 = touch_pos;
            animate();
        } else {
            Vec2 touch_pos = touch->getLocation();
            m_current_point2 = touch_pos;
            hunter->m_bullet_location = m_current_point2 - m_center_point2 + visible_size / 2;
            if (hunter->getPlayerWeapon() == 4) {
                auto pos = (hunter->m_bullet_location - winSize / 2) * 1.5 + winSize / 2;
                auto effect_circle = DrawNode::create();
                effect_circle->drawSolidCircle(pos, 100.0f, CC_DEGREES_TO_RADIANS(360), 15,
                                               Color4F(0.28f, 0.46f, 1.0f, 0.6f));
                effect_circle->runAction(
                        Sequence::create(FadeOut::create(0.02f), RemoveSelf::create(), NULL));
                addChild(effect_circle, 0);
            }
        }
    }
}

void Joystick::onTouchEnded(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visible_size.width / 2) {
            m_current_point1 = m_center_point1;
            hunter->stopAllActions();
            for (auto speed : hunter->m_speed)
                speed = false;
            animate();
        } else {
            m_current_point2 = m_center_point2;
            touchEnded(map_layer);
        }
    }
}

void Joystick::animate() {
    if (m_current_point1.x - m_center_point1.x > move_distance)
        hunter->m_speed[0] = true;
    else hunter->m_speed[0] = false;
    if (m_center_point1.x - m_current_point1.x > move_distance)
        hunter->m_speed[1] = true;
    else hunter->m_speed[1] = false;
    if (m_center_point1.y - m_current_point1.y > move_distance)
        hunter->m_speed[2] = true;
    else hunter->m_speed[2] = false;
    if (m_current_point1.y - m_center_point1.y > move_distance)
        hunter->m_speed[3] = true;
    else hunter->m_speed[3] = false;

    for (int i = 0; i <= 3; i++) {
        if (hunter->m_speed[i]) {
            switch (i) {
                case 0:
                    hunter->runAction(hunter->getCharacterAnimRight());
                    break;
                case 1:
                    hunter->runAction(hunter->getCharacterAnimLeft());
                    break;
                case 2:
                    hunter->runAction(hunter->getCharacterAnimDown());
                    break;
                case 3:
                    hunter->runAction(hunter->getCharacterAnimUp());
                    break;
                default:
                    break;
            }
            break;
        }
    }
}
