#include "audio_manager.h"
#include <iostream>
#include <fmod_errors.h>

AudioManager::AudioManager()
    : fmodSystem(nullptr), backgroundMusicSound(nullptr), flashlightSound(nullptr), backgroundMusicChannel(nullptr) {}

AudioManager::~AudioManager() {
    if (backgroundMusicChannel) {
        backgroundMusicChannel->stop();
    }
    if (flashlightSound) {
        flashlightSound->release();
    }
    if (backgroundMusicSound) {
        backgroundMusicSound->release();
    }
    if (fmodSystem) {
        fmodSystem->close();
        fmodSystem->release();
    }
}

bool AudioManager::init() {
    FMOD_RESULT result = FMOD::System_Create(&fmodSystem);
    if (checkFMODError(result, "FMOD System_Create")) {
        return false;
    }

    result = fmodSystem->init(512, FMOD_INIT_NORMAL, nullptr);
    if (checkFMODError(result, "FMOD System::init")) {
        return false;
    }
    return true;
}

void AudioManager::update() {
    if (fmodSystem) {
        fmodSystem->update();
    }
}

bool AudioManager::loadBackgroundMusic(const std::string& path) {
    if (!fmodSystem) {
        std::cerr << "FMOD System not initialized." << std::endl;
        return false;
    }

    FMOD_RESULT result = fmodSystem->createSound(
        path.c_str(),
        FMOD_LOOP_NORMAL | FMOD_2D,
        nullptr,
        &backgroundMusicSound
    );
    if (checkFMODError(result, "FMOD System::createSound (background music)")) {
        return false;
    }
    return true;
}

bool AudioManager::playBackgroundMusic() {
    if (!fmodSystem || !backgroundMusicSound) {
        std::cerr << "Background music not loaded or FMOD System not initialized." << std::endl;
        return false;
    }

    FMOD_RESULT result = fmodSystem->playSound(backgroundMusicSound, nullptr, false, &backgroundMusicChannel);
    if (checkFMODError(result, "FMOD System::playSound (background music)")) {
        return false;
    }
    return true;
}

void AudioManager::stopBackgroundMusic() {
    if (backgroundMusicChannel) {
        backgroundMusicChannel->stop();
    }
}

void AudioManager::setBackgroundMusicVolume(float volume) {
    if (backgroundMusicChannel) {
        backgroundMusicChannel->setVolume(volume);
    }
}

bool AudioManager::loadFlashlightSound(const std::string& path) {
    if (!fmodSystem) {
        std::cerr << "FMOD System not initialized." << std::endl;
        return false;
    }

    FMOD_RESULT result = fmodSystem->createSound(
        path.c_str(),
        FMOD_2D,
        nullptr,
        &flashlightSound
    );
    if (checkFMODError(result, "FMOD System::createSound (flashlight sound)")) {
        return false;
    }
    return true;
}

void AudioManager::playFlashlightSound() {
    if (fmodSystem && flashlightSound) {
        fmodSystem->playSound(flashlightSound, nullptr, false, nullptr);
    }
}

bool AudioManager::checkFMODError(FMOD_RESULT result, const std::string& message) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! " << message << ": " << FMOD_ErrorString(result) << std::endl;
        return true;
    }
    return false;
}