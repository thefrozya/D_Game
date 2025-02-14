#include <SFML/Graphics.hpp>
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include "Menu.h"
#include "Coin.h" // Подключаем класс Coin
#include <Box2D/Box2D.h>
#include <iostream>
#include "DebugUtils.h"

#include "Coin.h"
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include "Menu.h"
#include "DebugUtils.h"
#include <random>
#include <cmath>

// Функция для генерации случайной позиции монеты
sf::Vector2f generateRandomPosition(const std::vector<std::vector<int>>& levelData,
                                    const std::vector<Coin>& coins,
                                    int tileSizeInPixels, float scale, float minDistance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, levelData[0].size() - 1);
    std::uniform_int_distribution<> distY(0, levelData.size() - 1);

    while (true) {
        int x = distX(gen);
        int y = distY(gen);

        // Проверяем, что клетка пустая
        if (levelData[y][x] == 0) { // 0 означает пустую клетку
            sf::Vector2f position(x * tileSizeInPixels / scale, y * tileSizeInPixels / scale);

            // Проверяем расстояние до других монет
            bool isValid = true;
            for (const auto& coin : coins) {
                sf::FloatRect bounds = coin.getBoundingBox();
                sf::Vector2f coinCenter(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

                float distance = std::sqrt(std::pow(position.x - coinCenter.x, 2) + std::pow(position.y - coinCenter.y, 2));
                if (distance < minDistance) {
                    isValid = false; // Монета слишком близко к другой монете
                    break;
                }
            }

            if (isValid) {
                return position; // Возвращаем допустимую позицию
            }
        }
    }
}

// Глобальная текстура для персонажа
sf::Texture playerTexture;

// Глобальная текстура для монет
sf::Texture coinTexture;

// Функция для сброса уровня
void resetLevel(sf::RenderWindow& window, b2World& world, sf::Texture& tilesetTexture,
                std::vector<std::vector<int>>& levelData, int& firstgid, Player& player,
                sf::View& viewPlayer, const std::string& levelPath, float scale, ContactListener& contactListener, sf::Texture& playerTexture, std::vector<Coin>& coins) {
    // Удаляем все тела из мира Box2D
    for (b2Body* body = world.GetBodyList(); body; ) {
        b2Body* nextBody = body->GetNext();
        world.DestroyBody(body);
        body = nextBody;
    }

    // Сбрасываем данные уровня
    levelData.clear();
    firstgid = 0;

    // Загружаем уровень заново
    sf::Vector2f spawnPoint(0.5f, 20.0f);
    if (!LevelLoader::loadLevel(levelPath, tilesetTexture, levelData, firstgid, world, spawnPoint, scale)) {
        std::cerr << "Failed to reload level from path: " << levelPath << std::endl;
        return;
    }

    // Пересоздаем игрока, используя глобальную текстуру
    player = Player(world, spawnPoint.x, spawnPoint.y, &contactListener, playerTexture);

    // Очищаем старые монеты
    coins.clear();

    // Создаем новые монеты в случайных позициях
    int coinCount = 10; // Количество монет
    for (int i = 0; i < coinCount; ++i) {
        sf::Vector2f position = generateRandomPosition(levelData, coins, TILE_SIZE, scale, 2.0f);
        coins.emplace_back(world, position.x, position.y, coinTexture, 6, 0.1f);
    }
    std::cout << "Number of coins: " << coins.size() << std::endl;

    // Сбрасываем камеру
    sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
    viewPlayer.setCenter(startPosition);
    window.setView(viewPlayer);
}

