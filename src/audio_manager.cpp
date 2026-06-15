#include "audio_manager.h"
#include <iostream>
#include <fmod_errors.h>

extern float g_sfxVolume;
extern float g_musicVolume;

AudioManager::AudioManager()
    : fmodSystem(nullptr), backgroundMusicSound(nullptr), flashlightSound(nullptr), keySound(nullptr), doorLockedSound(nullptr), backgroundMusicChannel(nullptr) {}

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
    if (keySound) {
        keySound->release();
    }
    if (doorLockedSound) {
        doorLockedSound->release();
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
    if (backgroundMusicChannel) {
        backgroundMusicChannel->setVolume(g_musicVolume);
    }
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
        FMOD::Channel* channel = nullptr;
        fmodSystem->playSound(flashlightSound, nullptr, false, &channel);
        if (channel) channel->setVolume(g_sfxVolume);
    }
}

bool AudioManager::loadKeySound(const std::string& path) {
    if (!fmodSystem) return false;
    FMOD_RESULT result = fmodSystem->createSound(path.c_str(), FMOD_2D, nullptr, &keySound);
    if (checkFMODError(result, "FMOD System::createSound (key sound)")) {
        return false;
    }
    return true;
}

void AudioManager::playKeySound() {
    if (fmodSystem && keySound) {
        FMOD::Channel* channel = nullptr;
        fmodSystem->playSound(keySound, nullptr, false, &channel);
        if (channel) channel->setVolume(g_sfxVolume);
    }
}

bool AudioManager::loadDoorLockedSound(const std::string& path) {
    if (!fmodSystem) return false;
    FMOD_RESULT result = fmodSystem->createSound(path.c_str(), FMOD_2D, nullptr, &doorLockedSound);
    if (checkFMODError(result, "FMOD System::createSound (door sound)")) {
        return false;
    }
    return true;
}

void AudioManager::playDoorLockedSound() {
    if (fmodSystem && doorLockedSound) {
        FMOD::Channel* channel = nullptr;
        fmodSystem->playSound(doorLockedSound, nullptr, false, &channel);
        if (channel) channel->setVolume(g_sfxVolume);
    }
}

bool AudioManager::checkFMODError(FMOD_RESULT result, const std::string& message) {
    if (result != FMOD_OK) {
        std::cerr << "FMOD error! " << message << ": " << FMOD_ErrorString(result) << std::endl;
        return true;
    }
    return false;
}