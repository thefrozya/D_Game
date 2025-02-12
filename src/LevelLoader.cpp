#include "LevelLoader.h"
#include <tinyxml2.h>
#include <iostream>
#include <sstream>
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
    fixtureDef.friction = 0.8f;
    body->CreateFixture(&fixtureDef);
}

bool LevelLoader::loadLevel(const std::string& filePath, sf::Texture& tilesetTexture,
                            std::vector<std::vector<int>>& levelData, int& firstgid,
                            b2World& world, sf::Vector2f& spawnPoint, float scale) {
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
            rowData.push_back(std::stoi(currentNumber)); // Преобразуем строку в число и добавляем в массив
            currentNumber.clear(); // Очищаем строку для следующего числа
        }
    }

    float tileSizeInMeters = TILE_SIZE / scale;

    if (!rowData.empty()) {
        int width = layer->IntAttribute("width");
        int height = layer->IntAttribute("height");
        levelData.resize(height, std::vector<int>(width));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                levelData[y][x] = rowData[y * width + x];
                
                if (levelData[y][x] != 0) { // Если тайл не пустой
                    float tileX = x * TILE_SIZE / scale;
                    float tileY = y * TILE_SIZE / scale;
                    createStaticBody(world, tileX + tileSizeInMeters / 2.0f, tileY + tileSizeInMeters / 2.0f,
                                     tileSizeInMeters, tileSizeInMeters, PLATFORM_USER_DATA);
                }
            }
        }
    }

    // Загрузка объектов (например, точки спавна, платформ и гриба)
    for (tinyxml2::XMLElement* objectGroup = root->FirstChildElement("objectgroup"); objectGroup; objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
        const char* groupName = objectGroup->Attribute("name");
        if (groupName) {
            for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) {
                const char* name = object->Attribute("name");
                if (name) {
                    if (strcmp(name, "Start") == 0) {
                        // Установка точки спавна игрока
                        float spawnX = object->FloatAttribute("x") / scale;
                        float spawnY = object->FloatAttribute("y") / scale;
                        spawnPoint.x = spawnX;
                        spawnPoint.y = spawnY - tileSizeInMeters / 2.0f;
                        std::cout << "Spawn point loaded: (" << spawnPoint.x << ", " << spawnPoint.y << ")" << std::endl;
                    } else if (strcmp(groupName, "Mashroom") == 0 && strcmp(name, "Mashroom") == 0) {
                        // Создание гриба
                        float mashroomX = object->FloatAttribute("x") / scale;
                        float mashroomY = object->FloatAttribute("y") / scale;
                        createStaticBody(world, mashroomX + tileSizeInMeters / 2.0f, mashroomY + tileSizeInMeters / 2.0f,
                                         tileSizeInMeters, tileSizeInMeters, MASHROOM_USER_DATA);
                    } else if (strcmp(groupName, "Platforms") == 0 && strcmp(name, "Platforms") == 0) {
                        // Создание платформ
                        float platformX = object->FloatAttribute("x") / scale;
                        float platformY = object->FloatAttribute("y") / scale;
                        float platformWidth = object->FloatAttribute("width") / scale;
                        float platformHeight = object->FloatAttribute("height") / scale;
                        createStaticBody(world, platformX + platformWidth / 2.0f, platformY + platformHeight / 2.0f,
                                         platformWidth, platformHeight, PLATFORM_USER_DATA);
                    }
                }
            }
        }
    }

    return true;
}