#include "Bullet.h"

bool Bullet::isInScreen() { return true; }

void Bullet::showEffect(Vec2 pos) {
	auto effectCircle = DrawNode::create();
	effectCircle->setGlobalZOrder(1);
	effectCircle->drawSolidCircle(pos, 6.0f, CC_DEGREES_TO_RADIANS(360), 15, Color4F(1.0f, 0.0f, .0f, 0.6f));
	effectCircle->runAction(Sequence::create(FadeOut::create(0.3f), RemoveSelf::create(), NULL));
}