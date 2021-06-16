#include "MapLayer.h"

#include <utility>

USING_NS_CC;

Layer* MapLayer::createScene(std::vector<Character*> &gameHunter)
{
	return MapLayer::create(gameHunter);
}

// 0->map, 1->item/gun, 2->enemy, 3->bullet, 4->hunter
bool MapLayer::init(std::vector<Character*> &gameHunter)
{
	if (!Layer::init())
	{
		return false;
	}
	this->scheduleUpdate();

	map = TMXTiledMap::create("map/Desert.tmx");
	addChild(map, 0);

	meta = map->getLayer("water");

	mapHeight = map->getMapSize().height;
	mapWidth = map->getMapSize().width;
	tileWidth = map->getTileSize().width;
	tileHeight = map->getTileSize().height;

	m_enemy = gameHunter;
	if (chclient != nullptr)
		hunter = m_enemy[chclient->getUid() - 1];
	else
		hunter = m_enemy[0];
	if (chclient != nullptr && chserver == nullptr)
		initSetItemForClient();
	else
		initSetItem();
	initBullet();

	if(chclient == nullptr)
		schedule(CC_SCHEDULE_SELECTOR(MapLayer::enemyFire), 0.5);
	if (chserver != nullptr)
	{
		MapInformationInit mii;
		mii.is_updated = true;
		for (int i = 0; i < m_ammunition_number; i++)
		{
			auto posa = m_ammunition[i]->getPosition();
			mii.m_ammunition_position[i][0] = posa.x, mii.m_ammunition_position[i][1] = posa.y;
		}
		for (int i = 0; i < m_bandage_number; i++)
		{
			auto posa = m_bandage[i]->getPosition();
			mii.m_bandage_position[i][0] = posa.x, mii.m_bandage_position[i][1] = posa.y;
		}
		for (int i = 0; i < m_weapon_number; i++)
		{
			auto posa = weapons[i]->getPosition();
			mii.m_weapon_position[i][0] = posa.x, mii.m_weapon_position[i][1] = posa.y;
			mii.m_weapon_type[i] = weapons[i]->getWeaponType();
		}
		chserver->mapInformationInit(mii);
	}

	AudioEngine::lazyInit();
	AudioEngine::preload("music/bulletEffect.wav");
	AudioEngine::preload("music/knifeEffect.wav");

	registerKeyboardEvent();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	registerTouchEvent();
#endif
	
	return true;
}

void MapLayer::initBullet() {

}

//fix the animation problem
void MapLayer::registerKeyboardEvent() {
	auto listener = EventListenerKeyboard::create();

	listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->m_speed[0] = true;
			if (!hunter->m_speed[2] && !hunter->m_speed[3])
				hunter->runAction(hunter->getCharacterAnimRight());
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->m_speed[1] = true;
			if (!hunter->m_speed[2] && !hunter->m_speed[3])
				hunter->runAction(hunter->getCharacterAnimLeft());
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->m_speed[2] = true;
			if (!hunter->m_speed[0] && !hunter->m_speed[1])
				hunter->runAction(hunter->getCharacterAnimDown());
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->m_speed[3] = true;
			if (!hunter->m_speed[0] && !hunter->m_speed[1])
				hunter->runAction(hunter->getCharacterAnimUp());
			break;
		case EventKeyboard::KeyCode::KEY_E:
			if (chclient != nullptr)
				chclient->m_localaction.pick = true;
			judgePick(hunter);
			break;
		case EventKeyboard::KeyCode::KEY_SPACE:
			roll(hunter);
			break;
		default:
			break;
		}
	};

	listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
		hunter->stopAllActions();
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_D:
			hunter->m_speed[0] = false;
			break;
		case EventKeyboard::KeyCode::KEY_A:
			hunter->m_speed[1] = false;
			break;
		case EventKeyboard::KeyCode::KEY_S:
			hunter->m_speed[2] = false;
			break;
		case EventKeyboard::KeyCode::KEY_W:
			hunter->m_speed[3] = false;
			break;
		default:
			break;
		}
		for (int i = 0; i <= 3; i++)
		{
			if (hunter->m_speed[i])
			{
				switch (i) {
				case 0: hunter->runAction(hunter->getCharacterAnimRight());
					break;
				case 1: hunter->runAction(hunter->getCharacterAnimLeft());
					break;
				case 2: hunter->runAction(hunter->getCharacterAnimDown());
					break;
				case 3: hunter->runAction(hunter->getCharacterAnimUp());
					break;
				default:
					break;
				}
				break;
			}
		}
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void MapLayer::roll(Character* character) {
	float dx = 0, dy = 0;
	if (character->m_speed[0])
	{
		dx += 40;
	}
	if (character->m_speed[1]) {
		dx += -40;
	}
	if (character->m_speed[2]) {
		dy += -40;
	}
	if (character->m_speed[3]) {
		dy += 40;
	}
	CCLOG("roll");
	character->runAction(MoveBy::create(0.1f, Vec2(dx, dy)));
}

