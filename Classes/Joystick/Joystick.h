#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_
 
#include "cocos2d.h"
#include "Character/Character.h"
USING_NS_CC;
 
class Joystick : public Layer
{
private:
	Point m_centerPoint;
	Point m_currentPoint;

	Sprite* m_back;
	Sprite* m_button;

	Character *hunter;

	Size visibleSize;

	float moveDistance = 30.0f;

public:
 
	bool init();
	CREATE_FUNC(Joystick);

	bool onTouchBegan(Touch* touch, Event* event);
	void onTouchMoved(Touch *touch, Event *event);
	void onTouchEnded(Touch *touch, Event *event);
 
	virtual void update(float dt);
	void bind(Character* player);

};
#endif