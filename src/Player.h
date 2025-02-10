#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Constants.h"
#include "ContactListener.h" // Подключаем заголовочный файл для ContactListener

class Player {
public:
    Player() = default; // Конструктор по умолчанию
    Player(b2World& world, float x, float y, ContactListener* contactListener); // Добавляем параметр ContactListener
    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window);

    b2Body* getBody() const { return body; }
    sf::Vector2f getPosition() const;

private:
    b2Body* body = nullptr; // Физическое тело
    sf::Texture texture; // Текстура игрока
    sf::Sprite sprite; // Спрайт игрока

    bool isRunning = false; // Флаг для бега
    ContactListener* contactListener; // Указатель на обработчик контактов
};

#endif