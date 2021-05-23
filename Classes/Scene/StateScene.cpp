#include "StateScene.h"

USING_NS_CC;

Layer* State::createLayer()
{
    return State::create();
}

bool State::init()
{
    if ( !State::init() )
    { 
        return false;
    }

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create("images/StartBackground1024.png");
	background->setAnchorPoint(Vec2(0, 0));
	background->setPosition(origin);
	this->addChild(background, 0);

	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);

	//auto* chnStrings = CCDictionary::createWithContentsOfFile("CHNStrings.xml");

	Vector<MenuItem*> menus{ singleGame, multiGame, exitGame };
	auto menu = Menu::createWithArray(menus);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 5);
	this->addChild(menu, 1);
	menu->alignItemsHorizontally();

	return true;
}