void MapLayer::judgePick(Character* character) {
	Rect rect_character = character->getBoundingBox();
	for (auto weapon : weapons) {
		if (weapon->getBoundingBox().intersectsRect(rect_character)) {
			auto weaponType = weapon->getWeaponType();
			if (character->m_gun[weaponType] == nullptr) {
				weapon->retain();
				character->m_gun[weaponType] = weapon;
				character->setPlayerWeapon(weaponType);
				character->setPlayerRefresh(true);
				weapon->removeFromParent();
				weapons.erase(find(weapons.begin(), weapons.end(), weapon));
				break;
			}
		}
	}

	for (auto bn : m_bandage)
	{
		if (bn->getBoundingBox().intersectsRect(rect_character))
		{
			int bleed = character->getPlayerBleed() + bn->getRecoverHP();
			if (bleed > character->getMAXBLEED())
				bleed = character->getMAXBLEED();
			character->setPlayerBleed(bleed);
			bn->removeFromParent();
			m_bandage.erase(find(m_bandage.begin(), m_bandage.end(), bn));
			break;
		}
	}

	for (auto am : m_ammunition)
	{
		if (am->getBoundingBox().intersectsRect(rect_character))
		{
			character->setPlayerBullet(character->getPlayerBullet() + am->getAddBullet());
			am->removeFromParent();
			m_ammunition.erase(find(m_ammunition.begin(), m_ammunition.end(), am));
			break;
		}
	}
}

// Android
void MapLayer::touchBegan(Touch* touch) {
	auto weaponType = hunter->getPlayerWeapon();
	if (4 == weaponType) {
		auto knifeAudioID = AudioEngine::play2d("music/knifeEffect.mp3", false);
		AudioEngine::setVolume(knifeAudioID, M_Volume);
		makeKnifeAttack(hunter);
		return;
	}
	Fire(0);
	schedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire), hunter->getBulletSpeed() - hunter->m_gun[hunter->getPlayerWeapon()]->getFireWeaponSpeed());
}
// Android
void MapLayer::touchEnded() {
	unschedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire));
}
// Android
void MapLayer::bindTouchMap(std::function<void(MapLayer*, Touch* touch)>& began, std::function<void(MapLayer*)>& ended) {
	began = &MapLayer::touchBegan;
	ended = &MapLayer::touchEnded;
}

//add firing continuously
void MapLayer::registerTouchEvent() {
	auto touchListener = EventListenerTouchOneByOne::create();

	touchListener->onTouchBegan = [&](Touch* touch, Event* event) {
		auto weaponType = hunter->getPlayerWeapon();
		if (4 == weaponType) {
			auto knifeAudioID = AudioEngine::play2d("music/knifeEffect.mp3", false);
			AudioEngine::setVolume(knifeAudioID, M_Volume);
			makeKnifeAttack(hunter);
			return true;
		}

		hunter->bulletLocation = touch->getLocation();
		Fire(0);
		schedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire), hunter->getBulletSpeed() - hunter->m_gun[hunter->getPlayerWeapon()]->getFireWeaponSpeed());
		return true;
	};

	touchListener->onTouchMoved = [&](Touch* touch, Event* event) {
		//CCLOG("%f %f", touch->getLocation().x, touch->getLocation().y);
		hunter->bulletLocation = touch->getLocation();
	};

	touchListener->onTouchEnded = [&](Touch* touch, Event* event) {
		unschedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire));
		return true;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void MapLayer::makeKnifeAttack(Character* character) {
	Vec2 pos = character->getPosition();
	showEffect(pos);
	for (auto enemy : m_enemy) {
		if (enemy == character || enemy->getPlayerDeath())
			continue;
		Vec2 enemyPos = enemy->getPosition();
		if (enemyPos.getDistance(pos) < 100) {
			auto bleed = enemy->getPlayerBleed() - 5 * character->getPlayerAttack() * enemy->getPlayerDefense();
			if (bleed < 0)
				bleed = 0;
			enemy->setPlayerBleed(static_cast<int>(bleed));
			showAttacked(enemy->getPosition());
		}
	}
}

