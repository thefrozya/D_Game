#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>

class ContactListener; // Предварительное объявление

class Player {
public:
    // Конструкторы
    Player(b2World& world, float x, float y, ContactListener* contactListener, const sf::Texture& runTexture, const sf::Texture& jumpTexture, const sf::Texture& deathTexture);
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
    const float BOUNCE_FORCE = 5.0f; // Пример силы отскока
private:
    // Физика Box2D
    b2World& world; // Ссылка на мир Box2D
    b2Body* body;

    float velocity = 0.0f;
   

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

    // Указатель на обработчик контактов
    ContactListener* contactListener;

    // Точка спавна игрока
    sf::Vector2f spawnPoint;

    bool shouldDestroyBody; // Флаг для управления удалением
};

#endif // PLAYER_H