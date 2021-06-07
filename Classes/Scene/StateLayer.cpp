#include "StateLayer.h"

USING_NS_CC;

Layer* State::createScene(Character* gameHunter)
{
    return State::create(gameHunter);
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool State::init(Character* gameHunter)
{
    if ( !Layer::init() )
	{
        return false;
    }
	scheduleUpdate();

	hunter = gameHunter;
	initState();
	initGun();

    return true;
}

void State::initState()
{
	auto exit_img = MenuItemImage::create(
	    "exit_0.png",
		"exit_1.png",
		CC_CALLBACK_1(State::menuCloseCallback, this));

	auto exit_menu = Menu::create(exit_img, NULL);
	exit_menu->setPosition(988, 738);
	this->addChild(exit_menu, TOP);

	auto blood_back = Sprite::create("images/blood_back.png");

	blood_bar = ui::LoadingBar::create("images/blood.png");

	blood_bar->setDirection(ui::LoadingBar::Direction::LEFT);

	blood_back->setPosition(512, 30);
	blood_bar->setPosition(Vec2(512, 30));

	this->addChild(blood_back, TOP);
	this->addChild(blood_bar, TOP);

	blood_back->setGlobalZOrder(TOP);
	blood_bar->setGlobalZOrder(TOP);

	blood_label = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 25);
	survivor_label = Label::createWithTTF("SURVIVOR : 1", "fonts/Marker Felt.ttf", 30);

	blood_label->setPosition(Vec2(300, 28));
	survivor_label->setPosition(Vec2(80, 750));

	this->addChild(blood_label, TOP);
	this->addChild(survivor_label, TOP);

	blood_label->setGlobalZOrder(TOP);
	survivor_label->setGlobalZOrder(TOP);


	hunter->setPlayerBleed(80); //
}

void State::initGun() {
	for (int i = 0; i < 4; ++i) {
		gun[i][0] = MenuItemImage::create(StringUtils::format("images/gun%d_0.png", i), StringUtils::format("images/gun%d_1.png", i));
		gun[i][1] = MenuItemImage::create(StringUtils::format("images/gun%d_1.png", i), StringUtils::format("images/gun%d_0.png", i));
		gun[i][0]->retain(); // !!!
		gun[i][1]->retain(); // !!!
	}
	gunMenu = Menu::createWithArray(guns);
	gunMenu->setPosition(winSize.width / 2, 80);
	addChild(gunMenu, TOP);
}

void State::update(float fDelta) {
	if (hunter->getPlayerRefresh()) {
		hunter->setPlayerRefresh(false);
		gunMenu->removeAllChildren();
		int nowWeapon = hunter->getPlayerWeapon();
		for (int i = 0; i < 4; ++i) {
			if (hunter->m_gun[i] != nullptr) {
				assert(gunMenu != nullptr);
				if (i == nowWeapon)
					gunMenu->addChild(gun[i][1]);
				else gunMenu->addChild(gun[i][0]);
			}
		}
		gunMenu->alignItemsHorizontally();
	}
	

	blood_bar->setPercent(hunter->getPlayerBleed() * 100.0f / hunter->m_MAX_BLEED);
	blood_label->setString(Value(hunter->getPlayerBleed()).asString());
}

void State::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();
}
