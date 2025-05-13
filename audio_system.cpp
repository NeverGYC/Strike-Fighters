#include "audio_system.h"
#include <stdexcept>
#include <iostream>

namespace game {

    AudioSystem::AudioSystem() : device_(nullptr), context_(nullptr), bgm_source_(0) {
    }

    AudioSystem::~AudioSystem() {
        // Ensure we stop all sounds before cleanup
        StopAll();

        // Clean up sources
        for (ALuint source : sources_) {
            alDeleteSources(1, &source);
        }
        sources_.clear();

        if (bgm_source_) {
            alSourceStop(bgm_source_);
            alDeleteSources(1, &bgm_source_);
            bgm_source_ = 0;
        }

        // Clean up OpenAL context and device
        if (context_) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context_);
            context_ = nullptr;
        }

        if (device_) {
            alcCloseDevice(device_);
            device_ = nullptr;
        }
    }

    void AudioSystem::Init() {
        // Initialize ALUT
        if (!alutInit(0, nullptr)) {
            throw std::runtime_error("Failed to initialize ALUT");
        }

        // Open default device
        device_ = alcOpenDevice(nullptr);
        if (!device_) {
            throw std::runtime_error("Failed to open default audio device");
        }

        // Create context
        context_ = alcCreateContext(device_, nullptr);
        if (!context_) {
            alcCloseDevice(device_);
            throw std::runtime_error("Failed to create audio context");
        }

        // Make context current
        if (!alcMakeContextCurrent(context_)) {
            alcDestroyContext(context_);
            alcCloseDevice(device_);
            throw std::runtime_error("Failed to make audio context current");
        }

        // Create source for background music
        alGenSources(1, &bgm_source_);
        if (alGetError() != AL_NO_ERROR) {
            throw std::runtime_error("Failed to create background music source");
        }

        // Set up background music source properties
        alSource3f(bgm_source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(bgm_source_, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSource3f(bgm_source_, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
        alSourcef(bgm_source_, AL_ROLLOFF_FACTOR, 0.0f);
        alSourcei(bgm_source_, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(bgm_source_, AL_GAIN, 0.5f); // Set volume to 50%
    }

    ALuint AudioSystem::LoadSound(const std::string& filename) {
        // Load audio file using ALUT
        ALuint buffer = alutCreateBufferFromFile(filename.c_str());
        if (buffer == AL_NONE) {
            throw std::runtime_error("Failed to load audio file: " + filename);
        }
        return buffer;
    }

    void AudioSystem::PlaySound(ALuint buffer) {
        // Create a new source for this sound
        ALuint source;
        alGenSources(1, &source);
        if (alGetError() != AL_NO_ERROR) {
            return;
        }

        // Set up source properties
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
        alSourcef(source, AL_ROLLOFF_FACTOR, 0.0f);
        alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

        // Attach buffer and play
        alSourcei(source, AL_BUFFER, buffer);
        alSourcei(source, AL_LOOPING, AL_FALSE);
        alSourcePlay(source);

        // Store source for cleanup
        sources_.push_back(source);
    }

    void AudioSystem::PlayBackgroundMusic(ALuint buffer) {
        // Attach buffer to background music source
        alSourcei(bgm_source_, AL_BUFFER, buffer);
        alSourcei(bgm_source_, AL_LOOPING, AL_TRUE);
        alSourcePlay(bgm_source_);
    }

    void AudioSystem::Update() {
        // Remove finished sources
        auto it = sources_.begin();
        while (it != sources_.end()) {
            ALint state;
            alGetSourcei(*it, AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED) {
                alDeleteSources(1, &(*it));
                it = sources_.erase(it);
            }
            else {
                ++it;
            }
        }

        // Check if background music is still playing
        ALint state;
        alGetSourcei(bgm_source_, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            // Restart background music if it stopped
            alSourcePlay(bgm_source_);
        }
    }

    void AudioSystem::StopAll() {
        // Stop background music
        alSourceStop(bgm_source_);

        // Stop all sound effects
        for (ALuint source : sources_) {
            alSourceStop(source);
        }
    }



} // namespace game