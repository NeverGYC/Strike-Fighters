#ifndef AUDIO_SYSTEM_H_
#define AUDIO_SYSTEM_H_

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>
#include <vector>

namespace game {

    class AudioSystem {
    public:
        AudioSystem();
        ~AudioSystem();

        // Initialize the audio system
        void Init();

        // Load a sound file and return its buffer ID
        ALuint LoadSound(const std::string& filename);

        // Play a sound once
        void PlaySound(ALuint buffer);

        // Play background music (looping)
        void PlayBackgroundMusic(ALuint buffer);

		// Update the audio system (remove finished sources)
		void Update();

        // Stop all sounds
        void StopAll();

    private:
        ALCdevice* device_;
        ALCcontext* context_;
        std::vector<ALuint> sources_;
        ALuint bgm_source_;  // Dedicated source for background music


    };

} // namespace game

#endif // AUDIO_SYSTEM_H_