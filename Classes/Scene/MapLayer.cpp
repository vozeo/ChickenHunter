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
	addChild(map);
	meta = map->getLayer("water");

	mapHeight = map->getMapSize().height - 1;
	mapWidth = map->getMapSize().width - 1;
	tileWidth = map->getTileSize().width - 1;
	tileHeight = map->getTileSize().height - 1;

	hunter = gameHunter;
	addChild(hunter, 1);
	//add enemies
	srand((int)time(0));
	initSetEnemy();

	//add items
	initSetItem();

	hunter->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	runAction(Follow::create(hunter, Rect::ZERO));

	registerKeyboardEvent();

    return true;
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

void MapLayer::update(float fDelta) {
	auto left = EventKeyboard::KeyCode::KEY_A;
	auto right = EventKeyboard::KeyCode::KEY_D;
	auto up = EventKeyboard::KeyCode::KEY_W;
	auto down = EventKeyboard::KeyCode::KEY_S;
	auto pick= EventKeyboard::KeyCode::KEY_F;

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
	if (keyMap[pick])
	{
		Rect rect_hunter = hunter->getBoundingBox();
		/*float x = rect_hunter.origin.x + rect_hunter.size.width * 0.3;
		float y = rect_hunter.origin.y + rect_hunter.size.height * 0.4;
		float width = rect_hunter.size.width * 0.3;
		float height = rect_hunter.size.height * 0.6;
		Rect rect_hunter_pick=*/

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

	float nextX = hunter->getPositionX() + dx;
	float nextY = hunter->getPositionY() + dy;
	
	auto nextMapX = nextX / 32;
	auto nextMapY = mapHeight - nextY / 32 + 1;

	//CCLOG("%f %f, %f %f, %f, %f", nextX, nextY, mapHeight, nextMapY, X, Y);

	if (nextMapX <= mapWidth && nextMapX >= 0 && nextMapY <= mapHeight && nextMapY >= 0
		&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
		hunter->runAction(MoveTo::create(1.0f / 120, Vec2(nextX, nextY)));
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
        rx = static_cast<float>(rand() % static_cast<int>(mapWidth * tileWidth-100));
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