void MapLayer::makeBulletAttack(Character* character, Weapon* weapon, float bulletX, float bulletY) {
	float time = sqrt(bulletX * bulletX + bulletY * bulletY) / 1000;
	float delta = 1;
	int f = 0, fmax = 1;
	auto angle = static_cast<float>(atan(0.15) * 180 / PI);
	auto rot1 = calRotation(bulletX, bulletY) + angle;
	auto rot2 = calRotation(bulletX, bulletY) - angle;
	if (character == hunter)
		delta = 0.6f;
	if (weapon->getWeaponType() == 2)
	{
		fmax = 3;
	}
	for (auto bullet : bullets) {
		if (!bullet->getBulletActive()) {
			bullet->setBulletActive(true);
			bullet->setPosition(character->getPositionX() + bulletX / time / 20, character->getPositionY() + bulletY / time / 20);
			switch (f)
			{
				case 0: bullet->setRotation(calRotation(bulletX, bulletY));
					bullet->runAction(RepeatForever::create(MoveBy::create(delta * weapon->getWeaponSpeed(), Vec2(bulletX / time, bulletY / time)))); 
					break;
				case 1: 
					bullet->setRotation(rot1);
					bullet->runAction(RepeatForever::create(MoveBy::create(delta * weapon->getWeaponSpeed(), Vec2(static_cast<float>(cos(rot1 * PI / 180)) * 1000, static_cast<float>(-sin(rot1 * PI / 180)) * 1000))));
					break;
				case 2: 
					bullet->setRotation(rot2);
					bullet->runAction(RepeatForever::create(MoveBy::create(delta * weapon->getWeaponSpeed(), Vec2(static_cast<float>(cos(rot2 * PI / 180)) * 1000, static_cast<float>(-sin(rot2 * PI / 180)) * 1000))));
					break;
				default:
					break;
			}
			
			bullet->setBulletAttack(static_cast<int>(weapon->getWeaponAttack() * character->getPlayerAttack()));
			bullet->setVisible(true);
			f++;
			if (f >= fmax)
				break;
		}
	}

}

void MapLayer::Fire(float dt)
{
	Weapon* weapon = hunter->m_gun[hunter->getPlayerWeapon()];
	if (hunter->getPlayerBullet() > 0 && weapon->getWeaponType() != 2)
		hunter->setPlayerBullet(hunter->getPlayerBullet() - 1);
	else if(hunter->getPlayerBullet() > 2 && weapon->getWeaponType() == 2)
		hunter->setPlayerBullet(hunter->getPlayerBullet() - 3);
	else return;
	auto bulletAudioID = AudioEngine::play2d("music/bulletEffect.mp3", false);
	AudioEngine::setVolume(bulletAudioID, M_Volume);
	Vec2 bulletLocation = hunter->bulletLocation;
	auto bulletX = bulletLocation.x - winSize.width / 2;
	auto bulletY = bulletLocation.y - winSize.height / 2;
	makeBulletAttack(hunter, weapon, bulletX, bulletY);
}

float MapLayer::calRotation(float bulletX, float bulletY) {
	if (bulletX == 0 && bulletY > 0)
		return -90.0f;
	else if (bulletX == 0 && bulletY < 0)
		return 90.0f;
	else if (bulletX > 0)
		return -180.0f * atan(bulletY / bulletX) / PI;
	else return -180.0f * atan(bulletY / bulletX) / PI + 180.0f;
}

void MapLayer::showEffect(Vec2 pos) {
	auto effectCircle = DrawNode::create();
	addChild(effectCircle, 2);
	effectCircle->drawSolidCircle(pos, 100.0f, CC_DEGREES_TO_RADIANS(360), 15, Color4F(0.28f, 0.46f, 1.0f, 0.6f));
	effectCircle->runAction(Sequence::create(FadeOut::create(0.5f), RemoveSelf::create(), NULL));
}

