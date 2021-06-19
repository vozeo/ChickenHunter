#include "Scene/RankLayer.h"

USING_NS_CC;

bool RankLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    Sprite *back = Sprite::create("images/ranklist.png");
    addChild(back, 0);
    back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

    auto choiceYes = MenuItemFont::create("Back", [=](Ref *render) {
        Director::getInstance()->getOpenGLView()->setCursorVisible(true);
        Director::getInstance()->popScene();
    });
    auto menu = Menu::create();
    menu->addChild(choiceYes);
    menu->setPosition(winSize.width / 2, winSize.height / 4);
    addChild(menu, 1);

    return true;
}

bool comp(Character *playerA, Character *playerB) {
    return playerA->getPlayerPoint() > playerB->getPlayerPoint();
}

void RankLayer::rankInit(bool isWin, std::vector<Character *> player) {
    auto text = Label::createWithTTF("", "fonts/Marker Felt.ttf", 70);
    if (isWin)
        text->setString("You win!");
    else text->setString("Game Over!");
    text->setPosition(Vec2(winSize.width / 2, winSize.height - winSize.height / 7));
    addChild(text, 1);
    sort(player.begin(), player.end(), comp);
    if (chclient != nullptr) {
        int now_position = 1;
        for (decltype(player.size()) i = 0; i < player.size(); ++i) {
            if (player[i]->m_has_controller) {
                now_position++;
                auto playerLabel = Label::createWithTTF(player[i]->getName() +
                                                        StringUtils::format("     %d",
                                                                            player[i]->getPlayerPoint()),
                                                        "fonts/Marker Felt.ttf", 30);
                playerLabel->setPosition(winSize.width / 2, winSize.height - winSize.height / 4 -
                                                            winSize.height / player.size() / 2 *
                                                            now_position);
                addChild(playerLabel, 1);
            }
        }
    } else//single player mode
    {
        for (decltype(player.size()) i = 0; i < player.size(); ++i) {
            auto playerLabel = Label::createWithTTF(player[i]->getName() +
                                                    StringUtils::format("     %d",
                                                                        player[i]->getPlayerPoint()),
                                                    "fonts/Marker Felt.ttf", 30);
            playerLabel->setPosition(winSize.width / 2, winSize.height - winSize.height / 4 -
                                                        winSize.height / player.size() / 2 * i);
            addChild(playerLabel, 1);
        }
    }
}