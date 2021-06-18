#include "Scene/SettingLayer.h"

USING_NS_CC;

bool SettingLayer::init(Character* hunter)
{
    if (!Layer::init())
    {
        return false;
    }

	Sprite* back = Sprite::create("images/notice.png");
	addChild(back, 0);
	back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

	volumeSlider = Slider::create();
	volumeSlider->loadBarTexture("images/sliderBack.png");
	volumeSlider->loadSlidBallTextures("images/sliderNodeNormal.png", "images/sliderNodePressed.png", "images/sliderNodePressed.png");
	volumeSlider->loadProgressBarTexture("images/sliderBar.png");

	volumeSlider->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
			M_Volume = static_cast<float>(volumeSlider->getPercent()) / 100.0f;
		});
	volumeSlider->setPosition(Vec2(winSize.width / 1.8f, winSize.height / 1.7f));
	volumeSlider->setPercent(static_cast<int>(M_Volume * 100.0f));
	volumeSlider->setScale(0.5f);
	this->addChild(volumeSlider, 1);

	auto volumeLabel = Label::createWithTTF("Volume", "fonts/Marker Felt.ttf", 30);
	volumeLabel->setPosition(Vec2(winSize.width / 2.5f, winSize.height / 1.7f));
	addChild(volumeLabel, 1);
	
	auto choiceBack = MenuItemFont::create("Back", [=](Ref* render) {
		removeFromParent();
	});
	auto menu = Menu::create();
	menu->addChild(choiceBack);
	menu->setPosition(Vec2(winSize.width / 2, winSize.height / 2.9f));
	addChild(menu, 1);

	if (hunter == nullptr)
		return true;
	

	std::vector<CheckBox*> checkBoxes;
	for (int i = 0; i < 4; ++i) {
		auto checkbox = CheckBox::create("images/check_box_normal.png",
			"images/check_box_normal_press.png",
			"images/check_box_active.png",
			"images/check_box_normal_disable.png",
			"images/check_box_active_disable.png");
		checkBoxes.push_back(checkbox);
	}

	checkBoxes[0]->setSelected(add_in_aiming);
	checkBoxes[0]->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			add_in_aiming = !add_in_aiming;
			checkBoxes[0]->setSelected(add_in_aiming);
		}
	});

	checkBoxes[1]->setSelected(hunter->getPlayerLockedBleed());
	checkBoxes[1]->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			hunter->setPlayerLockedBleed(!hunter->getPlayerLockedBleed());
			checkBoxes[1]->setSelected(hunter->getPlayerLockedBleed());
		}
	});

	checkBoxes[2]->setSelected(hunter->getPlayerLockedBullet());
	checkBoxes[2]->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			hunter->setPlayerLockedBullet(!hunter->getPlayerLockedBullet());
			checkBoxes[2]->setSelected(hunter->getPlayerLockedBullet());
		}
	});

	checkBoxes[3]->setSelected(hunter->getPlayerAutoAttack());
	checkBoxes[3]->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			hunter->setPlayerAutoAttack(!hunter->getPlayerAutoAttack());
			checkBoxes[3]->setSelected(hunter->getPlayerAutoAttack());
		}
	});
	
	
	std::vector<Label*> addinLabel;
	for (int i = 0; i < 4; ++i) {
		checkBoxes[i]->setPosition(Vec2(winSize.width / 2 + winSize.width / 20, winSize.height / 1.9f - 30 * i));
		addChild(checkBoxes[i], 1);

		addinLabel.push_back(Label::createWithTTF("", "fonts/Marker Felt.ttf", 30));
		addinLabel[i]->setPosition(Vec2(winSize.width / 2 - winSize.width / 20, winSize.height / 1.9f - 30 * i));
		addChild(addinLabel[i], 1);
	}
	addinLabel[0]->setString("Add Sight Line");
	addinLabel[1]->setString("Lock Bleed");
	addinLabel[2]->setString("Lock Bullet");
	addinLabel[3]->setString("Auto attack");

	return true;
}

void SettingLayer::closeCallback(cocos2d::Ref* pSender) {
	removeFromParent();
}
