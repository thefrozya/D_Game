#include "LevelLoader.h"
#include <iostream>
#include <sstream>
#include <tinyxml2.h>
#include "Constants.h"

// Вспомогательная функция для создания статического тела
void createStaticBody(b2World& world, float x, float y, float width, float height, uintptr_t userData) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    b2Body* body = world.CreateBody(&bodyDef);
    body->GetUserData().pointer = userData;

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 1.5f;

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

bool LevelLoader::loadLevel(const std::string& filePath, sf::Texture& tilesetTexture,
                            std::vector<std::vector<int>>& levelData, int& firstgid,
                            b2World& world, sf::Vector2f& spawnPoint, float scale,
                            int& mapWidthInTiles, int& mapHeightInTiles, int& tileSize) {
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

    // Загрузка тайлсета
    tinyxml2::XMLElement* tilesetElement = root->FirstChildElement("tileset");
    if (!tilesetElement) {
        std::cerr << "No tileset found in TMX file!" << std::endl;
        return false;
    }

    const char* tilesetSource = tilesetElement->Attribute("source");
    if (!tilesetSource) {
        std::cerr << "Tileset source not found!" << std::endl;
        return false;
    }

    if (!tilesetTexture.loadFromFile(tilesetSource)) {
        std::cerr << "Failed to load tileset texture: " << tilesetSource << std::endl;
        return false;
    }

    firstgid = tilesetElement->IntAttribute("firstgid");

    // Загрузка слоя данных уровня
    tinyxml2::XMLElement* layer = root->FirstChildElement("layer");
    if (!layer) {
        std::cerr << "No layer found in TMX file!" << std::endl;
        return false;
    }

    tinyxml2::XMLElement* data = layer->FirstChildElement("data");
    if (!data) {
        std::cerr << "No data found in layer!" << std::endl;
        return false;
    }

    const char* dataText = data->GetText();
    if (!dataText) {
        std::cerr << "Layer data is empty!" << std::endl;
        return false;
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
        levelData.resize(height, std::vector<int>(width));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                levelData[y][x] = rowData[y * width + x];
                if (levelData[y][x] != 0) { // Если тайл не пустой
                    float tileX = x * tileSize / scale;
                    float tileY = y * tileSize / scale;
                    float tileSizeInMeters = tileSize / scale;
                  
                }
            }
        }
    }
    

    // Загрузка объектов
    for (tinyxml2::XMLElement* objectGroup = root->FirstChildElement("objectgroup"); objectGroup; objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
        const char* groupName = objectGroup->Attribute("name");
        if (!groupName) continue;

        for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) {
            const char* name = object->Attribute("name");
            if (!name) continue;

            float x = object->FloatAttribute("x") / scale;
            float y = object->FloatAttribute("y") / scale;

            std::cout << "Loading object: " << name << " from group: " << groupName << std::endl;

            if (strcmp(name, "Start") == 0) {
                // Установка точки спавна игрока
                spawnPoint.x = x;
                spawnPoint.y = y - tileSize / scale / 2.0f;
                std::cout << "Spawn point loaded from TMX: (" << spawnPoint.x << ", " << spawnPoint.y << ")" << std::endl;
            } else if (strcmp(groupName, "Finish") == 0 && strcmp(name, "Finish") == 0) {
                // Создание финишной точки
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, Finish_USER_DATA);
                std::cout << "Created Finish object at (" << x << ", " << y << ")" << std::endl;
            } else if (strcmp(groupName, "Platforms") == 0) {
                // Создание платформ
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, PLATFORM_USER_DATA);
                std::cout << "Created platform at (" << x << ", " << y << ") with size (" << width << ", " << height << ")" << std::endl;
            } else if (strcmp(groupName, "Coin") == 0 && strcmp(name, "Coin") == 0) {
                // Создание монеты
                                // Создание платформ
                                float width = object->FloatAttribute("width") / scale;
                                float height = object->FloatAttribute("height") / scale;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, PLATFORM_USER_DATA);
                std::cout << "Created coin at position: (" << x << ", " << y << ")" << std::endl;
            } else if (strcmp(groupName, "DEATH") == 0 && strcmp(name, "DEATH") == 0) {
                // Создание триггера смерти
                float width = object->FloatAttribute("width") / scale;
                float height = object->FloatAttribute("height") / scale;
                createStaticBody(world, x + width / 2.0f, y + height / 2.0f, width, height, DEATH_USER_DATA);
                std::cout << "Created DEATH trigger at (" << x << ", " << y << ") with size (" << width << ", " << height << ")" << std::endl;
            } else if (strcmp(groupName, "Wall") == 0) {
                // Создание стен
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
                std::cout << "Created Wall at (" << x << ", " << y << ") with size (" << width << ", " << height << ")" << std::endl;
            }
        }

    }
    

    return true;
}