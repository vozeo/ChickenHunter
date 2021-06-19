#pragma once
#ifndef __MAP_SCENE_H__
#define __MAP_SCENE_H__

#include "cocos2d.h"
#include "AudioEngine.h"
#include "Const.h"
#include "ui/CocosGUI.h"
#include "Character/Character.h"
#include "RankLayer.h"
#include "Item/Item.h"
#include "SettingLayer.h"
#include "Network/Client.h"
#include "Network/Server.h"
#include "Network/TransStructure.h"
#include "Scene/WarningLayer.h"

#include <map>
#include <ctime>
#include <chrono>
#include <utility>

using namespace std::chrono;

USING_NS_CC;

#define winSize Director::getInstance()->getWinSize()

class MapLayer : public cocos2d::Layer {
private:
    TMXTiledMap *map;
    TMXLayer *meta;

    float map_height, map_width;
    float tile_height, tile_width;

    Character *hunter;

    //add map information
    MapInformation save_map;
    //add enemies

    const int m_enemy_number = 10;
    const int m_weapon_number = 30;
    const int m_bandage_number = 30;
    const int m_ammunition_number = 30;
    clock_t t1 = clock(), t2;

    //add items
    std::vector<Character *> m_enemy;
    std::vector<Weapon *> weapons;
    std::vector<Bandage *> m_bandage;
    std::vector<Ammunition *> m_ammunition;

    std::array<Bullet *, 60> bullets;

    bool weapon_refresh = true;
    bool has_auto_attack = false;
    system_clock::time_point start_time;


public:
    MapLayer();

    static MapLayer *create(std::vector<Character *> game_hunter) {
        MapLayer *pRet = new(std::nothrow) MapLayer();
        if (pRet && pRet->init(game_hunter)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    static cocos2d::Layer *createScene(std::vector<Character *> &game_hunter);

    virtual bool init(std::vector<Character *> &game_hunter);

    virtual void update(float fDelta);

    void registerKeyboardEvent();

    void registerTouchEvent();

    void touchBegan(Touch *touch);

    void touchEnded();

    static void bindTouchMap(std::function<void(MapLayer *, Touch *touch)> &began,
                             std::function<void(MapLayer *)> &ended);

    static float calRotation(float bullet_X, float bullet_Y);

    void initSetItem();

    void initSetItemForClient();

    Vec2 explosion_point;

    DrawNode *m_line;

    void initMouse();

    EventListenerMouse *mouse_listener;

    template<class T>
    void setRandPos(T *elem);

    template<class T>
    void initItem(std::vector<T *> &items, int number);

    static void roll(Character *character);

    void judgePick(Character *character);

    void makeKnifeAttack(Character *character);

    void makeExplosionEffect(float dt);

    void enemyExplosionEffect(float dt);

    void makeExplosionEffectOnlyForShow(float posx, float posy);

    void makeBulletAttack(Character *character, Weapon *weapon, float bullet_X, float bullet_Y);

    void showEffect(Vec2 pos);

    void showAttacked(Vec2 pos);

    void enemyFire(float delt);

    void AIFireForServer(float delt);

    void autoFire(float dt);

    void Fire(float dt);

    int getTime();

    void showAimLine();
};

#endif
