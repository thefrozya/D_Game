#include "LevelLoader.h"
#include <tinyxml2.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Constants.h"
#include <cmath>
#include "MovingPlatform.h"
#include "Staircase.h"
#include "ContactListener.h"
#include "Interactable.h"



// Вспомогательная функция для создания статического тела
void createStaticBody(b2World& world, float x, float y, float width, float height, uintptr_t userData) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    b2Body* body = world.CreateBody(&bodyDef);

    if (userData != PLATFORM_USER_DATA && userData != WALL_USER_DATA && userData != DEATH_USER_DATA &&
        userData != TRAP_USER_DATA && userData != FINISH_USER_DATA && userData != LAVA_USER_DATA && userData != CLOUDS_USER_DATA) { 
        std::cerr << "Error: Invalid userData value (" << userData << ") in createStaticBody!" << std::endl;
        return;
    }
    
    body->GetUserData().pointer = userData;
    std::cout << "Created Static Body with userData: " << body->GetUserData().pointer << " at (" << x << ", " << y << ")" << std::endl;

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 10.0f;

    body->CreateFixture(&fixtureDef);
}

// Функция для парсинга полилинии
std::vector<b2Vec2> parsePolyline(const char* pointsStr, float scale) {
    std::vector<b2Vec2> vertices;
    if (!pointsStr) return vertices;

    std::stringstream ss(pointsStr);
    std::string pointStr;
    while (std::getline(ss, pointStr, ' ')) {
        size_t commaPos = pointStr.find(',');
        if (commaPos != std::string::npos) {
            float x = std::stof(pointStr.substr(0, commaPos)) / scale;
            float y = std::stof(pointStr.substr(commaPos + 1)) / scale;
            vertices.emplace_back(x, y);
        }
    }

    if (vertices.size() < 2) {
        std::cerr << "Error: Polyline must contain at least two points!" << std::endl;
        return {};
    }

    return vertices;
}

