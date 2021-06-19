#include "Scene/ChatLayer.h"

USING_NS_CC;

bool ChatLayer::init() {
    if (!Layer::init()) {
        return false;
    }

    Sprite *back = Sprite::create("images/chatBackWhite.png");
    addChild(back, 0);
    back->setAnchorPoint(Vec2(0, 1));
    back->setPosition(Vec2(50, winSize.height - 80));

    chatText = TextField::create("type in your message", "fonts/Marker Felt.ttf", 30);
    chatText->setMaxLength(15);
    chatText->setPosition(Vec2(winSize.width / 8 + 40, winSize.height / 5 - 35));
    chatText->setTextColor(Color4B(0, 0, 0, 255));
    addChild(chatText, 1);

    auto sendButton = Button::create("images/sendNormal.png", "images/sendSelected.png", "images/sendSelected.png");
    sendButton->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED && chatText->getString().size() > 0) {
            chclient->sendChatMessage(chatText->getString().c_str());
            chatText->setString("");
        }
    });
    sendButton->setPosition(Vec2(winSize.width / 3 - 55, winSize.height / 5 - 40));
    addChild(sendButton, 1);

    for (decltype(chats.size()) i = 0; i < 10; ++i) {
        chats.push_back(Label::createWithTTF("", "fonts/Marker Felt.ttf", 30));
        chats[i]->setAnchorPoint(Vec2(0, 0.5f));
        chats[i]->setPosition(winSize.width / 10, winSize.height - 155 - 48 * i);
        chats[i]->setTextColor(Color4B(0, 0, 0, 255));
        addChild(chats[i], 1);
    }

    return true;
}

void ChatLayer::showChat(const char* name, const char* text) {
    std::string nameStr = name;
    std::string textStr = text;

    ++chatNumber;
    if (chatNumber > 10) {
        --chatNumber;
        for (int i = 0; i < chatNumber - 1; ++i)
            chats[i]->setString(chats[i + 1]->getString());
    }
    chats[chatNumber - 1]->setString(nameStr + " : " + textStr);
}