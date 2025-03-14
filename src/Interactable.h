#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Interactable {
public:
    Interactable(b2World& world, float x, float y, float width, float height, const std::string& type, bool isSensor);
    virtual ~Interactable() = default;
    virtual void update(float deltaTime) {}
    virtual void draw(sf::RenderWindow& window) = 0;
    b2Body* getBody() const { return body; }
    void destroyBody(b2World& world);
    sf::FloatRect getBoundingBox() const { return shape.getGlobalBounds(); }
    std::string getType() const { return type; }
    void setBody(b2Body* b) { body = b; }
    
   

protected:
    b2Body* body;
    sf::RectangleShape shape;
    std::string type;
};

class Key : public Interactable {
public:
    Key(b2World& world, float x, float y, const sf::Texture& texture, const std::string& keyType);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    bool isCollected() const { return collected; }
    void collect(int scoreValue);
    ~Key();
private:
    sf::Sprite sprite;
    bool collected;
    int frameCount;
    float frameTime;
    int currentFrame;
    float animationTimer;
};

class Door : public Interactable {
public:
    Door(b2World& world, float x, float y, float width, float height, const std::string& doorType ,const sf::Texture& texture);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    bool isActive() const { return isActivated; }
    void activate();
    bool isExit() const { return type.find('L') != std::string::npos; }
    Door* getLinkedDoor() const { return linkedDoor; }
    void setLinkedDoor(Door* door) { linkedDoor = door; }
    static void linkDoors(std::vector<Door>& doors);
    void startOpeningAnimation();
    ~Door();
private:
    bool isActivated;
    Door* linkedDoor;
    sf::Sprite sprite;           // Спрайт для отрисовки анимации
    bool isAnimating;            // Флаг анимации
    float animationTimer;        // Таймер анимации
    int currentFrame;            // Текущий кадр
    int frameCount;              // Количество кадров
    float frameTime;             // Время на кадр
    std::vector<sf::IntRect> frames; // Кадры анимации
};

#endif