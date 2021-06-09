#include "MapLayer.h"

USING_NS_CC;

Layer* MapLayer::createScene(Character* gameHunter)
{
    return MapLayer::create(gameHunter);
}

// 0->map, 1->item/gun, 2->enemy, 3->bullet, 4->hunter
bool MapLayer::init(Character* gameHunter)
{
    if ( !Layer::init() )
    { 
        return false;
    }
	this->scheduleUpdate();

	map = TMXTiledMap::create("map/Desert.tmx");
	addChild(map, 0);

	meta = map->getLayer("water");

	mapHeight = map->getMapSize().height;
	mapWidth = map->getMapSize().width;
	tileWidth = map->getTileSize().width;
	tileHeight = map->getTileSize().height;

	hunter = gameHunter;
	addChild(hunter, 4);

	hunter->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	runAction(Follow::create(hunter, Rect::ZERO));

	AudioEngine::lazyInit();
	AudioEngine::preload("music/bulletEffect.wav");
	AudioEngine::preload("music/knifeEffect.wav");

	initSetItem();
	initBullet();

	registerKeyboardEvent();
	registerTouchEvent();

    return true;
}

void MapLayer::initBullet() {
	for (auto& bullet : bullets) {
		bullet = Bullet::create("images/bullet.png");
		bullet->setVisible(false);
		bullet->setBulletActive(false);
		addChild(bullet, 3);
	}
}

/*
void MapLayer::initWeapon() {
	for (auto& weapon : weapons) {
		auto weaponType = random(0, 3);
		weapon = Weapon::create(StringUtils::format("images/weapon_%d.png", weaponType));
		weapon->setWeaponType(weaponType);
		weapon->setWeaponSpeed(0.5f + rand_0_1());
		weapon->setWeaponAttack(0.5f + rand_0_1());
		weapon->setWeaponState(true);
		int posX, posY;
		while (true) {
			posX = random(0, static_cast<int>(mapWidth * 32));
			posY = random(0, static_cast<int>(mapHeight * 32));
			//if (!meta->getTileGIDAt(Vec2(posX / 32, mapHeight - posY / 32)))
				break;
		}
		weapon->setPosition(Vec2(posX, posY));
		addChild(weapon, 1);
	}
	Weapon* weapon = Weapon::create("images/gun4_1.png");
	weapon->retain();
	weapon->weaponInit(1, 1, 4, 0);
	hunter->m_gun[4] = weapon;
	hunter->setPlayerRefresh(true);
}
*/
void MapLayer::registerKeyboardEvent() {
	auto listener = EventListenerKeyboard::create();

	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		//keyMap[keyCode] = true;
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->m_speed[0] = true;
			hunter->runAction(hunter->getCharacterAnimRight());
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->m_speed[1] = true;
			hunter->runAction(hunter->getCharacterAnimLeft());
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->m_speed[2] = true;
			hunter->runAction(hunter->getCharacterAnimDown());
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->m_speed[3] = true;
			hunter->runAction(hunter->getCharacterAnimUp());
			break;
		case EventKeyboard::KeyCode::KEY_E:
			judgePick();
			break;
		default:
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
		default:
			break;
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MapLayer::judgePick() {
	Rect rect_hunter = hunter->getBoundingBox();
	for (auto weapon : weapons) {
		if (weapon->getBoundingBox().intersectsRect(rect_hunter)) {
			auto weaponType = weapon->getWeaponType();
			if (hunter->m_gun[weaponType] == nullptr) {
				weapon->retain();
				weapon->removeFromParent();
				weapons.erase(find(weapons.begin(), weapons.end(), weapon));
				hunter->m_gun[weaponType] = weapon;
				hunter->setPlayerWeapon(weaponType);
				hunter->setPlayerRefresh(true);
				break;
			}
		}

	}

	for (auto bn : m_bandage)
	{
		if (bn->getBoundingBox().intersectsRect(rect_hunter))
		{
			bn->removeFromParent();
			m_bandage.erase(find(m_bandage.begin(), m_bandage.end(), bn));
			break;
		}
	}

	for (auto am : m_ammunition)
	{
		if (am->getBoundingBox().intersectsRect(rect_hunter))
		{
			am->removeFromParent();
			m_ammunition.erase(find(m_ammunition.begin(), m_ammunition.end(), am));
			break;
		}
	}
}

void MapLayer::registerTouchEvent() {
	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = [&](Touch* touch, Event* event) {
		auto weaponType = hunter->getPlayerWeapon();
		if (4 == weaponType) {
			AudioEngine::play2d("music/knifeEffect.mp3", false);
			showEffect(hunter->getPosition());
			return true;
		}
		AudioEngine::play2d("music/bulletEffect.mp3", false);
			
		Weapon* weapon = hunter->m_gun[weaponType];
		auto bulletLocation = touch->getLocation();
		auto bulletX = bulletLocation.x - winSize.width / 2;
		auto bulletY = bulletLocation.y - winSize.height / 2;

		//CCLOG("%f %f", bulletX, bulletY);

		float time = sqrt(bulletX * bulletX + bulletY * bulletY) / 1000;
		for (auto bullet : bullets) {
			if (!bullet->getBulletActive()) {
				bullet->setBulletActive(true);
				bullet->setPosition(hunter->getPosition());
				bullet->setRotation(calRotation(bulletX, bulletY));
				bullet->runAction(RepeatForever::create(MoveBy::create(weapon->getWeaponSpeed(), Vec2(bulletX / time, bulletY / time))));
				bullet->setBulletAttack(weapon->getWeaponAttack());
				bullet->setVisible(true);
				break;
			}
		}
		return true;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
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

void MapLayer::showEffect(Vec2 pos) {
	auto effectCircle = DrawNode::create();
	addChild(effectCircle, 2);
	effectCircle->drawSolidCircle(pos, 100.0f, CC_DEGREES_TO_RADIANS(360), 15, Color4F(0.28f, 0.46f, 1.0f, 0.6f));
	effectCircle->runAction(Sequence::create(FadeOut::create(0.5f), RemoveSelf::create(), NULL));
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
	
	auto nextMapX = nextX / tileWidth;
	auto nextMapY = mapHeight - nextY / tileHeight;

	if (nextMapX < mapWidth && nextMapX >= 0 && nextMapY < mapHeight && nextMapY >= 0
		&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
		hunter->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX, nextY)));
	else hunter->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX - 2 * dx, nextY - 2 * dy)));

	for (auto bullet : bullets) {
		if (bullet->getBulletActive()) {
			auto bulletX = bullet->getPositionX();
			auto bulletY = bullet->getPositionY();
			if (bulletX < 0 || bulletX >= mapWidth * tileWidth || bulletY < 0 || bulletY >= mapHeight * tileHeight
				|| meta->getTileGIDAt(Vec2(bulletX / tileWidth, mapHeight - bulletY / tileHeight))) {
				bullet->setVisible(false);
				bullet->stopAllActions();
				bullet->setBulletActive(false);
			}
		}
	}
}

