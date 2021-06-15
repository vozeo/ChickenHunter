#include "Scene/RoomScene.h"
#include <cstring>
USING_NS_CC;

Scene* Room::createScene(bool isServer)
{
    return Room::create(isServer);
}

bool Room::init(bool isServer)
{
    if ( !Scene::init() )
    { 
        return false;
    }
	auto background = Sprite::create("images/backstar.png");
	background->setPosition(winSize.width / 2, winSize.height / 2);
	background->setScale(winSize.width / background->getTextureRect().getMaxX(), winSize.height / background->getTextureRect().getMaxY());
	addChild(background, 0);


	for (int i = 0; i < m_playerNumber; ++i) {
		playerLabel.push_back(Label::createWithTTF(StringUtils::format("player %d", i), "fonts/Marker Felt.ttf", 30));
		playerLabel[i]->setPosition(winSize.width / 2, winSize.height / 2 + winSize.height / m_playerNumber / 2 * i);
		addChild(playerLabel[i], 1);
	}

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

	if (isServer) {
		startGame = MenuItemFont::create("Start", [=](Ref* render) {
				chserver->startGame();
			});
		startGame->setColor(Color3B(255, 215, 0));

		auto choiceMenu = Menu::create();
		choiceMenu->addChild(startGame);
		choiceMenu->setPosition(winSize.width / 2, winSize.height / 5);
		choiceMenu->alignItemsHorizontally();
		this->addChild(choiceMenu, 1);
	}
	this->scheduleUpdate();
	return true;
}

void Room::update(float fDelta)
{
	if (chserver != nullptr)
	{
		chserver->room_update();
	}

	if (chclient->room.player_num < 1) return;
	for (int i = 0; i < MAX_CONNECTIONS - 1; i++)
	{
		//string s = chclient->room.player_name[i + 1];
		//if (s.length() > 2)
			//playerLabel[i]->setString(s);
		if(chclient->room.player_alive[i + 1])
			playerLabel[i]->setString(chclient->room.player_name[i + 1]);
		else
			playerLabel[i]->setString("Unoccupied");
		if (chclient->isStarted() && (!started))
		{
			started = true;
			CCLOG("GAME STARTED!");
			
			Director::getInstance()->replaceScene(TransitionFade::create(0.3f, Game::create(), Color3B(0, 255, 255)));
		}
	}
}
