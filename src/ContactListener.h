#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>
#include "Player.h"

class ContactListener : public b2ContactListener {
public:
    // Конструктор
    ContactListener(Player* player = nullptr) : player(player), isGameOver(false), onGround(false) {}

    // Метод для обновления указателя на игрока
    void setPlayer(Player* newPlayer) {
        player = newPlayer;
    }

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

    
    bool isOnGround() const { return onGround; } // Метод для получения состояния "на земле"

    // Переменные для состояния игры
    bool isGameOver; // Состояние завершения игры

private:
    Player* player;
    bool onGround; // Флаг "на земле"

    // Объявление методов проверки контактов
    bool IsPlayerPlatformContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData);
    bool IsPlayerMashroomContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData);
    bool IsPlayerDeathContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData);
};

#endif // CONTACTLISTENER_H