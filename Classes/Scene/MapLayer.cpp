#include "MapLayer.h"

USING_NS_CC;

bool add_in_aiming = false;

MapLayer::MapLayer() = default;

Layer *MapLayer::createScene(std::vector<Character *> &game_hunter) {
    return MapLayer::create(game_hunter);
}

// 0->map, 1->item/gun, 2->enemy, 3->bullet, 4->hunter
bool MapLayer::init(std::vector<Character *> &game_hunter) {
    if (!Layer::init()) {
        return false;
    }
    this->scheduleUpdate();

    map = TMXTiledMap::create("map/Moremp.tmx");
    addChild(map, 0);

    meta = map->getLayer("water");

    map_height = map->getMapSize().height;
    map_width = map->getMapSize().width;
    tile_width = map->getTileSize().width;
    tile_height = map->getTileSize().height;

    m_line = DrawNode::create();
    initMouse();
    addChild(m_line, 4);

    m_enemy = game_hunter;
    if (chclient != nullptr)
        hunter = m_enemy[chclient->getUid() - 1];
    else
        hunter = m_enemy[0];
    if (chclient != nullptr && chserver == nullptr)
        initSetItemForClient();
    else
        initSetItem();

    if (chclient == nullptr)
        schedule(CC_SCHEDULE_SELECTOR(MapLayer::enemyFire), 0.5);
    else if (chserver != nullptr)
        schedule(CC_SCHEDULE_SELECTOR(MapLayer::AIFireForServer), 0.5);

    if (chclient != nullptr) {
        for (int i = 1; i < MAX_CONNECTIONS; i++) {
            if (chclient->m_room.player_alive[i]) {
                m_enemy[i - 1]->m_has_controller = true;
                string str = chclient->m_room.player_name[i];
                m_enemy[i - 1]->setName(str);
            } else {
                m_enemy[i - 1]->setPlayerBleed(0);
                m_enemy[i - 1]->setPlayerDeath(true);
                m_enemy[i - 1]->setVisible(false);
            }

        }
    }

    if (chserver != nullptr) {
        MapInformationInit mii;
        mii.is_updated = true;

        for (int i = 0; i < m_ammunition_number; i++) {
            auto posa = m_ammunition[i]->getPosition();
            mii.m_ammunition_position[i][0] = posa.x, mii.m_ammunition_position[i][1] = posa.y;
        }
        for (int i = 0; i < m_bandage_number; i++) {
            auto posa = m_bandage[i]->getPosition();
            mii.m_bandage_position[i][0] = posa.x, mii.m_bandage_position[i][1] = posa.y;
        }
        for (int i = 0; i < m_weapon_number; i++) {
            auto posa = weapons[i]->getPosition();
            mii.m_weapon_position[i][0] = posa.x, mii.m_weapon_position[i][1] = posa.y;
            mii.m_weapon_type[i] = weapons[i]->getWeaponType();
        }
        for (int i = 1; i < MAX_CONNECTIONS; i++)
            if (chserver->isAi(i))
                m_enemy[i - 1]->m_is_ai = true;

        chserver->mapInformationInit(mii);
    }

    AudioEngine::lazyInit();
    AudioEngine::preload("music/bulletEffect.wav");
    AudioEngine::preload("music/knifeEffect.wav");

    registerKeyboardEvent();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    registerTouchEvent();
#endif

    start_time = system_clock::now();

    return true;
}