void MapLayer::showAttacked(Vec2 pos) {
	auto effectCircle = DrawNode::create();
	addChild(effectCircle, 5);
	effectCircle->drawSolidCircle(pos, 20.0f, CC_DEGREES_TO_RADIANS(360), 15, Color4F(1.0f, 0, 0, 0.6f));
	effectCircle->runAction(Sequence::create(FadeOut::create(0.3f), RemoveSelf::create(), NULL));
}

int action_activated[MAX_CONNECTIONS] = { 0 };

void MapLayer::update(float fDelta) {
	if (chclient != nullptr)
	{
		if (chserver != nullptr)
		{
			//local hunter directly given to the server
			chserver->paction[1].speed[0] = hunter->m_speed[0];
			chserver->paction[1].speed[1] = hunter->m_speed[1];
			chserver->paction[1].speed[2] = hunter->m_speed[2];
			chserver->paction[1].speed[3] = hunter->m_speed[3];
			chserver->paction[1].pick = chclient->m_localaction.pick;
			chclient->m_localaction.pick = false;
			//server's update
			chserver->mapUploadInit();
			chserver->m_map_trans.player_left_num = 0;
			for (int i = 1; i < MAX_CONNECTIONS; i++)
			{
				if (m_enemy[i - 1]->getPlayerBleed() > 0)
				{
					//CCLOG("UPDATEING PLAYER#%d", i);
					chserver->m_map_trans.player[i].alive = true;
					float dx = 0, dy = 0;
					chserver->m_map_trans.player_left_num++;
					if (i != 1 && chserver->paction[i].speed[0] == 0 && chserver->paction[i].speed[1] == 0 && chserver->paction[i].speed[2] == 0 && chserver->paction[i].speed[3] == 0)
					{
						action_activated[i - 1] = 0;
						m_enemy[i - 1]->stopAllActions();
					}
					if (chserver->paction[i].speed[0])
					{
						if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 1)
						{
							action_activated[i - 1] = 1;
							m_enemy[i - 1]->stopAllActions();
							m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimRight());
						}
						dx = 4;
					}
					if (chserver->paction[i].speed[1])
					{
						if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 2)
						{
							action_activated[i - 1] = 2;
							m_enemy[i - 1]->stopAllActions();
							m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimLeft());
						}
						dx = -4;
					}
					if (chserver->paction[i].speed[2])
					{
						if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 3)
						{
							action_activated[i - 1] = 3;
							m_enemy[i - 1]->stopAllActions();
							m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimDown());
						}
						dy = -4;
					}
					if (chserver->paction[i].speed[3])
					{
						if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 4)
						{
							action_activated[i - 1] = 4;
							m_enemy[i - 1]->stopAllActions();
							m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimUp());
						}
						dy = 4;
					}
					if (dx == 0 && dy == 0)
						continue;

					auto enemyPos = m_enemy[i - 1]->getPosition();
					if (enemyPos.x < -10 || enemyPos.x > mapWidth * tileWidth + 9
						|| enemyPos.y < -10 || enemyPos.y > mapHeight * tileHeight + 9)
						m_enemy[i - 1]->setPlayerBleed(0);

					auto nextX = enemyPos.x + dx;
					auto nextY = enemyPos.y + dy;
					auto nextMapX = nextX / tileWidth;
					auto nextMapY = mapHeight - nextY / tileHeight;
					if (nextMapX < mapWidth && nextMapX >= 0 && nextMapY < mapHeight && nextMapY >= 0
						&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
					{
						m_enemy[i - 1]->runAction(MoveTo::create(0, Vec2(nextX, nextY)));
					}
					else
					{
						m_enemy[i - 1]->runAction(MoveTo::create(0, Vec2(nextX - 2 * dx, nextY - 2 * dy)));
					}
					
					
					//CCLOG("UPDATE COMPLETE");
				}
				//memset(&chserver->paction[i], 0, sizeof(PlayerAction));
			}
			//服务端地图上传
			for (int i = 1; i < MAX_CONNECTIONS; i++)
			{
				auto pos = m_enemy[i - 1]->getPosition();
				chserver->m_map_trans.player[i].position_x = pos.x, chserver->m_map_trans.player[i].position_y = pos.y;
				chserver->m_map_trans.player[i].hp = m_enemy[i - 1]->getPlayerBleed();
				chserver->m_map_trans.player[i].is_pick = chserver->paction[i].pick;
				chserver->m_map_trans.player[i].bullet = m_enemy[i - 1]->getPlayerBullet();
				if (chserver->paction[i].pick && !m_enemy[i - 1]->getPlayerDeath())
				{
					CCLOG("#MAP_UPDATE# PLAYER#%d PICK", i);
					judgePick(m_enemy[i - 1]);
				}
				
				memset(&chserver->paction[i], 0, sizeof(PlayerAction));//人物动作删除
			}
			chserver->mapUpload();
		}
		else
		{
			
			//本地操作上传
			chclient->m_localaction.speed[0] = hunter->m_speed[0];
			chclient->m_localaction.speed[1] = hunter->m_speed[1];
			chclient->m_localaction.speed[2] = hunter->m_speed[2];
			chclient->m_localaction.speed[3] = hunter->m_speed[3];
			
			chclient->upload();
			MapInformation& current_map = chclient->m_map;
			if (current_map.is_updated)
			{
				for (int i = 0; i < MAX_CONNECTIONS - 1; i++)
					if (current_map.player[i + 1].alive)
					{
						if (!(current_map.player[i + 1].position_x != save_map.player[i + 1].position_x || current_map.player[i + 1].position_y != save_map.player[i + 1].position_y) && m_enemy[i] != hunter)
						{
							action_activated[i + 1] = 0;
							m_enemy[i]->stopAllActions();
						}
						if (current_map.player[i + 1].position_x > save_map.player[i + 1].position_x && m_enemy[i] != hunter  && current_map.is_updated && save_map.is_updated && action_activated[i + 1] != 1)
						{
							action_activated[i + 1] = 1;
							CCLOG("ENEMY#%d RIGHT", i);
							m_enemy[i]->stopAllActions();
							m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimRight());
						}
						if (current_map.player[i + 1].position_x < save_map.player[i + 1].position_x && m_enemy[i] != hunter && current_map.is_updated && save_map.is_updated && action_activated[i + 1] != 2)
						{
							action_activated[i + 1] = 2;
							//CCLOG("ENEMY#%d LEFT", i);
							m_enemy[i]->stopAllActions();
							m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimLeft());
						}
						if (current_map.player[i + 1].position_y > save_map.player[i + 1].position_y && m_enemy[i] != hunter  && current_map.is_updated && save_map.is_updated && action_activated[i + 1] != 3)
						{
							action_activated[i + 1] = 3;
							//CCLOG("ENEMY#%d UP", i);
							m_enemy[i]->stopAllActions();
							m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimUp());
						}
						if (current_map.player[i + 1].position_y < save_map.player[i + 1].position_y && m_enemy[i] != hunter && current_map.is_updated && save_map.is_updated && action_activated[i + 1] != 4)
						{
							action_activated[i + 1] = 4;
							//CCLOG("ENEMY#%d DOWN", i);
							m_enemy[i]->stopAllActions();
							m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimDown());
						}
						//if(m_enemy[i])
						m_enemy[i]->runAction(MoveTo::create(0, Vec2(current_map.player[i + 1].position_x, current_map.player[i + 1].position_y)));
						m_enemy[i]->setPlayerBleed(current_map.player[i + 1].hp);
						m_enemy[i]->setPlayerBullet(current_map.player[i + 1].bullet);
						//拾取东西的同步
						if (current_map.player[i + 1].is_pick)
						{
							judgePick(m_enemy[i]);
						}
					}
				save_map = current_map;
			}
	
			memset(&chclient->m_map, 0, sizeof(MapInformation));
		}
	}
	else//单机版游戏逻辑
	{
		for (auto bullet : bullets) {
			if (bullet->getBulletActive()) {
				auto bulletX = bullet->getPositionX();
				auto bulletY = bullet->getPositionY();
				if (bulletX < 0 || bulletX >= mapWidth * tileWidth || bulletY < 0 || bulletY >= mapHeight * tileHeight
					|| meta->getTileGIDAt(Vec2(bulletX / tileWidth, mapHeight - bulletY / tileHeight))) {
					bullet->setVisible(false);
					bullet->stopAllActions();
					bullet->setBulletActive(false);
					continue;
				}
				Rect rect_bullet = bullet->getBoundingBox();
				for (auto enemy : m_enemy) {
					if (enemy->getPlayerDeath())
						continue;
					Rect rect_enemy = enemy->getBoundingBox();
					if (rect_enemy.intersectsRect(rect_bullet)) {
						showAttacked(enemy->getPosition());
						auto bleed = enemy->getPlayerBleed() - bullet->getBulletAttack() * enemy->getPlayerDefense();
						if (bleed < 0)
							bleed = 0;
						enemy->setPlayerBleed(static_cast<int>(bleed));
						bullet->setVisible(false);
						bullet->stopAllActions();
						bullet->setBulletActive(false);
					}
				}
			}
		}
		for (auto enemy : m_enemy)
		{
			if (enemy->getPlayerDeath())
				continue;
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
			judgePick(enemy);
			if (enemy != hunter) {
#else
			if (enemy != hunter) {
				judgePick(enemy);
#endif
				int nextT = enemy->getThought() + int(fDelta * 1000);
				enemy->setThought(nextT);
				if (nextT >= enemy->getThinkTime())
				{
					enemy->setThought(0);
					enemy->m_speed[0] = random(0, 1);
					enemy->m_speed[1] = random(0, 1);
					enemy->m_speed[2] = random(0, 1);
					enemy->m_speed[3] = random(0, 1);
					enemy->stopAllActions();
				}
			}
			float dx = 0, dy = 0;
			if (enemy->m_speed[0])
			{
				if (enemy != hunter && enemy->getThought() == 0)
					enemy->runAction(enemy->getCharacterAnimRight());
				dx = 4;
			}
			if (enemy->m_speed[1]) {
				if (enemy != hunter && enemy->getThought() == 0)
					enemy->runAction(enemy->getCharacterAnimLeft());
				dx = -4;
			}
			if (enemy->m_speed[2]) {
				if (enemy != hunter && enemy->getThought() == 0)
					enemy->runAction(enemy->getCharacterAnimDown());
				dy = -4;
			}
			if (enemy->m_speed[3]) {
				if (enemy != hunter && enemy->getThought() == 0)
					enemy->runAction(enemy->getCharacterAnimUp());
				dy = 4;
			}

			auto enemyPos = enemy->getPosition();

			if (enemyPos.x < -10 || enemyPos.x > mapWidth * tileWidth + 9
				|| enemyPos.y < -10 || enemyPos.y > mapHeight * tileHeight + 9)
				enemy->setPlayerBleed(0);

			auto nextX = enemyPos.x + dx;
			auto nextY = enemyPos.y + dy;

			auto nextMapX = nextX / tileWidth;
			auto nextMapY = mapHeight - nextY / tileHeight;

			if (nextMapX < mapWidth && nextMapX >= 0 && nextMapY < mapHeight && nextMapY >= 0
				&& !meta->getTileGIDAt(Vec2(nextMapX, nextMapY)))
				enemy->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX, nextY)));
			else
			{
				enemy->m_speed[0] = !enemy->m_speed[0];
				enemy->m_speed[1] = !enemy->m_speed[1];
				enemy->m_speed[2] = !enemy->m_speed[2];
				enemy->m_speed[3] = !enemy->m_speed[3];
				enemy->stopAllActions();
				enemy->runAction(MoveTo::create(1.0f / 80.0f, Vec2(nextX - 2 * dx, nextY - 2 * dy)));
			}
		}
		hunter->setPosition(hunter->getPosition());
	}
}

