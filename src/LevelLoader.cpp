#include "LevelLoader.h"
#include <tinyxml2.h>
#include <iostream>
#include "Constants.h"

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
currentNumber += c;
} else if (!currentNumber.empty()) {
rowData.push_back(std::stoi(currentNumber));
currentNumber.clear();
}
}

// Размер тайла в метрах (объявляем здесь, чтобы использовать в обоих блоках)
float tileSizeInMeters = TILE_SIZE / scale;

if (!rowData.empty()) {
int width = layer->IntAttribute("width");
int height = layer->IntAttribute("height");
levelData.resize(height, std::vector<int>(width));

for (int y = 0; y < height; ++y) {
for (int x = 0; x < width; ++x) {
levelData[y][x] = rowData[y * width + x];

if (levelData[y][x] != 0) { // Если тайл не пустой
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody; // Статическое тело

    // Преобразование координат из пикселей в метры
    float tileX = x * TILE_SIZE / scale;
    float tileY = y * TILE_SIZE / scale;

    // Установка позиции тела (центр тайла)
    bodyDef.position.Set(tileX + tileSizeInMeters / 2.0f, tileY + tileSizeInMeters / 2.0f);

    b2Body* platformBody = world.CreateBody(&bodyDef);

    // Устанавливаем пользовательские данные для платформы
    platformBody->GetUserData().pointer = PLATFORM_USER_DATA;

    // Создаем форму тела (прямоугольник)
    b2PolygonShape shape;
    shape.SetAsBox(tileSizeInMeters / 2.0f, tileSizeInMeters / 2.0f);

    // Создаем фикстуру
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.0f; // Плотность (не нужна для статических тел)
    fixtureDef.friction = 0.8f; // Трение
    platformBody->CreateFixture(&fixtureDef);

    std::cout << "Platform created at position: (" 
              << tileX + tileSizeInMeters / 2.0f << ", " << tileY + tileSizeInMeters / 2.0f << ")" << std::endl;
}
}
}
}

// Загрузка объектов (например, точки спавна, платформ и гриба)
for (tinyxml2::XMLElement* objectGroup = root->FirstChildElement("objectgroup"); objectGroup; objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
    const char* groupName = objectGroup->Attribute("name");
    if (groupName) {
        std::cout << "Processing object group: " << groupName << std::endl;

        // Перебираем все объекты в группе
        for (tinyxml2::XMLElement* object = objectGroup->FirstChildElement("object"); object; object = object->NextSiblingElement("object")) {
            const char* name = object->Attribute("name");
            if (name) {
                std::cout << "Found object with name: " << name << std::endl;

                if (strcmp(name, "Start") == 0) {
                    // Установка точки спавна игрока
                    float spawnX = object->FloatAttribute("x") / scale;
                    float spawnY = object->FloatAttribute("y") / scale;

                    // Корректировка по Y (центр тайла)
                    spawnPoint.x = spawnX;
                    spawnPoint.y = spawnY - tileSizeInMeters / 2.0f;

                    std::cout << "Spawn point set at: (" << spawnX << ", " << spawnPoint.y << ")" << std::endl;
                } else if (strcmp(groupName, "Mashroom") == 0 && strcmp(name, "Mashroom") == 0) {
                    // Создание гриба (только в группе "Mashroom")
                    float mashroomX = object->FloatAttribute("x") / scale;
                    float mashroomY = object->FloatAttribute("y") / scale;

                    std::cout << "Loading Mashroom at position: (" << mashroomX << ", " << mashroomY << ")" << std::endl;

                    b2BodyDef bodyDef;
                    bodyDef.type = b2_staticBody; // Гриб статичен
                    bodyDef.position.Set(mashroomX + tileSizeInMeters / 2.0f, mashroomY + tileSizeInMeters / 2.0f); // Центр объекта
                    b2Body* mashroomBody = world.CreateBody(&bodyDef);

                    // Устанавливаем пользовательские данные для гриба
                    mashroomBody->GetUserData().pointer = MASHROOM_USER_DATA;

                    // Создаем форму тела (прямоугольник)
                    b2PolygonShape shape;
                    shape.SetAsBox(tileSizeInMeters / 2.0f, tileSizeInMeters / 2.0f);

                    // Создаем фикстуру
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &shape;
                    fixtureDef.density = 0.0f; // Плотность (не нужна для статических тел)
                    fixtureDef.friction = 0.8f; // Трение
                    mashroomBody->CreateFixture(&fixtureDef);

                    std::cout << "Mashroom created at position: (" << mashroomX << ", " << mashroomY << ")" << std::endl;
                } else if (strcmp(groupName, "Platforms") == 0 && strcmp(name, "Platforms") == 0) {
                    // Создание платформ (только в группе "Platforms")
                    float platformX = object->FloatAttribute("x") / scale;
                    float platformY = object->FloatAttribute("y") / scale;
                    float platformWidth = object->FloatAttribute("width") / scale;
                    float platformHeight = object->FloatAttribute("height") / scale;

                    std::cout << "Loading platform at position: (" << platformX << ", " << platformY << ")" << std::endl;

                    b2BodyDef bodyDef;
                    bodyDef.type = b2_staticBody; // Платформа статична
                    bodyDef.position.Set(platformX + platformWidth / 2.0f, platformY + platformHeight / 2.0f); // Центр объекта
                    b2Body* platformBody = world.CreateBody(&bodyDef);

                    // Устанавливаем пользовательские данные для платформы
                    platformBody->GetUserData().pointer = PLATFORM_USER_DATA;

                    // Создаем форму тела (прямоугольник)
                    b2PolygonShape shape;
                    shape.SetAsBox(platformWidth / 2.0f, platformHeight / 2.0f);

                    // Создаем фикстуру
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &shape;
                    fixtureDef.density = 0.0f; // Плотность (не нужна для статических тел)
                    fixtureDef.friction = 0.8f; // Трение
                    platformBody->CreateFixture(&fixtureDef);

                    std::cout << "Platform created at position: (" << platformX << ", " << platformY << ")" << std::endl;
                }
            }
        }
    }
}

return true;
}