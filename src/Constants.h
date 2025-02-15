#ifndef CONSTANTS_H
#define CONSTANTS_H

// Масштаб (пиксели в метры)
static constexpr float SCALE = 32.0f; // 1 метр = 30 пикселей

// Типы объектов для пользовательских данных
static constexpr uintptr_t PLAYER_USER_DATA = 1; // Идентификатор игрока
static constexpr uintptr_t PLATFORM_USER_DATA = 2; // Идентификатор платформы
static constexpr uintptr_t Finish_USER_DATA = 3; // Идентификатор гриба
static constexpr uintptr_t COIN_USER_DATA = 5;   // ID монеты
static constexpr uintptr_t DEATH_USER_DATA = 6;  // ID триггера смерти
static constexpr uintptr_t WALL_USER_DATA = 7;

// Размеры окна
static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;

// Границы камеры (будут вычисляться динамически)
static float CAMERA_MIN_X = 0.0f;
static float CAMERA_MAX_X = 0.0f;
static float CAMERA_MIN_Y = 0.0f;
static float CAMERA_MAX_Y = 0.0f;

// Гравитация
static constexpr float GRAVITY_X = 0.0f;
static constexpr float GRAVITY_Y = 15.0f;

// Параметры игрока
static constexpr float PLAYER_RUN_FORCE = 5.0f; // Сила бега
static constexpr float PLAYER_JUMP_IMPULSE = -7.0f; // Импульс прыжка
static constexpr float PLAYER_MAX_SPEED = 5.0f; // Максимальная скорость
static constexpr float PLAYER_SLOWDOWN_FACTOR = 0.5f; // Коэффициент замедления

// Размер тайла
static constexpr int TILE_SIZE = 16;

// Размеры уровня (будут загружаться из TMX-файла)
static int MAP_WIDTH_IN_TILES = 0; // Ширина карты в тайлах
static int MAP_HEIGHT_IN_TILES = 0; // Высота карты в тайлах
static int TILE_SIZE_IN_PIXELS = 16; // Размер одного тайла в пикселях (по умолчанию 16)

// Флаг для отладочной визуализации
static bool DEBUG_DRAW_ENABLED = true;

#endif