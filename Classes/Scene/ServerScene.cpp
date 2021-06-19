#include "Scene/ServerScene.h"
#include <chrono>

using namespace std::chrono;

USING_NS_CC;

Server::Server() = default;

Scene *Server::createScene() {
    return Server::create();
}

bool Server::init() {
    if (!Scene::init()) {
        return false;
    }
    if (chserver != nullptr) {
        delete chserver;
        chserver = nullptr;
    }
    if (chclient != nullptr) {
        delete chclient;
        chclient = nullptr;
    }
    auto background = Sprite::create("images/backstar.png");
    background->setPosition(winSize.width / 2, winSize.height / 2);
    background->setScale(winSize.width / background->getTextureRect().getMaxX(),
                         winSize.height / background->getTextureRect().getMaxY());
    addChild(background, 0);

    address_text = TextField::create("type in IP address", "fonts/Marker Felt.ttf", 40);
    address_text->setMaxLength(19);
    address_text->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        CCLOG("%s", address_text->getString().c_str());
    });
    address_text->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + winSize.height / 5));
    this->addChild(address_text, 1);

    name_text = TextField::create("type in your name", "fonts/Marker Felt.ttf", 40);
    name_text->setMaxLength(9);
    name_text->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        CCLOG("%s", name_text->getString().c_str());
    });
    name_text->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
    this->addChild(name_text, 1);

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


    MenuItemFont::setFontName("fonts/Sthupo.ttf");
    MenuItemFont::setFontSize(60);

    auto create_room = MenuItemFont::create("Create Room   ", [=](Ref *render) {
        if (name_text->getString().length() < 2 || name_text->getString().length() > 9) {
            auto warning_layer = WarningLayer::create(
                    std::string("\nYour name is illegal:\n need to be 2 - 9 words"));
            addChild(warning_layer, 2);
            return;
        }
        CCLOG("!!\n!!\n!!\n!!\nName:%s", name_text->getString().c_str());
        chserver = new CHServer("0.0.0.0", 25595);
        chserver->listen();
        chclient = new CHClient("127.0.0.1", 25595);
        chclient->link();

        system_clock::time_point link_start_time = system_clock::now(); // connect test
        bool get_started = false;
        while (duration_cast<milliseconds>(system_clock::now() - link_start_time).count() < 800) {
            if (chclient->getUid() != 0) {
                get_started = true;
                break;
            }
        }
        if (!get_started) {
            auto warning_layer = WarningLayer::create(
                    std::string("\nCreate Room Error:\n Game Port has been occupied."));
            addChild(warning_layer, 3);
            delete chclient;
            chclient = nullptr;
            delete chserver;
            chserver = nullptr;
            return;
        }
        chclient->setName(name_text->getString().c_str());
        Director::getInstance()->replaceScene(
                TransitionFade::create(0.3f, Room::create(true), Color3B(0, 255, 255)));
    });
    create_room->setColor(Color3B(255, 215, 0));

    auto add_room = MenuItemFont::create("   Enter Room", [=](Ref *render) {
        CCLOG("Name:%s IP:%s", name_text->getString().c_str(), address_text->getString().c_str());
        if (name_text->getString().length() < 2 || name_text->getString().length() > 9) {
            auto warninglayer = WarningLayer::create(
                    std::string("\nYour name is illegal:\n need to be 2 - 9 words"));
            addChild(warninglayer, 2);
            return;
        }
        if (address_text->getString().length() > 19) {
            auto warninglayer = WarningLayer::create(
                    std::string("\nYour name is illegal:\n need to be shorter than 19 words"));
            addChild(warninglayer, 2);
            return;
        } else if (address_text->getString().length() < 2)
            address_text->setString("127.0.0.1");

        chclient = new CHClient(address_text->getString().c_str(), 25595);
        chclient->link();
        system_clock::time_point link_start_time = system_clock::now();
        bool get_started = false;
        while (duration_cast<milliseconds>(system_clock::now() - link_start_time).count() < 800) {
            if (chclient->getUid() != 0) {
                get_started = true;
                break;
            }
        }
        if (!get_started) {
            auto warninglayer = WarningLayer::create(
                    std::string("\nEnter Room Error:\n Can't find certain room!"));
            addChild(warninglayer, 3);
            delete chclient;
            chclient = nullptr;
            return;
        }

        chclient->setName(name_text->getString().c_str());
        Director::getInstance()->replaceScene(
                TransitionFade::create(0.3f, Room::create(true), Color3B(0, 255, 255)));
    });
    add_room->setColor(Color3B(127, 255, 212));

    Vector<MenuItem *> choice_menus{create_room, add_room};
    auto choice_menu = Menu::createWithArray(choice_menus);
    choice_menu->setPosition(winSize.width / 2, winSize.height / 5);
    choice_menu->alignItemsHorizontally();
    addChild(choice_menu, 1);

    return true;
}