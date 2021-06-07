#ifndef _BULLET_H_
#define _BULLET_H_

#include "cocos2d.h"

USING_NS_CC;

class Bullet : public Sprite {
public:
    static Bullet* create(const std::string& filename)
    {
        Bullet* sprite = new (std::nothrow) Bullet();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

	void showEffect(Point); //显示击中特效
	bool isInScreen();

    CC_SYNTHESIZE(bool, m_active, BulletActive);
	CC_SYNTHESIZE(int, m_speed, BulletSpeed);
	CC_SYNTHESIZE(int, m_attack, BulletAttack);
};
#endif;