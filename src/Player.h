#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <SFML/Audio.hpp>
#include "ObjectBase.h"
#include <iostream>
#include "Staircase.h"
#include <set>

class ContactListener; // Предварительное объявление

class Player : public ObjectBase {
public: 

ObjectType getType() const override {
  return ObjectType::Player;
}
    Player(); // Добавляем конструктор по умолчанию
    // Конструкторы
    Player(b2World& world, float x, float y, ContactListener* contactListener, 
      const sf::Texture& runTexture, const sf::Texture& jumpTexture, const sf::Texture& deathTexture);
    Player(Player&& other) noexcept; // Конструктор перемещения
    ~Player(); // Деструктор для удаления физического тела

    // Оператор присваивания перемещения
    Player& operator=(Player&& other) noexcept;

    // Методы для обновления, обработки ввода и отрисовки
    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBoundingBox() const; // Добавляем метод
    
    sf::Vector2f getPosition() const;// Метод для получения позиции игрока

    // Метод для проверки пиксельной коллизии
    bool checkPixelCollision(const std::vector<sf::Vector2f>& otherPixels, const sf::Vector2f& otherPosition);

    // Методы для управления состоянием прыжка
    void setIsJumping(bool value);
    bool getIsJumping() const;

    bool isOnStaircase() const { return onStaircase; }
    void setOnStaircase(bool on) { onStaircase = on; }

    b2Body* getBody() const {
      if (!body) {
          std::cerr << "Warning: Attempt to get null body in Player at: " << this << std::endl;
      }
      return body;
  }

    void destroyBody(b2World& world);
    void setBody(b2Body* b) { body = b; }
    
    bool isInLava() const { return inLava; } 
    void setInLava(bool value) { inLava = value; } 

    void loadTextureAndCreateCollisionMask();
    void updateAnimation(float deltaTime); // Новый метод для обновления анимации
    

    void takeDamage(int damage, b2Vec2 damageSourcePosition = b2Vec2(0.0f, 0.0f));
    bool isDead() const; // Метод для проверки, жив ли игрок
    void respawn(float x, float y); // Метод для возрождения
    int getHealth() const; // Метод для получения здоровья

    // Новые методы для проверки состояний
    bool isDying() const { return _isDying; }
    bool isWaitingForRespawn() const { return _isWaitingForRespawn; }

    void bounce();
    void markForNoDestruction();
    const float BOUNCE_FORCE = 5.0f;

    void playJumpSound(); // Метод для воспроизведения звука прыжка
    void stopPlayJumpSound();  
      
    ContactListener* contactListener;// Указатель на обработчик контактов


    void addKey(const std::string& keyType);
    bool hasKey(const std::string& keyType) const;
private:
    // Физика Box2D
    b2World& world; // Ссылка на мир Box2D
    b2Body* body;

    float velocity = 0.0f;

    bool onStaircase = false;

    bool inLava = false; 
   
    // Звук прыжка
    sf::SoundBuffer jumpSoundBuffer; // Буфер для хранения звука
    sf::Sound jumpSound;            // Объект для воспроизведения звука

    // Графика SFML
    sf::Texture runTexture; // Текстура для бега и статичных состояний
    sf::Texture jumpTexture; // Текстура для прыжка
    sf::Texture deathTexture; // Текстура для смерти
    sf::Sprite sprite;

    // Состояние игрока
    bool isRunning = false;
    bool isJumping = false;
    bool _isDying = false;
    bool _isWaitingForRespawn = false; // Флаг ожидания ввода для возрождения
    int health; // Здоровье игрока

    // Таймер задержки получения урона
    float damageCooldown; // Время задержки в секундах
    float damageTimer;    // Текущее значение таймера
    bool isInvulnerable;  // Флаг неуязвимости

    // Коллизии
    std::vector<sf::Vector2f> collisionPixels;

    // Анимация
    sf::IntRect currentFrame; // Текущий кадр анимации
    std::vector<sf::IntRect> framesRunning; // Кадры анимации бега
    std::vector<sf::IntRect> framesJumping; // Кадры анимации прыжка
    std::vector<sf::IntRect> framesDeath; // Кадры анимации смерти
    float animationTimer = 0.0f; // Таймер для анимации
    float animationSpeed = 0.1f; // Скорость анимации (время между кадрами)
    bool facingRight = true; // Направление движения игрока



    // Точка спавна игрока
    sf::Vector2f spawnPoint;

    bool shouldDestroyBody= true; // Флаг для управления удалением

    std::set<std::string> collectedKeys;
};

#endif // PLAYER_H