//set enemies/items randomly and at anywhere except water space.
template <class T>
void MapLayer::setRandPos(T* elem)
{
	decltype(mapWidth) rx, ry, mrx, mry;
	while (true)
	{
		rx = static_cast<decltype(tileWidth)>(random(50, static_cast<int>(mapWidth * tileWidth - 50 - 1)));
		mrx = rx / tileWidth;
		ry = static_cast<decltype(tileHeight)>(random(50, static_cast<int>(mapHeight * tileHeight - 50 - 1)));
		mry = mapHeight - ry / tileWidth;
		if (mrx < mapWidth && mrx >= 0 && mry < mapHeight && mry >= 0 && !meta->getTileGIDAt(Vec2(mrx, mry)))
			break;
	}
	elem->setPosition(Vec2(rx, ry));
}

template <class T>
void MapLayer::initItem(std::vector<T*>& items, int number) {
	items.clear();
	for (int i = 0; i < number; i++)
	{
		items.push_back(T::create());
		addChild(items[i], 1);
		setRandPos(items[i]);
	}
}

/*while(!(chclient->getMapInitState() == false && chclient->m_map_information_init.is_updated == true))
	{
		chclient->setMapInited();
		for (int i = 0; i < m_ammunition_number; i++)
			m_ammunition[i]->setPosition(chclient->m_map_information_init.m_ammunition_position[i][0], chclient->m_map_information_init.m_ammunition_position[i][1]);
		for (int i = 0; i < m_weapon_number; i++)
		{
			weapons[i]->setPosition(chclient->m_map_information_init.m_weapon_position[i][0], chclient->m_map_information_init.m_weapon_position[i][1]);
			weapons[i]->setWeaponType(chclient->m_map_information_init.m_weapon_type[i]);
			weapons[i]->weaponInit(weapons[i]->getWeaponType(), i);
		}
		for (int i = 0; i < m_bandage_number; i++)
			m_bandage[i]->setPosition(chclient->m_map_information_init.m_bandage_position[i][0], chclient->m_map_information_init.m_bandage_position[i][1]);
	}*/

