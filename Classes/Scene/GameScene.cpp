#include "GameScene.h"

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init()
{
    if ( !Scene::init() )
    { 
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    map = TMXTiledMap::create("map.tmx");
    addChild(map);
    //meta = map->getLayer("Obstacle");
    //meta->setVisible(false);

    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(Game::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(Game::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void Game::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d pressed", keyCode);
    if (EventKeyboard::KeyCode::KEY_W == keyCode)
        map->setPositionY(map->getPositionY() - 33);
    else if (EventKeyboard::KeyCode::KEY_S == keyCode)
        map->setPositionY(map->getPositionY() + 33);
    else if (EventKeyboard::KeyCode::KEY_A == keyCode)
        map->setPositionX(map->getPositionX() + 33);
    else if (EventKeyboard::KeyCode::KEY_D == keyCode)
        map->setPositionX(map->getPositionX() - 33);
}

void Game::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d released", keyCode);
}

void Game::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
