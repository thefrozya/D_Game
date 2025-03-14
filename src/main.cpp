#include <SFML/Graphics.hpp>
#include "LevelLoader.h"
#include "LevelRenderer.h"
#include "Player.h"
#include "Constants.h"
#include "ContactListener.h"
#include "Menu.h"
#include "Coin.h"
#include "DebugUtils.h"
#include "Enemy.h"
#include "MovingPlatform.h"
#include "Staircase.h"
#include "Interactable.h"
#include <iostream>
#include <vector>
#include <memory>
#include <SFML/Audio.hpp>

bool DEBUG_DRAW_ENABLED = false;

sf::Texture playerRunTexture, playerJumpTexture, playerDeathTexture, coinTexture, enemyTexture, keyTexture , doorOpenTexture,platformTexture;
std::vector<std::unique_ptr<Enemy>> enemies;
std::vector<std::unique_ptr<MovingPlatform>> movingPlatforms;
std::vector<Staircase> staircases;
std::vector<Key> keys;
std::vector<Door> doors;
LevelRenderer renderer;
sf::Texture worldTileset, houseTileset;
std::vector<std::vector<int>> backgroundData, foregroundData;

void resetGameState(Player& player, std::vector<Coin>& coins, int& score, sf::Vector2f spawnPoint) {
    player.respawn(spawnPoint.x, spawnPoint.y);
    coins.clear();
    enemies.clear();
    score = 0;
}

void clearGameObjects(b2World& world, std::vector<Coin>& coins, int& score, const std::vector<std::unique_ptr<Enemy>>& enemies, Player& player) {
    for (b2Body* body = world.GetBodyList(); body; ) {
        b2Body* nextBody = body->GetNext();
        world.DestroyBody(body);
        body = nextBody;
    }
    if (player.getBody()) {
        player.destroyBody(world);
    }
    coins.clear();
    score = 0;
    player.respawn(0.0f, 0.0f);
}

void resetLevel(sf::RenderWindow& window, b2World& world, sf::Texture& tilesetTexture,
                std::vector<std::vector<int>>& levelData, int& firstgid, std::unique_ptr<Player>& player,
                sf::View& viewPlayer, const std::string& levelPath, float scale, ContactListener& contactListener,
                sf::Texture& playerRunTexture, sf::Texture& playerJumpTexture, sf::Texture& playerDeathTexture,
                int mapWidthInTiles, int mapHeightInTiles, int tileSize,
                sf::Texture& coinTexture, int& score, std::vector<Coin>& coins,
                sf::Texture& enemyTexture, std::vector<std::unique_ptr<Enemy>>& enemies,
                std::vector<std::unique_ptr<MovingPlatform>>& movingPlatforms,sf::Texture& platformTexture, std::vector<Staircase>& staircases,
                std::vector<Key>& keys, const sf::Texture& keyTexture, std::vector<Door>& doors, const sf::Texture& doorTexture) {
    contactListener.cleanupDestroyedBodies(world);

    int bodyCount = world.GetBodyCount();
    std::cout << "Bodies in world before cleanup: " << bodyCount << std::endl;
    for (b2Body* body = world.GetBodyList(); body; ) {
        b2Body* nextBody = body->GetNext();
        world.DestroyBody(body);
        body = nextBody;
    }
    std::cout << "Bodies in world after cleanup: " << world.GetBodyCount() << std::endl;

    levelData.clear();
    firstgid = 0;
    score = 0;
    coins.clear();
    enemies.clear();
    movingPlatforms.clear();
    staircases.clear();
    keys.clear();
    doors.clear();

    sf::Vector2f spawnPoint(0.0f, 0.0f);
    if (!LevelLoader::loadLevel(levelPath, worldTileset, houseTileset,
                                backgroundData, foregroundData, firstgid, world, spawnPoint, scale,
                                mapWidthInTiles, mapHeightInTiles, tileSize, coinTexture, coins,
                                enemyTexture, enemies, movingPlatforms,platformTexture, staircases, keys, keyTexture, doors,doorOpenTexture)) {
        std::cerr << "Failed to reload level from path: " << levelPath << std::endl;
        return;
    }

    player = std::make_unique<Player>(world, spawnPoint.x, spawnPoint.y, &contactListener,
                                      playerRunTexture, playerJumpTexture, playerDeathTexture);
    contactListener.setPlayer(player.get());
    contactListener.setStaircases(staircases);
    contactListener.setDoors(&doors);

    std::cout << "Player reset at: " << player.get() << std::endl;
    std::cout << "Number of coins: " << coins.size() << std::endl;
    std::cout << "Number of enemies: " << enemies.size() << std::endl;

    sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
    viewPlayer.setCenter(startPosition);
    window.setView(viewPlayer);
}

