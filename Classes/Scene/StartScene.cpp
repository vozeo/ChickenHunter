#include "Scene/StartScene.h"

USING_NS_CC;

float M_Volume = 1.0f;

Scene *Start::createScene() {
    return Start::create();
}

bool Start::init() {
    if (!Scene::init()) {
        return false;
    }
    this->scheduleUpdate();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("images/StartBackground1366.png");
    background->setPosition(winSize.width / 2, winSize.height / 2);
    background->setScale(winSize.width / background->getTextureRect().getMaxX(),
                         winSize.height / background->getTextureRect().getMaxY());
    this->addChild(background, 0);

    MenuItemFont::setFontName("fonts/Sthupo.ttf");
    MenuItemFont::setFontSize(60);

    AudioEngine::lazyInit();
    AudioEngine::preload("music/startBgm.mp3");
    startAudioID = AudioEngine::play2d("music/startBgm.mp3", true);


    //Network init
    delete chclient;
    delete chserver;

    auto exit_img = MenuItemImage::create(
            "exit_0.png",
            "exit_1.png",
            [=](Ref *render) { addChild(ExitLayer::create(), 3); });
    exit_img->setAnchorPoint(Vec2(1, 1));
    auto setting_img = MenuItemImage::create(
            "setting_0.png",
            "setting_1.png",
            [=](Ref *render) {
                SettingLayer *setting = SettingLayer::create();
                addChild(setting, 3);
            });
    setting_img->setAnchorPoint(Vec2(1, 1));
    Vector<MenuItem *> menus{setting_img, exit_img};
    auto menu = Menu::createWithArray(menus);
    addChild(menu, 2);
    menu->setAnchorPoint(Vec2(1, 1));
    menu->setPosition(winSize.width - 30, winSize.height);
    menu->alignItemsHorizontally();

    auto singleGame = MenuItemFont::create("Singleplayer   ", [=](Ref *render) {
        AudioEngine::pause(startAudioID);
        Director::getInstance()->pushScene(
                TransitionFade::create(0.3f, Game::create(), Color3B(0, 255, 255)));
    });
    singleGame->setColor(Color3B(255, 215, 0));
    auto multiGame = MenuItemFont::create("   Multiplayer   ", [=](Ref *render) {
        AudioEngine::pause(startAudioID);
        Director::getInstance()->pushScene(
                TransitionFade::create(0.3f, Server::createScene(), Color3B(0, 255, 255)));
    });
    multiGame->setColor(Color3B(127, 255, 212));
    auto exitGame = MenuItemFont::create("   Exit", [=](Ref *render) {
        Director::getInstance()->end();
    });
    exitGame->setColor(Color3B(127, 255, 0));

    Vector<MenuItem *> choiceMenus{singleGame, multiGame};
    auto choiceMenu = Menu::createWithArray(choiceMenus);
    choiceMenu->setPosition(visibleSize.width / 2, visibleSize.height / 5);
    choiceMenu->alignItemsHorizontally();
    this->addChild(choiceMenu, 1);

    return true;
}

void Start::update(float dt) {
    AudioEngine::setVolume(startAudioID, M_Volume);
}