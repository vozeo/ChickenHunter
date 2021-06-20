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

    chat_text = TextField::create("type in your message", "fonts/Marker Felt.ttf", 30);
    chat_text->setMaxLength(15);
    chat_text->setPosition(Vec2(winSize.width / 8 + 40, winSize.height / 5 - 35));
    chat_text->setTextColor(Color4B(0, 0, 0, 255));
    addChild(chat_text, 1);

    auto send_button = Button::create("images/sendNormal.png", "images/sendSelected.png",
                                      "images/sendSelected.png");
    send_button->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED && chat_text->getString().size() > 0) {
            chclient->sendChatMessage(chat_text->getString().c_str());
            chat_text->setString("");
        }
    });
    send_button->setPosition(Vec2(winSize.width / 3 - 55, winSize.height / 5 - 40));
    addChild(send_button, 1);

    for (decltype(chats.size()) i = 0; i < 10; ++i) {
        chats.push_back(Label::createWithTTF("", "fonts/Marker Felt.ttf", 30));
        chats[i]->setAnchorPoint(Vec2(0, 0.5f));
        chats[i]->setPosition(winSize.width / 10, winSize.height - 155 - 48 * i);
        chats[i]->setTextColor(Color4B(0, 0, 0, 255));
        addChild(chats[i], 1);
    }

    return true;
}

void ChatLayer::showChat(const char *name, const char *text) {
    std::string name_str = name;
    std::string text_str = text;

    assert(chats[9] != nullptr);

    ++chat_number;
    if (chat_number > 10) {
        chat_number = 10;
        for (int i = 0; i < 9; ++i) {
            assert(chats[i] && chats[i + 1]);
            chats[i]->setString(chats[i + 1]->getString());
        }
    }
    chats[chat_number - 1]->setString(name_str + " : " + text_str);
}