bool LevelLoader::loadLevel(const std::string &filePath,
    sf::Texture &worldTileset,
    sf::Texture &houseTileset,
    std::vector<std::vector<int>> &backgroundData,
    std::vector<std::vector<int>> &foregroundData,
    int &firstgid,
    b2World &world,
    sf::Vector2f &spawnPoint,
    float scale,
    int &mapWidthInTiles,
    int &mapHeightInTiles,
    int &tileSize,
    sf::Texture &coinTexture,
    std::vector<Coin> &coins,
    sf::Texture &enemyTexture,
    std::vector<std::unique_ptr<Enemy>> &enemies,
    std::vector<std::unique_ptr<MovingPlatform>> &movingPlatforms,
    sf::Texture& platformTexture,
    std::vector<Staircase>& staircases,std::vector<Key>& keys, 
    const sf::Texture& keyTexture, std::vector<Door>& doors,
    const sf::Texture& doorTexture) {

    // Загрузка TMX-файла
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load TMX file: " << filePath << std::endl;
        return false;
    }

    // Парсинг данных уровня
    tinyxml2::XMLElement* root = doc.FirstChildElement("map");
    if (!root) {
        std::cerr << "Invalid TMX file format!" << std::endl;
        return false;
    }

    // Извлекаем размеры карты
    mapWidthInTiles = root->IntAttribute("width");
    mapHeightInTiles = root->IntAttribute("height");
    tileSize = root->IntAttribute("tilewidth");

    std::cout << "Map size: " << mapWidthInTiles << "x" << mapHeightInTiles
              << " tiles, tile size: " << tileSize << " pixels" << std::endl;

    // Загрузка первого tileset'а (World-Tiles.png)
    tinyxml2::XMLElement* tilesetElement = root->FirstChildElement("tileset");
    if (!tilesetElement) {
        std::cerr << "No tileset found in TMX file!" << std::endl;
        return false;
    }

    const char* worldTilesetSource = tilesetElement->Attribute("source");
    if (!worldTileset.loadFromFile(worldTilesetSource)) {
        std::cerr << "Failed to load world tileset texture: " << worldTilesetSource << std::endl;
        return false;
    }

    firstgid = tilesetElement->IntAttribute("firstgid");
    std::cout << "Loaded world tileset with firstgid: " << firstgid << ", source: " << worldTilesetSource << std::endl;

    // Загрузка второго tileset'а (HOUSE.png)
    tilesetElement = tilesetElement->NextSiblingElement("tileset");
    if (!tilesetElement) {
        std::cerr << "No second tileset found in TMX file!" << std::endl;
        return false;
    }

    const char* houseTilesetSource = tilesetElement->Attribute("source");
    if (!houseTileset.loadFromFile(houseTilesetSource)) {
        std::cerr << "Failed to load house tileset texture: " << houseTilesetSource << std::endl;
        return false;
    }

    int houseFirstGID = tilesetElement->IntAttribute("firstgid");
    std::cout << "Loaded house tileset with firstgid: " << houseFirstGID << ", source: " << houseTilesetSource << std::endl;

    // Инициализация данных слоёв
    backgroundData.resize(mapHeightInTiles, std::vector<int>(mapWidthInTiles, 0));
    foregroundData.resize(mapHeightInTiles, std::vector<int>(mapWidthInTiles, 0));

    // Загрузка всех слоёв данных уровня
    int layerIndex = 0;
    for (tinyxml2::XMLElement* layer = root->FirstChildElement("layer"); layer; layer = layer->NextSiblingElement("layer")) {
        const char* layerName = layer->Attribute("name");
        if (!layerName) {
            std::cerr << "Layer without name found!" << std::endl;
            continue;
        }

        tinyxml2::XMLElement* data = layer->FirstChildElement("data");
        if (!data) {
            std::cerr << "No data found in layer: " << layerName << "!" << std::endl;
            continue;
        }

        const char* dataText = data->GetText();
        if (!dataText) {
            std::cerr << "Layer data is empty for layer: " << layerName << "!" << std::endl;
            continue;
        }

        // Разбор данных уровня
        std::vector<int> rowData;
        std::string currentNumber;
        for (char c : std::string(dataText)) {
            if (isdigit(c)) {
                currentNumber += c; // Собираем цифры в строку
            } else if (!currentNumber.empty()) {
                rowData.push_back(std::stoi(currentNumber)); // Преобразуем строку в число
                currentNumber.clear(); // Очищаем строку для следующего числа
            }
        }

        if (!rowData.empty()) {
            int width = layer->IntAttribute("width");
            int height = layer->IntAttribute("height");

            if (width != mapWidthInTiles || height != mapHeightInTiles) {
                std::cerr << "Layer " << layerName << " has incorrect dimensions!" << std::endl;
                continue;
            }

            std::vector<std::vector<int>> &targetData = (layerIndex == 0) ? backgroundData : foregroundData;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    targetData[y][x] = rowData[y * width + x];
                }
            }

            std::cout << "Loaded layer: " << layerName << " with " << width * height << " tiles" << std::endl;
            ++layerIndex;
        }
    }

    // Загрузка объектов
    for (tinyxml2::XMLElement* objectGroup = root->FirstChildElement("objectgroup"); objectGroup; objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
        const char* groupName = objectGroup->Attribute("name");
        if (!groupName) {
            std::cerr << "Object group without name found!" << std::endl;
            continue;
        }
        std::cout << "Loading object group: " << groupName << std::endl;

        int coinCount = 0;
        for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) { 
            const char* name = object->Attribute("name");
            if (!name) continue;

            float x = object->FloatAttribute("x") / scale;
            float y = object->FloatAttribute("y") / scale;

            std::cout << "Loading object: " << name << " from group: " << groupName << std::endl;

            if (strcmp(name, "Start") == 0) {
                spawnPoint.x = x;
                spawnPoint.y = y - tileSize / scale / 2.0f;
            } else if (strcmp(groupName, "Finish") == 0 && strcmp(name, "Finish") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, FINISH_USER_DATA);
            } else if (strcmp(groupName, "Platforms") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, PLATFORM_USER_DATA);
            } else if (strcmp(groupName, "Lava") == 0 && strcmp(name, "Lava") == 0) { 
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Created Lava at (" << x << ", " << y << ") with size (" << width << ", " << height << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, LAVA_USER_DATA);
            } else if (strcmp(groupName, "Clouds") == 0 && strcmp(name, "Clouds") == 0) { 
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, CLOUDS_USER_DATA);
                std::cout << "Created Cloud at (" << x << ", " << y << ") with size (" << width << ", " << height << ")" << std::endl;
            } else if (strcmp(groupName, "Trap") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, TRAP_USER_DATA);
            } else if (strcmp(groupName, "Coin") == 0 && strcmp(name, "Coin") == 0) {
                coins.emplace_back(world, x, y, coinTexture, 6, 0.1f);
                std::cout << "Created coin at position: (" << x << ", " << y << ")" << std::endl;
                coinCount++;
            } else if (strcmp(groupName, "Key") == 0) {
                for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) { 
                    const char* name = object->Attribute("name");
                    if (!name || strncmp(name, "Key", 3) != 0) continue;
    
                    float x = object->FloatAttribute("x") / scale;
                    float y = object->FloatAttribute("y") / scale;
    
                    keys.emplace_back(world, x, y, keyTexture, name);
                }
            } else if (strcmp(groupName, "Doors") == 0) {
                for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) { 
                    const char* name = object->Attribute("name");
                    if (!name || strncmp(name, "Door", 4) != 0) continue;
    
                    float x = object->FloatAttribute("x") / scale;
                    float y = object->FloatAttribute("y") / scale;
                    float width = object->FloatAttribute("width") / scale;
                    float height = object->FloatAttribute("height") / scale;
    
                    doors.emplace_back(world, x, y, width, height, name, doorTexture);
            }
                   
        }else if (strcmp(groupName, "DEATH") == 0 && strcmp(name, "DEATH") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, DEATH_USER_DATA);
            } else if (strcmp(groupName, "Wall") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                b2BodyDef bodyDef;
                bodyDef.type = b2_staticBody;
                bodyDef.position.Set(x + width / 2.0f, y + height / 2.0f);
                b2Body* wallBody = world.CreateBody(&bodyDef);
                b2PolygonShape shape;
                shape.SetAsBox(width / 2.0f, height / 2.0f);
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shape;
                fixtureDef.friction = 0.0f;
                fixtureDef.userData.pointer = WALL_USER_DATA;
                wallBody->CreateFixture(&fixtureDef);
            } else if (strcmp(groupName, "Staircase") == 0) {
                for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) { 
                    const char* name = object->Attribute("name");
                    if (!name || strcmp(name, "Staircase") != 0) continue;
    
                    float x = object->FloatAttribute("x") / scale;
                    float y = object->FloatAttribute("y") / scale;
                    float width = object->FloatAttribute("width") / scale;
                    float height = object->FloatAttribute("height") / scale;
    
                    staircases.emplace_back(world, x, y, width, height);
                    std::cout << "Loaded staircase at (" << x << ", " << y << ")" << std::endl;
    
                    // Проверка после создания
                    b2Body* body = staircases.back().getBody();
                    b2Fixture* fixture = body->GetFixtureList();
                    std::cout << "Post-load check: fixture userData=" << fixture->GetUserData().pointer << std::endl;
            }}else if (strcmp(groupName, "Enemy") == 0 && strcmp(name, "Enemy") == 0) {
                enemies.emplace_back(std::make_unique<Enemy>(world, x, y, enemyTexture));
            } else if (strcmp(groupName, "MovePlatforms") == 0) {
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                float speed = 1.5f;
                tinyxml2::XMLElement* props = object->FirstChildElement("properties");
                if (props) {
                    for (tinyxml2::XMLElement* prop = props->FirstChildElement("property"); prop; prop = prop->NextSiblingElement("property")) {
                        if (std::string(prop->Attribute("name")) == "speed") {
                            prop->QueryFloatAttribute("value", &speed);
                        }
                    }
                }
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                movingPlatforms.emplace_back(std::make_unique<MovingPlatform>(world, x + width / 2.0f, y + height / 2.0f, width, height, speed,platformTexture));
                std::cout << "Created MovingPlatform at (" << x << ", " << y << ") with size (" << width << ", " << height << ") and speed " << speed << std::endl;
            }
        }

        if (!doors.empty()) {
            Door::linkDoors(doors);
        }
        if (strcmp(groupName, "Coin") == 0) {
            std::cout << "Total coins in group 'Coin': " << coinCount << std::endl;
        }
    }
    std::cout << "Total coins loaded: " << coins.size() << std::endl;
    
    return true;
}