/*
void MapLayer::initSetEnemy()
{
	m_enemy.clear();
	for (int i = 0; i < m_enemy_number; i++)
	{
		m_enemy.push_back(Character::create());
		addChild(m_enemy[i], 1);
		setRandPos(m_enemy[i]);
	}
}
*/
//set enemies/items randomly and at anywhere except water space.
template <class T>
void MapLayer::setRandPos(T* elem)
{
	int rx, ry, mrx, mry;
	while (true)
	{
        rx = random(50, static_cast<int>(mapWidth * tileWidth - 50 - 1));
		mrx = rx / tileWidth;
		ry = random(50, static_cast<int>(mapHeight * tileHeight - 50 - 1));
		mry = mapHeight - ry / tileWidth;
		if (mrx < mapWidth && mrx >= 0 && mry < mapHeight && mry >= 0 && !meta->getTileGIDAt(Vec2(mrx, mry)))
			break;
	}
	elem->setPosition(Vec2(rx, ry));
}

template <class T>
void MapLayer::initItem(std::vector<T*> &items, int number) {
	items.clear();
	for (int i = 0; i < number; i++)
	{
		items.push_back(T::create());
		addChild(items[i], 1);
		setRandPos(items[i]);
	}
}

void MapLayer::initSetItem()
{
	initItem(m_enemy, m_enemy_number);

	initItem(weapons, m_weapon_number);
	initItem(m_bandage, m_bandage_number);
	initItem(m_ammunition, m_ammunition_number);
	
	

	Weapon* weapon = Weapon::create();
	weapon->retain();
	weapon->weaponInit(1, 1, 4, 0);
	hunter->m_gun[4] = weapon;

	hunter->setPlayerRefresh(true);
}
