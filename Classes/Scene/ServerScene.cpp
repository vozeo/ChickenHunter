#include "Scene/ServerScene.h"

USING_NS_CC;

Scene* Server::createScene()
{
    return Server::create();
}

bool Server::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }
	auto background = Sprite::create("images/backstar.png");
	background->setPosition(winSize.width / 2, winSize.height / 2);
	background->setScale(winSize.width / background->getTextureRect().getMaxX(), winSize.height / background->getTextureRect().getMaxY());
	addChild(background, 0);

	addressText = TextField::create("type in IP address", "fonts/Marker Felt.ttf", 40);
	addressText->setMaxLength(19);
	addressText->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		CCLOG("%s", addressText->getString().c_str());
		});
	addressText->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + winSize.height / 5));
	this->addChild(addressText, 1);

	nameText = TextField::create("type in your name", "fonts/Marker Felt.ttf", 40);
	nameText->setMaxLength(9);
	nameText->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		CCLOG("%s", nameText->getString().c_str());
		});
	nameText->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(nameText, 1);

	auto exit_img = MenuItemImage::create(
		"exit_0.png",
		"exit_1.png",
		[=](Ref* render) { addChild(ExitLayer::create(), 3); });
	exit_img->setAnchorPoint(Vec2(1, 1));
	auto setting_img = MenuItemImage::create(
		"setting_0.png",
		"setting_1.png",
		[=](Ref* render) {
			SettingLayer* setting = SettingLayer::create();
			addChild(setting, 3);
		});
	setting_img->setAnchorPoint(Vec2(1, 1));
	Vector<MenuItem*> menus{ setting_img, exit_img };
	auto menu = Menu::createWithArray(menus);
	addChild(menu, 2);
	menu->setAnchorPoint(Vec2(1, 1));
	menu->setPosition(winSize.width - 30, winSize.height);
	menu->alignItemsHorizontally();


	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);
	/*
	AudioEngine::lazyInit();
	AudioEngine::preload("music/startBgm.mp3");
	startAudioID = AudioEngine::play2d("music/startBgm.mp3", true);
	*/

	auto createRoom = MenuItemFont::create("Create Room   ", [=](Ref* render) {
		chserver = new CHServer("0.0.0.0", 25595);
		chserver->listen();
		chclient = new CHClient("127.0.0.1", 25595);
		chclient->link();
		chclient->setName(nameText->getString().c_str());
		Director::getInstance()->replaceScene(TransitionFade::create(0.3f, Room::create(true), Color3B(0, 255, 255)));
		});
	createRoom->setColor(Color3B(255, 215, 0));
	auto addRoom = MenuItemFont::create("   Enter Room", [=](Ref* render) {
		CCLOG("Name:%s IP:%s", nameText->getString().c_str(), addressText->getString().c_str());
		chclient = new CHClient(addressText->getString().c_str(), 25595);
		chclient->link();
		chclient->setName(nameText->getString().c_str());
		Director::getInstance()->replaceScene(TransitionFade::create(0.3f, Room::create(true), Color3B(0, 255, 255)));
		});
	addRoom->setColor(Color3B(127, 255, 212));

	Vector<MenuItem*> choiceMenus{ createRoom, addRoom };
	auto choiceMenu = Menu::createWithArray(choiceMenus);
	choiceMenu->setPosition(winSize.width / 2, winSize.height / 5);
	choiceMenu->alignItemsHorizontally();
	
	this->addChild(choiceMenu, 1);
	return true;
}