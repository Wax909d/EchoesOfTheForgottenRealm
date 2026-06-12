#pragma once

constexpr int TILE_SIZE   = 24;
constexpr int MAP_WIDTH   = 20;
constexpr int MAP_HEIGHT  = 15;

constexpr unsigned int WINDOW_WIDTH  = TILE_SIZE * MAP_WIDTH;
constexpr unsigned int WINDOW_HEIGHT = TILE_SIZE * MAP_HEIGHT;

constexpr int ENTITY_SIZE = 16;
constexpr int ENTITY_INSET = (TILE_SIZE - ENTITY_SIZE) / 2;

constexpr float CAMERA_ZOOM = 2.0f;

constexpr unsigned int WINDOW_SCALE = 2;

constexpr float PLAYER_SPRITE_SCALE = 1.15f;
