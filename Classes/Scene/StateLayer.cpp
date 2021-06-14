#include "StateLayer.h"

USING_NS_CC;

Layer* State::createScene(std::vector<Character*> gameHunter)
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
bool State::init(std::vector<Character*> gameHunter)
{
    if ( !Layer::init() )
	{
        return false;
    }
	scheduleUpdate();
	
	m_enemy = gameHunter;
	hunter = m_enemy[0];
	aliveNumber = m_enemy.size();

	initState();
	initGun();

	startTime = system_clock::now();

    return true;
}

void State::initState()
{
	auto blood_back = Sprite::create("images/blood_back.png");

	blood_bar = ui::LoadingBar::create("images/blood.png");

	blood_bar->setDirection(ui::LoadingBar::Direction::LEFT);

	blood_back->setPosition(winSize.width / 2, winSize.height / 25);
	blood_bar->setPosition(Vec2(winSize.width / 2, winSize.height / 25));

	this->addChild(blood_back, 1);
	this->addChild(blood_bar, 1);

	blood_label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 25);
	survivor_label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 30);
	time_label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 30);
	bullet_label = Label::createWithTTF("", "fonts/Marker Felt.ttf", 30);

	blood_label->setPosition(Vec2(winSize.width / 2 - winSize.width / 6.2f, winSize.height / 25));
	survivor_label->setAnchorPoint(Vec2(0, 1));
	survivor_label->setPosition(Vec2(0, winSize.height));

	time_label->setAnchorPoint(Vec2(0.5, 1));
	time_label->setPosition(Vec2(winSize.width / 2, winSize.height));

	bullet_label->setPosition(winSize.width / 4, winSize.height / 25);

	this->addChild(blood_label, 1);
	this->addChild(time_label, 1);
	this->addChild(survivor_label, 1);
	addChild(bullet_label, 1);
}

void State::initGun() {
	for (int i = 0; i < 5; ++i) {
		gun[i][0] = MenuItemImage::create(
			StringUtils::format("images/gun%d_0.png", i),
			StringUtils::format("images/gun%d_1.png", i),
			[=](Ref* render) {
				hunter->setPlayerWeapon(i);
				hunter->setPlayerRefresh(true);
			}
		);
		gun[i][1] = MenuItemImage::create(
			StringUtils::format("images/gun%d_1.png", i),
			StringUtils::format("images/gun%d_0.png", i)
		);
		gun[i][0]->retain(); // !!!
		gun[i][1]->retain(); // !!!
	}
	gunMenu = Menu::createWithArray(guns);
	gunMenu->setPosition(winSize.width / 2, winSize.height / 10);
	addChild(gunMenu, TOP);
}

void State::update(float fDelta) {
	for (auto enemy : m_enemy) {
		if (!enemy->getPlayerDeath() && enemy->getPlayerBleed() <= 0) {
			--aliveNumber;
			enemy->setPlayerDeath(true);
			enemy->setPlayerPoint(getTime());
			if (enemy == hunter) {
				for (auto aliveEnemy : m_enemy) {
					if (!aliveEnemy->getPlayerDeath()) {
						aliveEnemy->setPlayerPoint(getTime() * 4 / 3 + aliveEnemy->getPlayerBullet() / 10 + aliveEnemy->getPlayerBleed() / 10);
					}
				}
				RankLayer* rank = RankLayer::create();
				rank->rankInit(false, m_enemy);
				addChild(rank, 6);
			}
			enemy->removeFromParent();
			if (aliveNumber == 1 && !hunter->getPlayerDeath()) {
				hunter->setPlayerPoint(getTime() + 10);
				RankLayer* rank = RankLayer::create();
				rank->rankInit(true, m_enemy);
				addChild(rank, 6);
			}
		}
	}


	if (hunter->getPlayerRefresh()) {
		hunter->setPlayerRefresh(false);
		gunMenu->removeAllChildren();
		int nowWeapon = hunter->getPlayerWeapon();
		for (int i = 0; i < 5; ++i) {
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

	survivor_label->setString("Survivor : " + Value(aliveNumber).asString());
	int nowTime = getTime();
	int nowSec = nowTime % 60, nowMin = nowTime / 60;
	std::string sec = Value(nowSec).asString();
	if (nowSec < 10)
		sec = "0" + sec;
	std::string min = Value(nowMin).asString();
	if (nowMin < 10)
		min = "0" + min;
	time_label->setString("Time : " + min + ":" + sec);
	bullet_label->setString("Bullet : " + Value(hunter->getPlayerBullet()).asString());
}

int State::getTime() {
	return static_cast<int>(duration_cast<seconds>(system_clock::now() - startTime).count());
}
