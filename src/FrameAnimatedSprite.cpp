#include "FrameAnimatedSprite.h"

bool FrameAnimatedSprite::initFromPlist(std::string plist) {
    CCSprite::init();

    if (!isPlistLoaded(plist)) {
        CCSpriteFrameCache* frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
        frameCache->addSpriteFramesWithFile(plist.c_str());
        loadedPlists.push_back(plist);
    }

    CCDictionary* dictionary = CCDictionary::createWithContentsOfFile(plist.c_str());
    CCDictionary* framesDict = (CCDictionary*)dictionary->objectForKey("frames");
    CCDictElement* pElement = NULL;
    animationSprites = CCArray::create();
    animationSprites->retain();
    for (auto [frameName, frameDict] : CCDictionaryExt<std::string, CCDictionary*>(framesDict)){
        CCSpriteFrame* spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());
        animationSprites->addObject(spriteFrame);
    }

    return true;
}

bool FrameAnimatedSprite::initFromFiles(std::string baseFilename) {
    CCSprite::init();
    if (baseFilename.empty()) return false;

    animationSprites = CCArray::create();
    animationSprites->retain();
    int index = 0;
	while (true) {
		char buf[256];
		sprintf(buf, "%s%d.png", baseFilename.c_str(), index);
		std::string fileName = buf;

		if (!CCFileUtils::sharedFileUtils()->isFileExist("Resources/" + fileName)) {
			break;
		}

		CCSpriteFrame* spriteFrame = CCSpriteFrame::create(fileName.c_str(), 
			CCRect(0, 0, 
					   CCTextureCache::sharedTextureCache()->addImage(fileName.c_str(), false)->getContentSize().width,
					   CCTextureCache::sharedTextureCache()->addImage(fileName.c_str(), false)->getContentSize().height));
		if (spriteFrame) {
			animationSprites->addObject(spriteFrame);
		}
		index++;
	}

    if (animationSprites->count() == 0) return false;

    return true;
}

bool FrameAnimatedSprite::isPlistLoaded(std::string plist) {
    auto it = std::find(loadedPlists.begin(), loadedPlists.end(), plist);
    return it != loadedPlists.end();
}

void FrameAnimatedSprite::restart(float fullAnimationTime) {
    stopAllActions();
    CCAnimation* animation = CCAnimation::createWithSpriteFrames(animationSprites, fullAnimationTime / animationSprites->count());
    this->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
}

void FrameAnimatedSprite::pause() { 
    pauseSchedulerAndActions();
}

void FrameAnimatedSprite::resume() {
    resumeSchedulerAndActions();
}

unsigned int FrameAnimatedSprite::getCountOfFrames() {
    return animationSprites->count();
}


CCSize FrameAnimatedSprite::getContentSize() {
    return static_cast<CCSpriteFrame*>(animationSprites->firstObject())->getRect().size;
}

FrameAnimatedSprite* FrameAnimatedSprite::createFromPlist(std::string plist) {
    FrameAnimatedSprite* sprite = new FrameAnimatedSprite();
    if (sprite && sprite->initFromPlist(plist)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

FrameAnimatedSprite* FrameAnimatedSprite::createFromFiles(std::string baseFilename) {
    FrameAnimatedSprite* sprite = new FrameAnimatedSprite();
    if (sprite && sprite->initFromFiles(baseFilename)) {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

std::vector<std::string> FrameAnimatedSprite::loadedPlists = {};