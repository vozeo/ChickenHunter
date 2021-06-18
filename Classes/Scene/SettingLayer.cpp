#include "Scene/SettingLayer.h"

USING_NS_CC;

bool SettingLayer::init()
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
	volumeSlider->loadSlidBallTextures("images/sliderNode.png", "images/sliderNode.png", "images/sliderNode.png");
	volumeSlider->loadProgressBarTexture("images/sliderBar.png");

	volumeSlider->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
			M_Volume = static_cast<float>(volumeSlider->getPercent()) / 100.0f;
		});
	volumeSlider->setPosition(Vec2(winSize.width / 2, winSize.height / 1.7f));
	volumeSlider->setPercent(static_cast<int>(M_Volume * 100.0f));
	this->addChild(volumeSlider, 1);
	
	auto choiceBack = MenuItemFont::create("Back", [=](Ref* render) {
		removeFromParent();
	});
	auto AddIn1= MenuItemFont::create("Add-in 1", [=](Ref* render) {
		add_in_aiming = !add_in_aiming;
	});
	auto menu = Menu::create();
	auto menu_1= Menu::create();
	menu->addChild(choiceBack);
	menu_1->addChild(AddIn1);
	menu->setPosition(winSize.width / 2, winSize.height / 2.7f);
	menu_1->setPosition(winSize.width / 2, winSize.height / 2.0f);
	addChild(menu, 1);
	addChild(menu_1, 1);

	return true;
}

void SettingLayer::closeCallback(cocos2d::Ref* pSender) {
	removeFromParent();
}
