#include "ContactListener.h"
#include "Constants.h"
#include <iostream>

void ContactListener::BeginContact(b2Contact* contact) {
    // Получаем пользовательские данные для обоих тел
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверяем, что одно из тел — игрок, а другое — гриб
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == MASHROOM_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == MASHROOM_USER_DATA)) {
        std::cout << "Player touched the mashroom! Ending the game..." << std::endl;
        isGameOver = true; // Флаг завершения игры
    }

    // Проверяем, что игрок касается платформы
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA)) {
        isJumping = false; // Игрок касается платформы
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    // Получаем пользовательские данные для обоих тел
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверяем, что игрок больше не касается платформы
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA)) {
        isJumping = true; // Игрок больше не касается платформы
    }
}