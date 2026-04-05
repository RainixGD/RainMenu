#pragma once
#include "./includes.h"
#include "./CloudLayer.h"
#include "./FrameAnimatedSprite.h"
#include "./TreeButton.h"
#include "./AnimationUtils.h"
#include "./SpriteUtils.h"
#include "./ParticleSystemBuilder.h"
#include <Geode/ui/GeodeUI.hpp>

#define SCALE_POS_FROM_UHD(x, y, uhdParentSize, parentSize) { \
    (x) / (uhdParentSize).width * (parentSize).width,         \
    (y) / (uhdParentSize).height * (parentSize).height        \
}

#define SCALE_VAL_FROM_UHD(v, uhdParentSize, parentSize) (   \
    (v) / (uhdParentSize).height * (parentSize).height       \
)

CCPoint convertChildPositionFromUHD(CCPoint childPositionInUHD, CCSize parentSizeInUHD, CCNode* parent) {
	return { childPositionInUHD.x / parentSizeInUHD.width * parent->getContentWidth(), childPositionInUHD.y / parentSizeInUHD.height * parent->getContentHeight() };
}

float convertChildScaleFromUHD(float scaleInUHD, float parentHeightInUHD, CCNode* parent) {
	return scaleInUHD / parentHeightInUHD * parent->getContentHeight();
}

class RainMenuLayer : public CCLayer {
	enum RainMenuLayerOrder {
		BG_Z_ORDER = 1,
		BG_OVERLAY_Z_ORDER = 2,
		RAIN_BACK_Z_ORDER = 3,
		TREE_Z_ORDER = 4,
		RAIN_MIDDLE_Z_ORDER = 5,
		CLOUD_LAYER_ORDER = 6,
		FOREGROUND_Z_ORDER = 7,
		RAIN_FRONT_Z_ORDER = 8,
		EXIT_MENU_Z_ORDER = 9
	};

	CCSprite* bg;
	CCLayerColor* bgOverlay;
	CCSprite* foreground;
	CCSprite* tree;

	CCParticleSystemQuad* rainBack;
	CCParticleSystemQuad* rainMiddle;
	CCParticleSystemQuad* rainFront;

	FrameAnimatedSprite* lightning;
	CloudLayer* cloudLayer;

	std::vector<TreeButton*> treeButtons;

