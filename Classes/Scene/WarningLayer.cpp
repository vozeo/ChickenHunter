#include "Scene/WarningLayer.h"

USING_NS_CC;

WarningLayer::WarningLayer() = default;

bool WarningLayer::init(std::string warning_text) {
    if (!Layer::init()) {
        return false;
    }

    Sprite *back = Sprite::create("images/notice.png");
    addChild(back, 0);
    back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

    auto text = Label::createWithTTF(warning_text, "fonts/Marker Felt.ttf", 45);
    addChild(text, 1);
    text->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + winSize.height / 10));

    auto choice = MenuItemFont::create("OK", CC_CALLBACK_1(WarningLayer::closeCallback, this));

    auto menu = Menu::create();
    menu->addChild(choice);
    menu->setPosition(winSize.width / 2, static_cast<float>(winSize.height / 2.7f));
    addChild(menu, 1);

    return true;
}

void WarningLayer::closeCallback(cocos2d::Ref *pSender) {
    removeFromParent();
}