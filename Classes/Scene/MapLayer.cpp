#include "MapLayer.h"

USING_NS_CC;

Layer* MapLayer::createScene(Character* gameHunter)
{
    return MapLayer::create(gameHunter);
}

bool MapLayer::init(Character* gameHunter)
{
    if ( !Layer::init() )
    { 
        return false;
    }
	this->scheduleUpdate();

	map = TMXTiledMap::create("map//Desert.tmx");
	addChild(map, 0);
	meta = map->getLayer("water");

	mapHeight = map->getMapSize().height - 1;
	mapWidth = map->getMapSize().width - 1;

	hunter = gameHunter;
	addChild(hunter, 2);
	hunter->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	runAction(Follow::create(hunter, Rect::ZERO));

	initBullet();

	registerKeyboardEvent();
	registerMouseEvent();

    return true;
}

void MapLayer::initBullet() {
	for (auto& bullet : bullets) {
		bullet = Bullet::create("images//bullet.png");
		bullet->setOpacity(0);
		bullet->setBulletActive(false);
		addChild(bullet, 1);
	}
}

void MapLayer::createBullet(Vec2 speed, Weapon* weapon) {
	Bullet* bullet = Bullet::create("images//bullet.png");
}

void MapLayer::registerKeyboardEvent() {
	auto listener = EventListenerKeyboard::create();

	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		keyMap[keyCode] = true;
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->runAction(hunter->getCharacterAnimRight());
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->runAction(hunter->getCharacterAnimLeft());
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->runAction(hunter->getCharacterAnimDown());
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->runAction(hunter->getCharacterAnimUp());
			break;
		}
	};

	listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		hunter->stopAllActions();
		keyMap[keyCode] = false;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MapLayer::registerMouseEvent() {
	auto mouseListener = EventListenerMouse::create();

	mouseListener->onMouseDown = [&](EventMouse* event) {
		auto bulletLocation = event->getLocation();
		auto bulletX = bulletLocation.x - winSize.width / 2;
		auto bulletY = winSize.height / 2 - bulletLocation.y;

		//CCLOG("%f %f", bulletX, bulletY);

		float time = sqrt(bulletX * bulletX + bulletY * bulletY) / 1000;
		for (auto bullet : bullets) {
			if (!bullet->getBulletActive()) {
				bullet->setBulletActive(true);
				bullet->setPosition(hunter->getPosition());
				bullet->setRotation(calRotation(bulletX, bulletY));
				bullet->runAction(RepeatForever::create(MoveBy::create(1, Vec2(bulletX / time, bulletY / time))));
				bullet->setOpacity(255);
				break;
			}
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

float MapLayer::calRotation(float bulletX, float bulletY) {
	if (bulletX == 0 && bulletY > 0)
		return -90.0f;
	else if (bulletX == 0 && bulletY < 0)
		return 90.0f;
	else if (bulletX > 0)
		return -180.0f * atan(bulletY / bulletX) / PI;
	else return -180.0f * atan(bulletY / bulletX) / PI + 180.0f;
}

void MapLayer::update(float fDelta) {
	auto left = EventKeyboard::KeyCode::KEY_A;
	auto right = EventKeyboard::KeyCode::KEY_D;
	auto up = EventKeyboard::KeyCode::KEY_W;
	auto down = EventKeyboard::KeyCode::KEY_S;

	float dx = 0, dy = 0;
	if (keyMap[left])
	{
		dx = -4;
	}
	if (keyMap[right])
	{
		dx = 4;
	}
	if (keyMap[up])
	{
		dy = 4;
	}
	if (keyMap[down])
	{
		dy = -4;
	}

	float nextX = hunter->getPositionX() + dx;
	float nextY = hunter->getPositionY() + dy;
	
	auto nextMapX = nextX / 32;
	auto nextMapY = mapHeight - nextY / 32 + 1;

	//CCLOG("%f %f, %f %f, %f, %f", nextX, nextY, mapHeight, nextMapY, X, Y);

	if (nextMapX <= mapWidth && nextMapX >= 0 && nextMapY <= mapHeight && nextMapY >= 0
		&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
		hunter->runAction(MoveTo::create(1.0f / 120, Vec2(nextX, nextY)));
	
	for (auto bullet : bullets) {
		if (bullet->getBulletActive()) {
			auto bulletX = bullet->getPositionX();
			auto bulletY = bullet->getPositionY();
			//CCLOG("%f, %f", bulletX, bulletY);
			if (bulletX < 0 || bulletX > mapWidth * 32 || bulletY < 0 || bulletY > mapHeight * 32
				|| meta->getTileGIDAt(Vec2(bulletX / 32,  mapHeight - bulletY / 32 + 1))) {
				bullet->setOpacity(0);
				bullet->stopAllActions();
				bullet->setBulletActive(false);
			}
		}
	}
	
}