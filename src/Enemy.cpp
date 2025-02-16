#include "Enemy.h"
#include "Constants.h"
#include <iostream>

// В классе Enemy добавьте переменные для границ движения
float leftBoundary;
float rightBoundary;

// Конструктор
Enemy::Enemy(b2World& world, float x, float y, const sf::Texture& enemyTexture)
    : world(world), texture(enemyTexture), isDeadFlag(false), facingRight(true), 
    animationTimer(0.0f), animationSpeed(0.1f),timer(0.0f), changeDirectionTime(3.0f){
    // Настройка физического тела
    // Настройка физического тела
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    body = world.CreateBody(&bodyDef);
    body->GetUserData().pointer = ENEMY_USER_DATA;
    body->SetFixedRotation(true);

    // Загрузка текстуры и создание маски коллизии
    loadTextureAndCreateCollisionMask();

    // Создание фикстуры
    const float frameWidth = 24.0f;
    const float frameHeight = 24.0f;
    const float ScaleCharacterX = frameWidth / SCALE;
    const float ScaleCharacterY = frameHeight / SCALE;

    b2PolygonShape shape;
    shape.SetAsBox(ScaleCharacterX / 3.2f, ScaleCharacterY / 2.6f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.5f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.1f;
    fixtureDef.userData.pointer = ENEMY_USER_DATA;

    body->CreateFixture(&fixtureDef);

    // Настройка спрайта
    sprite.setTexture(enemyTexture);
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    sprite.setPosition(x * SCALE, y * SCALE);

}

// Деструктор
Enemy::~Enemy() {
    if (!body) {
        world.DestroyBody(body);
        body = nullptr;
    }
}

// Метод для загрузки текстуры и создания маски коллизии
void Enemy::loadTextureAndCreateCollisionMask() {
    sf::Image image = texture.copyToImage();
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;
    // Создание маски коллизии
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            sf::Color pixel = image.getPixel(x, y);
            if (pixel.a > 0) {
                collisionPixels.push_back(sf::Vector2f(x - width / 2.0f, y - height / 2.0f));
            }
        }
    }
    const int FRAME_COUNT_RUN = 4; // Количество кадров бега
    int frameWidth = texture.getSize().x / FRAME_COUNT_RUN;
    int frameHeight = texture.getSize().y;
    std::cout << "Frame width: " << frameWidth << ", Frame height: " << frameHeight << std::endl;
    // Загрузка кадров бега
    for (int i = 0; i < FRAME_COUNT_RUN; ++i) {
        framesRun.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
    currentFrame = framesRun[0]; // Начинаем с первого кадра
    sprite.setTextureRect(currentFrame);
    std::cout << "Loaded " << framesRun.size() << " run frames and " 
          << std::endl;
}

// Метод для обновления состояния
void Enemy::update(float deltaTime) {
    b2Vec2 positionB2 = body->GetPosition();
    sf::Vector2f position(positionB2.x * SCALE, positionB2.y * SCALE);
    sprite.setPosition(position);

    float ENEMY_SPEED = 1.0f;

    // Обновляем таймер
    timer += deltaTime;

    // Проверяем, пора ли разворачиваться
    if (timer >= changeDirectionTime) {
        facingRight = !facingRight; // Разворачиваемся
        timer = 0.0f; // Сбрасываем таймер
    }

    // Простое движение влево-вправо
    b2Vec2 velocity = body->GetLinearVelocity();
    if (facingRight) {
        velocity.x = ENEMY_SPEED; // Движение вправо
    } else {
        velocity.x = -ENEMY_SPEED; // Движение влево
    }

    body->SetLinearVelocity(velocity);

    // Обновление спрайта
    if (facingRight) {
        sprite.setScale(1.0f, 1.0f);
    } else {
        sprite.setScale(-1.0f, 1.0f);
    }

    updateAnimation(deltaTime);
}

// Метод для обновления анимации
void Enemy::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= animationSpeed) {
        animationTimer = 0.0f;
        // Циклический переход между кадрами бега
        static int currentFrameIndex = 0;
        currentFrameIndex = (currentFrameIndex + 1) % framesRun.size();
        currentFrame = framesRun[currentFrameIndex];
        sprite.setTextureRect(currentFrame);
    }
    // Поворот спрайта в зависимости от направления движения
    if (facingRight) {
        sprite.setScale(1.0f, 1.0f);
    } else {
        sprite.setScale(-1.0f, 1.0f); // Используем отрицательное значение для горизонтального отражения
    }
    /*std::cout << "Current animation frame: (" 
    << currentFrame.left << ", " << currentFrame.top << ")" << std::endl;*/
}
// Метод для получения позиции
sf::Vector2f Enemy::getPosition() const {
    b2Vec2 position = body->GetPosition();  
    return sf::Vector2f(position.x, position.y);
    
}

// Метод для отрисовки 
void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);

}

// Метод для убийства врага
void Enemy::kill() {
    if (body) {
        world.DestroyBody(body); // Удаляем тело из мира Box2D
        body = nullptr; // Обнуляем указатель
    }
    isDeadFlag = true;
}

// Метод для проверки, мертв ли враг
bool Enemy::isDead() const {
    return isDeadFlag;
}

