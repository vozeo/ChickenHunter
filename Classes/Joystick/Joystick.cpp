#include "Joystick.h"

bool Joystick::init()
{
	visibleSize = Director::getInstance()->getVisibleSize();

	m_back = Sprite::create("images/joyback.png");
	m_back->setPosition(Vec2(visibleSize.width / 6, visibleSize.height / 4));
	addChild(m_back, 0);

	m_button = Sprite::create("images/joybutton.png");
	m_button->setPosition(m_back->getPosition());
	addChild(m_button, 1);

	m_centerPoint = m_back->getPosition();
	m_currentPoint = m_centerPoint;

	this->scheduleUpdate();

	auto m_listener = EventListenerTouchOneByOne::create();
	m_listener->onTouchBegan = CC_CALLBACK_2(Joystick::onTouchBegan, this);
	m_listener->onTouchMoved = CC_CALLBACK_2(Joystick::onTouchMoved, this);
	m_listener->onTouchEnded = CC_CALLBACK_2(Joystick::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, this);

	return true;
}

void Joystick::update(float dt)
{
	m_button->setPosition(m_currentPoint);

	if (m_currentPoint.x - m_centerPoint.x > moveDistance)
		hunter->m_speed[0] = true;
	else hunter->m_speed[0] = false;
	if (m_centerPoint.x - m_currentPoint.x > moveDistance)
		hunter->m_speed[1] = true;
	else hunter->m_speed[1] = false;
	if (m_centerPoint.y - m_currentPoint.y > moveDistance)
		hunter->m_speed[2] = true;
	else hunter->m_speed[2] = false;
	if (m_currentPoint.y - m_centerPoint.y > moveDistance)
		hunter->m_speed[3] = true;
	else hunter->m_speed[3] = false;
}

void Joystick::bind(Character* player)
{
	hunter = player;
}

bool Joystick::onTouchBegan(Touch* touch, Event* event)
{
	Rect rect = m_back->getBoundingBox();
	Vec2 touchPos = touch->getLocation();
	if (rect.containsPoint(touchPos))
		m_currentPoint = touchPos;
	return true;
}

void Joystick::onTouchMoved(Touch* touch, Event* event)
{
	Rect rect = m_back->getBoundingBox();
	Vec2 touchPos = touch->getLocation();
	CCLOG("%f %f %f %f", rect.getMaxX(), rect.getMaxY(), touchPos.x, touchPos.y);
	if (rect.containsPoint(touchPos))
		m_currentPoint = touchPos;
}

void Joystick::onTouchEnded(Touch* touch, Event* event)
{
	m_currentPoint = m_centerPoint;
	hunter->stopAllActions();
	for (auto speed : hunter->m_speed)
		speed = false;
}