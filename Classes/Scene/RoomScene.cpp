#include "Scene/RoomScene.h"

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

	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);
	/*
	AudioEngine::lazyInit();
	AudioEngine::preload("music/startBgm.mp3");
	startAudioID = AudioEngine::play2d("music/startBgm.mp3", true);
	*/

	if (isServer) {
		auto startGame = MenuItemFont::create("Start", [=](Ref* render) {
			auto* scene = Game::create();
			Director::getInstance()->pushScene(TransitionFade::create(0.3f, static_cast<Scene*>(scene), Color3B(0, 255, 255)));
			});
		startGame->setColor(Color3B(255, 215, 0));

		auto choiceMenu = Menu::create();
		choiceMenu->addChild(startGame);
		choiceMenu->setPosition(winSize.width / 2, winSize.height / 5);
		choiceMenu->alignItemsHorizontally();
		this->addChild(choiceMenu, 1);
	}

	return true;
}