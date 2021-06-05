#include "Scene/StartScene.h"
#include "Scene/GameScene.h"

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

	auto background = Sprite::create("images/StartBackground1024.png");
	background->setAnchorPoint(Vec2(0, 0));
	background->setPosition(origin);
	this->addChild(background, 0);

	MenuItemFont::setFontName("fonts/Sthupo.ttf");
	MenuItemFont::setFontSize(60);

	//auto* chnStrings = CCDictionary::createWithContentsOfFile("CHNStrings.xml");

	auto singleGame = MenuItemFont::create("Singleplayer   ", [=](Ref* render) {
		auto scene = Game::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene, Color3B(0, 255, 255)));
		});
	auto multiGame = MenuItemFont::create("   Multiplayer   ", [=](Ref* render) {
		auto scene = Game::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene, Color3B(0, 255, 255)));
		});
	auto exitGame = MenuItemFont::create("   Exit", [=](Ref* render) {
		Director::getInstance()->end();
		});

	Vector<MenuItem*> menus{ singleGame, multiGame, exitGame };
	auto menu = Menu::createWithArray(menus);
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 5);
	menu->alignItemsHorizontally();
	this->addChild(menu, 1);

	return true;
}