//fix the animation problem
void MapLayer::registerKeyboardEvent() {
    auto listener = EventListenerKeyboard::create();

    listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event *event) {
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

    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event *event) {
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
        for (int i = 0; i <= 3; i++) {
            if (hunter->m_speed[i]) {
                switch (i) {
                    case 0:
                        hunter->runAction(hunter->getCharacterAnimRight());
                        break;
                    case 1:
                        hunter->runAction(hunter->getCharacterAnimLeft());
                        break;
                    case 2:
                        hunter->runAction(hunter->getCharacterAnimDown());
                        break;
                    case 3:
                        hunter->runAction(hunter->getCharacterAnimUp());
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

void MapLayer::roll(Character *character) {
    float dx = 0, dy = 0;
    if (character->m_speed[0]) {
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

void MapLayer::judgePick(Character *character) {
    Rect rect_character = character->getBoundingBox();
    for (auto weapon : weapons) {
        if(chserver != nullptr && weapon->getWeaponType() == 4 && character->m_is_ai)
            continue;

        if (weapon->getBoundingBox().intersectsRect(rect_character)) {
            auto weapon_type = weapon->getWeaponType();
            try {
                if (weapon == nullptr)
                    throw R"(Weapon is nullptr)";
            } catch (exception& e) {
                string err = e.what();
                CCLOG("%s", err.c_str());
            }
            if (character->m_gun[weapon_type] == nullptr) {
                weapon->retain();
                character->m_gun[weapon_type] = weapon;
                character->setPlayerWeapon(weapon_type);
                character->setPlayerRefresh(true);
                weapon->removeFromParent();
                weapons.erase(find(weapons.begin(), weapons.end(), weapon));
                if (weapon_type == 4)
                    character->setPlayerGrenade(character->getPlayerGrenade() + 3);
            } else if (weapon_type == 4) {
                weapon->removeFromParent();
                weapons.erase(find(weapons.begin(), weapons.end(), weapon));
                character->setPlayerGrenade(character->getPlayerGrenade() + 3);
            }
            break;
        }
    }

    for (auto bn : m_bandage) {
        if (bn->getBoundingBox().intersectsRect(rect_character)) {
            int bleed = character->getPlayerBleed() + bn->getRecoverHP();
            if (bleed > character->getMAXBLEED())
                bleed = character->getMAXBLEED();
            character->setPlayerBleed(bleed);
            bn->removeFromParent();
            m_bandage.erase(find(m_bandage.begin(), m_bandage.end(), bn));
            break;
        }
    }

    for (auto am : m_ammunition) {
        if (am->getBoundingBox().intersectsRect(rect_character)) {
            character->setPlayerBullet(character->getPlayerBullet() + am->getAddBullet());
            am->removeFromParent();
            m_ammunition.erase(find(m_ammunition.begin(), m_ammunition.end(), am));
            break;
        }
    }
}

// Android
void MapLayer::touchBegan(Touch *touch) {
    auto weapon_type = hunter->getPlayerWeapon();
    if (4 == weapon_type) {
        auto knife_audio_ID = AudioEngine::play2d("music/knifeEffect.mp3", false);
        AudioEngine::setVolume(knife_audio_ID, M_Volume);
        if (hunter->getPlayerGrenade() <= 0)
            makeKnifeAttack(hunter);
        return;
    } else if (5 == weapon_type) {
        auto knife_audio_ID = AudioEngine::play2d("music/knifeEffect.mp3", false);
        AudioEngine::setVolume(knife_audio_ID, M_Volume);
        makeKnifeAttack(hunter);
        return;
    }
    Fire(0);
    schedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire), hunter->getBulletSpeed() -
                                                   hunter->m_gun[hunter->getPlayerWeapon()]->getFireWeaponSpeed());
}

// Android
void MapLayer::touchEnded() {
    if (4 == hunter->getPlayerWeapon() && hunter->getPlayerGrenade() > 0) {
        hunter->setPlayerGrenade(hunter->getPlayerGrenade() - 1);
        hunter->m_bullet_location = (hunter->m_bullet_location - winSize / 2) * 1.5 + winSize / 2;
        showEffect(hunter->m_bullet_location - winSize / 2 + hunter->getPosition());
        scheduleOnce(CC_SCHEDULE_SELECTOR(MapLayer::makeExplosionEffect), 0.5);
    }
    unschedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire));
}

// Android
void MapLayer::bindTouchMap(std::function<void(MapLayer *, Touch *touch)> &began,
                            std::function<void(MapLayer *)> &ended) {
    began = &MapLayer::touchBegan;
    ended = &MapLayer::touchEnded;
}

//add firing continuously
void MapLayer::registerTouchEvent() {
    auto touchListener = EventListenerTouchOneByOne::create();

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        auto weapon_type = hunter->getPlayerWeapon();
        if (4 == weapon_type) {
            auto knife_audio_ID = AudioEngine::play2d("music/knifeEffect.mp3", false);
            AudioEngine::setVolume(knife_audio_ID, M_Volume);
            if (hunter->getPlayerGrenade() > 0) {
                hunter->setPlayerGrenade(hunter->getPlayerGrenade() - 1);
                hunter->m_bullet_location = touch->getLocation();
                showEffect(convertToNodeSpace(hunter->m_bullet_location));
                scheduleOnce(CC_SCHEDULE_SELECTOR(MapLayer::makeExplosionEffect), 0.5);
            } else makeKnifeAttack(hunter);
            return true;
        } else if (5 == weapon_type) {
            auto knife_audio_ID = AudioEngine::play2d("music/knifeEffect.mp3", false);
            AudioEngine::setVolume(knife_audio_ID, M_Volume);
            makeKnifeAttack(hunter);
            return true;
        }
        hunter->m_bullet_location = touch->getLocation();
        Fire(0);
        schedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire), hunter->getBulletSpeed() -
                                                       hunter->m_gun[hunter->getPlayerWeapon()]->getFireWeaponSpeed());
        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        hunter->m_bullet_location = touch->getLocation();
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        unschedule(CC_SCHEDULE_SELECTOR(MapLayer::Fire));
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void MapLayer::enemyExplosionEffect(float dt)
{
    auto pos = explosion_point;
    auto explo_particle = ParticleExplosion::create();
    auto explo_texture = Director::getInstance()->getTextureCache()->addImage("stars.png");
    explo_particle->setTexture(explo_texture);
    explo_particle->setPosition(explosion_point);
    explo_particle->setLife(1.2f);
    explo_particle->setLifeVar(0.2f);
    Color4F start = { 0.7f, 0.0f, 0.0f, 1.0f };
    Color4F var = { 0.3f, 0.3f, 0.2f, 0.0f };
    explo_particle->setStartColor(start);
    explo_particle->setStartColorVar(var);
    Color4F end = { 0.7f, 0.0f, 0.0f, 0.0f };
    explo_particle->setEndColor(end);
    explo_particle->setEndColorVar(var);
    explo_particle->setPositionType(ParticleSystem::PositionType::RELATIVE);
    addChild(explo_particle);

    auto dis = pos.getDistance(hunter->getPosition());
    if (dis < 100) {
        auto bleed = hunter->getPlayerBleed() - 20 - 0.3f * (100 - dis);
        if (bleed < 0)
            bleed = 0;
        hunter->setPlayerBleed(static_cast<int>(bleed));
        showAttacked(hunter->getPosition());
    }
    for (auto enemy : m_enemy) {
        if (enemy->getPlayerDeath())
            continue;
        auto dis = pos.getDistance(enemy->getPosition());
        if (dis < 100) {
            auto bleed = enemy->getPlayerBleed() - 20 - 0.3f * (100 - dis);
            if (bleed < 0)
                bleed = 0;
            enemy->setPlayerBleed(static_cast<int>(bleed));
            showAttacked(enemy->getPosition());
        }
    }
}

void MapLayer::makeExplosionEffect(float dt) {
    auto pos = hunter->m_bullet_location + hunter->getPosition() - winSize / 2;
    if (chclient != nullptr) {
        chclient->m_localaction.is_drop_grenade = true;
        chclient->m_localaction.grenade_x = pos.x;
        chclient->m_localaction.grenade_y = pos.y;
    }
    auto explo_particle = ParticleExplosion::create();
    auto explo_texture = Director::getInstance()->getTextureCache()->addImage("stars.png");
    explo_particle->setTexture(explo_texture);
    explo_particle->setPosition(pos);
    explo_particle->setLife(1.2f);
    explo_particle->setLifeVar(0.2f);
    Color4F start = {0.7f, 0.0f, 0.0f, 1.0f};
    Color4F var = {0.3f, 0.3f, 0.2f, 0.0f};
    explo_particle->setStartColor(start);
    explo_particle->setStartColorVar(var);
    Color4F end = {0.7f, 0.0f, 0.0f, 0.0f};
    explo_particle->setEndColor(end);
    explo_particle->setEndColorVar(var);
    explo_particle->setPositionType(ParticleSystem::PositionType::RELATIVE);
    addChild(explo_particle);

    for (auto enemy : m_enemy) {
        if (enemy->getPlayerDeath())
            continue;
        auto dis = pos.getDistance(enemy->getPosition());
        if (dis < 100) {
            auto bleed = enemy->getPlayerBleed() - 20 - 0.3f * (100 - dis);
            if (bleed < 0)
                bleed = 0;
            enemy->setPlayerBleed(static_cast<int>(bleed));
            showAttacked(enemy->getPosition());
        }
    }
}

void MapLayer::makeExplosionEffectOnlyForShow(float posx, float posy) {

    Vec2 pos(posx, posy);
    auto explo_particle = ParticleExplosion::create();
    auto explo_texture = Director::getInstance()->getTextureCache()->addImage("stars.png");

    explo_particle->setTexture(explo_texture);
    explo_particle->setPosition(pos);
    explo_particle->setLife(1.2f);
    explo_particle->setLifeVar(0.2f);
    Color4F start = {0.7f, 0.0f, 0.0f, 1.0f};
    Color4F var = {0.3f, 0.3f, 0.2f, 0.0f};
    explo_particle->setStartColor(start);
    explo_particle->setStartColorVar(var);
    Color4F end = {0.7f, 0.0f, 0.0f, 0.0f};
    explo_particle->setEndColor(end);
    explo_particle->setEndColorVar(var);
    explo_particle->setPositionType(ParticleSystem::PositionType::RELATIVE);
    addChild(explo_particle);

    for (auto enemy : m_enemy) {
        if (enemy->getPlayerDeath())
            continue;
        auto dis = pos.getDistance(enemy->getPosition());
        if (dis < 100) {
            auto bleed = enemy->getPlayerBleed() - 20 - 0.3f * (100 - dis);
            if (bleed < 0)
                bleed = 0;
            enemy->setPlayerBleed(static_cast<int>(bleed));
            showAttacked(enemy->getPosition());
        }
    }
}

void MapLayer::makeKnifeAttack(Character *character) {
    if (chclient != nullptr && character == hunter) // upload local attack
    {
        chclient->m_localaction.is_shoot = true;
        chclient->m_localaction.weapon_type = 5;
    }
    Vec2 pos = character->getPosition();
    showEffect(pos);
    for (auto enemy : m_enemy) {
        if (enemy == character || enemy->getPlayerDeath())
            continue;
        Vec2 enemy_pos = enemy->getPosition();
        if (enemy_pos.getDistance(pos) < 100) {
            auto bleed = enemy->getPlayerBleed() -
                         5 * character->getPlayerAttack() * enemy->getPlayerDefense();
            if (bleed < 0)
                bleed = 0;
            enemy->setPlayerBleed(static_cast<int>(bleed));
            showAttacked(enemy->getPosition());
        }
    }
}

static inline short getWeaponTypeForBulletAttack(Character *character, Weapon *weapon) {
    for (short i = 0; i < 5; i++)
        if (character->m_gun[i] == weapon)
            return i;
    return -1;
}

void MapLayer::makeBulletAttack(Character *character, Weapon *weapon, float bulletX, float bulletY) {
    if (chclient != nullptr && character == hunter)
    {
        short wtype = getWeaponTypeForBulletAttack(character, weapon);
        if (wtype != -1) {
            chclient->m_localaction.is_shoot = true;
            chclient->m_localaction.weapon_type = wtype;
            chclient->m_localaction.bullet_x = bulletX, chclient->m_localaction.bullet_y = bulletY;
        }
    }
    float time = sqrt(bulletX * bulletX + bulletY * bulletY) / 1000;
    float delta = 1;
    int f = 0, fmax = 1;
    auto angle = static_cast<float>(atan(0.15) * 180 / PI);
    auto rot1 = calRotation(bulletX, bulletY) + angle;
    auto rot2 = calRotation(bulletX, bulletY) - angle;
    if (chclient == nullptr) {
        if (character == hunter)
            delta = 0.6f;
    } else {
        if (!character->m_is_ai)
            delta = 0.6f;
    }
    if (weapon->getWeaponType() == 2) {
        fmax = 3;
    }
    for (auto bullet : bullets) {
        if (!bullet->getBulletActive()) {
            bullet->setBulletActive(true);
            bullet->setPosition(character->getPositionX() + bulletX / time / 20,
                                character->getPositionY() + bulletY / time / 20);
            switch (f) {
                case 0:
                    bullet->setRotation(calRotation(bulletX, bulletY));
                    bullet->runAction(RepeatForever::create(
                            MoveBy::create(delta * weapon->getWeaponSpeed(),
                                           Vec2(bulletX / time, bulletY / time))));
                    break;
                case 1:
                    bullet->setRotation(rot1);
                    bullet->runAction(RepeatForever::create(
                            MoveBy::create(delta * weapon->getWeaponSpeed(),
                                           Vec2(static_cast<float>(cos(rot1 * PI / 180)) * 1000,
                                                static_cast<float>(-sin(rot1 * PI / 180)) *
                                                1000))));
                    break;
                case 2:
                    bullet->setRotation(rot2);
                    bullet->runAction(RepeatForever::create(
                            MoveBy::create(delta * weapon->getWeaponSpeed(),
                                           Vec2(static_cast<float>(cos(rot2 * PI / 180)) * 1000,
                                                static_cast<float>(-sin(rot2 * PI / 180)) *
                                                1000))));
                    break;
                default:
                    break;
            }

            bullet->setBulletAttack(
                    static_cast<int>(weapon->getWeaponAttack() * character->getPlayerAttack()));
            bullet->setVisible(true);
            f++;
            if (f >= fmax)
                break;
        }
    }

}

void MapLayer::Fire(float dt) {
    Weapon *weapon = hunter->m_gun[hunter->getPlayerWeapon()];
    if (hunter->getPlayerBullet() > 0 && weapon->getWeaponType() != 2)
        hunter->setPlayerBullet(hunter->getPlayerBullet() - 1);
    else if (hunter->getPlayerBullet() > 2 && weapon->getWeaponType() == 2)
        hunter->setPlayerBullet(hunter->getPlayerBullet() - 3);
    else {
        auto knife_audio_ID = AudioEngine::play2d("music/knifeEffect.mp3", false);
        AudioEngine::setVolume(knife_audio_ID, M_Volume);
        makeKnifeAttack(hunter);
        return;
    }

    auto bullet_audio_ID = AudioEngine::play2d("music/bulletEffect.mp3", false);
    AudioEngine::setVolume(bullet_audio_ID, M_Volume);
    Vec2 bullet_location = hunter->m_bullet_location;
    auto bulletX = bullet_location.x - winSize.width / 2;
    auto bulletY = bullet_location.y - winSize.height / 2;
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
    auto effect_circle = DrawNode::create();
    addChild(effect_circle, 2);
    effect_circle->drawSolidCircle(pos, 100.0f, CC_DEGREES_TO_RADIANS(360), 15,
                                   Color4F(0.28f, 0.46f, 1.0f, 0.6f));
    effect_circle->runAction(Sequence::create(FadeOut::create(0.5f), RemoveSelf::create(), NULL));
}

void MapLayer::showAttacked(Vec2 pos) {
    auto effect_circle = DrawNode::create();
    addChild(effect_circle, 5);
    effect_circle->drawSolidCircle(pos, 20.0f, CC_DEGREES_TO_RADIANS(360), 15,
                                   Color4F(1.0f, 0, 0, 0.6f));
    effect_circle->runAction(Sequence::create(FadeOut::create(0.3f), RemoveSelf::create(), NULL));
}

int action_activated[MAX_CONNECTIONS] = {};

void MapLayer::update(float fDelta) {
    if (chclient != nullptr) {
        if (chserver != nullptr) {
            // local hunter directly given to the server
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
            judgePick(hunter);
            chserver->paction[1].pick = true;
#endif
            // server AI
            for (int i = 1; i < MAX_CONNECTIONS; i++)
                if (chserver->isAi(i)) {
                    assert(m_enemy[i - 1] != nullptr);
                    int nextT = m_enemy[i - 1]->getThought() + int(fDelta * 1000);
                    m_enemy[i - 1]->setThought(nextT);
                    if (nextT >= m_enemy[i - 1]->getThinkTime()) {
                        m_enemy[i - 1]->setThought(0);
                        m_enemy[i - 1]->m_speed[0] = random(0, 1);
                        m_enemy[i - 1]->m_speed[1] = random(0, 1);
                        m_enemy[i - 1]->m_speed[2] = random(0, 1);
                        m_enemy[i - 1]->m_speed[3] = random(0, 1);
                    }
                    chserver->paction[i].speed[0] = m_enemy[i - 1]->m_speed[0];
                    chserver->paction[i].speed[1] = m_enemy[i - 1]->m_speed[1];
                    chserver->paction[i].speed[2] = m_enemy[i - 1]->m_speed[2];
                    chserver->paction[i].speed[3] = m_enemy[i - 1]->m_speed[3];
                    chserver->paction[i].pick = true;
                }

            chserver->paction[1].speed[0] = hunter->m_speed[0];
            chserver->paction[1].speed[1] = hunter->m_speed[1];
            chserver->paction[1].speed[2] = hunter->m_speed[2];
            chserver->paction[1].speed[3] = hunter->m_speed[3];
            chserver->paction[1].pick = chclient->m_localaction.pick;
            chserver->paction[1].is_shoot = chclient->m_localaction.is_shoot;
            chserver->paction[1].weapon_type = chclient->m_localaction.weapon_type;
            chserver->paction[1].bullet_x = chclient->m_localaction.bullet_x;
            chserver->paction[1].bullet_y = chclient->m_localaction.bullet_y;
            chserver->paction[1].is_drop_grenade = chclient->m_localaction.is_drop_grenade;
            chserver->paction[1].grenade_x = chclient->m_localaction.grenade_x;
            chserver->paction[1].grenade_y = chclient->m_localaction.grenade_y;
            chclient->m_localaction.pick = false;
            chclient->m_localaction.is_shoot = false;
            chclient->m_localaction.bullet_x = 0, chclient->m_localaction.bullet_y = 0;
            chclient->m_localaction.weapon_type = 0;
            chserver->paction[1].is_bleed_locked = hunter->getPlayerLockedBleed();
            chserver->paction[1].is_bullet_locked = hunter->getPlayerLockedBullet();
            //server's update
            chserver->mapUploadInit();
            chclient->m_localaction.is_drop_grenade = false;

            for (auto bullet : bullets) {
                if (bullet->getBulletActive()) {
                    auto bulletX = bullet->getPositionX();
                    auto bulletY = bullet->getPositionY();
                    if (bulletX < 0 || bulletX >= map_width * tile_width || bulletY < 0 ||
                        bulletY >= map_height * tile_height
                        || meta->getTileGIDAt(
                            Vec2(bulletX / tile_width, map_height - bulletY / tile_height))) {
                        bullet->setVisible(false);
                        bullet->stopAllActions();
                        bullet->setBulletActive(false);
                        continue;
                    }
                    Rect rect_bullet = bullet->getBoundingBox();
                    for (int i = 0; i < MAX_CONNECTIONS - 1; i++) {
                        if (m_enemy[i]->getPlayerDeath())
                            continue;
                        Rect rect_enemy = m_enemy[i]->getBoundingBox();
                        if (rect_enemy.intersectsRect(rect_bullet)) {
                            showAttacked(m_enemy[i]->getPosition());
                            auto bleed = m_enemy[i]->getPlayerBleed() -
                                         bullet->getBulletAttack() * m_enemy[i]->getPlayerDefense();
                            if (bleed < 0)
                                bleed = 0;
                            if (bleed == 0) {
                                chserver->m_map_trans.player[i + 1].alive = false;
                                chserver->m_map_trans.player[i + 1].just_game_over = true;

                            }
                            m_enemy[i]->setPlayerBleed(static_cast<int>(bleed));
                            bullet->setVisible(false);
                            bullet->stopAllActions();
                            bullet->setBulletActive(false);
                        }
                    }
                }
            }

            showAimLine();

            chserver->m_map_trans.player_left_num = 0;
            for (int i = 1; i < MAX_CONNECTIONS; i++) {
                if (m_enemy[i - 1]->getPlayerBleed() > 0) {
                    assert(m_enemy[i - 1] != nullptr);
                    //CCLOG("UPDATEING PLAYER#%d", i);
                    chserver->m_map_trans.player[i].alive = true;
                    float dx = 0, dy = 0;
                    chserver->m_map_trans.player_left_num++;
                    if (i != 1 && chserver->paction[i].speed[0] == 0 &&
                        chserver->paction[i].speed[1] == 0 && chserver->paction[i].speed[2] == 0 &&
                        chserver->paction[i].speed[3] == 0) {
                        action_activated[i - 1] = 0;
                        m_enemy[i - 1]->stopAllActions();
                    }
                    if (chserver->paction[i].speed[0]) {
                        if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 1) {
                            action_activated[i - 1] = 1;
                            m_enemy[i - 1]->stopAllActions();
                            m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimRight());
                        }
                        dx += 4;
                    }
                    if (chserver->paction[i].speed[1]) {
                        if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 2) {
                            action_activated[i - 1] = 2;
                            m_enemy[i - 1]->stopAllActions();
                            m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimLeft());
                        }
                        dx += -4;
                    }
                    if (chserver->paction[i].speed[2]) {
                        if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 3) {
                            action_activated[i - 1] = 3;
                            m_enemy[i - 1]->stopAllActions();
                            m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimDown());
                        }
                        dy += -4;
                    }
                    if (chserver->paction[i].speed[3]) {
                        if (m_enemy[i - 1] != hunter && action_activated[i - 1] != 4) {
                            action_activated[i - 1] = 4;
                            m_enemy[i - 1]->stopAllActions();
                            m_enemy[i - 1]->runAction(m_enemy[i - 1]->getCharacterAnimUp());
                        }
                        dy += 4;
                    }
                    if (dx == 0 && dy == 0)
                        continue;

                    auto enemy_pos = m_enemy[i - 1]->getPosition();
                    if (enemy_pos.x < -10 || enemy_pos.x > map_width * tile_width + 9
                        || enemy_pos.y < -10 || enemy_pos.y > map_height * tile_height + 9)
                        m_enemy[i - 1]->setPlayerBleed(0);

                    auto next_X = enemy_pos.x + dx;
                    auto next_Y = enemy_pos.y + dy;
                    auto next_map_X = next_X / tile_width;
                    auto next_map_Y = map_height - next_Y / tile_height;
                    if (next_map_X < map_width && next_map_X >= 0 && next_map_Y < map_height &&
                        next_map_Y >= 0
                        && !meta->getTileGIDAt(Vec2(next_map_X, next_map_Y))) {
                        m_enemy[i - 1]->runAction(MoveTo::create(0, Vec2(next_X, next_Y)));
                    } else {
                        if (chserver->isAi(i)) {
                            m_enemy[i - 1]->m_speed[0] = !m_enemy[i - 1]->m_speed[0];
                            m_enemy[i - 1]->m_speed[1] = !m_enemy[i - 1]->m_speed[1];
                            m_enemy[i - 1]->m_speed[2] = !m_enemy[i - 1]->m_speed[2];
                            m_enemy[i - 1]->m_speed[3] = !m_enemy[i - 1]->m_speed[3];
                        }
                        m_enemy[i - 1]->runAction(
                                MoveTo::create(0, Vec2(next_X - 2 * dx, next_Y - 2 * dy)));
                    }
                }
            }

            // Server map upload
            for (int i = 1; i < MAX_CONNECTIONS; i++) {
                assert(m_enemy[i - 1] != nullptr);

                auto pos = m_enemy[i - 1]->getPosition();
                chserver->m_map_trans.player[i].position_x = pos.x, chserver->m_map_trans.player[i].position_y = pos.y;
                chserver->m_map_trans.player[i].is_pick = chserver->paction[i].pick;
                chserver->m_map_trans.player[i].bullet = m_enemy[i - 1]->getPlayerBullet();
                chserver->m_map_trans.player[i].is_shoot = chserver->paction[i].is_shoot;
                chserver->m_map_trans.player[i].weapon_type = chserver->paction[i].weapon_type;
                chserver->m_map_trans.player[i].bullet_x = chserver->paction[i].bullet_x;
                chserver->m_map_trans.player[i].bullet_y = chserver->paction[i].bullet_y;
                chserver->m_map_trans.player[i].alive = !m_enemy[i - 1]->getPlayerDeath();
                chserver->m_map_trans.player[i].grenade = m_enemy[i - 1]->getPlayerGrenade();
                chserver->m_map_trans.player[i].is_drop_grenade = chserver->paction[i].is_drop_grenade;
                chserver->m_map_trans.player[i].grenade_x = chserver->paction[i].grenade_x;
                chserver->m_map_trans.player[i].grenade_y = chserver->paction[i].grenade_y;

                if (chserver->paction[i].pick && !m_enemy[i - 1]->getPlayerDeath()) {
                    //CCLOG("#MAP_UPDATE# PLAYER#%d PICK", i);
                    judgePick(m_enemy[i - 1]);
                }

                // Bullet synchronization
                if (chserver->paction[i].is_shoot && m_enemy[i - 1] != hunter) {
                    if (chserver->paction[i].weapon_type == 4 ||
                        chserver->paction[i].weapon_type == 5) {
                        //CCLOG("PLAYER#%d KNIFE WEAPON#%d LEFT_BULLET#%d ", i, chserver->paction[i].weapon_type],);
                        makeKnifeAttack(m_enemy[i - 1]);
                    } else {
                        //CCLOG("PLAYER#%d MAKE BULLET ATTACK#%d", i + 1,chserver->paction[i].weapon_type);
                        if (chserver->paction[i].weapon_type != 2)
                            m_enemy[i - 1]->setPlayerBullet(m_enemy[i - 1]->getPlayerBullet() - 1);
                        else if (chserver->paction[i].weapon_type == 2)
                            m_enemy[i - 1]->setPlayerBullet(m_enemy[i - 1]->getPlayerBullet() - 3);
                        makeBulletAttack(m_enemy[i - 1],
                                         m_enemy[i - 1]->m_gun[chserver->paction[i].weapon_type],
                                         chserver->paction[i].bullet_x,
                                         chserver->paction[i].bullet_y);
                    }

                }
                // Grenade synchronization
                if (chserver->paction[i].is_drop_grenade && m_enemy[i - 1] != hunter) {
                    CCLOG("PLAYER#%d MAKE GRENADE ATTACK", i);
                    makeExplosionEffectOnlyForShow(chserver->paction[i].grenade_x,
                                                   chserver->paction[i].grenade_y);
                }

                m_enemy[i - 1]->setPlayerLockedBleed(chserver->paction[i].is_bleed_locked);
                m_enemy[i - 1]->setPlayerLockedBullet(chserver->paction[i].is_bullet_locked);

                if (m_enemy[i - 1]->getPlayerLockedBleed())
                    m_enemy[i - 1]->setPlayerBleed(m_enemy[i - 1]->getMAXBLEED());

                if (m_enemy[i - 1]->getPlayerLockedBullet()) {
                    m_enemy[i - 1]->setPlayerBullet(999);
                    m_enemy[i - 1]->setPlayerGrenade(999);
                }

                chserver->m_map_trans.player[i].hp = m_enemy[i - 1]->getPlayerBleed();
                memset(&chserver->paction[i], 0, sizeof(PlayerAction));// Character action deletion
            }
            chserver->mapUpload();
        } else {

            for (int i = 1; i < MAX_CONNECTIONS; i++)
                if (chclient->m_map.player[i].just_game_over)// Death settlement
                    m_enemy[i - 1]->setPlayerBleed(0);

            for (auto bullet : bullets) {// Bullet recovery and disposal
                if (bullet->getBulletActive()) {
                    auto bullet_X = bullet->getPositionX();
                    auto bullet_Y = bullet->getPositionY();
                    if (bullet_X < 0 || bullet_X > map_width * tile_width - 1 || bullet_Y < 0 ||
                        bullet_Y > map_height * tile_height - 1
                        || meta->getTileGIDAt(
                            Vec2(bullet_X / tile_width, map_height - bullet_Y / tile_height))) {
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
                            bullet->setVisible(false);
                            bullet->stopAllActions();
                            bullet->setBulletActive(false);
                        }
                    }
                }
            }

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
            judgePick(hunter);
            chclient->m_localaction.pick = true;