void MapLayer::initSetItemForClient()
{
	for (auto enemy : m_enemy) {
		addChild(enemy, 2);
		setRandPos(enemy);
	}

	hunter->setLocalZOrder(4);

	runAction(Follow::create(hunter, Rect::ZERO));
	initItem(m_bandage, m_bandage_number);
	initItem(m_ammunition, m_ammunition_number);
	while (!chclient->m_map_information_init.is_updated)
		;
	for (int i = 0; i < m_ammunition_number; i++)
		m_ammunition[i]->setPosition(chclient->m_map_information_init.m_ammunition_position[i][0], chclient->m_map_information_init.m_ammunition_position[i][1]);
	for (int i = 0; i < m_bandage_number; i++)
		m_bandage[i]->setPosition(chclient->m_map_information_init.m_bandage_position[i][0], chclient->m_map_information_init.m_bandage_position[i][1]);

	weapons.clear();
	for (int i = 0; i < m_weapon_number; i++)
	{
		weapons.push_back(Weapon::create(chclient->m_map_information_init.m_weapon_type[i]));
		addChild(weapons[i], 1);
		weapons[i]->setPosition(chclient->m_map_information_init.m_weapon_position[i][0], chclient->m_map_information_init.m_weapon_position[i][1]);
	}
	chclient->setMapInited();

	Weapon* weapon = Weapon::create();
	weapon->retain();
	weapon->weaponInit(4, false);
	hunter->m_gun[4] = weapon;

	hunter->setPlayerRefresh(true);
}

