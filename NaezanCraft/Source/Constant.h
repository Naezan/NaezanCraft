#pragma once

#define CHUNK_X 16
#define CHUNK_Y 127//TO DO 256
#define CHUNK_Z 16
constexpr int LOOK_CHUNK_SIZE = 20;
constexpr int CHUNK_SIZE = CHUNK_X * CHUNK_Y * CHUNK_Z;
constexpr int ATLAS_WIDTH = 512;
constexpr int ATLAS_HEIGHT = 512;
constexpr int SPRITE_WIDTH = 16;
constexpr int SPRITE_HEIGHT = 16;
constexpr float CROSSHAIR_HALFSIZE = 16.f;

constexpr float SKY_SIZE = 500;

constexpr int WATER_HEIGHT = 60;

constexpr float PLAYER_SPEED = 0.2f;
constexpr float CLOUD_SPEED = 0.1f;

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;