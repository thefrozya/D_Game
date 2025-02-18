#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>

class Enemy {
public:
    // Конструктор и деструктор
    Enemy(b2World& world, float x, float y, const sf::Texture& enemyTexture);
    ~Enemy();

    // Методы для обновления и отрисовки
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    // Методы для взаимодействия
    void kill(); // Убивает врага
    bool isDead() const; // Проверяет, мертв ли враг

    // Геттеры
    sf::FloatRect getBoundingBox() const;
    sf::Vector2f getPosition() const;
    
    void loadTextureAndCreateCollisionMask();
    void markForNoDestruction();
    void markForDestruction();
    
    bool shouldDestroyBody = false;
    bool isMarkedForDestruction() const;

    // Метод для получения тела Box2D
    b2Body* getBody() const {
        return body;
    }
    
    // Метод для установки тела Box2D
     void setBody(b2Body* newBody) {
        body = newBody;
    }
    
    

    
private:

    
    // Методы для внутреннего использования
    void updateAnimation(float deltaTime);
    
    float timer; // Таймер для разворота
    float changeDirectionTime; // Время до разворота
    
    // Физический мир и тело
    b2World& world;
    b2Body* body;

    // Текстуры и спрайты
    const sf::Texture& texture;
    sf::Sprite sprite;

    // Анимации
    std::vector<sf::IntRect> framesRun; // Кадры для состояния покоя
    //std::vector<sf::IntRect> framesDeath; // Кадры для состояния смерти
    sf::IntRect currentFrame;
    float animationTimer;
    float animationSpeed;

    // Состояния
    bool isDeadFlag =false;
    bool facingRight;

    // Параметры коллизии
    std::vector<sf::Vector2f> collisionPixels;

};

#endif // ENEMY_H