#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include <fmod.hpp>
#include <string>
#include <vector>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();
    void update();
    bool loadBackgroundMusic(const std::string& path);
    bool playBackgroundMusic();
    void stopBackgroundMusic();
    void setBackgroundMusicVolume(float volume);
    bool loadFlashlightSound(const std::string& path);
    void playFlashlightSound();
    bool loadDoorLockedSound(const std::string& path);
    void playDoorLockedSound();

private:
    FMOD::System* fmodSystem;
    FMOD::Sound* backgroundMusicSound;
    FMOD::Sound* flashlightSound;
    FMOD::Sound* doorLockedSound;
    FMOD::Channel* backgroundMusicChannel;
    bool checkFMODError(FMOD_RESULT result, const std::string& message);
};

#endif // __AUDIO_MANAGER_H__