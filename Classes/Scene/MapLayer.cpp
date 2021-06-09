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

	map = TMXTiledMap::create("map/Desert.tmx");
	addChild(map, 0);

	meta = map->getLayer("water");

	mapHeight = map->getMapSize().height - 1;
	mapWidth = map->getMapSize().width - 1;
	tileWidth = map->getTileSize().width - 1;
	tileHeight = map->getTileSize().height - 1;

	hunter = gameHunter;
	addChild(hunter, 1);

	hunter->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	runAction(Follow::create(hunter, Rect::ZERO));

	AudioEngine::lazyInit();
	AudioEngine::preload("music/bulletEffect.wav");
	AudioEngine::preload("music/knifeEffect.wav");

	//add enemies
	srand((int)time(0));
	initSetEnemy();

	//add items
	initSetItem();

	initBullet();
	initWeapon();

	registerKeyboardEvent();
	registerTouchEvent();

    return true;
}

void MapLayer::initBullet() {
	for (auto& bullet : bullets) {
		bullet = Bullet::create("images/bullet.png");
		bullet->setOpacity(0);
		bullet->setBulletActive(false);
		addChild(bullet, 2);
	}
}
//StringUtils::format("%s_avatar.png", player_name[i].c_str())
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

		for (auto bn : m_bandage)
		{
			if (bn->getBoundingBox().intersectsRect(rect_hunter))
			{
				auto place = std::find(m_bandage.begin(), m_bandage.end(), bn);
				bn->removeFromParent();
				m_bandage.erase(place);
				break;
			}
		}

		for (auto am : m_ammunition)
		{
			if (am->getBoundingBox().intersectsRect(rect_hunter))
			{
				auto place = std::find(m_ammunition.begin(), m_ammunition.end(), am);
				am->removeFromParent();
				m_ammunition.erase(place);
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
				bullet->setOpacity(255);
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

	float nextX = hunter->getPositionX() + dx;
	float nextY = hunter->getPositionY() + dy;
	
	auto nextMapX = nextX / 32;
	auto nextMapY = mapHeight - nextY / 32 + 1;

	if (nextMapX <= mapWidth && nextMapX >= 0 && nextMapY <= mapHeight && nextMapY >= 0
		&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
		hunter->runAction(MoveTo::create(1.0f / 120, Vec2(nextX, nextY)));

	for (auto bullet : bullets) {
		if (bullet->getBulletActive()) {
			auto bulletX = bullet->getPositionX();
			auto bulletY = bullet->getPositionY();
			if (bulletX < 0 || bulletX > mapWidth * 32 || bulletY < 0 || bulletY > mapHeight * 32
				|| meta->getTileGIDAt(Vec2(bulletX / 32, mapHeight - bulletY / 32))) {
				bullet->setOpacity(0);
				bullet->stopAllActions();
				bullet->setBulletActive(false);
			}
		}
	}
	
	for (auto weapon : weapons) {
		if (weapon->getWeaponState()) {
			if (weapon->getBoundingBox().containsPoint(hunter->getPosition())) {
				auto weaponType = weapon->getWeaponType();
				if (hunter->m_gun[weaponType] == nullptr) {
					weapon->setOpacity(0);
					weapon->setWeaponState(false);
					hunter->m_gun[weaponType] = weapon;
					hunter->setPlayerWeapon(weaponType);
					hunter->setPlayerRefresh(true);
				}
			}
		}
	}
}

int MapLayer::m_enemy_number = 5;
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


//set enemies/items randomly and at anywhere except water space.
template <class T>
void MapLayer::setRandPos(T ele)
{
	int rx, ry,mrx,mry;
	while (1)
	{
        rx = static_cast<float>(rand() % static_cast<int>(mapWidth * tileWidth - 100));
		mrx = (50.0 + rx) / 32;
		ry = static_cast<float>(rand() % static_cast<int>(mapHeight * tileHeight - 100));
		mry = mapHeight - (50.0 + ry) / 32 + 1;
		if(mrx <= mapWidth && mrx >= 0 && mry <= mapHeight && mry >= 0
			&& !meta->getTileGIDAt(Vec2(mrx, mry)))
			break;
	}
	ele->setPosition(Vec2(50.0 + rx,50.0 + ry));
}

void MapLayer::initSetItem()
{
	m_bandage.clear();
	int number = Bandage::getNumber();
	for (int i = 0; i < number; i++)
	{
		m_bandage.push_back(Bandage::create());
		addChild(m_bandage[i], 0);
		setRandPos(m_bandage[i]);
	}

	m_ammunition.clear();
	number = Ammunition::getNumber();
	for (int i = 0; i < number; i++)
	{
		m_ammunition.push_back(Ammunition::create());
		addChild(m_ammunition[i], 0);
		setRandPos(m_ammunition[i]);
	}
}
