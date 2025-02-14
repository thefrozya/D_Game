#include "ContactListener.h"
#include "Constants.h"
#include <iostream>
#include "Coin.h" // Подключаем класс Coin

// Реализация метода IsPlayerPlatformContact
bool ContactListener::IsPlayerPlatformContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA);
}

// Реализация метода IsPlayerMashroomContact
bool ContactListener::IsPlayerMashroomContact(uintptr_t bodyAUserData, uintptr_t bodyBUserData) {
    return (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == MASHROOM_USER_DATA) ||
           (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == MASHROOM_USER_DATA);
}

void ContactListener::BeginContact(b2Contact* contact) {
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    if (IsPlayerMashroomContact(bodyAUserData, bodyBUserData)) {
        isGameOver = true; // Завершение игры
    }

    if (IsPlayerPlatformContact(bodyAUserData, bodyBUserData)) {
        onGround = true; // Устанавливаем флаг "на земле"
        std::cout << "Player is on ground: true" << std::endl;
    }
    if (bodyAUserData == PLAYER_USER_DATA && bodyBUserData == COIN_USER_DATA) {
        Player* player = reinterpret_cast<Player*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        Coin* coin = reinterpret_cast<Coin*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
        coin->collect();
    } else if (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == COIN_USER_DATA) {
        Player* player = reinterpret_cast<Player*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
        Coin* coin = reinterpret_cast<Coin*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
        coin->collect();
    }
}


void ContactListener::EndContact(b2Contact* contact) {
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

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
        std::cout << "Player is on ground: " << (onGround ? "true" : "false") << std::endl;
    }
}