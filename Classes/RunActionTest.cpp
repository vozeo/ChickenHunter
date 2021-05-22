#include"RunActionTest.h"
USING_NS_CC;

Scene* FigureMoveLayer::createScene()
{
    auto scene = Scene::create();
    auto layer = FigureMoveLayer::create();
    scene->addChild(layer);

    return scene;
}

bool FigureMoveLayer::init() {
    if (!Layer::init())
    {
        return false;
    }

    this->scheduleUpdate();

    //Add title
    auto title = Label::createWithSystemFont(this->title(), "", 25);
    this->addChild(title);
    title->setPosition(Vec2(
        winSize.width / 2,
        winSize.height - title->getContentSize().height / 2));

    //Draw Line
    auto drawNode = DrawNode::create();
    drawNode->drawLine(Vec2(0, winSize.height / 2), Vec2(winSize.width * 2, winSize.height / 2), Color4F::RED);
    drawNode->drawLine(Vec2(0, winSize.height), Vec2(winSize.width * 2, winSize.height), Color4F::RED);
    drawNode->drawLine(Vec2(winSize.width / 2, 0), Vec2(winSize.width / 2, winSize.height * 2), Color4F::RED);
    drawNode->drawLine(Vec2(winSize.width, 0), Vec2(winSize.width, winSize.height * 2), Color4F::RED);
    this->addChild(drawNode);

    //Icon sprite
    FigureMoveLayer::main_sprite_ = Figure::create();
    auto spr = FigureMoveLayer::main_sprite_;
    addChild(spr);
    spr->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
   this->runAction(Follow::create(spr, Rect::ZERO));
    //spr->runAction(spr->getAniMoveRight());

    registerKeyboardEvent();
    
    return true;
}

void FigureMoveLayer::registerKeyboardEvent() {
    auto listener = EventListenerKeyboard::create();
    auto spr = FigureMoveLayer::main_sprite_;

    listener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        keyMap[keyCode] = true;
        switch (keyCode) {
            case EventKeyboard::KeyCode::KEY_D:
                spr->runAction(spr->getAniMoveRight());
                break;
            case EventKeyboard::KeyCode::KEY_A:
                spr->runAction(spr->getAniMoveLeft());
                break;
            case EventKeyboard::KeyCode::KEY_S:
                spr->runAction(spr->getAniMoveDown());
                break;
            case EventKeyboard::KeyCode::KEY_W:
                spr->runAction(spr->getAniMoveUp());
                break;
        }
    };

    listener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
        spr->stopAllActions();
        keyMap[keyCode] = false;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void FigureMoveLayer::update(float delta){
    auto left = EventKeyboard::KeyCode::KEY_A;
    auto right = EventKeyboard::KeyCode::KEY_D;
    auto up = EventKeyboard::KeyCode::KEY_W;
    auto down = EventKeyboard::KeyCode::KEY_S;

    int dx = 0,dy=0;
    if (keyMap[left])
    {
        dx = -4;
    }
    if (keyMap[right])
    {
        dx = 4;
    }
    if (keyMap[up])
    {
        dy = 4;
    }
    if (keyMap[down])
    {
        dy = -4;
    }
    auto spr = FigureMoveLayer::main_sprite_;
    auto moveTo = MoveTo::create(0.2, Vec2(spr->getPositionX() + dx, spr->getPositionY() + dy));
    spr->runAction(moveTo);
}

Figure::Figure()
    :ani_move_down_(nullptr),
    ani_move_left_(nullptr),
    ani_move_right_(nullptr),
    ani_move_up_(nullptr),
    figure_frame_cache_(nullptr){}

Figure::~Figure() {
    ani_move_down_->release();
    ani_move_up_->release();
    ani_move_left_->release();
    ani_move_right_->release();
}

bool Figure::init() {
    if (!Sprite::init())
    {
        return false;
    }

    initAnimate();
    initSprite();

    return true;
}

void Figure::initAnimate() {
    double ani_walk_delay_time = 0.1f;

    figure_frame_cache_ = SpriteFrameCache::getInstance();
    figure_frame_cache_->addSpriteFramesWithFile("character.plist","character.png");
    
    Vector<SpriteFrame*>ani_move_up_vec;
    Vector<SpriteFrame*>ani_move_right_vec;
    Vector<SpriteFrame*>ani_move_left_vec;
    Vector<SpriteFrame*>ani_move_down_vec;
    
    char s[20];
    for (int i = 1; i <=3; i++)
    {
        sprintf(s, "character_%d.png", i);
        auto frame = figure_frame_cache_->getSpriteFrameByName(s);
        ani_move_up_vec.pushBack(frame);
    }
    ani_move_up_vec.pushBack(figure_frame_cache_->getSpriteFrameByName("character_2.png"));

    for (int i = 4; i <=6; i++)
    {
        sprintf(s, "character_%d.png", i);
        auto frame = figure_frame_cache_->getSpriteFrameByName(s);
        ani_move_right_vec.pushBack(frame);
    }
    ani_move_right_vec.pushBack(figure_frame_cache_->getSpriteFrameByName("character_5.png"));

    for (int i = 7; i <=9; i++)
    {
        sprintf(s, "character_%d.png", i);
        auto frame = figure_frame_cache_->getSpriteFrameByName(s);
        ani_move_left_vec.pushBack(frame);
    }
    ani_move_left_vec.pushBack(figure_frame_cache_->getSpriteFrameByName("character_8.png"));

    for (int i = 10; i <=12; i++)
    {
        sprintf(s, "character_%d.png", i);
        auto frame = figure_frame_cache_->getSpriteFrameByName(s);
        ani_move_down_vec.pushBack(frame);
    }
    ani_move_down_vec.pushBack(figure_frame_cache_->getSpriteFrameByName("character_11.png"));

    ani_move_down_ = Animate::create(Animation::createWithSpriteFrames(ani_move_down_vec, ani_walk_delay_time,-1));
    ani_move_left_ = Animate::create(Animation::createWithSpriteFrames(ani_move_left_vec, ani_walk_delay_time,-1));
    ani_move_right_ = Animate::create(Animation::createWithSpriteFrames(ani_move_right_vec, ani_walk_delay_time,-1));
    ani_move_up_ = Animate::create(Animation::createWithSpriteFrames(ani_move_up_vec, ani_walk_delay_time,-1));

    ani_move_down_->retain();
    ani_move_left_->retain();
    ani_move_right_->retain();
    ani_move_up_->retain();
}

void Figure::initSprite(){
    auto frame = figure_frame_cache_->getSpriteFrameByName("character_11.png");
    this->initWithSpriteFrame(frame);
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
}



