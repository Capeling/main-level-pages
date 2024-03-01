#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace LevelUtils {
	void cloneMain(bool userCoins, GJGameLevel* copyLevel) {

		auto GLM = GameLevelManager::sharedState();
		GJGameLevel* level = GLM->createNewLevel();
		level->copyLevelInfo(copyLevel);
		if(userCoins) { 
			gd::string decompString = ZipUtils::decompressString(level->m_levelString, true, 0);
			std::string stdDecompString = decompString;
			for(int i = 0; i < 3; i++) {
				size_t where = stdDecompString.find(";1,142,"); //shh it works bestie dw about it
				stdDecompString.replace(where, 7, ";1,1329,");
			}
			level->m_levelString = stdDecompString;
			stdDecompString.clear();
			decompString.clear();
		}

		auto scene = CCScene::create();
		scene->addChild(EditLevelLayer::create(level));
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, scene));
	}
    	void onCloneMain(GJGameLevel* level, bool dontAskForConvert) {
		if(dontAskForConvert) return cloneMain(false, level);
		auto popup = createQuickPopup("Convert Secret Coins", 
		"Convert <cy>Secret Coins</c> to <cl>User Coins</c>?", "NO", "YES",
		[level](auto, bool btn2) {
			if(!btn2) {
			    cloneMain(false, level);
				return;
			}
			if(btn2) {
				cloneMain(true, level);
				return;
			}
		}, false, true);
		popup->show();
	}
}