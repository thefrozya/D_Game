#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>

class ContactListener; // Предварительное объявление

class Player {
public:
    // Конструкторы
    Player(b2World& world, float x, float y, ContactListener* contactListener,const sf::Texture& texture);
    Player(Player&& other) noexcept; // Конструктор перемещения
    ~Player(); // Деструктор для удаления физического тела

    // Оператор присваивания перемещения
    Player& operator=(Player&& other) noexcept;

    // Методы для обновления, обработки ввода и отрисовки
    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBoundingBox() const; // Добавляем метод

    // Метод для получения позиции игрока
    sf::Vector2f getPosition() const;

    // Метод для проверки пиксельной коллизии
    bool checkPixelCollision(const std::vector<sf::Vector2f>& otherPixels, const sf::Vector2f& otherPosition);

    // Методы для управления состоянием прыжка
    void setIsJumping(bool value);
    bool getIsJumping() const;

    b2Body* getBody() const { return body; } // Метод для получения физического тела

        // Приватные методы
        void loadTextureAndCreateCollisionMask();
        void updateAnimation(float deltaTime); // Новый метод для обновления анимации

private:


    // Физика Box2D
    b2World& world; // Ссылка на мир Box2D
    b2Body* body;

    // Графика SFML
    sf::Texture texture;
    sf::Sprite sprite;

    // Состояние игрока
    bool isRunning = false;
    bool isJumping ;

    // Коллизии
    std::vector<sf::Vector2f> collisionPixels;

    // Анимация
    sf::IntRect currentFrame; // Текущий кадр анимации
    std::vector<sf::IntRect> frames; // Все кадры анимации
    float animationTimer = 0.0f; // Таймер для анимации
    float animationSpeed = 0.1f; // Скорость анимации (время между кадрами)
    bool facingRight = true; // Направление движения игрока

    // Указатель на обработчик контактов
    ContactListener* contactListener;
};

#endif // PLAYER_H