int main() {
    // Создание окна
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Platformer");
    window.setFramerateLimit(60);

    // Загрузка текстуры тайлсета
    sf::Texture tilesetTexture;
    if (!tilesetTexture.loadFromFile("assets/textures/tileset.png")) {
        std::cerr << "Failed to load tileset texture from path: assets/textures/tileset.png" << std::endl;
        return -1;
    }

    // Загрузка текстуры игрока один раз
    if (!playerTexture.loadFromFile("assets/textures/player_spritesheet.png")) {
        std::cerr << "Failed to load player texture!" << std::endl;
        return -1;
    }

    // Загрузка текстуры монеты
    if (!coinTexture.loadFromFile("assets/textures/coin_spritesheet.png")) {
        std::cerr << "Failed to load coin spritesheet!" << std::endl;
        return -1;
    }


    // Создание мира Box2D
    b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
    b2World world(gravity);

    // Создание меню
    Menu menu(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Переменные для игры
    sf::Vector2f spawnPoint(0.5f, 20.0f);
    std::vector<std::vector<int>> levelData;
    int firstgid = 0;

    // Создание обработчика контактов
    ContactListener contactListener(nullptr);

    // Создание игрока
    Player player(world, spawnPoint.x, spawnPoint.y, &contactListener, playerTexture);

    // Обновляем указатель на игрока в ContactListener
    contactListener.setPlayer(&player);

    // Устанавливаем обработчик контактов в мир Box2D
    world.SetContactListener(&contactListener);

    // Приближенная камера (следует за игроком)
    sf::View viewPlayer(sf::FloatRect(50, 50, WINDOW_WIDTH, WINDOW_HEIGHT));
    const float cameraSpeed = 0.1f;

    // Глобальная камера (показывает всю карту)
    sf::View viewGlobal(sf::FloatRect(0, 0, 50 * TILE_SIZE, 50 * TILE_SIZE));
    viewGlobal.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));
    bool isGlobalView = false;

    // Флаг для отображения меню
    bool isMenuActive = true;

    // Счёт игрока
    int score = 0;

    // Список монет
    std::vector<Coin> coins;

    // Шрифт для текста
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    // Текст для отображения счёта
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    // Основной цикл игры
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (isMenuActive) {
                // Обработка событий меню
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        menu.moveUp();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        menu.moveDown();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        int selectedItem = menu.getPressedItem();
                        if (selectedItem == 0) { // Start Game
                            std::cout << "Starting game..." << std::endl;
                            isMenuActive = false;
                            // Загрузка уровня
                            if (!LevelLoader::loadLevel("assets/maps/level1.tmx", tilesetTexture, levelData, firstgid, world, spawnPoint, SCALE)) {
                                std::cerr << "Failed to load level!" << std::endl;
                                return -1;
                            }
                            // Установка начальной позиции камеры
                            sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
                            viewPlayer.setCenter(startPosition);
                            window.setView(viewPlayer);
                        } else if (selectedItem == 1) { // Options
                            std::cout << "Opening options..." << std::endl;
                            // Логика для настроек
                        } else if (selectedItem == 2) { // Exit
                            std::cout << "Exiting game..." << std::endl;
                            window.close();
                        }
                    }
                }
            } else {
                // Обработка событий игры
                if (event.type == sf::Event::Resized) {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                    viewPlayer.setSize(event.size.width, event.size.height);
                    window.setView(viewPlayer);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
                    isGlobalView = !isGlobalView;
                    window.setView(isGlobalView ? viewGlobal : viewPlayer);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    resetLevel(window, world, tilesetTexture, levelData, firstgid, player, viewPlayer, "assets/maps/level1.tmx", SCALE, contactListener, playerTexture, coins);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                    DEBUG_DRAW_ENABLED = !DEBUG_DRAW_ENABLED;
                }
            }
        }

        // Очистка экрана
        window.clear(sf::Color(192, 192, 192)); // Серый фон

        if (isMenuActive) {
            // Отрисовка меню
            menu.draw(window);
        } else {
            // Проверяем, завершилась ли игра
            if (contactListener.isGameOver) {
                isMenuActive = true; // Возвращаемся в меню при завершении игры
                continue;
            }

            float deltaTime = clock.restart().asSeconds();

            // Обработка ввода и обновление физики
            player.handleInput();
            world.Step(deltaTime, 8, 3);
            player.update(deltaTime);

            // Обновление анимации монет
            for (auto& coin : coins) {
                coin.updateAnimation(deltaTime);
            }
            // Отладочный вывод позиций монет
            for (const auto& coin : coins) {
                sf::FloatRect bounds = coin.getBoundingBox();
            std::cout << "Coin position: (" << bounds.left << ", " << bounds.top << ")\n";
            }

            // Проверка сбора монет
            for (auto& coin : coins) {
                if (!coin.isCollected() && player.getBoundingBox().intersects(coin.getBoundingBox())) {
                    coin.collect(); // Вызываем метод collect()
                    score += 10; // Увеличиваем счёт
                    std::cout << "Coin collected! Current score: " << score << std::endl;
                }
            }

            // Обновление камеры
            if (!isGlobalView) {
                sf::Vector2f playerPosition = player.getPosition();
                sf::Vector2f currentCenter = viewPlayer.getCenter();
                sf::Vector2f targetCenter(playerPosition.x * SCALE, playerPosition.y * SCALE);
                viewPlayer.setCenter(currentCenter + (targetCenter - currentCenter) * cameraSpeed);
                float minX = window.getSize().x / 2.0f;
                float maxX = 50 * TILE_SIZE - window.getSize().x / 2.0f;
                float minY = window.getSize().y / 2.0f;
                float maxY = 50 * TILE_SIZE - window.getSize().y / 2.0f;
                sf::Vector2f center = viewPlayer.getCenter();
                center.x = std::max(minX, std::min(center.x, maxX));
                center.y = std::max(minY, std::min(center.y, maxY));
                viewPlayer.setCenter(center);
                window.setView(viewPlayer);
            }

            // Отрисовка уровня
            LevelRenderer::renderLevel(window, tilesetTexture, levelData, TILE_SIZE, firstgid);

            // Отрисовка игрока
            player.draw(window);

            // Отрисовка монет
            for (auto& coin : coins) {
                coin.draw(window);
            }

            // Отладочная визуализация физических тел
            debugDrawPhysics(window, world, SCALE);

            // Обновление текста счёта
            scoreText.setString("Score: " + std::to_string(score));

            // Отрисовка счёта
            window.draw(scoreText);
        }

        // Отображение содержимого
        window.display();
    }

    return 0;
}