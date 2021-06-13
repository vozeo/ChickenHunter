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
		hunter_client = new CHClient("127.0.0.1", 25595);
		hunter_client->link();
		hunter_client->setName(nameText->getString().c_str());
		auto* scene = Room::create(true);
		Director::getInstance()->pushScene(TransitionFade::create(0.3f, static_cast<Scene*>(scene), Color3B(0, 255, 255)));
		});
	createRoom->setColor(Color3B(255, 215, 0));
	auto addRoom = MenuItemFont::create("   Add Room", [=](Ref* render) {
		CCLOG("Name:%s IP:%s", nameText->getString().c_str(), addressText->getString().c_str());
		hunter_client = new CHClient(addressText->getString().c_str(), 25595);
		hunter_client->link();
		hunter_client->setName(nameText->getString().c_str());
		
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