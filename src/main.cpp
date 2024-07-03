#include <Geode/Geode.hpp>

#include <cstdlib>
#include <chrono>
#include <math.h>

using namespace geode::prelude;
using namespace std::chrono;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

bool enable = true;
bool audio = true;
bool onclick = false;

float currSpeed = 2;

double lower = 0.5;
double upper = 1.5;

double rate = 1;

$execute {
	listenForSettingChanges("lower-bound", +[](double value) {
		lower = value;
	});
	listenForSettingChanges("upper-bound", +[](double value) {
		upper = value;
	});
	listenForSettingChanges("onclick", +[](bool value) {
		onclick = value;
	});
	listenForSettingChanges("enable", +[](bool value) {
		enable = value;
	});
	listenForSettingChanges("audio", +[](bool value) {
		audio = value;
	});
	listenForSettingChanges("rate", +[](double value) {
		rate = value;
	});
	enable = Mod::get()->getSettingValue<bool>("enable");
	audio = Mod::get()->getSettingValue<bool>("audio");
	onclick = Mod::get()->getSettingValue<bool>("onclick");
	lower = Mod::get()->getSettingValue<double>("lower-bound");
	upper = Mod::get()->getSettingValue<double>("upper-bound");
	rate = Mod::get()->getSettingValue<double>("rate");
}

class $modify(CCScheduler) {
	void update(float x) {
		auto time = high_resolution_clock::now();
		if (duration_cast<milliseconds>(time.time_since_epoch()).count() % (int)floor(rate * 1000.f) == 0 && !onclick) {
			currSpeed = ((float)rand() / (float)RAND_MAX) * (upper - lower) + lower;
			auto fmod = FMODAudioEngine::sharedEngine();
		}

		if (enable) {
			x *= currSpeed;
		}
		
		CCScheduler::update(x);
	}
};

class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool a, bool b) {
		if (!enable) return CCKeyboardDispatcher::dispatchKeyboardMSG(key, a, b);
		if (onclick) {
			currSpeed = ((float)rand() / (float)RAND_MAX) * (upper - lower) + lower;
		}

		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, a, b);
	}
};

class $modify(FMODAudioEngine) {
	void update(float delta) {
		FMODAudioEngine::update(delta);

		float speed = currSpeed;

		if (!enable || !audio) speed = 1;

		auto fmod = FMODAudioEngine::sharedEngine();
		FMOD_RESULT result;
		FMOD::ChannelGroup* master_group = nullptr;
		result = fmod->m_system->getMasterChannelGroup(&master_group);
		if (result == FMOD_OK) {
			master_group->setPitch(speed);
		}
		fmod->m_globalChannel->setPitch(speed);
	}
};