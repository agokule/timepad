// taken from https://github.com/agokule/TerminalVideoPlayer/blob/master/TerminalVideoPlayer/AudioPlayer.h
#pragma once

#include "miniaudio.h"
#include <stdexcept>

class AudioPlayer {
public:
    AudioPlayer(const char *filePath) : length_in_frames {0}, sample_rate {0} {
        ma_result result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS) {
            throw std::runtime_error("Failed to initialize audio engine.");
        }

        result = ma_sound_init_from_file(&engine, filePath, 0, NULL, NULL, &sound);
        if (result != MA_SUCCESS) {
            ma_engine_uninit(&engine);
            throw std::runtime_error("Failed to load sound file.");
        }

        result = ma_data_source_get_length_in_pcm_frames(&sound, &length_in_frames);
        if (result != MA_SUCCESS) {
            ma_sound_uninit(&sound);
            ma_engine_uninit(&engine);
            throw std::runtime_error("Failed to get sound length.");
        }

        ma_data_source* pDataSource = ma_sound_get_data_source(&sound);

        result = ma_data_source_get_data_format(pDataSource, NULL, NULL, &sample_rate, NULL, 0);
        if (result != MA_SUCCESS) {
            ma_sound_uninit(&sound);
            ma_engine_uninit(&engine);
            throw std::runtime_error("Failed to get sample rate.");
        }

        float length_in_seconds = static_cast<float>(length_in_frames) / sample_rate;
    }

    ~AudioPlayer() {
        ma_sound_uninit(&sound);
        ma_engine_uninit(&engine);
    }

    void play() {
        ma_sound_start(&sound);
        is_playing = true;
    }

    void pause() {
        ma_sound_stop(&sound);
        is_playing = false;
    }

    bool is_playing_or_not() {
        return is_playing;
    }

    inline ma_uint32 get_sample_rate() const {
        return sample_rate;
    }

    void seek_to(double seconds) {
        ma_uint64 newFrame = seconds * static_cast<ma_uint64>(sample_rate);
        ma_sound_seek_to_pcm_frame(&sound, newFrame);
    }

private:
    ma_engine engine;
    ma_sound sound;

    ma_uint64 length_in_frames;
    ma_uint32 sample_rate;
    bool is_playing = false;
};
