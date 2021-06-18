#include "Joystick.h"

Joystick::Joystick() = default;

bool Joystick::init() {
    visibleSize = Director::getInstance()->getVisibleSize();

    m_back1 = Sprite::create("images/joyback.png");
    m_back1->setPosition(Vec2(visibleSize.width / 6, visibleSize.height / 4));
    addChild(m_back1, 1);

    m_button1 = Sprite::create("images/joybutton.png");
    m_button1->setPosition(m_back1->getPosition());
    addChild(m_button1, 2);

    m_centerPoint1 = m_back1->getPosition();
    m_currentPoint1 = m_centerPoint1;

    m_back2 = Sprite::create("images/joyback.png");
    m_back2->setPosition(Vec2(5 * visibleSize.width / 6, visibleSize.height / 4));
    addChild(m_back2, 1);

    m_button2 = Sprite::create("images/joybutton.png");
    m_button2->setPosition(m_back2->getPosition());
    addChild(m_button2, 2);

    m_centerPoint2 = m_back2->getPosition();
    m_currentPoint2 = m_centerPoint2;

    this->scheduleUpdate();

    auto m_listener = EventListenerTouchAllAtOnce::create();
    m_listener->onTouchesBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
    m_listener->onTouchesMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
    m_listener->onTouchesEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, this);

    return true;
}

void Joystick::update(float dt) {
    m_button1->setPosition(m_currentPoint1);
    m_button2->setPosition(m_currentPoint2);
}

void
Joystick::bindTouch(Character *player, std::function<void(MapLayer *, cocos2d::Touch *touch)> began,
                    std::function<void(MapLayer *)> ended) {
    hunter = player;
    touchBegan = began;
    touchEnded = ended;
}

bool Joystick::onTouchBegan(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visibleSize.width / 2) {
            Vec2 touchPos = touch->getLocation();
            m_currentPoint1 = touchPos;
            animate();
        } else {
            Vec2 touchPos = touch->getLocation();
            m_currentPoint2 = touchPos;
            hunter->bulletLocation = m_currentPoint2 - m_centerPoint2 +
                                     Vec2(visibleSize.width / 2, visibleSize.height / 2);
            touchBegan(mapLayer, touch);
        }
    }
    return true;
}

void Joystick::onTouchMoved(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visibleSize.width / 2) {
            Vec2 touchPos = touch->getLocation();
            m_currentPoint1 = touchPos;
            animate();
        } else {
            Vec2 touchPos = touch->getLocation();
            m_currentPoint2 = touchPos;
            hunter->bulletLocation = m_currentPoint2 - m_centerPoint2 +
                                     Vec2(visibleSize.width / 2, visibleSize.height / 2);
            auto effectCircle = DrawNode::create();
            addChild(effectCircle, 0);
            effectCircle->drawSolidCircle(3 * hunter->bulletLocation, 100.0f,
                                          CC_DEGREES_TO_RADIANS(360), 15,
                                          Color4F(0.28f, 0.46f, 1.0f, 0.6f));
            effectCircle->runAction(
                    Sequence::create(FadeOut::create(0.05f), RemoveSelf::create(), NULL));
        }
    }

}

void Joystick::onTouchEnded(const std::vector<Touch *> &touches, Event *event) {
    for (auto touch : touches) {
        if (touch->getLocation().x < visibleSize.width / 2) {
            m_currentPoint1 = m_centerPoint1;
            hunter->stopAllActions();
            for (auto speed : hunter->m_speed)
                speed = false;
            animate();
        } else {
            m_currentPoint2 = m_centerPoint2;
            touchEnded(mapLayer);
        }
    }

}

void Joystick::animate() {
    if (m_currentPoint1.x - m_centerPoint1.x > moveDistance) {
        hunter->m_speed[0] = true;
    } else hunter->m_speed[0] = false;
    if (m_centerPoint1.x - m_currentPoint1.x > moveDistance) {
        hunter->m_speed[1] = true;
    } else hunter->m_speed[1] = false;
    if (m_centerPoint1.y - m_currentPoint1.y > moveDistance) {
        hunter->m_speed[2] = true;
    } else hunter->m_speed[2] = false;
    if (m_currentPoint1.y - m_centerPoint1.y > moveDistance) {
        hunter->m_speed[3] = true;
    } else hunter->m_speed[3] = false;

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
            }
            break;
        }
    }
}
