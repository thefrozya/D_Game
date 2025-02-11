#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
class ContactListener; // Предварительное объявление

class Player {
public:
    Player(b2World& world, float x, float y, ContactListener* contactListener);
    ~Player(); // Деструктор для удаления физического тела
    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    // Метод для проверки пиксельной коллизии
    bool checkPixelCollision(const std::vector<sf::Vector2f>& otherPixels, const sf::Vector2f& otherPosition);

private:
    void loadTextureAndCreateCollisionMask();
    void updateAnimation(float deltaTime); // Новый метод для обновления анимации
    b2World* world; // Изменено с ссылки на указатель
    b2Body* body;
    sf::Texture texture;
    sf::Sprite sprite;
    bool isRunning = false;
    ContactListener* contactListener;
    // Маска коллизии (непрозрачные пиксели)
    std::vector<sf::Vector2f> collisionPixels;
    // Параметры анимации
    sf::IntRect currentFrame; // Текущий кадр анимации
    std::vector<sf::IntRect> frames; // Все кадры анимации
    float animationTimer = 0.0f; // Таймер для анимации
    float animationSpeed = 0.1f; // Скорость анимации (время между кадрами)
    bool facingRight = true; // Направление движения игрока
};

#endif // PLAYER_H