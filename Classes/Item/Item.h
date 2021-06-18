#ifndef _ITEM_H_
#define _ITEM_H_

#include "cocos2d.h"

USING_NS_CC;

class Item : public Sprite {

protected:
CC_SYNTHESIZE(char*, m_name, ItemName);
CC_SYNTHESIZE(char*, m_content, ItemContent);
CC_SYNTHESIZE(SpriteFrameCache*, m_item_frame_cache, ItemFrameCache);

public:
    Item();

    virtual bool init();
};

class Bullet : public Item {
public:
    Bullet();

    static Bullet *create(const std::string &filename) {
        Bullet *sprite = new(std::nothrow) Bullet();
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

CC_SYNTHESIZE(bool, m_active, BulletActive);
CC_SYNTHESIZE(int, m_speed, BulletSpeed);
CC_SYNTHESIZE(int, m_attack, BulletAttack);
};

class Weapon : public Item {
public:
    Weapon();

    static Weapon *create() {
        Weapon *weapon = new(std::nothrow) Weapon();
        auto weaponType = random(0, 4);
        if (weapon &&
            weapon->initWithFile(StringUtils::format("images/weapon_%d.png", weaponType))) {
            weapon->autorelease();
            weapon->weaponInit(weaponType, true);
            return weapon;
        }
        CC_SAFE_DELETE(weapon);
        return nullptr;
    }

    static Weapon *create(int weaponType) {
        Weapon *weapon = new(std::nothrow) Weapon();
        if (weapon &&
            weapon->initWithFile(StringUtils::format("images/weapon_%d.png", weaponType))) {
            weapon->autorelease();
            weapon->weaponInit(weaponType, true);
            return weapon;
        }
        CC_SAFE_DELETE(weapon);
        return nullptr;
    }

CC_SYNTHESIZE(float, m_speed, WeaponSpeed);
CC_SYNTHESIZE(float, m_fire_speed, FireWeaponSpeed);
CC_SYNTHESIZE(float, m_attack, WeaponAttack);
CC_SYNTHESIZE(int, m_type, WeaponType);
CC_SYNTHESIZE(bool, m_state, WeaponState);

    void weaponInit(int type, bool state);
};

class Bandage : public Item {
private:
CC_SYNTHESIZE(int, m_recover_HP, RecoverHP);
public:
    Bandage();

    virtual bool init();

    CREATE_FUNC(Bandage);

};

class Ammunition : public Item {
private:
CC_SYNTHESIZE(int, m_add_bullet, AddBullet);
public:
    Ammunition();

    virtual bool init();

    CREATE_FUNC(Ammunition);
};


#endif // !_ITEM_H_