#endif

            // Local operation upload
            chclient->m_localaction.speed[0] = hunter->m_speed[0];
            chclient->m_localaction.speed[1] = hunter->m_speed[1];
            chclient->m_localaction.speed[2] = hunter->m_speed[2];
            chclient->m_localaction.speed[3] = hunter->m_speed[3];
            chclient->m_localaction.is_bleed_locked = hunter->getPlayerLockedBleed();
            if (chclient->m_localaction.is_bleed_locked)
                CCLOG("BLEED IS LOCK!");
            chclient->m_localaction.is_bullet_locked = hunter->getPlayerLockedBullet();
            if (chclient->m_localaction.is_bullet_locked)
                CCLOG("BULLET IS LOCK!");
            chclient->upload();

            showAimLine();

            MapInformation &current_map = chclient->m_map;
            if (current_map.is_updated) {
                for (int i = 0; i < MAX_CONNECTIONS - 1; i++)
                    if (current_map.player[i + 1].alive) {
                        if (!(current_map.player[i + 1].position_x !=
                              save_map.player[i + 1].position_x ||
                              current_map.player[i + 1].position_y !=
                              save_map.player[i + 1].position_y) && m_enemy[i] != hunter) {
                            action_activated[i + 1] = 0;
                            m_enemy[i]->stopAllActions();
                        }
                        if (current_map.player[i + 1].position_x >
                            save_map.player[i + 1].position_x && m_enemy[i] != hunter &&
                            current_map.is_updated && save_map.is_updated &&
                            action_activated[i + 1] != 1) {
                            action_activated[i + 1] = 1;
                            CCLOG("ENEMY#%d RIGHT", i);
                            m_enemy[i]->stopAllActions();
                            m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimRight());
                        }
                        if (current_map.player[i + 1].position_x <
                            save_map.player[i + 1].position_x && m_enemy[i] != hunter &&
                            current_map.is_updated && save_map.is_updated &&
                            action_activated[i + 1] != 2) {
                            action_activated[i + 1] = 2;
                            //CCLOG("ENEMY#%d LEFT", i);
                            m_enemy[i]->stopAllActions();
                            m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimLeft());
                        }
                        if (current_map.player[i + 1].position_y >
                            save_map.player[i + 1].position_y && m_enemy[i] != hunter &&
                            current_map.is_updated && save_map.is_updated &&
                            action_activated[i + 1] != 3) {
                            action_activated[i + 1] = 3;
                            //CCLOG("ENEMY#%d UP", i);
                            m_enemy[i]->stopAllActions();
                            m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimUp());
                        }
                        if (current_map.player[i + 1].position_y <
                            save_map.player[i + 1].position_y && m_enemy[i] != hunter &&
                            current_map.is_updated && save_map.is_updated &&
                            action_activated[i + 1] != 4) {
                            action_activated[i + 1] = 4;
                            //CCLOG("ENEMY#%d DOWN", i);
                            m_enemy[i]->stopAllActions();
                            m_enemy[i]->runAction(m_enemy[i]->getCharacterAnimDown());
                        }
                        //if(m_enemy[i])
                        m_enemy[i]->runAction(MoveTo::create(0, Vec2(current_map.player[i +
                                                                                        1].position_x,
                                                                     current_map.player[i +
                                                                                        1].position_y)));
                        m_enemy[i]->setPlayerBleed(current_map.player[i + 1].hp);
                        m_enemy[i]->setPlayerGrenade(current_map.player[i + 1].grenade);
                        m_enemy[i]->setPlayerBullet(current_map.player[i + 1].bullet);
                        // Pick things up synchronization
                        if (current_map.player[i + 1].is_pick) {
                            judgePick(m_enemy[i]);
                        }
                        // Bullet synchronization
                        if (current_map.player[i + 1].is_shoot == true && m_enemy[i] != hunter) {
                            if (current_map.player[i + 1].weapon_type == 4 ||
                                current_map.player[i + 1].weapon_type == 5) {
                                if (current_map.player[i + 1].bullet == 0)
                                    makeKnifeAttack(m_enemy[i]);
                                //CCLOG("PLAYER#%d MAKE KNIFE ATTACK", i + 1);
                            } else if (current_map.player[i + 1].weapon_type >= 0 &&
                                       current_map.player[i + 1].weapon_type < 4) {
                                //CCLOG("PLAYER#%d MAKE BULLET ATTACK", i + 1,current_map.player[i + 1].weapon_type);
                                makeBulletAttack(m_enemy[i],
                                                 m_enemy[i]->m_gun[current_map.player[i +
                                                                                      1].weapon_type],
                                                 current_map.player[i + 1].bullet_x,
                                                 current_map.player[i + 1].bullet_y);
                            }

                        }
                        // Grenade synchronization
                        if (current_map.player[i + 1].is_drop_grenade && m_enemy[i] != hunter) {
                            CCLOG("PLAYER#%d MAKE GRENADE ATTACK", i + 1);
                            makeExplosionEffectOnlyForShow(current_map.player[i + 1].grenade_x,
                                                           current_map.player[i + 1].grenade_y);
                        }
                    }
                save_map = current_map;
            }


            memset(&chclient->m_map, 0, sizeof(MapInformation));
        }
    } else // Stand-alone game logic
    {
        if (!(getTime() % 90) && !weapon_refresh) {
            weapon_refresh = true;
            for (auto bd : m_bandage)
                bd->removeFromParent();
            m_bandage.clear();
            for (auto am : m_ammunition)
                am->removeFromParent();
            m_ammunition.clear();
            initItem(m_bandage, m_bandage_number);
            initItem(m_ammunition, m_ammunition_number);
        }

        if (getTime() % 90)
            weapon_refresh = false;

        for (auto bullet : bullets) {
            if (bullet->getBulletActive()) {
                auto bullet_X = bullet->getPositionX();
                auto bullet_Y = bullet->getPositionY();
                if (bullet_X < 0 || bullet_X >= map_width * tile_width || bullet_Y < 0 ||
                    bullet_Y >= map_height * tile_height
                    || meta->getTileGIDAt(
                        Vec2(bullet_X / tile_width, map_height - bullet_Y / tile_height))) {
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
                        auto bleed = enemy->getPlayerBleed() -
                                     bullet->getBulletAttack() * enemy->getPlayerDefense();
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
        for (auto enemy : m_enemy) {
            if (enemy->getPlayerDeath())
                continue;

#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
            judgePick(enemy);
            if (enemy != hunter) {
#else
                if (enemy != hunter) {
                    judgePick(enemy);
#endif
                int next_time = enemy->getThought() + int(fDelta * 1000);
                enemy->setThought(next_time);
                if (next_time >= enemy->getThinkTime()) {
                    enemy->setThought(0);
                    enemy->m_speed[0] = random(0, 1);
                    enemy->m_speed[1] = random(0, 1);
                    enemy->m_speed[2] = random(0, 1);
                    enemy->m_speed[3] = random(0, 1);
                    enemy->stopAllActions();
                }
            } else {
                if (enemy->getPlayerLockedBleed())
                    enemy->setPlayerBleed(enemy->getMAXBLEED());

                if (enemy->getPlayerLockedBullet()) {
                    enemy->setPlayerBullet(999);
                    enemy->setPlayerGrenade(999);
                }

                if (enemy->getPlayerAutoAttack() && !has_auto_attack) {
                    schedule(CC_SCHEDULE_SELECTOR(MapLayer::autoFire), 0.3f);
                    has_auto_attack = true;
                }

                if (!enemy->getPlayerAutoAttack() && has_auto_attack) {
                    unschedule(CC_SCHEDULE_SELECTOR(MapLayer::autoFire));
                    has_auto_attack = false;
                }
            }

            float dx = 0, dy = 0;
            if (enemy->m_speed[0]) {
                if (enemy != hunter && enemy->getThought() == 0)
                    enemy->runAction(enemy->getCharacterAnimRight());
                dx += 4;
            }
            if (enemy->m_speed[1]) {
                if (enemy != hunter && enemy->getThought() == 0)
                    enemy->runAction(enemy->getCharacterAnimLeft());
                dx += -4;
            }
            if (enemy->m_speed[2]) {
                if (enemy != hunter && enemy->getThought() == 0)
                    enemy->runAction(enemy->getCharacterAnimDown());
                dy += -4;
            }
            if (enemy->m_speed[3]) {
                if (enemy != hunter && enemy->getThought() == 0)
                    enemy->runAction(enemy->getCharacterAnimUp());
                dy += 4;
            }
            if (dx != 0 || dy != 0)
                showAimLine();

            auto enemy_pos = enemy->getPosition();

            if (enemy_pos.x < -10 || enemy_pos.x > map_width * tile_width + 9
                || enemy_pos.y < -10 || enemy_pos.y > map_height * tile_height + 9)
                enemy->setPlayerBleed(0);

            auto next_X = enemy_pos.x + dx;
            auto next_Y = enemy_pos.y + dy;

            auto next_map_X = next_X / tile_width;
            auto next_map_Y = map_height - next_Y / tile_height;

            if (next_map_X < map_width && next_map_X >= 0 && next_map_Y < map_height &&
                next_map_Y >= 0
                && !meta->getTileGIDAt(Vec2(next_map_X, next_map_Y)))
                enemy->runAction(MoveTo::create(1.0f / 80.0f, Vec2(next_X, next_Y)));
            else {
                if (enemy != hunter) {
                    enemy->m_speed[0] = !enemy->m_speed[0];
                    enemy->m_speed[1] = !enemy->m_speed[1];
                    enemy->m_speed[2] = !enemy->m_speed[2];
                    enemy->m_speed[3] = !enemy->m_speed[3];
                }
                enemy->stopAllActions();
                enemy->runAction(
                        MoveTo::create(1.0f / 80.0f, Vec2(next_X - 2 * dx, next_Y - 2 * dy)));
            }
        }
    }
}

// set enemies/items randomly and at anywhere except water space.
template<class T>
void MapLayer::setRandPos(T *elem) {
    decltype(map_width) rx, ry, mrx, mry;
    while (true) {
        rx = static_cast<decltype(tile_width)>(random(50,
                                                      static_cast<int>(map_width * tile_width - 50 -
                                                                       1)));
        mrx = rx / tile_width;
        ry = static_cast<decltype(tile_height)>(random(50,
                                                       static_cast<int>(map_height * tile_height -
                                                                        50 -
                                                                        1)));
        mry = map_height - ry / tile_width;
        if (mrx < map_width && mrx >= 0 && mry < map_height && mry >= 0 &&
            !meta->getTileGIDAt(Vec2(mrx, mry)))
            break;
    }
    elem->setPosition(Vec2(rx, ry));
}

template<class T>
void MapLayer::initItem(std::vector<T *> &items, int number) {
    items.clear();
    for (int i = 0; i < number; i++) {
        items.push_back(T::create());
        addChild(items[i], 1);
        setRandPos(items[i]);
    }
}

void MapLayer::initSetItemForClient() {
    for (auto enemy : m_enemy) {
        addChild(enemy, 2);
        setRandPos(enemy);
    }
    for (auto &bullet : bullets) {
        bullet = Bullet::create("images/bullet.png");
        bullet->setVisible(false);
        bullet->setBulletActive(false);
        addChild(bullet, 3);
    }

    hunter->setLocalZOrder(4);

    runAction(Follow::create(hunter, Rect::ZERO));
    initItem(m_bandage, m_bandage_number);
    initItem(m_ammunition, m_ammunition_number);
    while (!chclient->m_map_information_init.is_updated);
    for (int i = 0; i < m_ammunition_number; i++)
        m_ammunition[i]->setPosition(chclient->m_map_information_init.m_ammunition_position[i][0],
                                     chclient->m_map_information_init.m_ammunition_position[i][1]);
    for (int i = 0; i < m_bandage_number; i++)
        m_bandage[i]->setPosition(chclient->m_map_information_init.m_bandage_position[i][0],
                                  chclient->m_map_information_init.m_bandage_position[i][1]);

    weapons.clear();
    for (int i = 0; i < m_weapon_number; i++) {
        weapons.push_back(Weapon::create(chclient->m_map_information_init.m_weapon_type[i]));
        addChild(weapons[i], 1);
        weapons[i]->setPosition(chclient->m_map_information_init.m_weapon_position[i][0],
                                chclient->m_map_information_init.m_weapon_position[i][1]);
    }
    chclient->setMapInited();
}

void MapLayer::initSetItem() {
    for (auto enemy : m_enemy) {
        addChild(enemy, 2);
        setRandPos(enemy);
    }

    hunter->setLocalZOrder(4);

    runAction(Follow::create(hunter, Rect::ZERO));

    for (auto &bullet : bullets) {
        bullet = Bullet::create("images/bullet.png");
        bullet->setVisible(false);
        bullet->setBulletActive(false);
        addChild(bullet, 3);
    }

    initItem(weapons, m_weapon_number);
    initItem(m_bandage, m_bandage_number);
    initItem(m_ammunition, m_ammunition_number);

}

//aim at hunter automatically and fire
void MapLayer::enemyFire(float delt) {
    Rect rect_hunter = hunter->getBoundingBox();
    for (auto enemy : m_enemy) {
        if (enemy->getPlayerDeath() || enemy == hunter)
            continue;
        Rect rect_enemy(enemy->getPosition().x - 300, enemy->getPosition().y - 300, 600, 600);
        if (rect_enemy.intersectsRect(rect_hunter)) {
            auto weapon_type = enemy->getPlayerWeapon();
            if (5 == weapon_type) {
                makeKnifeAttack(enemy);
                continue;
            }
            if (4 == weapon_type) {
                if (enemy->getPlayerGrenade() > 0) {
                    enemy->setPlayerGrenade(0);
                    explosion_point.set(hunter->getPosition());
                    showEffect(explosion_point);
                    scheduleOnce(CC_SCHEDULE_SELECTOR(MapLayer::enemyExplosionEffect), 0.5f);
                }
                continue;
            }
            if (enemy->getPlayerBullet() > 0)
                enemy->setPlayerBullet(enemy->getPlayerBullet() - 1);
            else {
                enemy->setPlayerWeapon(5);
                continue;
            }
            Weapon *weapon = enemy->m_gun[weapon_type];
            try {
                if (weapon == nullptr)
                    throw R"(Weapon is nullptr)";
            }
            catch (exception& e) {
                string err = e.what();
                CCLOG("%s", err.c_str());
            }
            auto bullet_location = hunter->getPosition();    //enemy aims at hunter
            auto bullet_X = bullet_location.x - enemy->getPositionX();
            auto bullet_Y = bullet_location.y - enemy->getPositionY();
            makeBulletAttack(enemy, weapon, bullet_X, bullet_Y);
        }
    }
}

void MapLayer::AIFireForServer(float delt) {
    for (int j = 0; j < MAX_CONNECTIONS - 1; j++) {
        if (m_enemy[j]->getPlayerDeath() || m_enemy[j]->m_is_ai == false)
            continue;
        Rect rect_enemy(m_enemy[j]->getPosition().x - 300, m_enemy[j]->getPosition().y - 300, 600,
                        600);
        for (int i = 0; i < MAX_CONNECTIONS - 1; i++) {
            if (!m_enemy[i]->getPlayerDeath() && m_enemy[i]->m_has_controller &&
                !m_enemy[i]->m_is_ai && rect_enemy.intersectsRect(m_enemy[i]->getBoundingBox())) {
                auto weapon_type = m_enemy[j]->getPlayerWeapon();
                if (weapon_type == 5 || m_enemy[j]->getPlayerBullet() < 3) {
                    CCLOG("AI#%d TRY TO KNIFE#%d LEFT BULLET#%d WEAPONTYPE#%d", j, i,
                          m_enemy[j]->getPlayerBullet(), weapon_type);
                    chserver->paction[j + 1].is_shoot = true;
                    chserver->paction[j + 1].weapon_type = 4;
                    chserver->m_map_trans.player[j + 1].is_shoot = true;
                    chserver->m_map_trans.player[j + 1].weapon_type = weapon_type;
                    continue;
                }

                CCLOG("AI#%d TRY TO BULLET#%d LEFT BULLET#%d WEAPONTYPE#%d", j, i,
                      m_enemy[j]->getPlayerBullet(), weapon_type);
                auto bulletLocation = hunter->getPosition(); // enemy aims at hunter
                auto bulletX = bulletLocation.x - m_enemy[j]->getPositionX();
                auto bulletY = bulletLocation.y - m_enemy[j]->getPositionY();
                chserver->paction[j + 1].is_shoot = true;
                chserver->paction[j + 1].weapon_type = weapon_type;
                chserver->paction[j + 1].bullet_x = bulletX;
                chserver->paction[j + 1].bullet_y = bulletY;
                chserver->m_map_trans.player[j + 1].is_shoot = true;
                chserver->m_map_trans.player[j + 1].weapon_type = weapon_type;
                chserver->m_map_trans.player[j + 1].bullet_x = bulletX;
                chserver->m_map_trans.player[j + 1].bullet_y = bulletY;
                break;
            }
        }
    }
}

void MapLayer::autoFire(float dt) {
    Rect rect_hunter = hunter->getBoundingBox();
    for (auto enemy : m_enemy) {
        if (enemy->getPlayerDeath() || enemy == hunter)
            continue;
        Rect rect_enemy(enemy->getPosition().x - 400, enemy->getPosition().y - 400, 800, 800);
        if (rect_enemy.intersectsRect(rect_hunter)) {
            auto weapon_type = hunter->getPlayerWeapon();
            if (5 == weapon_type) {
                makeKnifeAttack(hunter);
            } else if (4 == weapon_type) {
                if (hunter->getPlayerGrenade() > 0) {
                    hunter->setPlayerGrenade(hunter->getPlayerGrenade() - 1);
                    hunter->m_bullet_location =
                            enemy->getPosition() - hunter->getPosition() + winSize / 2;
                    scheduleOnce(CC_SCHEDULE_SELECTOR(MapLayer::makeExplosionEffect), 0.5f);
                }
            } else {
                hunter->m_bullet_location =
                        enemy->getPosition() - hunter->getPosition() + winSize / 2;
                scheduleOnce(CC_SCHEDULE_SELECTOR(MapLayer::Fire), hunter->getBulletSpeed() -
                                                                   hunter->m_gun[hunter->getPlayerWeapon()]->getFireWeaponSpeed());
            }
        }
    }
}

int MapLayer::getTime() {
    return static_cast<int>(duration_cast<seconds>(system_clock::now() - start_time).count());
}

void MapLayer::showAimLine() {
    m_line->clear(); // Auxiliary firing line
    if (add_in_aiming == 1) {
        m_line->drawLine(hunter->getPosition(),
                         Vec2(10 * (hunter->m_bullet_location.x - winSize.width / 2) +
                              hunter->getPositionX(),
                              10 * (hunter->m_bullet_location.y - winSize.height / 2) +
                              hunter->getPositionY()),
                         Color4F(1.0f, 0.46f, 0.0f, 0.5f));
        m_line->setLineWidth(50);
    }
}

void MapLayer::initMouse() {
    mouse_listener = EventListenerMouse::create();

    mouse_listener->onMouseMove = [&](EventMouse *event) {
        m_line->clear();
        if (add_in_aiming == 1) {
            hunter->m_bullet_location = Vec2(event->getCursorX(), event->getCursorY());
            m_line->drawLine(hunter->getPosition(),
                             Vec2(10 * (hunter->m_bullet_location.x - winSize.width / 2) +
                                  hunter->getPositionX(),
                                  10 * (hunter->m_bullet_location.y - winSize.height / 2) +
                                  hunter->getPositionY()), Color4F(1.0f, 0.46f, 0.0f, 0.5f));
            m_line->setLineWidth(50);
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse_listener, this);
}