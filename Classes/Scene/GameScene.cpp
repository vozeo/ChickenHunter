#include "GameScene.h"

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

bool Game::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }
	this->scheduleUpdate();

	auto background = Sprite::create("images/backocean.png");
	background->setPosition(winSize.width / 2, winSize.height / 2);
	background->setScale(winSize.width / background->getTextureRect().getMaxX(), winSize.height / background->getTextureRect().getMaxY());
	this->addChild(background, 0);

	Director::getInstance()->getOpenGLView()->setCursorVisible(false);
	initMouse();
	addChild(m_cursor, 4);

	for (int i = 0; i < m_hunter_number; i++)
	{
		m_hunter.push_back(Character::create());
		m_hunter[i]->retain();
		m_hunter[i]->setName("Player" + Value(i).asString());
	}
	hunter = m_hunter[0];
	hunter->setName("hunter");

    map = MapLayer::create(m_hunter);
    addChild(map, 1);
	map->bindTouchMap(touchBegan, touchEnded);

	stateUI = State::create(m_hunter);
	addChild(stateUI, 2);
	stateUI->mapLayer = map;
	stateUI->bindTouch(touchBegan, touchEnded);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	stateUI->touchInit();
#endif

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

void Game::onExit()
{
	AudioEngine::stop(backgroundAudioID);
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
	AudioEngine::setVolume(backgroundAudioID, M_Volume);
}
