#include "Item/Item.h"

USING_NS_CC;

Item::Item() :
        m_name(nullptr),
        m_content(nullptr),
        m_item_frame_cache(nullptr) {}

bool Item::init() {
    if (!Sprite::init()) {
        return false;
    }
    m_item_frame_cache = SpriteFrameCache::getInstance();
    m_item_frame_cache->addSpriteFramesWithFile("item.plist", "item.png");

    return true;
}

Weapon::Weapon() :
        m_attack(0),
        m_fire_speed(0),
        m_speed(0),
        m_state(0),
        m_type(0) {}

void Weapon::weaponInit(int type, bool state) {
    float sp[4] = {0.8f, 1.0f, 1.5f, 1.2f};
    m_speed = sp[type];
    float fsp[4] = {0.15f, 0.12f, 0.07f, 0.1f};
    m_fire_speed = fsp[type];
    float at[4] = {5.0f, 10.0f, 8.0f, 15.0f};
    m_attack = at[type];
    m_type = type;
    m_state = state;
}

Bandage::Bandage() :
        m_recover_HP(0) {}

bool Bandage::init() {
    if (!Item::init()) {
        return false;
    }

    auto frame = m_item_frame_cache->getSpriteFrameByName("bandage.png");
    this->initWithSpriteFrame(frame);

    char name[] = "bandage";
    setItemName(name);
    setRecoverHP(random(10, 80));    //random recovery from 10 to 80

    return true;
}

Ammunition::Ammunition() :
        m_add_bullet(0) {}

bool Ammunition::init() {
    if (!Item::init()) {
        return false;
    }

    auto frame = m_item_frame_cache->getSpriteFrameByName("ammunition.png");
    this->initWithSpriteFrame(frame);

    char name[] = "ammunition";
    setItemName(name);
    setAddBullet(random(20, 90));    //add random bullets from 20 to 90

    return true;
}

Bullet::Bullet() = default;