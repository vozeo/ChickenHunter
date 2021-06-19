#include "Scene/SettingLayer.h"

USING_NS_CC;

SettingLayer::SettingLayer() = default;

bool SettingLayer::init(Character *hunter) {
    if (!Layer::init()) {
        return false;
    }

    Sprite *back = Sprite::create("images/notice.png");
    addChild(back, 0);
    back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

    //volume
    volume_slider = Slider::create();
    volume_slider->loadBarTexture("images/sliderBack.png");
    volume_slider->loadSlidBallTextures("images/sliderNodeNormal.png",
                                        "images/sliderNodePressed.png",
                                        "images/sliderNodePressed.png");
    volume_slider->loadProgressBarTexture("images/sliderBar.png");

    volume_slider->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == ui::Widget::TouchEventType::ENDED)
            M_Volume = static_cast<float>(volume_slider->getPercent()) / 100.0f;
    });
    volume_slider->setPosition(Vec2(winSize.width / 1.8f, winSize.height / 1.7f));
    volume_slider->setPercent(static_cast<int>(M_Volume * 100.0f));
    volume_slider->setScale(0.5f);
    this->addChild(volume_slider, 1);

    auto volume_label = Label::createWithTTF("Volume", "fonts/Marker Felt.ttf", 30);
    volume_label->setPosition(Vec2(winSize.width / 2.5f, winSize.height / 1.7f));
    addChild(volume_label, 1);

    //back
    auto choice_back = MenuItemFont::create("Back", [=](Ref *render) {
        removeFromParent();
    });
    auto menu = Menu::create();
    menu->addChild(choice_back);
    menu->setPosition(Vec2(winSize.width / 2, winSize.height / 2.9f));
    addChild(menu, 1);

    if (hunter == nullptr)
        return true;

    // add-in
    std::vector<CheckBox *> check_boxes;
    for (int i = 0; i < 4; ++i) {
        auto check_box = CheckBox::create("images/check_box_normal.png",
                                          "images/check_box_normal_press.png",
                                          "images/check_box_active.png",
                                          "images/check_box_normal_disable.png",
                                          "images/check_box_active_disable.png");
        check_boxes.push_back(check_box);
    }

    check_boxes[0]->setSelected(add_in_aiming);
    check_boxes[0]->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            add_in_aiming = !add_in_aiming;
        }
    });

    check_boxes[1]->setSelected(hunter->getPlayerLockedBleed());
    check_boxes[1]->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            hunter->setPlayerLockedBleed(!hunter->getPlayerLockedBleed());
        }
    });

    check_boxes[2]->setSelected(hunter->getPlayerLockedBullet());
    check_boxes[2]->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            hunter->setPlayerLockedBullet(!hunter->getPlayerLockedBullet());
        }
    });

    check_boxes[3]->setSelected(hunter->getPlayerAutoAttack());
    check_boxes[3]->addTouchEventListener([=](Ref *sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            hunter->setPlayerAutoAttack(!hunter->getPlayerAutoAttack());
        }
    });

    std::vector<Label *> addinLabel;
    for (int i = 0; i < 4; ++i) {
        check_boxes[i]->setPosition(
                Vec2(winSize.width / 2 + winSize.width / 13, winSize.height / 1.9f - 30 * i));
        addChild(check_boxes[i], 1);

        addinLabel.push_back(Label::createWithTTF("", "fonts/Marker Felt.ttf", 30));
        addinLabel[i]->setPosition(Vec2(winSize.width / 2, winSize.height / 1.9f - 30 * i));
        addChild(addinLabel[i], 1);
    }
    addinLabel[0]->setString("Add Sight Line");
    addinLabel[1]->setString("Lock Bleed");
    addinLabel[2]->setString("Lock Bullet");
    addinLabel[3]->setString("Auto attack");

    return true;
}

void SettingLayer::closeCallback(cocos2d::Ref *pSender) {
    removeFromParent();
}