void updateCamera(sf::View& camera, const Player& player, float deltaTime, float smoothing,
                  float levelWidth, float levelHeight) {
    b2Vec2 playerPos = player.getBody()->GetPosition();
    sf::Vector2f targetPos(playerPos.x * SCALE, playerPos.y * SCALE);
    sf::Vector2f currentPos = camera.getCenter();
    sf::Vector2f newPos;
    newPos.x = currentPos.x + (targetPos.x - currentPos.x) * smoothing * deltaTime;
    newPos.y = currentPos.y + (targetPos.y - currentPos.y) * smoothing * deltaTime;

    // Ограничиваем камеру границами уровня
    float cameraHalfWidth = camera.getSize().x / 2.0f;
    float cameraHalfHeight = camera.getSize().y / 2.0f;

   
    /*std::cout << "Player pos: (" << playerPos.x * SCALE << ", " << playerPos.y * SCALE << "), "
              << "Camera target before clamp: (" << newPos.x << ", " << newPos.y << "), "
              << "Level bounds: (0, 0, " << levelWidth << ", " << levelHeight << "), "
              << "Camera size: (" << camera.getSize().x << ", " << camera.getSize().y << ")" << std::endl;*/

    newPos.x = std::max(cameraHalfWidth, std::min(newPos.x, levelWidth - cameraHalfWidth));
    newPos.y = std::max(cameraHalfHeight, std::min(newPos.y, levelHeight - cameraHalfHeight));

    // Дополнительная проверка на случай превышения правого края
    if (newPos.x > levelWidth - cameraHalfWidth) {
        //std::cout << "Camera capped at right edge: " << levelWidth - cameraHalfWidth << std::endl;
        newPos.x = levelWidth - cameraHalfWidth;
    }

    //std::cout << "Camera target after clamp: (" << newPos.x << ", " << newPos.y << ")" << std::endl;

    camera.setCenter(newPos);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "D_Game");
    window.setFramerateLimit(60);

    if (!playerRunTexture.loadFromFile("assets/textures/player_spritesheet.png") ||
        !playerJumpTexture.loadFromFile("assets/textures/player_spritesheetJump.png") ||
        !playerDeathTexture.loadFromFile("assets/textures/player_spritesheetDeath.png") ||
        !coinTexture.loadFromFile("assets/textures/coin_spritesheet.png") ||
        !enemyTexture.loadFromFile("assets/textures/enemy_spritesheet.png") ||
        !keyTexture.loadFromFile("assets/textures/key_spritesheet.png") ||
        !doorOpenTexture.loadFromFile("assets/textures/door_open_spritesheet.png") ||
        !platformTexture.loadFromFile("assets/textures/MovingPlatform.png")) {
        std::cerr << "Failed to load textures!" << std::endl;
        return -1;
    }

    sf::View camera(sf::FloatRect(0, 0, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
    window.setView(camera);

    b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
    b2World world(gravity);

    ContactListener contactListener(nullptr);
    std::unique_ptr<Player> player;
    world.SetContactListener(&contactListener);

    Menu menu(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Vector2f spawnPoint(0.0f, 0.0f);
    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::Text scoreText("Score: 0", font, 24);
    sf::Text healthText("HP: 0", font, 24);
    sf::Text winText("Congratulations You Win!", font, 48);
    sf::Text enterText("Press Enter to return to menu", font, 30);
    scoreText.setPosition(10, 10);
    healthText.setPosition(WINDOW_WIDTH - 100, 10);
    winText.setPosition(WINDOW_WIDTH / 2.0f - winText.getLocalBounds().width / 2.0f,
                        WINDOW_HEIGHT / 2.0f - winText.getLocalBounds().height - 20);
    enterText.setPosition(WINDOW_WIDTH / 2.0f - enterText.getLocalBounds().width / 2.0f,
                          WINDOW_HEIGHT / 2.0f + 20);
    scoreText.setFillColor(sf::Color::White);
    healthText.setFillColor(sf::Color::Red);
    winText.setFillColor(sf::Color::Black);
    enterText.setFillColor(sf::Color::Black);

    float winDelayTimer = 0.0f;
    const float winDelay = 2.0f; // Задержка перед показом "Press Enter"
    float enterBlinkTimer = 0.0f;
    bool showEnterText = true;

    sf::View viewPlayer(sf::FloatRect(0, 0, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f));
    sf::View viewMenu(sf::FloatRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    int mapWidthInTiles = 0, mapHeightInTiles = 0, tileSize = 0, firstgid = 0, score = 0;
    std::vector<std::vector<int>> levelData;

    bool isMenuActive = true;
    bool isGameRunning = false;

    sf::Clock clock;
    float smoothing = 4.0f;

    enemies.reserve(5);
    movingPlatforms.reserve(5);
    staircases.reserve(10);
    std::vector<Coin> coins;
    coins.reserve(1000);
    keys.reserve(5);
    doors.reserve(15);

    // Границы уровня в пикселях
    float levelWidth = 0.0f;
    float levelHeight = 0.0f;

    float lavaDamageTimer = 0.0f; // Таймер для урона от лавы

    while (window.isOpen()) {
        sf::Event event;
        float deltaTime = clock.restart().asSeconds();

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                menu.stopBackgroundMusic();
                window.close();
            }
            if (isMenuActive) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) menu.moveUp();
                    else if (event.key.code == sf::Keyboard::Down) menu.moveDown();
                    else if (event.key.code == sf::Keyboard::Enter) {
                        int selectedItem = menu.getPressedItem();
                        if (selectedItem == 0) { // Start Game
                            menu.stopBackgroundMusic();
                            isMenuActive = false;
                            isGameRunning = true;

                            std::cout << "Clearing world, initial body count: " << world.GetBodyCount() << std::endl;
                            for (b2Body* body = world.GetBodyList(); body;) {
                                b2Body* nextBody = body->GetNext();
                                world.DestroyBody(body);
                                body = nextBody;
                            }
                            std::cout << "World cleared, body count: " << world.GetBodyCount() << std::endl;

                            coins.clear();
                            enemies.clear();
                            movingPlatforms.clear();
                            staircases.clear();
                            keys.clear();
                            doors.clear();

                            if (!LevelLoader::loadLevel("assets/maps/Lvl1.tmx", worldTileset, houseTileset,
                                                        backgroundData, foregroundData, firstgid, world, spawnPoint, SCALE,
                                                        mapWidthInTiles, mapHeightInTiles, tileSize, coinTexture, coins,
                                                        enemyTexture, enemies, movingPlatforms, platformTexture , staircases, keys, keyTexture, doors ,doorOpenTexture)) {
                                std::cerr << "Failed to load level!" << std::endl;
                                return -1;
                            }

                            
                            levelWidth = mapWidthInTiles * tileSize; 
                            levelHeight = mapHeightInTiles * tileSize;
                            /*std::cout << "Level bounds set: width=" << levelWidth << ", height=" << levelHeight 
                                      << ", mapWidthInTiles=" << mapWidthInTiles << ", mapHeightInTiles=" << mapHeightInTiles 
                                      << ", tileSize=" << tileSize << std::endl;*/

                            std::cout << "Loaded " << coins.size() << " coins from TMX" << std::endl;

                            player = std::make_unique<Player>(world, spawnPoint.x, spawnPoint.y, &contactListener,
                                                              playerRunTexture, playerJumpTexture, playerDeathTexture);
                            contactListener.setPlayer(player.get());
                            contactListener.setStaircases(staircases);
                            contactListener.setDoors(&doors);
                            world.SetContactListener(&contactListener);
                            std::cout << "Player created at: " << player.get() << std::endl;

                            sf::Vector2f startPosition(spawnPoint.x * SCALE, spawnPoint.y * SCALE);
                            viewPlayer.setCenter(startPosition);
                            window.setView(viewPlayer);
                        } else if (selectedItem == 2) window.close();
                    }
                }
            } else if (isGameRunning && player) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::R) {
                        resetLevel(window, world, worldTileset, levelData, firstgid, player, viewPlayer,
                                   "assets/maps/Lvl1.tmx", SCALE, contactListener, playerRunTexture,
                                   playerJumpTexture, playerDeathTexture, mapWidthInTiles, mapHeightInTiles,
                                   tileSize, coinTexture, score, coins, enemyTexture, enemies, movingPlatforms,platformTexture, staircases,
                                   keys, keyTexture, doors , doorOpenTexture);
                        contactListener.setPlayer(player.get());
                    }
                    if (event.key.code == sf::Keyboard::F1) {
                        DEBUG_DRAW_ENABLED = !DEBUG_DRAW_ENABLED;
                    }
                    if (contactListener.isGameWon && event.key.code == sf::Keyboard::Enter && winDelayTimer >= winDelay) {
                        std::cout << "Enter pressed, returning to menu" << std::endl;
                        isGameRunning = false;
                        isMenuActive = true;
                        contactListener.isGameWon = false;
                        winDelayTimer = 0.0f;
                        enterBlinkTimer = 0.0f;
                        showEnterText = true;

                        std::cout << "Cleaning up world, body count before: " << world.GetBodyCount() << std::endl;
                        if (player && player->getBody()) {
                            world.DestroyBody(player->getBody());
                            player->setBody(nullptr);
                            std::cout << "Player body destroyed" << std::endl;
                        }
                        for (auto& coin : coins) {
                            if (coin.getBody()) {
                                world.DestroyBody(coin.getBody());
                                coin.setBody(nullptr);
                            }
                        }
                        coins.clear();
                        std::cout << "Coins cleared" << std::endl;
                        for (auto& enemy : enemies) {
                            if (enemy->getBody()) {
                                world.DestroyBody(enemy->getBody());
                                enemy->setBody(nullptr);
                            }
                        }
                        enemies.clear();
                        std::cout << "Enemies cleared" << std::endl;
                        for (auto& platform : movingPlatforms) {
                            if (platform->getBody()) {
                                world.DestroyBody(platform->getBody());
                                platform->setBody(nullptr);
                            }
                        }
                        movingPlatforms.clear();
                        std::cout << "Moving platforms cleared" << std::endl;
                        for (auto& staircase : staircases) {
                            if (staircase.getBody()) {
                                world.DestroyBody(staircase.getBody());
                                staircase.setBody(nullptr);
                            }
                        }
                        staircases.clear();
                        std::cout << "Staircases cleared" << std::endl;
                        for (auto& key : keys) {
                            if (key.getBody()) {
                                world.DestroyBody(key.getBody());
                                key.setBody(nullptr);
                            }
                        }
                        keys.clear();
                        std::cout << "Keys cleared" << std::endl;
                        for (auto& door : doors) {
                            if (door.getBody()) {
                                world.DestroyBody(door.getBody());
                                door.setBody(nullptr);
                            }
                        }
                        doors.clear();
                        std::cout << "Doors cleared" << std::endl;
                        contactListener.clearDestroyedBodies();
                        std::cout << "ContactListener buffer cleared" << std::endl;

                        std::cout << "Remaining bodies before final cleanup: " << world.GetBodyCount() << std::endl;
                        if (world.GetBodyCount() > 0) {
                            for (b2Body* body = world.GetBodyList(); body;) {
                                b2Body* nextBody = body->GetNext();
                                std::cout << "Destroying remaining body at: " << body << std::endl;
                                world.DestroyBody(body);
                                body = nextBody;
                            }
                        }
                        std::cout << "World cleaned, body count after: " << world.GetBodyCount() << std::endl;

                        player.reset();
                        score = 0;
                        menu.playBackgroundMusic();
                        window.setView(viewMenu);
                        std::cout << "Returned to menu" << std::endl;
                    }
                }
            }
        }

        window.clear(sf::Color(192, 192, 192));
        if (isMenuActive) {
            menu.draw(window);
            window.setView(viewMenu);
        } else if (isGameRunning && player) {
            if (!player->isDying() && !player->isWaitingForRespawn()) {
                player->handleInput();
                world.Step(deltaTime, 8, 3);
                contactListener.cleanupDestroyedBodies(world);
            }

            if (contactListener.shouldTeleport) {
                player->getBody()->SetTransform(contactListener.teleportPosition, player->getBody()->GetAngle());
                contactListener.shouldTeleport = false;
                std::cout << "Player teleported to (" << contactListener.teleportPosition.x << ", " << contactListener.teleportPosition.y << ")" << std::endl;
            }


            // Логика урона от лавы
            if (contactListener.isInLava()) {
                lavaDamageTimer += deltaTime;
                if (lavaDamageTimer >= 1.0f) { // Каждую секунду
                    player->takeDamage(1, b2Vec2_zero); // 1 урон без отскока
                    std::cout << "Player took 1 damage from lava. Health: " << player->getHealth() << std::endl;
                    lavaDamageTimer = 0.0f; // Сбрасываем таймер
                }
            } else {
                lavaDamageTimer = 0.0f; // Сбрасываем таймер, если игрок не в лаве
            }
            
            player->update(deltaTime);

            // Проверка выхода за границы мира и возврат к точке спавна
            b2Vec2 playerPos = player->getBody()->GetPosition();
            float playerX = playerPos.x * SCALE;
            float playerY = playerPos.y * SCALE;
            if (playerX < 0 || playerX > levelWidth || playerY < 0 || playerY > levelHeight) {
                /*std::cout << "Player out of bounds at (" << playerX << ", " << playerY << "), respawning to (" 
                          << spawnPoint.x << ", " << spawnPoint.y << ")" << std::endl;*/
                player->respawn(spawnPoint.x, spawnPoint.y);
            }

            for (auto& platform : movingPlatforms) {
                platform->update();
            }
            for (auto& enemy : enemies) {
                enemy->update(deltaTime);
            }
            for (auto& door : doors) {
                door.update(deltaTime); 
            }


            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [](const std::unique_ptr<Enemy>& enemy) { return enemy->isDead(); }), enemies.end());

            for (auto it = coins.begin(); it != coins.end();) {
                it->updateAnimation(deltaTime);
                it->draw(window);
                if (!it->isCollected() && player->getBoundingBox().intersects(it->getBoundingBox())) {
                    it->collect(10);
                    score += 10;
                    std::cout << "Coin collected at (" << it->getBody()->GetPosition().x * SCALE 
                              << ", " << it->getBody()->GetPosition().y * SCALE << ")! Current score: " << score << std::endl;
                    it->destroyBody(world);
                    it = coins.erase(it);
                } else {
                    ++it;
                }
            }
            for (auto it = keys.begin(); it != keys.end();) {
                it->update(deltaTime);
                it->draw(window);
                if (!it->isCollected() && player->getBoundingBox().intersects(it->getBoundingBox())) {
                    std::string keyType = it->getType();
                    it->collect(10);
                    it->destroyBody(world);
                    player->addKey(keyType); 
                    it = keys.erase(it);
                } else {
                    ++it;
                }
            }
                    
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                sf::FloatRect playerBounds = player->getBoundingBox();
                for (auto& door : doors) {
                    if (!door.isActive() && playerBounds.intersects(door.getBoundingBox())) {
                        std::string doorType = door.getType();
                        if ((doorType == "DoorsOne" || doorType == "DoorsOneL") && player->hasKey("KeyOne")) {
                            door.activate();
                        } else if ((doorType == "DoorsTwo" || doorType == "DoorsTwoL") && player->hasKey("KeyTwo")) {
                            door.activate();
                        } else if ((doorType == "DoorsThree" || doorType == "DoorsThreeL") && player->hasKey("KeyThree")) {
                            door.activate();
                        } else if ((doorType == "DoorsFour" || doorType == "DoorsFourL") && player->hasKey("KeyFour")) {
                            door.activate();
                        } else if ((doorType == "DoorsFive" || doorType == "DoorsFiveL") && player->hasKey("KeyFive")) {
                            door.activate();
                        }
                    }
                }
            }

            int coinScoreThisFrame = contactListener.getCoinScore();
            int enemyScoreThisFrame = contactListener.getEnemyScore();
            if (coinScoreThisFrame > 0 || enemyScoreThisFrame > 0) {
                /*std::cout << "Adding to score: coin=" << coinScoreThisFrame 
                          << ", enemy=" << enemyScoreThisFrame << std::endl;
                score += coinScoreThisFrame + enemyScoreThisFrame;*/
            }

            updateCamera(camera, *player, deltaTime, smoothing, levelWidth, levelHeight);
            window.setView(camera);

            if (contactListener.isGameWon) {
                winDelayTimer += deltaTime;
                window.setView(window.getDefaultView());
                window.draw(winText);
                if (winDelayTimer >= winDelay) {
                    enterBlinkTimer += deltaTime;
                    if (enterBlinkTimer >= 0.5f) {
                        showEnterText = !showEnterText;
                        enterBlinkTimer = 0.0f;
                    }
                    if (showEnterText) {
                        window.draw(enterText);
                    }
                }
            } else {
                renderer.renderLevel(window, worldTileset, houseTileset, backgroundData, foregroundData, tileSize, 1, 1478);
                player->draw(window);
                for (auto& enemy : enemies) enemy->draw(window);
                for (auto& staircase : staircases) window.draw(staircase.getShape());
                for (auto& coin : coins) coin.draw(window);
                for (auto& key : keys) key.draw(window);
                for (auto& door : doors) door.draw(window);
                for (const auto& platform : movingPlatforms) {
                    platform->draw(window); // Отрисовываем платформы
                }
                debugDrawPhysics(window, world, SCALE);

                sf::View currentView = window.getView();
                window.setView(window.getDefaultView());
                healthText.setString("HP: " + std::to_string(player->getHealth()));
                scoreText.setString("Score: " + std::to_string(score));
                window.draw(healthText);
                window.draw(scoreText);
                window.setView(currentView);
            }

            if (player->isWaitingForRespawn() && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                player->respawn(spawnPoint.x, spawnPoint.y);
            }
        }
        window.display();
    }
    menu.stopBackgroundMusic();
    return 0;
}