#include "GameScene.h"

USING_NS_CC;

Game::Game() = default;

Game::~Game() {
    for (auto enemy : m_hunter)
        enemy->release();
    if (chserver != nullptr) {
        delete chserver;
        chserver = nullptr;
    }
    if (chclient != nullptr) {
        delete chclient;
        chclient = nullptr;
    }
    AudioEngine::stop(background_audio_ID);
}

Scene *Game::createScene() {
    return Game::create();
}

bool Game::init() {
    if (!Scene::init()) {
        return false;
    }
    this->scheduleUpdate();

    auto background = Sprite::create("images/backocean.png");
    background->setPosition(winSize.width / 2, winSize.height / 2);
    background->setScale(winSize.width / background->getTextureRect().getMaxX(),
                         winSize.height / background->getTextureRect().getMaxY());
    this->addChild(background, 0);

    Director::getInstance()->getOpenGLView()->setCursorVisible(false);
    initMouse();
    addChild(m_cursor, 4);

    for (int i = 0; i < m_hunter_number; i++) {
        m_hunter.push_back(Character::create());
        m_hunter[i]->retain();
        m_hunter[i]->setName("Player" + Value(i).asString());
    }
    if (chclient != nullptr && chserver == nullptr)
        hunter = m_hunter[chclient->getUid() - 1];
    else
        hunter = m_hunter[0];
    hunter->setName("hunter");

    map = MapLayer::create(m_hunter);
    addChild(map, 1);
    map->bindTouchMap(touchBegan, touchEnded);

    state_UI = State::create(m_hunter);
    addChild(state_UI, 2);
    state_UI->map_layer = map;
    state_UI->bindTouch(touchBegan, touchEnded);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    state_UI->touchInit();
#endif

    auto exit_img = MenuItemImage::create(
            "exit_0.png",
            "exit_1.png",
            [=](Ref *render) { addChild(ExitLayer::create(), 3); });
    exit_img->setAnchorPoint(Vec2(1, 1));
    auto setting_img = MenuItemImage::create(
            "setting_0.png",
            "setting_1.png",
            [=](Ref *render) {
                SettingLayer *setting = SettingLayer::create(hunter);
                addChild(setting, 3);
            });
    setting_img->setAnchorPoint(Vec2(1, 1));


    Vector<MenuItem *> menus{setting_img, exit_img};
    auto menu = Menu::createWithArray(menus);
    addChild(menu, 2);
    menu->setAnchorPoint(Vec2(1, 1));
    menu->setPosition(winSize.width - 30, winSize.height);
    menu->alignItemsHorizontally();

    AudioEngine::lazyInit();
    AudioEngine::preload("music/gameBgm.mp3");
    background_audio_ID = AudioEngine::play2d("music/gameBgm.mp3", true);

    return true;
}

void Game::initMouse() {
    m_cursor = Sprite::create();
    m_cursor->addChild(Sprite::create("images/cursorUp.png"));


    mouse_listener = EventListenerMouse::create();

    mouse_listener->onMouseMove = [&](EventMouse *event) {
        m_cursor->setPosition(Vec2(event->getCursorX(), event->getCursorY()));
    };
    mouse_listener->onMouseUp = [&](EventMouse *event) {
        m_cursor->removeAllChildren();
        m_cursor->addChild(Sprite::create("images/cursorUp.png"));
    };
    mouse_listener->onMouseDown = [&](EventMouse *event) {
        m_cursor->removeAllChildren();
        m_cursor->addChild(Sprite::create("images/cursorDown.png"));
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);
}

void Game::update(float dt) {
    AudioEngine::setVolume(background_audio_ID, M_Volume);
}
