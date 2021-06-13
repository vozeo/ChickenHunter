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

	auto addressText = TextField::create("type in IP address", "fonts/Marker Felt.ttf", 40);
	addressText->setMaxLength(20);
	addressText->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		CCLOG("%s", addressText->getString());
		});
	addressText->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + winSize.height / 5));
	this->addChild(addressText, 1);

	auto nameText = TextField::create("type in your name", "fonts/Marker Felt.ttf", 40);
	nameText->setMaxLength(10);
	nameText->addTouchEventListener([=](Ref* sender, Widget::TouchEventType type) {
		CCLOG("%s", nameText->getString());
		});
	nameText->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(nameText, 1);


	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);
	/*
	AudioEngine::lazyInit();
	AudioEngine::preload("music/startBgm.mp3");
	startAudioID = AudioEngine::play2d("music/startBgm.mp3", true);
	*/


	auto createRoom = MenuItemFont::create("Create Room   ", [=](Ref* render) {
		auto* scene = Room::create(true);
		Director::getInstance()->pushScene(TransitionFade::create(0.3f, static_cast<Scene*>(scene), Color3B(0, 255, 255)));
		});
	createRoom->setColor(Color3B(255, 215, 0));
	auto addRoom = MenuItemFont::create("   Add Room", [=](Ref* render) {
		auto scene = Room::create(false);
		Director::getInstance()->pushScene(TransitionFade::create(0.3f, scene, Color3B(0, 255, 255)));
		});
	addRoom->setColor(Color3B(127, 255, 212));

	Vector<MenuItem*> choiceMenus{ createRoom, addRoom };
	auto choiceMenu = Menu::createWithArray(choiceMenus);
	choiceMenu->setPosition(winSize.width / 2, winSize.height / 5);
	choiceMenu->alignItemsHorizontally();
	this->addChild(choiceMenu, 1);

	return true;
}