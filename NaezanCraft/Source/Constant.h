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
constexpr float INVEN_SLOT_HALF_WIDTH = 182.f;
constexpr float INVEN_SLOT_HEIGHT = 44.f;
constexpr float INVEN_SLOT_POINT_HALF_WIDTH = 48.f;
constexpr float INVEN_SLOT_POINT_HEIGHT = 46.f;
constexpr float INVEN_SLOT_POINT_INER_WIDTH = 40.f;

constexpr int INVEN_SPRITE_WIDTH = 32;
constexpr int INVEN_SPRITE_HEIGHT = 32;

constexpr float SKY_SIZE = 500;

constexpr int WATER_HEIGHT = 60;

constexpr float PLAYER_SPEED = 0.1f;
constexpr float CLOUD_SPEED = 0.1f;

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

constexpr float GRAVITY = -9.81f;