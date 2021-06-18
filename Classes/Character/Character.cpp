#include "Character/Character.h"

USING_NS_CC;

Character::Character() :
        m_character_anim_down(nullptr),
        m_character_anim_left(nullptr),
        m_character_anim_right(nullptr),
        m_character_anim_up(nullptr),
        m_character_frame_cache(nullptr),
        m_MAX_BLEED(100),
        m_bleed(m_MAX_BLEED),
        m_attack(1),
        m_defense(1),
        m_weapon(5),
        m_bullet(0),
        m_grenade(0),
        m_point(0),
        m_refresh(false),
        m_hasDeath(false),
        m_lockedBleed(false),
        m_lockedBullet(false),
        m_autoAttack(false) {}

Character::~Character() {
    m_character_anim_down->release();
    m_character_anim_left->release();
    m_character_anim_right->release();
    m_character_anim_up->release();
}


bool Character::init() {
    if (!Sprite::init()) {
        return false;
    }

    initAnimate();
    initSprite();
    m_think_time = random(1500, 2000);// rand() % 4 * 200 + 1500
    m_thought = 0;
    m_bullet_speed = 0.3f;
    return true;
}

void Character::initAnimate() {
    double ani_walk_delay_time = 0.1f;

    m_character_frame_cache = SpriteFrameCache::getInstance();
    m_character_frame_cache->addSpriteFramesWithFile("character.plist", "character.png");

    Vector<SpriteFrame *> ani_move_up_vec;
    Vector<SpriteFrame *> ani_move_right_vec;
    Vector<SpriteFrame *> ani_move_left_vec;
    Vector<SpriteFrame *> ani_move_down_vec;

    char s[20];
    for (int i = 1; i <= 3; i++) {
        sprintf(s, "character_%d.png", i);
        auto frame = m_character_frame_cache->getSpriteFrameByName(s);
        ani_move_up_vec.pushBack(frame);
    }
    ani_move_up_vec.pushBack(m_character_frame_cache->getSpriteFrameByName("character_2.png"));

    for (int i = 4; i <= 6; i++) {
        sprintf(s, "character_%d.png", i);
        auto frame = m_character_frame_cache->getSpriteFrameByName(s);
        ani_move_right_vec.pushBack(frame);
    }
    ani_move_right_vec.pushBack(m_character_frame_cache->getSpriteFrameByName("character_5.png"));

    for (int i = 7; i <= 9; i++) {
        sprintf(s, "character_%d.png", i);
        auto frame = m_character_frame_cache->getSpriteFrameByName(s);
        ani_move_left_vec.pushBack(frame);
    }
    ani_move_left_vec.pushBack(m_character_frame_cache->getSpriteFrameByName("character_8.png"));

    for (int i = 10; i <= 12; i++) {
        sprintf(s, "character_%d.png", i);
        auto frame = m_character_frame_cache->getSpriteFrameByName(s);
        ani_move_down_vec.pushBack(frame);
    }
    ani_move_down_vec.pushBack(m_character_frame_cache->getSpriteFrameByName("character_11.png"));

    m_character_anim_down = Animate::create(Animation::createWithSpriteFrames(ani_move_down_vec,
                                                                              static_cast<float>(ani_walk_delay_time),
                                                                              -1));
    m_character_anim_left = Animate::create(Animation::createWithSpriteFrames(ani_move_left_vec,
                                                                              static_cast<float>(ani_walk_delay_time),
                                                                              -1));
    m_character_anim_right = Animate::create(Animation::createWithSpriteFrames(ani_move_right_vec,
                                                                               static_cast<float>(ani_walk_delay_time),
                                                                               -1));
    m_character_anim_up = Animate::create(Animation::createWithSpriteFrames(ani_move_up_vec,
                                                                            static_cast<float>(ani_walk_delay_time),
                                                                            -1));

    m_character_anim_down->retain();
    m_character_anim_left->retain();
    m_character_anim_right->retain();
    m_character_anim_up->retain();
}

void Character::initSprite() {
    auto frame = m_character_frame_cache->getSpriteFrameByName("character_11.png");
    this->initWithSpriteFrame(frame);
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
}
