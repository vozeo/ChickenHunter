#include "Character/Character.h"

USING_NS_CC;

Character::Character() :
	m_character_anim_down(nullptr),
	m_character_anim_left(nullptr),
	m_character_anim_right(nullptr),
	m_character_anim_up(nullptr),
	m_character_frame_cache(nullptr),
	m_bleed(m_MAX_BLEED),
	m_attack(1),
	m_defense(1){}

int* Character::getPlayerBandage() { return m_bandage; }
void Character::setPlayerBandage(int var, int pos) { m_bandage[pos] = var; }

int* Character::getPlayerGun() { return m_gun; }
void Character::setPlayerGun(int var, int pos) { m_gun[pos] = var; }

bool Character::init()
{
	if (!Sprite::init())
	{
		return false;
	}

	initAnimate();	//初始化动画对象
	initSprite();	//设置人物初始形象

	return true;
}

void Character::initAnimate()
{
	/*
	character_frame_cache_ = SpriteFrameCache::getInstance();
	character_frame_cache_->addSpriteFramesWithFile(StringUtils::format("%s.plist", name_.c_str()), StringUtils::format("%s.png", name_.c_str()));

	Vector<SpriteFrame*>character_anim_down_vec;
	Vector<SpriteFrame*>character_anim_left_vec;
	Vector<SpriteFrame*>character_anim_right_vec;
	Vector<SpriteFrame*>character_anim_up_vec;

	for (int i = 0; i < 4; ++i)
	{
		auto frame = character_frame_cache_->getSpriteFrameByName(StringUtils::format("%s-%d.png", name_.c_str(), i));
		character_anim_down_vec.pushBack(frame);
	}
	for (int i = 4; i < 8; ++i)
	{
		auto frame = character_frame_cache_->getSpriteFrameByName(StringUtils::format("%s-%d.png", name_.c_str(), i));
		character_anim_left_vec.pushBack(frame);
	}
	for (int i = 8; i < 12; ++i)
	{
		auto frame = character_frame_cache_->getSpriteFrameByName(StringUtils::format("%s-%d.png", name_.c_str(), i));
		character_anim_right_vec.pushBack(frame);
	}
	for (int i = 12; i < 16; ++i)
	{
		auto frame = character_frame_cache_->getSpriteFrameByName(StringUtils::format("%s-%d.png", name_.c_str(), i));
		character_anim_up_vec.pushBack(frame);
	}

	character_anim_down_ = Animate::create(Animation::createWithSpriteFrames(character_anim_down_vec, character_walk_delay_time));
	character_anim_left_ = Animate::create(Animation::createWithSpriteFrames(character_anim_left_vec, character_walk_delay_time));
	character_anim_right_ = Animate::create(Animation::createWithSpriteFrames(character_anim_right_vec, character_walk_delay_time));
	character_anim_up_ = Animate::create(Animation::createWithSpriteFrames(character_anim_up_vec, character_walk_delay_time));

	character_anim_down_->retain();
	character_anim_left_->retain();
	character_anim_right_->retain();
	character_anim_up_->retain();
	*/
}

void Character::initSprite()
{
	/*
	auto spf = character_frame_cache_->getSpriteFrameByName(StringUtils::format("%s-4.png", name_.c_str()));
	this->initWithSpriteFrame(spf);
	this->setAnchorPoint(Vec2(0.5f, 0.15f));
	*/
}