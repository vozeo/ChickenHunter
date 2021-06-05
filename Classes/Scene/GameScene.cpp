#include "GameScene.h"

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }
	hunter = Character::create();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    map = TMXTiledMap::create("maps//map.tmx");
    addChild(map);
    //meta = map->getLayer("Obstacle");
    //meta->setVisible(false);

	initState();

    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(Game::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(Game::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void Game::initState()
{
	auto exit_img = MenuItemImage::create(
		"exit_0.png",
		"exit_1.png",
		CC_CALLBACK_1(Game::menuCloseCallback, this));

	auto exit_menu = Menu::create(exit_img, NULL);
	exit_menu->setPosition(988, 738);
	this->addChild(exit_menu, TOP);
	//auto exit_sprite = Sprite::create("exit_0.png");
	//exit_sprite->setPosition(924, 726);
	//this->addChild(exit_sprite);
	//exit_sprite->setGlobalZOrder(TOP);

	/*创建状态信息进度条*/
	auto blood_back = Sprite::create("images//blood_back.png");

	blood_bar = ui::LoadingBar::create("images//blood.png");

	blood_bar->setDirection(ui::LoadingBar::Direction::LEFT);

	blood_back->setPosition(512, 30);
	blood_bar->setPosition(Vec2(512, 30));

	this->addChild(blood_back, TOP);
	this->addChild(blood_bar, TOP);

	blood_back->setGlobalZOrder(TOP);
	blood_bar->setGlobalZOrder(TOP);

	blood_label = Label::createWithTTF("0", "fonts//Marker Felt.ttf", 25);
	survivor_label = Label::createWithTTF("SURVIVOR : 1", "fonts//Marker Felt.ttf", 30);

	blood_label->setPosition(Vec2(300, 28));
	survivor_label->setPosition(Vec2(80, 750));

	this->addChild(blood_label, TOP);
	this->addChild(survivor_label, TOP);

	blood_label->setGlobalZOrder(TOP);
	survivor_label->setGlobalZOrder(TOP);

	initGun();

	hunter->setPlayerBleed(80);
	updateHunterInfo();
}

void Game::initGun() {
	Vector<MenuItem*> guns;

	for (int i = 0; i < 4; ++i) {
		gun[i][0] = MenuItemImage::create(StringUtils::format("images/gun%d_0.png", i), StringUtils::format("images/gun%d_1.png", i));
		gun[i][1] = MenuItemImage::create(StringUtils::format("images/gun%d_1.png", i), StringUtils::format("images/gun%d_0.png", i));
		guns.pushBack(gun[i][0]);
	}
	guns.popBack();
	guns.pushBack(gun[3][1]);

	auto menu = Menu::createWithArray(guns);
	menu->setPosition(512, 80);
	menu->alignItemsHorizontally();
	this->addChild(menu, TOP);
}

void Game::updateHunterInfo() {  //更新人物信息显示
	blood_bar->setPercent(hunter->getPlayerBleed() * 100.0f / hunter->m_MAX_BLEED);
	blood_label->setString(Value(hunter->getPlayerBleed()).asString());
}

void Game::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d pressed", keyCode);
    if (EventKeyboard::KeyCode::KEY_W == keyCode)
        map->setPositionY(map->getPositionY() - 33);
    else if (EventKeyboard::KeyCode::KEY_S == keyCode)
        map->setPositionY(map->getPositionY() + 33);
    else if (EventKeyboard::KeyCode::KEY_A == keyCode)
        map->setPositionX(map->getPositionX() + 33);
    else if (EventKeyboard::KeyCode::KEY_D == keyCode)
        map->setPositionX(map->getPositionX() - 33);
}

void Game::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d released", keyCode);
}

void Game::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
