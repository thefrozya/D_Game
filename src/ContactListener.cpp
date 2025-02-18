#include "ContactListener.h"
#include "Constants.h"
#include <iostream>
#include "Coin.h"  // Подключаем класс Coin
#include "Enemy.h" // Подключаем класс Enemy

// Реализация метода IsPlayerPlatformContact
bool ContactListener::IsPlayerPlatformContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA);
}

// Реализация метода IsPlayerMashroomContact
bool ContactListener::IsPlayerMashroomContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == Finish_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == Finish_USER_DATA);
}

// Реализация метода IsPlayerDeathContact
bool ContactListener::IsPlayerDeathContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == DEATH_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == DEATH_USER_DATA);
}

// Реализация метода IsPlayerEnemyContact
bool ContactListener::IsPlayerEnemyContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == ENEMY_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == ENEMY_USER_DATA);
}


// Обработка начала контакта
void ContactListener::BeginContact(b2Contact* contact) {
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверка контакта с финишной платформой (грибом)
    if (IsPlayerMashroomContact(bodyAUserData, bodyBUserData)) {
        isGameOver = true; // Завершение игры
        std::cout << "Player reached the finish!" << std::endl;
    }

    // Проверка контакта с платформой
    if (IsPlayerPlatformContact(bodyAUserData, bodyBUserData)) {
        onGround = true; // Устанавливаем флаг "на земле"
        //std::cout << "Player is on ground: true" << std::endl;
    }

    // Проверка контакта с триггером смерти
    if (IsPlayerDeathContact(bodyAUserData, bodyBUserData)) {
        std::cout << "Player touched the DEATH trigger!" << std::endl;
        if (player && !player->isDead()) { // Проверяем, жив ли игрок
            player->bounce();
            player->takeDamage(50); // Игрок получает урон
        }
    }

    // Проверка контакта с монетой
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == COIN_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == COIN_USER_DATA)) {
        Player* playerPtr = reinterpret_cast<Player*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        Coin* coin = reinterpret_cast<Coin*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

        if (bodyAUserData == COIN_USER_DATA) {
            coin = reinterpret_cast<Coin*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        }

        if (coin && !coin->isCollected()) {
            coin->collect(); // Собираем монету
            std::cout << "Coin collected!" << std::endl;
        }
    }
    if (bodyAUserData == ENEMY_USER_DATA || bodyBUserData == ENEMY_USER_DATA) {
        std::cout << "Enemy collision detected!" << std::endl;
    }
    /*if ((bodyAUserData == ENEMY_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
        (bodyBUserData == ENEMY_USER_DATA && bodyAUserData == PLATFORM_USER_DATA)) {
        std::cout << "Enemy is on ground." << std::endl;
    }*/

    // Проверка контакта с врагом
    if (IsPlayerEnemyContact(bodyAUserData, bodyBUserData)) {
        Player* playerPtr = nullptr;
        Enemy* enemy = nullptr;
        
        if (bodyAUserData == PLAYER_USER_DATA) {
            playerPtr = reinterpret_cast<Player*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
            enemy = reinterpret_cast<Enemy*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
        } else {
            playerPtr = reinterpret_cast<Player*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
            enemy = reinterpret_cast<Enemy*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        }
        
        if (!playerPtr || !enemy) {
            std::cerr << "Invalid contact data!" << std::endl;
            return;
        }

       /*if (playerPtr && enemy) {
            b2WorldManifold manifold;
            contact->GetWorldManifold(&manifold);
            std::cout << "Contact normal: (" << manifold.normal.x << ", " << manifold.normal.y << ")" << std::endl;
            // Если игрок касается врага сверху, уничтожаем врага
            if (manifold.normal.y < 0.7f) {
                player->bounce(); // Игрок подпрыгивает
                enemy->kill(); // Убиваем врага
                std::cout << "Player killed an enemy!" << std::endl;
            } else {
                //player->bounce(); // Игрок подпрыгивает
                player->takeDamage(20); // Игрок получает урон
                std::cout << "Player took damage from enemy!" << std::endl;
            }
        }*/
    }

    // Игнорирование столкновения со стенами
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == WALL_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == WALL_USER_DATA)) {
        contact->SetEnabled(false);
        std::cout << "Player collided with wall, collision ignored." << std::endl;
    }
}

// Обработка окончания контакта
void ContactListener::EndContact(b2Contact* contact) {
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверка окончания контакта с платформой
    if (IsPlayerPlatformContact(bodyAUserData, bodyBUserData)) {
        // Проверяем, остались ли другие активные контакты
        bool hasOtherContacts = false;
        b2ContactEdge* contactEdge = player->getBody()->GetContactList();
        while (contactEdge) {
            b2Contact* currentContact = contactEdge->contact;
            if (currentContact->IsTouching()) {
                uintptr_t otherBodyAUserData = currentContact->GetFixtureA()->GetBody()->GetUserData().pointer;
                uintptr_t otherBodyBUserData = currentContact->GetFixtureB()->GetBody()->GetUserData().pointer;
                if (IsPlayerPlatformContact(otherBodyAUserData, otherBodyBUserData)) {
                    hasOtherContacts = true;
                    break;
                }
            }
            contactEdge = contactEdge->next;
        }
        onGround = hasOtherContacts; // Обновляем флаг "на земле"
        //std::cout << "Player is on ground: " << (onGround ? "true" : "false") << std::endl;
    }
}