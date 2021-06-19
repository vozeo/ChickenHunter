#include "Scene/RoomScene.h"

USING_NS_CC;

Room::Room() = default;

Scene *Room::createScene(bool is_server) {
    return Room::create(is_server);
}

bool Room::init(bool is_server) {
    if (!Scene::init()) {
        return false;
    }
    auto background = Sprite::create("images/backstar.png");
    background->setPosition(winSize.width / 2, winSize.height / 2);
    background->setScale(winSize.width / background->getTextureRect().getMaxX(),
                         winSize.height / background->getTextureRect().getMaxY());
    addChild(background, 0);


    for (int i = 0; i < m_player_number; ++i) {
        player_label.push_back(
                Label::createWithTTF(StringUtils::format("player %d", i), "fonts/Marker Felt.ttf",
                                     30));
        player_label[i]->setPosition(winSize.width / 2,
                                     winSize.height / 2 + winSize.height / m_player_number / 2 * i);
        addChild(player_label[i], 1);
    }
    player_label[m_player_number - 1]->setVisible(false);
    auto exit_img = MenuItemImage::create(
            "exit_0.png",
            "exit_1.png",
            [=](Ref *render) {
                addChild(ExitLayer::create(), 3);
            });
    exit_img->setAnchorPoint(Vec2(1, 1));
    auto setting_img = MenuItemImage::create(
            "setting_0.png",
            "setting_1.png",
            [=](Ref *render) {
                addChild(SettingLayer::create(), 3);
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

    if (chserver != nullptr) {
        start_game = MenuItemFont::create("Start", [=](Ref *render) {
            chserver->startGame();
        });
        start_game->setColor(Color3B(255, 215, 0));

        auto choice_menu = Menu::create();
        choice_menu->addChild(start_game);
        choice_menu->setPosition(winSize.width / 2, winSize.height / 5);
        choice_menu->alignItemsHorizontally();
        this->addChild(choice_menu, 1);

        auto add_AI = MenuItemFont::create("Add AI", [=](Ref *render) {
            if (!chserver->addAi()) {
                auto warninglayer = WarningLayer::create(
                        std::string("\nYour Room is already full."));
                addChild(warninglayer, 2);
                return;
            }
            CCLOG("AI ADD SUCCESSFUL");
        });
        auto delete_AI = MenuItemFont::create("Delete AI", [=](Ref *render) {
            if (!chserver->deleteAi()) {
                auto warninglayer = WarningLayer::create(
                        std::string("\nThere is no ai player."));
                addChild(warninglayer, 2);
                return;
            }
            CCLOG("AI DELETE SUCCESSFUL");
        });
        delete_AI->setColor(Color3B(255, 0, 0));

        Vector<MenuItem *> menus_AI{add_AI, delete_AI};
        auto menu_AI = Menu::createWithArray(menus_AI);
        menu_AI->setPosition(4 * winSize.width / 5, winSize.height / 4);
        menu_AI->alignItemsVertically();
        addChild(menu_AI, 1);
    }
    this->scheduleUpdate();
    return true;
}

void Room::update(float fDelta) {
    if (chserver != nullptr) {
        chserver->roomUpdate();
    }

    if (chclient->m_room.player_num < 1) return;
    for (int i = 0; i < MAX_CONNECTIONS - 1; i++) {
        if (chclient->m_room.player_alive[i + 1])
            player_label[i]->setString(chclient->m_room.player_name[i + 1]);
        else
            player_label[i]->setString("Unoccupied");
        if (chclient->isStarted() && (!started)) {
            started = true;
            CCLOG("GAME STARTED!");
            Director::getInstance()->replaceScene(
                    TransitionFade::create(0.3f, Game::create(), Color3B(0, 255, 255)));
        }
    }
}