	virtual bool init() {
		if (!CCLayer::init()) return false;

		auto size = CCDirector::get()->getWinSize();
	
		bg = CCSprite::create("forestBG.png"_spr);
		scaleToFit(bg, size, ScaleMode::Cover);
		bg->setPosition({ size.width / 2, 0.f });
		bg->setScaleX(bg->getScaleX() * 1.02f); // animation fix
		bg->setAnchorPoint({ 0.5, 0 });
		this->addChild(bg, BG_Z_ORDER);

		auto bgSize = bg->getContentSize();
		CCSize bgUhdSize = { 480.f, 270.f };

		bgOverlay = CCLayerColor::create(ccc4(255, 255, 255, 200), size.width, size.height);
		bgOverlay->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
		bgOverlay->setOpacity(0);
		this->addChild(bgOverlay, BG_OVERLAY_Z_ORDER);

		bg->runAction(
			CCRepeatForever::create(
				CCSequence::createWithTwoActions(
					CCEaseInOut::create(CCSkewTo::create(4, -2, 0), 2),
					CCEaseInOut::create(CCSkewTo::create(4, 2, 0), 2)
				)
			)
		);
		
		foreground = CCSprite::create("foreground.png"_spr);
		scaleToFit(foreground, size, ScaleMode::Contain);
		foreground->setPosition({0.f, 0.f});
		foreground->setAnchorPoint({ 0.f, 0.f });
		this->addChild(foreground, FOREGROUND_Z_ORDER);

		tree = CCSprite::create("tree.png"_spr);
		scaleToFit(tree, size, ScaleMode::Contain);
		tree->setPosition({ size.width / 2, -17 });
		tree->setAnchorPoint({ 0.5, 0 });
		tree->setZOrder(2);
		this->addChild(tree, TREE_Z_ORDER);

		tree->runAction(
			CCRepeatForever::create(
				CCSequence::createWithTwoActions(
					CCEaseInOut::create(CCSkewTo::create(0.05, -0.03, -0.03), 3), 
					CCEaseInOut::create(CCSkewTo::create(0.05, 0.03, 0.03), 3)
				)
			)
		);

		tree->runAction(
			CCRepeatForever::create(
				CCSequence::createWithTwoActions(
					CCEaseIn::create(CCRotateTo::create(4, -1), 3), 
					CCEaseIn::create(CCRotateTo::create(4, 2), 3)
				)
			)
		);

	rainBack = ParticleSystemBuilder::buildGravity(4000)
		.spawnArea({size.width, 15.f})
		.gravity({0.f, -50.f})
		.color(
			{0.15f, 0.15f, 0.15f, 0.4f},
			{0.f,   0.f,   0.f,   0.f},
			{0.15f, 0.15f, 0.15f, 0.4f},
			{0.f,   0.f,   0.f,   0.f}
		)
		.size(0.5f, 1.f, 0.5f, 1.f)
		.speed(-100.f, 40.f)
		.angle(90.f, 0.f)
		.lifetime(2.f, 0.15f)
		.emissionRate(2000.f)
		.texture("rain.png"_spr)
		.get();

	rainBack->setPosition({size.width / 2.f, size.height * 1.5f});
	rainBack->setScaleY(5);
	this->addChild(rainBack, RAIN_BACK_Z_ORDER);

	rainMiddle = ParticleSystemBuilder::buildGravity(3000)
		.spawnArea({size.width, size.height})
		.gravity({0.f, -200.f})
		.color(
			{0.2f, 0.2f, 0.2f, 0.6f},
			{0.f,  0.f,  0.f,  0.f},
			{0.2f, 0.2f, 0.2f, 0.6f},
			{0.f,  0.f,  0.f,  0.f}
		)
		.size(0.5f, 1.f, 0.5f, 1.f)
		.speed(0.f, 40.f)
		.angle(90.f, 0.f)
		.lifetime(2.f, 0.15f)
		.emissionRate(1500.f)
		.texture("rain.png"_spr)
		.get();

	rainMiddle->setPosition({size.width / 2.f, size.height / 1.5f});
	rainMiddle->setScaleY(5);
	this->addChild(rainMiddle, RAIN_MIDDLE_Z_ORDER);

	rainFront = ParticleSystemBuilder::buildGravity(400)
		.spawnArea({size.width, 15.f})
		.gravity({0.f, -300.f})
		.color(
			{0.2f, 0.2f, 0.2f, 0.7f},
			{0.f,  0.f,  0.f,  0.f},
			{0.2f, 0.2f, 0.2f, 0.7f},
			{0.f,  0.f,  0.f,  0.f}
		)
		.size(1.f, 2.f, 1.f, 2.f)
		.speed(0.f, 40.f)
		.angle(90.f, 0.f)
		.lifetime(2.f, 0.15f)
		.emissionRate(170.f)
		.texture("rain.png"_spr)
		.get();

	rainFront->setPosition({size.width / 2.f, size.height});
	rainFront->setScaleY(5);
	this->addChild(rainFront, RAIN_FRONT_Z_ORDER);


	lightning = FrameAnimatedSprite::createFromPlist("Lightning.plist"_spr);
	lightning->setPosition(SCALE_POS_FROM_UHD(227.f, 270.f, bgUhdSize, bgSize));
	lightning->setScale(SCALE_VAL_FROM_UHD(0.425, bgUhdSize, bgSize));
	lightning->setRotation(71);
	lightning->restart(1.f / 17.f * 3.f);
	lightning->setOpacity(0);
	bg->addChild(lightning);

	cloudLayer = CloudLayer::create();
	this->addChild(cloudLayer, CLOUD_LAYER_ORDER);


	struct TreeButtonSettings {
		const char* texture;
		std::string label;
		CCPoint position;
		float scale;
		float scaleAnimationFadeDuration;
		std::function<void()> callback;
	};

	auto treeSize = tree->getContentSize();
	auto uhdTreeSize = CCSize(480.f, 270.f);
	
	std::vector<TreeButtonSettings> buttons = {
		{
			"createBtn.png"_spr,
			"create",
			SCALE_POS_FROM_UHD(177.f, 259.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(1.05f, uhdTreeSize, treeSize),
			4.f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onMyLevels(nullptr);
			}
		},
		{
			"dailyBtn.png"_spr,
			"daily",
			SCALE_POS_FROM_UHD(287.f, 223.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(1.f, uhdTreeSize, treeSize),
			3.5f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onDailyLevel(nullptr);
			}
		},
		{
			"eventBtn.png"_spr,
			"event",
			SCALE_POS_FROM_UHD(369.f, 219.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.95f, uhdTreeSize, treeSize),
			3.8f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onEventLevel(nullptr);
			}
		},
		{
			"featuredBtn.png"_spr,
			"featured",
			SCALE_POS_FROM_UHD(123.f, 243.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(1.05f, uhdTreeSize, treeSize),
			3.2f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onFeaturedLevels(nullptr);
			}
		},
		{
			"gauntletsBtn.png"_spr,
			"gauntlets",
			SCALE_POS_FROM_UHD(89.f, 125.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.775f, uhdTreeSize, treeSize),
			4.1f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onGauntlets(nullptr);
			}
		},
		{
			"iconsBtn.png"_spr,
			"icons",
			SCALE_POS_FROM_UHD(265.f, 247.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.9f, uhdTreeSize, treeSize),
			4.2f,
			[]() {
				auto scene = GJGarageLayer::scene();
				CCDirector::get()->pushScene(CCTransitionFade::create(0.3f, scene));
			}
		},
		{
			"mainLevelsBtn.png"_spr,
			"main levels",
			SCALE_POS_FROM_UHD(89.f, 171.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.975f, uhdTreeSize, treeSize),
			4.5f,
			[]() {
				auto scene = LevelSelectLayer::scene(0);	
				CCDirector::get()->pushScene(CCTransitionFade::create(0.3f, scene));
			}
		},
		{
			"mapPacksBtn.png"_spr,
			"map packs",
			SCALE_POS_FROM_UHD(130.f, 91.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.55f, uhdTreeSize, treeSize),
			4.6f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onMapPacks(nullptr);
			}
		},
		{
			"onlineLevelsBtn.png"_spr,
			"online levels",
			SCALE_POS_FROM_UHD(104.f, 208.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.95f, uhdTreeSize, treeSize),
			4.8f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onOnlineLevels(nullptr);
			}
		},
		{
			"optionsBtn.png"_spr,
			"options",
			SCALE_POS_FROM_UHD(226.f, 252.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.9f, uhdTreeSize, treeSize),
			3.3f,
			[]() {
				auto optionsLayer = OptionsLayer::create();
				CCDirector::get()->getRunningScene()->addChild(optionsLayer);
				optionsLayer->showLayer(false);
			}
		},
		{
			"questsBtn.png"_spr,
			"quests",
			SCALE_POS_FROM_UHD(146.f, 258.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.475f, uhdTreeSize, treeSize),
			3.9f,
			[]() {
				ChallengesPage::create()->show();
			}
		},
		{
			"savedLevelsBtn.png"_spr,
			"saved levels",
			SCALE_POS_FROM_UHD(132.f, 120.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.625f, uhdTreeSize, treeSize),
			4.2f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onSavedLevels(nullptr);
			}
		},
		{
			"topBtn.png"_spr,
			"top",
			SCALE_POS_FROM_UHD(376.f, 183.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.825f, uhdTreeSize, treeSize),
			5.f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onLeaderboards(nullptr);
			}
		},
		{
			"weeklyBtn.png"_spr,
			"weekly",
			SCALE_POS_FROM_UHD(328.f, 239.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(1.f, uhdTreeSize, treeSize),
			3.7f,
			[]() {
				auto cl = CreatorLayer::create();
				cl->onWeeklyLevel(nullptr);
			}
		},
		{
			"modsBtn.png"_spr,
			"mods",
			SCALE_POS_FROM_UHD(342.f, 101.f, uhdTreeSize, treeSize),
			SCALE_VAL_FROM_UHD(0.8f, uhdTreeSize, treeSize),
			3.6f,
			[]() {
				geode::openModsList();
			}
		}
		};

		auto menu = CCMenu::create();
		menu->setPosition({ 0.f, 0.f });
		tree->addChild(menu);

		for (const auto& buttonSettings : buttons) {
			auto button = TreeButton::create(buttonSettings.texture, buttonSettings.label, buttonSettings.scaleAnimationFadeDuration, buttonSettings.callback);
			button->setScale(buttonSettings.scale);
			button->setPosition(buttonSettings.position);
			menu->addChild(button);
			treeButtons.push_back(button);
		}

		this->schedule(schedule_selector(RainMenuLayer::startLightning), 10);

		
		auto exitMenu = CCMenu::create();
		exitMenu->setPosition({ 0.f, 0.f });
		this->addChild(exitMenu, EXIT_MENU_Z_ORDER);

		auto exitLabel = CCLabelBMFont::create("exit", "Lemonmilk.fnt"_spr);
		exitLabel->setOpacity(50);
		auto exitBtn = CCMenuItemSpriteExtra::create(exitLabel, this, menu_selector(RainMenuLayer::onQuit));
		exitBtn->setPosition({ 25.f, size.height - 20.f });
		exitMenu->addChild(exitBtn);
		

		return true;
	}

	void onQuit(CCObject*) {
		geode::utils::game::exit(true);
	}

	void startLightning(float dt) {
		bgOverlay->runAction(
			CCSequence::create(
				CCFadeTo::create(0.2f, 255),
				CCFadeTo::create(0.4f, 0),
				nullptr
			)
		);

		lightning->restart(0.2f);
		lightning->runAction(
			CCSequence::create(
				CCFadeTo::create(0.01f, 255),
				CCDelayTime::create(0.1f),
				CCFadeTo::create(0.08f, 0),
				nullptr
			)
		);

		this->runAction(shakeAction(15.f, 5.f, 0.02f));

		FMODAudioEngine::get()->playEffect("thunder.mp3"_spr);

		for (auto button : treeButtons) {
			button->runAction(
				CCSequence::create(
					CCCallFunc::create(button, callfunc_selector(TreeButton::hideRainbowAndGlow)),
					CCDelayTime::create(0.5f),
					CCCallFunc::create(button, callfunc_selector(TreeButton::showRainbowAndGlow)),
					nullptr
				)
			);
		}
	}

public:

	static RainMenuLayer* create() {
		auto ret = new RainMenuLayer;
		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}
};
