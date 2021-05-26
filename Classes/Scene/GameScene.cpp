#include "GameScene.h"

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }
	this->scheduleUpdate();

	hunter = Character::create();

    map = MapLayer::create(hunter);
    addChild(map);

	stateUI = State::create(hunter);
	addChild(stateUI, TOP);

    return true;
}

/*
void Game::registerKeyboardEvent() {
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

void Game::update(float fDelta) {
	auto left = EventKeyboard::KeyCode::KEY_A;
	auto right = EventKeyboard::KeyCode::KEY_D;
	auto up = EventKeyboard::KeyCode::KEY_W;
	auto down = EventKeyboard::KeyCode::KEY_S;

	int dx = 0, dy = 0;
	if (keyMap[left])
	{
		dx = -1;
	}
	if (keyMap[right])
	{
		dx = 1;
	}
	if (keyMap[up])
	{
		dy = 1;
	}
	if (keyMap[down])
	{
		dy = -1;
	}
	auto moveBy = MoveBy::create(0.1, Vec2(dx, dy));
	auto moveByRev = MoveBy::create(0.1, Vec2(-dx, -dy));
	//auto moveTo = MoveTo::create(0.1, Vec2(hunter->getPositionX() + dx, hunter->getPositionY() + dy));
	
	stateUI->runAction(moveBy);
	hunter->runAction(moveBy);
	
}

void Game::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d pressed", keyCode);
    if (EventKeyboard::KeyCode::KEY_W == keyCode)
        map->setPositionY(map->getPositionY() - 33);
    else if (EventKeyboard::KeyCode::KEY_S == keyCode)
        map->setPositionY(map->getPositionY() + 33);
    else if (EventKeyboard::KeyCode::KEY_A == keyCode)
        map->setPositionX(map->getPositionX() + 33);
    else if (EventKeyboard::KeyCode::KEY_D == keyCode)
        map->setPositionX(map->getPositionX() - 33);
}

void Game::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d released", keyCode);
}

*/
