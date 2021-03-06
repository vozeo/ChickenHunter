#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "cocos2d.h"
#include "Item/Item.h"
#include <vector>

USING_NS_CC;

class Character : public Sprite {
public:
    Character();

    ~Character();

    static cocos2d::Sprite *createSprite();

    virtual bool init();

    CREATE_FUNC(Character);


CC_SYNTHESIZE(Animate*, m_character_anim_down, CharacterAnimDown);
CC_SYNTHESIZE(Animate*, m_character_anim_left, CharacterAnimLeft);
CC_SYNTHESIZE(Animate*, m_character_anim_right, CharacterAnimRight);
CC_SYNTHESIZE(Animate*, m_character_anim_up, CharacterAnimUp);
CC_SYNTHESIZE(SpriteFrameCache*, m_character_frame_cache, FrameCache);
CC_SYNTHESIZE(int, m_think_time, ThinkTime);
CC_SYNTHESIZE(int, m_thought, Thought);
CC_SYNTHESIZE(float, m_bullet_speed, BulletSpeed);

    void initName();

    void initSprite();

    void initAnimate();

private:
CC_SYNTHESIZE(int, m_MAX_BLEED, MAXBLEED);
CC_SYNTHESIZE(int, m_bleed, PlayerBleed);
CC_SYNTHESIZE(float, m_attack, PlayerAttack);
CC_SYNTHESIZE(float, m_defense, PlayerDefense);
CC_SYNTHESIZE(int, m_weapon, PlayerWeapon);
CC_SYNTHESIZE(int, m_bullet, PlayerBullet);
CC_SYNTHESIZE(int, m_grenade, PlayerGrenade);
CC_SYNTHESIZE(int, m_point, PlayerPoint);
CC_SYNTHESIZE(bool, m_refresh, PlayerRefresh);
CC_SYNTHESIZE(bool, m_hasDeath, PlayerDeath);

CC_SYNTHESIZE(bool, m_lockedBleed, PlayerLockedBleed);
CC_SYNTHESIZE(bool, m_lockedBullet, PlayerLockedBullet);
CC_SYNTHESIZE(bool, m_autoAttack, PlayerAutoAttack);

public:
    std::vector<Weapon *> m_we;
    std::array<int, 2> m_bandage = {};
    std::array<Weapon *, 6> m_gun = {};
    std::array<bool, 4> m_speed = {};
    bool m_is_ai = false;
    bool m_has_controller = false;
    Vec2 m_bullet_location;
};

#endif // !_CHARACTER_H_