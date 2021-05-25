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

	hunter = gameHunter;
	addChild(hunter, 1);
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

	int dx = 0, dy = 0;
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
	auto moveBy = MoveBy::create(0.1, Vec2(dx, dy));
	hunter->runAction(moveBy);
}

TMXLayer* MapLayer::getBarrier() { return meta; }