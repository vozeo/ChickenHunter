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

	mapHeight = map->getMapSize().height;
	mapWidth = map->getMapSize().width;

	hunter = gameHunter;
	addChild(hunter, 3);
	hunter->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	runAction(Follow::create(hunter, Rect::ZERO));

	initBullet();
	initWeapon();

	registerKeyboardEvent();
	registerMouseEvent();

    return true;
}

void MapLayer::initBullet() {
	for (auto& bullet : bullets) {
		bullet = Bullet::create("images//bullet.png");
		bullet->setOpacity(0);
		bullet->setBulletActive(false);
		addChild(bullet, 2);
	}
}
//StringUtils::format("%s_avatar.png", player_name[i].c_str())
void MapLayer::initWeapon() {
	for (auto& weapon : weapons) {
		auto weaponType = random() % 4;
		weapon = Weapon::create(StringUtils::format("images//weapon_%d.png", weaponType));
		weapon->setWeaponType(weaponType);
		weapon->setWeaponSpeed(0.5f + rand_0_1());
		weapon->setWeaponAttack(0.5f + rand_0_1());
		weapon->setWeaponState(true);
		int posX, posY;
		while (true) {
			posX = random() % static_cast<int>(mapWidth * 32);
			posY = random() % static_cast<int>(mapHeight * 32);
			//if (!meta->getTileGIDAt(Vec2(posX / 32, mapHeight - posY / 32)))
				break;
		}
		weapon->setPosition(Vec2(posX, posY));
		addChild(weapon, 1);
	}
}

void MapLayer::registerKeyboardEvent() {
	auto listener = EventListenerKeyboard::create();

	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->runAction(hunter->getCharacterAnimRight());
			hunter->m_speed[0] = true;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->runAction(hunter->getCharacterAnimLeft());
			hunter->m_speed[1] = true;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->runAction(hunter->getCharacterAnimDown());
			hunter->m_speed[2] = true;
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->runAction(hunter->getCharacterAnimUp());
			hunter->m_speed[3] = true;
			break;
		}
	};

	listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		hunter->stopAllActions();
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->m_speed[0] = false;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->m_speed[1] = false;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->m_speed[2] = false;
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->m_speed[3] = false;
			break;
		}
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
	float dx = 0, dy = 0;
	if (hunter->m_speed[0])
		dx = 4;
	if (hunter->m_speed[1])
		dx = -4;
	if (hunter->m_speed[2])
		dy = -4;
	if (hunter->m_speed[3])
		dy = 4;

	auto hunterPos = hunter->getPosition();
	float nextX = hunterPos.x + dx;
	float nextY = hunterPos.y + dy;
	
	auto nextMapX = nextX / 32;
	auto nextMapY = mapHeight - nextY / 32;

	CCLOG("%f %f, %f %f", nextX, nextY, nextMapX, nextMapY);

	if (nextMapX < mapWidth && nextMapX >= 0 && nextMapY < mapHeight && nextMapY >= 0
		&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
		hunter->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX, nextY)));
	else hunter->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX - 2 * dx, nextY - 2 * dy)));
	
	for (auto bullet : bullets) {
		if (bullet->getBulletActive()) {
			auto bulletX = bullet->getPositionX();
			auto bulletY = bullet->getPositionY();
			if (bulletX < 0 || bulletX > mapWidth * 32 || bulletY < 0 || bulletY > mapHeight * 32
				|| meta->getTileGIDAt(Vec2(bulletX / 32,  mapHeight - bulletY / 32))) {
				bullet->setOpacity(0);
				bullet->stopAllActions();
				bullet->setBulletActive(false);
			}
		}
	}



	for (auto weapon : weapons) {
		if (weapon->getWeaponState()) {
			if (weapon->getBoundingBox().containsPoint(hunterPos)) {
				auto weaponType = weapon->getWeaponType();
				if (hunter->m_gun[weaponType] == nullptr) {
					weapon->setOpacity(0);
					weapon->setWeaponState(false);
					hunter->m_gun[weaponType] = weapon;
					hunter->setPlayerRefresh(true);
				}
			}
		}
	}
	
}