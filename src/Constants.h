#ifndef CONSTANTS_H
#define CONSTANTS_H

// Масштаб (пиксели в метры)
static constexpr float SCALE = 30.0f; 

// Типы объектов для пользовательских данных
static constexpr uintptr_t DOOR_USER_DATA = 1; 
static constexpr uintptr_t PLATFORM_USER_DATA = 2; 
static constexpr uintptr_t FINISH_USER_DATA = 3; 
static constexpr uintptr_t STAIRCASE_USER_DATA = 5;  
static constexpr uintptr_t DEATH_USER_DATA = 6;  
static constexpr uintptr_t WALL_USER_DATA = 7; 
static constexpr uintptr_t MOVE_PLATFORM_USER_DATA = 8; 
static constexpr uintptr_t TRAP_USER_DATA = 9;
static constexpr uintptr_t LAVA_USER_DATA = 10;
static constexpr uintptr_t CLOUDS_USER_DATA = 11;

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
static constexpr float GRAVITY_Y = 14.0f;

// Параметры игрока
static constexpr float PLAYER_RUN_FORCE = 5.0f; // Сила бега
static constexpr float PLAYER_JUMP_IMPULSE = -6.0f; // Импульс прыжка
static constexpr float PLAYER_MAX_SPEED = 5.0f; // Максимальная скорость
static constexpr float PLAYER_SLOWDOWN_FACTOR = 0.5f; // Коэффициент замедления

// Размер тайла
static constexpr int TILE_SIZE = 16;

// Размеры уровня (будут загружаться из TMX-файла)
static int MAP_WIDTH_IN_TILES = 0; // Ширина карты в тайлах
static int MAP_HEIGHT_IN_TILES = 0; // Высота карты в тайлах
static int TILE_SIZE_IN_PIXELS = 16; // Размер одного тайла в пикселях (по умолчанию 16)

extern bool DEBUG_DRAW_ENABLED;





#endif