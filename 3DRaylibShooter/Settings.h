#pragma once
#if defined(_WIN32)
#define NOGDI
#define NOUSER
#endif
#include "raylib.h"
#include "raymath.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <functional>

#include "Timer.h"

constexpr int WINDOW_WIDTH = 960;
constexpr int WINDOW_HEIGHT = 660;
constexpr Color BG_COLOR = BLACK;
constexpr float PLAYER_SPEED = 7.f;
constexpr float LASER_SPEED = 9.f;
constexpr int METEOR_SPEED_MIN = 3;
constexpr int METEOR_SPEED_MAX = 4;

constexpr float METEOR_TIMER_DURATION = 0.4f;

constexpr int FONT_SIZE = 60;
constexpr int FONT_PADDING = 60;

