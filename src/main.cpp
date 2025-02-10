#include <SFML/Graphics.hpp>
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include <Box2D/Box2D.h>
#include <iostream>

// Функция для сброса уровня
void resetLevel(sf::RenderWindow& window, b2World& world, sf::Texture& tilesetTexture,
    std::vector<std::vector<int>>& levelData, int& firstgid, Player& player,
    sf::View& viewPlayer, const std::string& levelPath, float scale, ContactListener& contactListener) {
// Удаляем все тела из мира Box2D
for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
world.DestroyBody(body);
}

// Сбрасываем данные уровня
levelData.clear();
firstgid = 0;

// Загружаем уровень заново
sf::Vector2f spawnPoint(0.0f, 0.0f);
if (!LevelLoader::loadLevel(levelPath, tilesetTexture, levelData, firstgid, world, spawnPoint, scale)) {
std::cerr << "Failed to reload level!" << std::endl;
return;
}

// Пересоздаем игрока
player = Player(world, spawnPoint.x, spawnPoint.y, &contactListener);

// Сбрасываем камеру
sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
viewPlayer.setCenter(startPosition);
window.setView(viewPlayer);

std::cout << "Level reset successfully!" << std::endl;
}

void debugDrawPhysics(sf::RenderWindow& window, b2World& world, float scale) {
    if (!DEBUG_DRAW_ENABLED) {
        return; // Если визуализация отключена, выходим из функции
    }

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            if (fixture->GetType() == b2Shape::e_polygon) {
                b2PolygonShape* shape = (b2PolygonShape*)fixture->GetShape();
                int vertexCount = shape->m_count;
                sf::ConvexShape polygon(vertexCount);

                for (int i = 0; i < vertexCount; ++i) {
                    b2Vec2 vertex = body->GetWorldPoint(shape->m_vertices[i]);
                    polygon.setPoint(i, sf::Vector2f(vertex.x * scale, vertex.y * scale));
                }

                if (body->GetType() == b2_dynamicBody) {
                    polygon.setFillColor(sf::Color::Transparent);
                    polygon.setOutlineThickness(2.0f);
                    polygon.setOutlineColor(sf::Color::Green); // Игрок
                } else if (body->GetUserData().pointer == MASHROOM_USER_DATA) {
                    polygon.setFillColor(sf::Color::Transparent);
                    polygon.setOutlineThickness(2.0f);
                    polygon.setOutlineColor(sf::Color::Blue); // Гриб
                } else {
                    polygon.setFillColor(sf::Color::Transparent);
                    polygon.setOutlineThickness(2.0f);
                    polygon.setOutlineColor(sf::Color::Red); // Платформы
                }

                window.draw(polygon);
            }
        }
    }
}

int main() {
    // Создание окна
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Platformer");
    window.setFramerateLimit(60);

    // Загрузка текстуры тайлсета
    sf::Texture tilesetTexture;
    if (!tilesetTexture.loadFromFile("assets/textures/tileset.png")) {
        std::cerr << "Failed to load tileset texture!" << std::endl;
        return -1;
    }

    std::vector<std::vector<int>> levelData;
    int firstgid = 0;
    sf::Vector2f spawnPoint(0.0f, 0.0f);

    // Создание мира Box2D
    b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
    b2World world(gravity);

    // Создание обработчика контактов
    ContactListener contactListener;
    world.SetContactListener(&contactListener);

    // Загрузка уровня
    if (!LevelLoader::loadLevel("assets/maps/level1.tmx", tilesetTexture, levelData, firstgid, world, spawnPoint, SCALE)) {
        std::cerr << "Failed to load level!" << std::endl;
        return -1;
    }

    // Создание игрока
    Player player(world, spawnPoint.x, spawnPoint.y, &contactListener);

    // Приближенная камера (следует за игроком)
    sf::View viewPlayer(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    const float cameraSpeed = 0.1f;

    // Глобальная камера (показывает всю карту)
    sf::View viewGlobal(sf::FloatRect(0, 0, 50 * TILE_SIZE, 50 * TILE_SIZE));
    viewGlobal.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));

    bool isGlobalView = false;

    // Установка начальной позиции камеры
    sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
    viewPlayer.setCenter(startPosition);
    window.setView(viewPlayer);

    // Основной цикл игры
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                viewPlayer.setSize(event.size.width, event.size.height);
                window.setView(viewPlayer);
                std::cout << "Window resized to: (" << event.size.width << ", " << event.size.height << ")\n";
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
                isGlobalView = !isGlobalView;
                window.setView(isGlobalView ? viewGlobal : viewPlayer);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetLevel(window, world, tilesetTexture, levelData, firstgid, player, viewPlayer, "assets/maps/level1.tmx", SCALE, contactListener);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                DEBUG_DRAW_ENABLED = !DEBUG_DRAW_ENABLED;
                std::cout << "Debug draw visualization " << (DEBUG_DRAW_ENABLED ? "enabled" : "disabled") << std::endl;
            }
        }
    
        // Проверяем, завершилась ли игра
        if (contactListener.isGameOver) {
            std::cout << "Game Over! Exiting..." << std::endl;
            break; // Выходим из игрового цикла
        }
    
        float deltaTime = clock.restart().asSeconds();
    
        // Обработка ввода и обновление физики
        player.handleInput();
        world.Step(deltaTime, 8, 3);
        player.update(deltaTime);
    
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
    
        // Очистка экрана
        window.clear(sf::Color(192, 192, 192)); // Серый фон
    
        // Отрисовка уровня
        renderLevel(window, tilesetTexture, levelData, TILE_SIZE, firstgid);
    
        // Отрисовка игрока
        player.draw(window);
    
        // Отладочная визуализация физических тел
        debugDrawPhysics(window, world, SCALE);
    
        // Отображение содержимого
        window.display();
    }
}