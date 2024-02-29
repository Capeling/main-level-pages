#include <Geode/Geode.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include "levelUtils.hpp"

#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET

class $modify(GJGameLevel) {
	int getAverageDifficulty() {
		if(this->m_levelType == GJLevelType::Local) {
			if(this->m_difficulty == GJDifficulty::Demon) {
				return 1;
			}
			return (int)this->m_difficulty;
		}
		return GJGameLevel::getAverageDifficulty();
	}

};

class $modify(LevelAreaInnerLayer) {
	void onDoor(CCObject* sender) {
		auto LLM = LocalLevelManager::sharedState();

		LevelAreaInnerLayer::onDoor(sender);
		/*auto level = GameLevelManager::sharedState()->getMainLevel();
		level->m_levelString = LLM->getMainLevelString(level->m_levelID.value());
		level->m_creatorName = "RobTop";
		level->m_coinsVerified = true;
		level->m_accountID = 71;
		level->m_levelLength = 3;
		level->m_demonDifficulty = 3;
		level->m_featured = 1;

		auto scene = CCScene::create();
		scene->addChild(LevelInfoLayer::create(level, false));

		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));*/ 
		
		//future update maybe
	}
};

class $modify(LevelPage) {
	void onPlay(CCObject*) {
		auto LLM = LocalLevelManager::sharedState();

		auto level = this->m_level;
		level->m_levelString = LLM->getMainLevelString(level->m_levelID.value());
		level->m_creatorName = "RobTop";
		level->m_coinsVerified = true;
		level->m_accountID = 71;
		level->m_levelLength = 3;
		level->m_demonDifficulty = 3;
		level->m_featured = 1;

		auto scene = CCScene::create();
		scene->addChild(LevelInfoLayer::create(level, false));

		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
	}
};

class $modify(LevelInfoLayerExt, LevelInfoLayer) {

	bool m_isMain = false;

	bool init(GJGameLevel* level, bool isGauntlet) {

		if(!LevelInfoLayer::init(level, isGauntlet))
			return false;
		
		m_fields->m_isMain = level->m_levelType == GJLevelType::Local;
		log::info("is main");
		auto SFC = CCSpriteFrameCache::sharedSpriteFrameCache();
		auto GSM = GameStatsManager::sharedState();
		if(m_fields->m_isMain) {
			m_likesLabel->setVisible(false);
			m_likesIcon->setVisible(false);
			m_downloadsLabel->setVisible(false);
			this->getChildByID("downloads-icon")->setVisible(false);

			m_orbsIcon->setPosition(this->getChildByID("length-icon")->getPosition());
			m_orbsLabel->setPosition(m_lengthLabel->getPosition());
			m_lengthLabel->setPosition(m_likesLabel->getPosition());
			this->getChildByID("length-icon")->setPosition(m_likesIcon->getPosition());
			for(int i = 0; i < m_coins->count(); i++) {
				log::info("starting coins");
				auto node = static_cast<CCSprite*>(m_coins->objectAtIndex(i));
				node->setDisplayFrame(CCSprite::create("goldcoin_small01_001.png"_spr)->displayFrame());
				if(GSM->hasSecretCoin(fmt::format("{}_{}", level->m_levelID.value(), i + 1).c_str()))
					node->setColor({255,255,255});
			}

			this->getChildByID("right-side-menu")->setVisible(false);
			this->getChildByID("garage-menu")->setPositionX(this->getChildByID("right-side-menu")->getPositionX());
			
			auto otherMenu = this->getChildByID("other-menu");
			for(int i = 0; i < otherMenu->getChildrenCount(); i++) {
				auto node = static_cast<CCNode*>(otherMenu->getChildren()->objectAtIndex(i));
				if(node->getID() != "info-button" && node->getID() != "favorite-button")
					node->setVisible(false);
			}
			log::info("finished ui");
		
			//auto playBtn = getChildOfType<CCMenuItemSpriteExtra>(m_playBtnMenu, 0);
			CCLabelBMFont* titleLabel = (CCLabelBMFont*)this->getChildByID("title-label");

			int secretCoins = GameStatsManager::sharedState()->getStat("8");
			if(secretCoins < level->m_requiredCoins) {
				m_playSprite->setDisplayFrame(SFC->spriteFrameByName("GJLargeLock_001.png"));

				m_songWidget->updateSongObject(SongInfoObject::create(0));
				m_songWidget->m_songLabel->setString("?");

				m_orbsIcon->setDisplayFrame(SFC->spriteFrameByName("GJ_coinsIcon_001.png"));
				m_orbsLabel->setString(fmt::format("{}/{}", secretCoins, level->m_requiredCoins).c_str());

				titleLabel->setString("?");
			} else {
				log::info("clone button setup started");
				m_cloneBtn->setEnabled(true);
				m_cloneBtn->setVisible(true);
				getChildOfType<CCSprite>(m_cloneBtn, 0)->setDisplayFrame(SFC->spriteFrameByName("GJ_duplicateBtn_001.png"));
				m_cloneBtn->m_pfnSelector = menu_selector(LevelInfoLayerExt::confirmCloneMain);
				log::info("clone button setup finished");
			}
		}
		log::info("LevelInfoLayer::init finished");
		return true;
	}

	void onBack(CCObject* sender) {
		if(m_fields->m_isMain) {
			auto scene = CCScene::create();
			scene->addChild(LevelSelectLayer::create(m_level->m_levelID.value() - 1));
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
			return;
		}
		LevelInfoLayer::onBack(sender);
	}
	void onPlay(CCObject* sender) {
		int secretCoins = GameStatsManager::sharedState()->getStat("8");
		if(secretCoins < m_level->m_requiredCoins && m_fields->m_isMain) {
			const char* text = fmt::format("Collect {} more <cy>Secret Coins</c> to unlock this <cl>level</c>!", m_level->m_requiredCoins - secretCoins).c_str();
			FLAlertLayer::create("Locked", text, "OK")->show();
			return;
		}
		LevelInfoLayer::onPlay(sender);
	}
	void confirmCloneMain(CCObject*) {
		createQuickPopup("Clone Main Level", 
		"Create a <cl>copy</c> of this <cg>main</c> level?", "NO", "YES",
		[this](auto, bool btn2) {
			if(btn2) {
				LevelUtils::onCloneMain(this->m_level);
				return;
			}
		}, true, true);
	}
};