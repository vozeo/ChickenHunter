#include "Scene/ExitLayer.h"

USING_NS_CC;

bool ExitLayer::init()
{
    if ( !Layer::init() )
    { 
        return false;
    }
	
    Sprite* back = Sprite::create("images/notice.png");
    addChild(back, 0);
    back->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

    auto text = Label::createWithTTF("Do you want to exit?", "fonts/Marker Felt.ttf", 45);
    addChild(text, 1);
    text->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + winSize.height / 10));

	auto choiceYes = MenuItemFont::create("Yes   ", [=](Ref* render) {
		Director::getInstance()->getOpenGLView()->setCursorVisible(true);
		Director::getInstance()->popScene();
		});
	choiceYes->setColor(Color3B(255, 0, 0));

	auto choiceNo = MenuItemFont::create("    No", [=](Ref* render) {
		removeFromParent();
		});
	
	Vector<MenuItem*> menus{ choiceYes, choiceNo };
	auto menu = Menu::createWithArray(menus);
	menu->setPosition(winSize.width / 2, winSize.height / 2.7);
	menu->alignItemsHorizontally();
	addChild(menu, 1);
	
	return true;
}

void ExitLayer::closeCallback(cocos2d::Ref* pSender) {
	removeFromParent();
}