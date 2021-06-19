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

    auto background = Sprite::create("images/StartBackground1366.png");
    background->setPosition(winSize.width / 2, winSize.height / 2);
    background->setScale(winSize.width / background->getTextureRect().getMaxX(),
                         winSize.height / background->getTextureRect().getMaxY());
    this->addChild(background, 0);

    MenuItemFont::setFontName("fonts/Sthupo.ttf");
    MenuItemFont::setFontSize(60);

    AudioEngine::lazyInit();
    AudioEngine::preload("music/startBgm.mp3");
    start_audio_ID = AudioEngine::play2d("music/startBgm.mp3", true);


    //Network init

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

    auto single_game = MenuItemFont::create("Singleplayer   ", [=](Ref *render) {
        if (chserver != nullptr) {
            delete chserver;
            chserver = nullptr;
        }
        if (chclient != nullptr) {
            delete chclient;
            chclient = nullptr;
        }
        AudioEngine::pause(start_audio_ID);
        Director::getInstance()->pushScene(
                TransitionFade::create(0.3f, Game::create(), Color3B(0, 255, 255)));
    });
    single_game->setColor(Color3B(255, 215, 0));
    auto multi_game = MenuItemFont::create("   Multiplayer   ", [=](Ref *render) {
        AudioEngine::pause(start_audio_ID);
        Director::getInstance()->pushScene(
                TransitionFade::create(0.3f, Server::createScene(), Color3B(0, 255, 255)));
    });
    multi_game->setColor(Color3B(127, 255, 212));
    auto exit_game = MenuItemFont::create("   Exit", [=](Ref *render) {
        Director::getInstance()->end();
    });
    exit_game->setColor(Color3B(127, 255, 0));

    Vector<MenuItem *> choiceMenus{single_game, multi_game};
    auto choice_menu = Menu::createWithArray(choiceMenus);
    choice_menu->setPosition(winSize.width / 2, winSize.height / 5);
    choice_menu->alignItemsHorizontally();
    this->addChild(choice_menu, 1);

    return true;
}

void Start::update(float dt) {
    AudioEngine::setVolume(start_audio_ID, M_Volume);
}