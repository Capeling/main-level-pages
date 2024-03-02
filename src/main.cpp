#include <Geode/Geode.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include "levelUtils.hpp"

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

class $modify(SecretLayer2) {
	void onSecretLevel(CCObject* sender) {
		int diamonds = GameStatsManager::sharedState()->getStat("13");
		if(diamonds < 200) return SecretLayer2::onSecretLevel(sender);
		auto LLM = LocalLevelManager::sharedState();

		auto level = GameLevelManager::sharedState()->getMainLevel(3001, false);
		level->m_levelString = LLM->getMainLevelString(level->m_levelID.value());
		level->m_creatorName = "RobTop";
		level->m_coinsVerified = true;
		level->m_accountID = 71;
		level->m_levelLength = 1;
		level->m_demonDifficulty = 3;
		level->m_featured = 0;

		auto scene = CCScene::create();
		scene->addChild(LevelInfoLayer::create(level, false));

		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
	}
};

class $modify(LevelAreaInnerLayer) {
	void onDoor(CCObject* sender) {
		auto LLM = LocalLevelManager::sharedState();

		//return LevelAreaInnerLayer::onDoor(sender);
		auto level = GameLevelManager::sharedState()->getMainLevel(sender->getTag(), true);
		level->m_levelString = LLM->getMainLevelString(level->m_levelID.value());
		level->m_creatorName = "RobTop";
		level->m_coinsVerified = true;
		level->m_accountID = 71;
		level->m_levelLength = 5;
		level->m_demonDifficulty = 3;
		level->m_featured = 1;

		auto scene = CCScene::create();
		scene->addChild(LevelInfoLayer::create(level, false));

		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
	}
	bool init(bool idk) {
		log::info("level area inner init bool: {}", idk);
		return LevelAreaInnerLayer::init(idk);
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
	bool m_isSecret = false;

	bool init(GJGameLevel* level, bool isGauntlet) {

		if(!LevelInfoLayer::init(level, isGauntlet))
			return false;
		
		//m_fields->m_isMain = level->m_levelType == GJLevelType::Local;
		m_fields->m_isSecret = m_fields->m_isMain && level->m_levelID.value() == 3001;
		if(!m_fields->m_isMain) {
			m_fields->m_isMain = level->m_levelID.value() < 23 || level->m_levelID.value() > 5000 && level->m_levelID.value() < 5005 || level->m_levelID.value() == 3001;
		}
		if(m_fields->m_isMain) level->m_levelType = GJLevelType::Local; //fix for that one person who had a mod fuck up their game
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
				CCMenuItemSpriteExtra* cloneBtn = (CCMenuItemSpriteExtra*)this->getChildByID("left-side-menu")->getChildByID("copy-button");
				cloneBtn->setEnabled(true);
				cloneBtn->setVisible(true);
				getChildOfType<CCSprite>(cloneBtn, 0)->setDisplayFrame(SFC->spriteFrameByName("GJ_duplicateBtn_001.png"));
				cloneBtn->m_pfnSelector = menu_selector(LevelInfoLayerExt::confirmCloneMain);
			}
		}
		return true;
	}

	void onBack(CCObject* sender) {
		if(m_fields->m_isMain && !this->m_level->isPlatformer() && !m_fields->m_isSecret) {
			auto scene = CCScene::create();
			scene->addChild(LevelSelectLayer::create(m_level->m_levelID.value() - 1));
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
			return;
		} else if(m_fields->m_isMain && this->m_level->isPlatformer()) {
			auto scene = CCScene::create();
			scene->addChild(LevelAreaInnerLayer::create(true));
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
		} else if(m_fields->m_isMain && m_fields->m_isSecret) {
			auto scene = CCScene::create();
			scene->addChild(SecretLayer2::create());
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
		}
		LevelInfoLayer::onBack(sender);
	}
	void confirmClone(CCObject* sender) {
		if(this->m_fields->m_isMain)
			return LevelInfoLayerExt::confirmCloneMain(sender);
		return LevelInfoLayer::confirmClone(sender);
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
				LevelUtils::onCloneMain(this->m_level, this->m_fields->m_isSecret);
				return;
			}
		}, true, true);
	}
};