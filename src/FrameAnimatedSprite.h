#pragma once
#include "./includes.h"

class FrameAnimatedSprite : public CCSprite {
    static std::vector<std::string> loadedPlists;

    bool initFromPlist(std::string plist);
    bool initFromFiles(std::string baseFilename);

    bool isPlistLoaded(std::string plist);

    CCArray* animationSprites;

public:
    virtual ~FrameAnimatedSprite() {
        animationSprites->release();
    }

    void restart(float fullAnimationTime);
    void pause();
    void resume();
    unsigned int getCountOfFrames();
    CCSize getContentSize();
    static FrameAnimatedSprite* createFromPlist(std::string plist);
    static FrameAnimatedSprite* createFromFiles(std::string baseFilename);
};