#include "audio_manager/audio_manager.hpp"

// Sounds
Mix_Chunk* gatling_gun_sound = nullptr;
Mix_Chunk* sniper_sound = nullptr;
Mix_Chunk* shotgun_sound = nullptr;
Mix_Chunk* rocket_launcher_sound = nullptr;
Mix_Chunk* flamethrower_sound = nullptr;
Mix_Chunk* energy_halo_sound = nullptr;
Mix_Chunk* reload_start_sound = nullptr;
Mix_Chunk* reload_end_sound = nullptr;
Mix_Chunk* no_ammo_sound = nullptr;
Mix_Chunk* explosion_sound = nullptr;
Mix_Chunk* cycle_weapon_sound = nullptr;
Mix_Chunk* player_hit_sound = nullptr;
Mix_Chunk* enemy_hit_sound = nullptr;
Mix_Chunk* game_start_sound = nullptr;
Mix_Chunk* game_over_sound = nullptr;

// Music
Mix_Music* start_menu_music = nullptr;
Mix_Music* game_music = nullptr;
Mix_Music* game_win_music = nullptr;
Mix_Music* boss_music = nullptr;

bool init_audio() {
    // Loading music and sounds with SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL Audio");
        return nullptr;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        fprintf(stderr, "Failed to open audio device");
        return nullptr;
    }

    // Load sounds
    gatling_gun_sound = Mix_LoadWAV(audio_path("gatling_gun.wav").c_str());
    sniper_sound = Mix_LoadWAV(audio_path("sniper.wav").c_str());
    shotgun_sound = Mix_LoadWAV(audio_path("shotgun.wav").c_str());
    rocket_launcher_sound = Mix_LoadWAV(audio_path("rocket_launcher_sound.wav").c_str());
    flamethrower_sound = Mix_LoadWAV(audio_path("flamethrower_sound.wav").c_str());
    energy_halo_sound = Mix_LoadWAV(audio_path("energy_halo_sound.wav").c_str());
    reload_start_sound = Mix_LoadWAV(audio_path("reload_start_sound.wav").c_str());
    reload_end_sound = Mix_LoadWAV(audio_path("reload_end_sound.wav").c_str());
    no_ammo_sound = Mix_LoadWAV(audio_path("no_ammo_sound.wav").c_str());
    explosion_sound = Mix_LoadWAV(audio_path("explosion.wav").c_str());
    cycle_weapon_sound = Mix_LoadWAV(audio_path("cycle_weapon_sound.wav").c_str());
    player_hit_sound = Mix_LoadWAV(audio_path("player_hit_sound.wav").c_str());
    enemy_hit_sound = Mix_LoadWAV(audio_path("enemy_hit_sound.wav").c_str());
    game_start_sound = Mix_LoadWAV(audio_path("game_start_sound.wav").c_str());
    game_over_sound = Mix_LoadWAV(audio_path("game_over_sound.wav").c_str());

    // Load music
    start_menu_music = Mix_LoadMUS(audio_path("start_menu_music.wav").c_str());
    game_music = Mix_LoadMUS(audio_path("game_music.wav").c_str());
    game_win_music = Mix_LoadMUS(audio_path("game_win_music.wav").c_str());
    boss_music = Mix_LoadMUS(audio_path("boss_music.wav").c_str());

    if (
        gatling_gun_sound == nullptr ||
        sniper_sound == nullptr ||
        shotgun_sound == nullptr ||
        rocket_launcher_sound == nullptr ||
        flamethrower_sound == nullptr ||
        energy_halo_sound == nullptr ||
        reload_start_sound == nullptr ||
        reload_end_sound == nullptr ||
        no_ammo_sound == nullptr ||
        explosion_sound == nullptr ||
        cycle_weapon_sound == nullptr ||
        player_hit_sound == nullptr ||
        enemy_hit_sound == nullptr ||
        game_start_sound == nullptr ||
        game_over_sound == nullptr ||
        start_menu_music == nullptr ||
        game_music == nullptr ||
        game_win_music == nullptr ||
        boss_music == nullptr
        ) {
        fprintf(stderr, "Failed to load sounds make sure the data directory is present");
        return nullptr;
    }

    return true;
}

void play_sound(Mix_Chunk* sound) {
    if (sound != nullptr) {
        Mix_PlayChannel(-1, sound, 0);
    }
}

void play_music(Mix_Music* music) {
    if (music != nullptr) {
        Mix_PlayMusic(music, -1);
    }
}

void stop_music() {
    Mix_HaltMusic();
}

void close_audio() {
    // Free sounds
    if (gatling_gun_sound != nullptr)
        Mix_FreeChunk(gatling_gun_sound);
    if (sniper_sound != nullptr)
        Mix_FreeChunk(sniper_sound);
    if (shotgun_sound != nullptr)
        Mix_FreeChunk(shotgun_sound);
    if (rocket_launcher_sound != nullptr)
        Mix_FreeChunk(rocket_launcher_sound);
    if (flamethrower_sound != nullptr)
        Mix_FreeChunk(flamethrower_sound);
    if (energy_halo_sound != nullptr)
        Mix_FreeChunk(energy_halo_sound);
    if (reload_start_sound != nullptr)
        Mix_FreeChunk(reload_start_sound);
    if (reload_end_sound != nullptr)
        Mix_FreeChunk(reload_end_sound);
    if (no_ammo_sound != nullptr)
        Mix_FreeChunk(no_ammo_sound);
    if (explosion_sound != nullptr)
        Mix_FreeChunk(explosion_sound);
    if (cycle_weapon_sound != nullptr)
        Mix_FreeChunk(cycle_weapon_sound);
    if (player_hit_sound != nullptr)
        Mix_FreeChunk(player_hit_sound);
    if (enemy_hit_sound != nullptr)
        Mix_FreeChunk(enemy_hit_sound);
    if (game_start_sound != nullptr)
        Mix_FreeChunk(game_start_sound);
    if (game_over_sound != nullptr)
        Mix_FreeChunk(game_over_sound);
   
    // Free music
    if (start_menu_music != nullptr)
        Mix_FreeMusic(start_menu_music);
    if (game_music != nullptr)
        Mix_FreeMusic(game_music);
    if (game_win_music != nullptr)
        Mix_FreeMusic(game_win_music);
    if (boss_music != nullptr)
        Mix_FreeMusic(boss_music);
}
