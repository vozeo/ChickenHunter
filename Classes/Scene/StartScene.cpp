#include "Scene/StartScene.h"


USING_NS_CC;

Scene* Start::createScene()
{
    return Start::create();
}

bool Start::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }

	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto background = Sprite::create("images/StartBackground1366.png");
	background->setAnchorPoint(Vec2(0, 0));
	background->setPosition(origin);
	this->addChild(background, 0);

	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);

	AudioEngine::lazyInit();
	AudioEngine::preload("music/startBgm.mp3");
	startAudioID = AudioEngine::play2d("music/startBgm.mp3", true);

	//auto* chnStrings = CCDictionary::createWithContentsOfFile("CHNStrings.xml");

	auto singleGame = MenuItemFont::create("Singleplayer   ", [=](Ref* render) {
		AudioEngine::pause(startAudioID);
		auto scene = Game::createScene();
		Director::getInstance()->pushScene(TransitionFade::create(0.3f, scene, Color3B(0, 255, 255)));
		});
	singleGame->setColor(Color3B(255, 215, 0));
	auto multiGame = MenuItemFont::create("   Multiplayer   ", [=](Ref* render) {
		AudioEngine::pause(startAudioID);
		auto scene = Game::createScene();
		Director::getInstance()->pushScene(TransitionFade::create(0.3f, scene, Color3B(0, 255, 255)));
		});
	multiGame->setColor(Color3B(127, 255, 212));
	auto exitGame = MenuItemFont::create("   Exit", [=](Ref* render) {
		Director::getInstance()->end();
		});
	exitGame->setColor(Color3B(127, 255, 0));

	Vector<MenuItem*> menus{ singleGame, multiGame, exitGame };
	auto menu = Menu::createWithArray(menus);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 5);
	menu->alignItemsHorizontally();
	this->addChild(menu, 1);

	return true;
}