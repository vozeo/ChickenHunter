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

	Director::getInstance()->getOpenGLView()->setCursorVisible(false);
	initMouse();
	addChild(m_cursor, 4);

	hunter = Character::create();

    map = MapLayer::create(hunter);
	map->setVolume(m_volume);
    addChild(map, 1);

	stateUI = State::create(hunter);
	addChild(stateUI, 2);

	auto exit_img = MenuItemImage::create(
		"exit_0.png",
		"exit_1.png",
		[=](Ref* render) { addChild(ExitLayer::create(), 3); });
	exit_img->setAnchorPoint(Vec2(1, 1));
	auto setting_img = MenuItemImage::create(
		"setting_0.png",
		"setting_1.png",
		[=](Ref* render) {
			SettingLayer* setting = SettingLayer::create();
			setting->settingInit(m_volume);
			addChild(setting, 3); 
		});
	setting_img->setAnchorPoint(Vec2(1, 1));


	Vector<MenuItem*> menus{ setting_img, exit_img };
	auto menu = Menu::createWithArray(menus);
	addChild(menu, 2);
	menu->setAnchorPoint(Vec2(1, 1));
	menu->setPosition(winSize.width - 30, winSize.height);
	menu->alignItemsHorizontally();

	AudioEngine::lazyInit();
	AudioEngine::preload("music/gameBgm.mp3");
	backgroundAudioID = AudioEngine::play2d("music/gameBgm.mp3", true);

    return true;
}

void Game::initMouse() {
	m_cursor = Sprite::create();
	m_cursor->addChild(Sprite::create("images/cursorUp.png"));

	mouseListener = EventListenerMouse::create();

	mouseListener->onMouseMove = [&](EventMouse* event) {
		m_cursor->setPosition(Vec2(event->getCursorX(), event->getCursorY()));
	};
	mouseListener->onMouseUp = [&](EventMouse* event) {
		m_cursor->removeAllChildren();
		m_cursor->addChild(Sprite::create("images/cursorUp.png"));
	};
	mouseListener->onMouseDown = [&](EventMouse* event) {
		m_cursor->removeAllChildren();
		m_cursor->addChild(Sprite::create("images/cursorDown.png"));
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void Game::update(float dt) {
	AudioEngine::setVolume(backgroundAudioID, *m_volume);
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