void MapLayer::initSetItem()
{
	for (auto enemy : m_enemy) {
		addChild(enemy, 2);
		setRandPos(enemy);
	}

	hunter->setLocalZOrder(4);
	
	runAction(Follow::create(hunter, Rect::ZERO));

	for (auto& bullet : bullets) {
		bullet = Bullet::create("images/bullet.png");
		bullet->setVisible(false);
		bullet->setBulletActive(false);
		addChild(bullet, 3);
	}

	initItem(weapons, m_weapon_number);
	initItem(m_bandage, m_bandage_number);
	initItem(m_ammunition, m_ammunition_number);

	Weapon* weapon = Weapon::create();
	weapon->retain();
	weapon->weaponInit(4, false);
	hunter->m_gun[4] = weapon;

	hunter->setPlayerRefresh(true);
}

//aim at hunter automatically and fire
void MapLayer::enemyFire(float delt)
{
	Rect rect_hunter = hunter->getBoundingBox();
	for (auto enemy : m_enemy)
	{
		if (enemy->getPlayerDeath() || enemy == hunter)
			continue;
		Rect rect_enemy(enemy->getPosition().x - 300, enemy->getPosition().y - 300, 600, 600);
		if (rect_enemy.intersectsRect(rect_hunter))
		{
			auto weaponType = enemy->getPlayerWeapon();
			if (4 == weaponType)
			{
				makeKnifeAttack(enemy);
				continue;
			}
			if (enemy->getPlayerBullet() > 0)
				enemy->setPlayerBullet(enemy->getPlayerBullet() - 1);
			else {
				enemy->setPlayerWeapon(4);
				continue;
			}
			Weapon* weapon = enemy->m_gun[weaponType];
			auto bulletLocation = hunter->getPosition();    //enemy aims at hunter
			auto bulletX = bulletLocation.x - enemy->getPositionX();
			auto bulletY = bulletLocation.y - enemy->getPositionY();
			makeBulletAttack(enemy, weapon, bulletX, bulletY);
		}
	}
}


