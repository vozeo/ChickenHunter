#ifndef _ITEM_H_
#define _ITEM_H_

#include "cocos2d.h"
#include "Character/Character.h"

USING_NS_CC;

class Item : public Sprite {
protected:
	CC_SYNTHESIZE(char*, m_name, ItemName);
	CC_SYNTHESIZE(char*, m_content, ItemContent);
	CC_SYNTHESIZE(SpriteFrameCache*, m_item_frame_cache, ItemFrameCache);

public:
	/*virtual void work(Character* hunter)
	{
		return;
	}*/
	virtual bool init();
};


class Bandage : public Item {
private:
	static int number;
	CC_SYNTHESIZE(int, m_recover_HP, RecoverHP);
public:
	//virtual void work(Character* hunter);
	virtual bool init();
	static int getNumber() { return number; }
	CREATE_FUNC(Bandage);

};

class Ammunition : public Item {
private:
	static int number;
	CC_SYNTHESIZE(int, m_add_bullet, AddBullet);
public:
	//virtual void work(Character* hunter);
	virtual bool init();
	static int getNumber() { return number; }
	CREATE_FUNC(Ammunition);
};


#endif // !_ITEM_H_
