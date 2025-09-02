#include "./includes.h"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include "./RainMenuLayer.h"

class $modify(GameManager) {
	static void onModify(auto& self) {
		if (Loader::get()->isModLoaded("elnexreal.menuloop_randomizer")) (void) self.setHookPriorityAfterPost("GameManager::getMenuMusicFile", "elnexreal.menuloop_randomizer");
		else (void) self.setHookPriority("GameManager::getMenuMusicFile", Priority::Replace);
	}
	gd::string getMenuMusicFile() {
		return gd::string("menuLoop.mp3"_spr);
	}
};

class MenuLayerManager {
	static bool originalMenuLayer;

	void reloadMenuLayerAndToggleState(CCObject*) {
		CCScene* scene = CCScene::get();
		if (!scene) return;
		auto menuLayer = scene->getChildByID("MenuLayer");
		auto rainMenuLayer = scene->getChildByID("RainMenuLayer"_spr);
		if (!menuLayer || !rainMenuLayer) return;

		originalMenuLayer = !originalMenuLayer;
		menuLayer->setVisible(originalMenuLayer);
		rainMenuLayer->setVisible(!originalMenuLayer);

		CCNode* switcherLabel = scene->querySelector("rainixdev.rainmenu/rain-menu-menu > rainixdev.rainmenu/rain-menu-toggle > rainixdev.rainmenu/rain-menu-toggle-label");
		if (!switcherLabel) return;

		static_cast<CCLabelBMFont*>(switcherLabel)->setString(originalMenuLayer ? "to rain layer" : "to original layer");
	}

public:
	static CCNode* getNode() {	
		auto menuLayerSwitcherMenu = CCMenu::create();
		menuLayerSwitcherMenu->setPosition({ 0.f, 0.f });

		auto menuLayerSwitcherLabel = CCLabelBMFont::create(originalMenuLayer ? "to rain layer" : "to original layer", "Lemonmilk.fnt"_spr);
		menuLayerSwitcherLabel->setOpacity(30);
		menuLayerSwitcherLabel->setScale(0.3f);
		menuLayerSwitcherLabel->setID("rain-menu-toggle-label"_spr);
		auto menuLayerSwitcherBtn = CCMenuItemSpriteExtra::create(menuLayerSwitcherLabel, menuLayerSwitcherMenu, menu_selector(MenuLayerManager::reloadMenuLayerAndToggleState));
		menuLayerSwitcherBtn->setPosition({ 30.f, 5.f });
		menuLayerSwitcherBtn->setID("rain-menu-toggle"_spr);
		menuLayerSwitcherMenu->addChild(menuLayerSwitcherBtn);
		menuLayerSwitcherMenu->setID("rain-menu-menu"_spr);

		return menuLayerSwitcherMenu;
	}

	static bool getState() {
		return originalMenuLayer;
	}
};
bool MenuLayerManager::originalMenuLayer = false;


class $modify(MenuLayer) {
	static cocos2d::CCScene* scene(bool isVideoOptionsOpen) {
		CCScene* scene = MenuLayer::scene(isVideoOptionsOpen);

		auto layer = RainMenuLayer::create();
		layer->setID("RainMenuLayer"_spr);
		scene->addChild(layer);
		scene->addChild(MenuLayerManager::getNode());

		bool origMenuLayer = MenuLayerManager::getState();
		scene->getChildByID("MenuLayer")->setVisible(origMenuLayer);
		layer->setVisible(!origMenuLayer);
		return scene;
	}
};