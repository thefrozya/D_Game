#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>
#include "ObjectBase.h" 
#include "Player.h"
#include "Enemy.h"
#include "Coin.h"
#include "MovingPlatform.h"
#include "Staircase.h"
#include "Interactable.h"
#include <memory>


class ContactListener : public b2ContactListener {
public:
    // Конструктор
    ContactListener(Player* player = nullptr) : player(player), isGameOver(false), onGround(false),isGameWon(false) {}

    // Метод для обновления указателя на игрока
    void setPlayer(Player* newPlayer) {
        player = newPlayer;
    }

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

    void cleanupDestroyedBodies(b2World& world);
    static std::vector<b2Body*> bodiesToDestroy;
    void clearDestroyedBodies() { bodiesToDestroy.clear(); } // метод
    void setOnGround(bool value);
    bool isOnGround() const { return onGround; } // Метод для получения состояния "на земле"

    // Переменные для состояния игры
    bool isGameOver; // Состояние завершения игры

    bool isInLava() const { return inLava; } // метод для проверки состояния
    bool isOnClouds() const { return onClouds; }

    int getCoinScore() const { return coinScore; } // Счёт за монеты
    int getEnemyScore() const { return enemyScore; } // Счёт за врагов
    void resetScores() { coinScore = 0; enemyScore = 0; } // Сброс счёта
    void addCoinScore(int value) { coinScore += value; } 

    void setMovingPlatforms(std::vector<std::unique_ptr<MovingPlatform>>* platforms) { movingPlatforms = platforms; }

    void setStaircases(std::vector<Staircase>& s) { staircases = &s; }

    void setDoors(std::vector<Door>* doors) { this->doors = doors; }
    // Поля для телепортации
    bool shouldTeleport = false;
    b2Vec2 teleportPosition;
    bool isGameWon = false; // Флаг для победы
    

private:
    Player* player;
    bool onGround; // Флаг "на земле"

    int coinScore;
    int enemyScore;
    int platformContactCount;
    std::vector<std::unique_ptr<MovingPlatform>>* movingPlatforms = nullptr;

    std::vector<Staircase>* staircases;
    std::vector<Door>* doors;

    bool inLava = false; 
    bool onClouds = false;
   
    // Объявление методов проверки контактов
    bool IsPlayerPlatformContact(uintptr_t a, uintptr_t b);
    bool IsPlayerFinishContact(uintptr_t a, uintptr_t b);
    bool IsPlayerDeathContact(uintptr_t a, uintptr_t b);
    bool IsPlayerTrapContact(uintptr_t a, uintptr_t b);
    bool IsPlayerWallContact(uintptr_t a, uintptr_t b);
    bool IsPlayerCoinContact(uintptr_t a, uintptr_t b); 
    bool IsPlayerEnemyContact(uintptr_t a, uintptr_t b); 
    bool IsMovingPlatformContact(uintptr_t a, uintptr_t b);
    bool IsMovingPlatformStaticContact(uintptr_t a, uintptr_t b); 
    bool IsPlayer(uintptr_t userData) const;
    bool IsPlayerStaircaseContact(uintptr_t a, uintptr_t b);
    Player* GetPlayerFromContact(uintptr_t a, uintptr_t b);
    Enemy* GetEnemyFromContact(uintptr_t a, uintptr_t b);
    Coin* GetCoinFromContact(uintptr_t a, uintptr_t b);

    void HandlePlayerContact(b2Fixture* fixture, bool begin);
};

#endif // CONTACTLISTENER_H