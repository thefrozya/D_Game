#include "ContactListener.h"
#include "Constants.h"

void ContactListener::BeginContact(b2Contact* contact) {
    // Получаем пользовательские данные для обоих тел
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверяем, что одно из тел — игрок, а другое — платформа
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA)) {
        isJumping = false; // Игрок касается платформы
    }
}

void ContactListener::EndContact(b2Contact* contact) {
    // Получаем пользовательские данные для обоих тел
    uintptr_t bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData().pointer;
    uintptr_t bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData().pointer;

    // Проверяем, что одно из тел — игрок, а другое — платформа
    if ((bodyAUserData == PLAYER_USER_DATA && bodyBUserData == PLATFORM_USER_DATA) ||
        (bodyBUserData == PLAYER_USER_DATA && bodyAUserData == PLATFORM_USER_DATA)) {
        isJumping = true; // Игрок больше не касается платформы
    }
}