#ifndef CONSTANTS_H
#define CONSTANTS_H

// Масштаб (пиксели в метры)
static constexpr float SCALE = 30.0f; // 1 метр = 30 пикселей

// Масштаб персонажа
static constexpr float ScaleCharacter = 0.5f; // Размер персонажа: 0.5 метра (вместо 1 метра)

// Типы объектов для пользовательских данных
static constexpr uintptr_t PLAYER_USER_DATA = 1; // Идентификатор игрока
static constexpr uintptr_t PLATFORM_USER_DATA = 2; // Идентификатор платформы
static constexpr uintptr_t MASHROOM_USER_DATA = 3; // Идентификатор гриба

// Размеры окна
static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;

// Гравитация
static constexpr float GRAVITY_X = 0.0f;
static constexpr float GRAVITY_Y = 9.8f;

// Параметры игрока
static constexpr float PLAYER_RUN_FORCE = 5.0f; // Сила бега
static constexpr float PLAYER_JUMP_IMPULSE = -7.0f; // Импульс прыжка
static constexpr float PLAYER_MAX_SPEED = 5.0f; // Максимальная скорость
static constexpr float PLAYER_SLOWDOWN_FACTOR = 0.5f; // Коэффициент замедления

// Размер тайла
static constexpr int TILE_SIZE = 16;

// Флаг для отображения отладочной визуализации
static bool DEBUG_DRAW_ENABLED = true;

#endif