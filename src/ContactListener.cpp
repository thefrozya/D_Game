#include "ContactListener.h"
#include "ObjectBase.h"
#include "Constants.h"
#include <iostream>
#include "Coin.h"
#include "Enemy.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Staircase.h"
#include "Interactable.h"
#include <cassert>

std::vector<b2Body*> ContactListener::bodiesToDestroy;

bool ContactListener::IsPlayerPlatformContact(uintptr_t a, uintptr_t b) {
    return (IsPlayer(a) && b == PLATFORM_USER_DATA) || (IsPlayer(b) && a == PLATFORM_USER_DATA);
}

bool ContactListener::IsPlayerFinishContact(uintptr_t a, uintptr_t b) {
    return (IsPlayer(a) && b == FINISH_USER_DATA) || (IsPlayer(b) && a == FINISH_USER_DATA);
}

bool ContactListener::IsPlayerDeathContact(uintptr_t a, uintptr_t b) {
    return (IsPlayer(a) && b == DEATH_USER_DATA) || (IsPlayer(b) && a == DEATH_USER_DATA);
}

bool ContactListener::IsPlayerTrapContact(uintptr_t a, uintptr_t b) {
    return (IsPlayer(a) && b == TRAP_USER_DATA) || (IsPlayer(b) && a == TRAP_USER_DATA);
}

bool ContactListener::IsPlayerWallContact(uintptr_t a, uintptr_t b) {
    return (IsPlayer(a) && b == WALL_USER_DATA) || (IsPlayer(b) && a == WALL_USER_DATA);
}

bool ContactListener::IsMovingPlatformContact(uintptr_t a, uintptr_t b) {
    return (a == MOVE_PLATFORM_USER_DATA && b == MOVE_PLATFORM_USER_DATA);
}

bool ContactListener::IsMovingPlatformStaticContact(uintptr_t a, uintptr_t b) {
    return (a == MOVE_PLATFORM_USER_DATA && b == PLATFORM_USER_DATA) ||
           (a == PLATFORM_USER_DATA && b == MOVE_PLATFORM_USER_DATA);
}

bool ContactListener::IsPlayerStaircaseContact(uintptr_t a, uintptr_t b) {
    bool result = (IsPlayer(a) && b == STAIRCASE_USER_DATA) || (IsPlayer(b) && a == STAIRCASE_USER_DATA);
    //std::cout << "IsPlayerStaircaseContact: a=" << a << ", b=" << b << ", result=" << (result ? "true" : "false") << std::endl;
    return result;
}

bool ContactListener::IsPlayerCoinContact(uintptr_t a, uintptr_t b) {
    if (IsPlayer(a) && b > 20) {
        ObjectBase* objB = reinterpret_cast<ObjectBase*>(b);
        return objB && objB->getType() == ObjectType::Coin;
    }
    if (IsPlayer(b) && a > 20) {
        ObjectBase* objA = reinterpret_cast<ObjectBase*>(a);
        return objA && objA->getType() == ObjectType::Coin;
    }
    return false;
}

bool ContactListener::IsPlayerEnemyContact(uintptr_t a, uintptr_t b) {
    if (IsPlayer(a) && b > 20) {
        ObjectBase* objB = reinterpret_cast<ObjectBase*>(b);
        return objB && objB->getType() == ObjectType::Enemy;
    }
    if (IsPlayer(b) && a > 20) {
        ObjectBase* objA = reinterpret_cast<ObjectBase*>(a);
        return objA && objA->getType() == ObjectType::Enemy;
    }
    return false;
}

bool ContactListener::IsPlayer(uintptr_t userData) const {
    if (userData == 0 || userData == PLATFORM_USER_DATA || userData == WALL_USER_DATA || 
        userData == DEATH_USER_DATA || userData == FINISH_USER_DATA || 
        userData == TRAP_USER_DATA || userData == STAIRCASE_USER_DATA || userData == LAVA_USER_DATA || userData == CLOUDS_USER_DATA) {
        return false;
    }
    if (!player) {
        std::cerr << "Error: Player pointer is null in ContactListener!" << std::endl;
        return false;
    }
    bool isPlayer = userData == reinterpret_cast<uintptr_t>(player);
    /*std::cout << "IsPlayer check: userData=" << userData << ", player=" << reinterpret_cast<uintptr_t>(player) 
              << ", result=" << (isPlayer ? "true" : "false") << std::endl;*/
    return isPlayer;
}

Player* ContactListener::GetPlayerFromContact(uintptr_t a, uintptr_t b) {
    if (IsPlayer(a)) return reinterpret_cast<Player*>(a);
    if (IsPlayer(b)) return reinterpret_cast<Player*>(b);
    return nullptr;
}

Enemy* ContactListener::GetEnemyFromContact(uintptr_t a, uintptr_t b) {
    if (a > 20) {
        ObjectBase* objA = reinterpret_cast<ObjectBase*>(a);
        if (objA && objA->getType() == ObjectType::Enemy) return dynamic_cast<Enemy*>(objA);
    }
    if (b > 20) {
        ObjectBase* objB = reinterpret_cast<ObjectBase*>(b);
        if (objB && objB->getType() == ObjectType::Enemy) return dynamic_cast<Enemy*>(objB);
    }
    return nullptr;
}

Coin* ContactListener::GetCoinFromContact(uintptr_t a, uintptr_t b) {
    if (a > 20) {
        ObjectBase* objA = reinterpret_cast<ObjectBase*>(a);
        if (objA && objA->getType() == ObjectType::Coin) return dynamic_cast<Coin*>(objA);
    }
    if (b > 20) {
        ObjectBase* objB = reinterpret_cast<ObjectBase*>(b);
        if (objB && objB->getType() == ObjectType::Coin) return dynamic_cast<Coin*>(objB);
    }
    return nullptr;
}

void ContactListener::BeginContact(b2Contact* contact) {
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    uintptr_t userDataA = bodyA->GetUserData().pointer;
    uintptr_t userDataB = bodyB->GetUserData().pointer;

    /*std::cout << "Contact: A=" << userDataA << " (pos: " << bodyA->GetPosition().x << "," << bodyA->GetPosition().y << "), "
              << "B=" << userDataB << " (pos: " << bodyB->GetPosition().x << "," << bodyB->GetPosition().y << ")" << std::endl;*/

    // Проверка игрока
    if (IsPlayer(userDataA)) //std::cout << "A is Player" << std::endl;
    if (IsPlayer(userDataB)) //std::cout << "B is Player" << std::endl;

    if (userDataA == FINISH_USER_DATA && IsPlayer(userDataB)) {
        if (!isGameWon) {
            isGameWon = true;
            std::cout << "Player reached the finish at (" << bodyA->GetPosition().x << ", " << bodyA->GetPosition().y << ")" << std::endl;
        }
        return;
    }
    // Столкновение с финишем (bodyB — финиш, bodyA — игрок)
    else if (userDataB == FINISH_USER_DATA && IsPlayer(userDataA)) {
        if (!isGameWon) {
            isGameWon = true;
            std::cout << "Player reached the finish at (" << bodyB->GetPosition().x << ", " << bodyB->GetPosition().y << ")" << std::endl;
        }
        return;
    }

    if (IsMovingPlatformContact(userDataA, userDataB) && movingPlatforms) {
        for (auto& platform : *movingPlatforms) {
            if (platform->getBody() == bodyA || platform->getBody() == bodyB) {
                b2Vec2 currentVelocity = platform->getBody()->GetLinearVelocity();
                platform->getBody()->SetLinearVelocity(b2Vec2(-currentVelocity.x, 0));
                std::cout << "MovingPlatform collided with another platform, reversing direction!" << std::endl;
            }
        }
    }

    if (IsMovingPlatformStaticContact(userDataA, userDataB)) {
        b2Body* movingPlatformBody = (userDataA == MOVE_PLATFORM_USER_DATA) ? bodyA : bodyB;
        b2Vec2 currentVelocity = movingPlatformBody->GetLinearVelocity();
        float newVelocityX = -currentVelocity.x;
        movingPlatformBody->SetLinearVelocity(b2Vec2(newVelocityX, 0));
        //std::cout << "MovingPlatform hit Static Platform, reversed velocity to: " << newVelocityX << std::endl;
    }

    if (IsPlayerPlatformContact(userDataA, userDataB) || 
        (IsPlayer(userDataA) && userDataB == MOVE_PLATFORM_USER_DATA) || 
        (IsPlayer(userDataB) && userDataA == MOVE_PLATFORM_USER_DATA)) {
        platformContactCount++;
        if (!onGround) {
            //std::cout << "Player contacted platform, contact count: " << platformContactCount << ", setting onGround to true" << std::endl;
            setOnGround(true);
        }
    }

    if (IsPlayerFinishContact(userDataA, userDataB)) {
        if (!isGameWon) {
            isGameWon = true;
            //std::cout << "Player reached the finish!" << std::endl;
        }
        return;
    }

    if (IsPlayerDeathContact(userDataA, userDataB)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        if (player && !player->isDead()) {
            player->bounce();
            player->takeDamage(50, b2Vec2_zero);
            //std::cout << "Player touched the DEATH trigger!" << std::endl;
        }
        return;
    }

    if (IsPlayerTrapContact(userDataA, userDataB)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        if (player && !player->isDead()) {
            player->bounce();
            player->takeDamage(10, b2Vec2_zero);
            //std::cout << "Player touched the TRAP trigger!" << std::endl;
        }
        return;
    }

    if (IsPlayerCoinContact(userDataA, userDataB)) {
        Coin* coin = GetCoinFromContact(userDataA, userDataB);
        if (coin) {
            /*std::cout << "Coin detected at pos: (" << coin->getBody()->GetPosition().x 
                      << ", " << coin->getBody()->GetPosition().y << ")" << std::endl;*/
            if (!coin->isCollected()) {
                coin->collect(10);
                addCoinScore(10);
                std::cout << "Coin collected, total coinScore: " << coinScore << std::endl;
            } else {
                std::cout << "Coin already collected" << std::endl;
            }
        } else {
            std::cout << "Coin pointer is null!" << std::endl;
        }
    }

    if (IsPlayerEnemyContact(userDataA, userDataB)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        Enemy* enemy = GetEnemyFromContact(userDataA, userDataB);

        if (!player || !enemy || player->isDead()) {
            return;
        }

        b2WorldManifold manifold;
        contact->GetWorldManifold(&manifold);

        if (manifold.normal.y < -0.5f) {
            player->bounce();
            enemy->kill();
            enemyScore += 50;
            //std::cout << "Enemy killed, score increased by 50, total enemyScore: " << enemyScore << std::endl;
            bodiesToDestroy.push_back(enemy->getBody());
            //std::cout << "Player killed an enemy!" << std::endl;
        } else {
            if (player) {
                b2Vec2 enemyPos = enemy->getBody()->GetPosition();
                player->takeDamage(10, enemyPos);
            }
        }
        return;
    }

    if (IsPlayerWallContact(userDataA, userDataB)) {
        contact->SetEnabled(false);
        //std::cout << "Player collided with wall, collision ignored." << std::endl;
        return;
    }

    if (IsPlayerStaircaseContact(userDataA, userDataB)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        b2Body* staircaseBody = (userDataA == STAIRCASE_USER_DATA) ? bodyA : bodyB;
        if (player && staircases) {
            std::cout << "Staircases count: " << staircases->size() << std::endl;
            for (auto& staircase : *staircases) {
                std::cout << "Checking staircase at (" << staircase.getBody()->GetPosition().x << ", " 
                          << staircase.getBody()->GetPosition().y << ")" << std::endl;
                if (staircase.getBody() == staircaseBody) {
                    player->setOnStaircase(true);
                    staircase.setPlayerOnStaircase(true);
                    std::cout << "Player on staircase at (" << staircaseBody->GetPosition().x << ", " 
                              << staircaseBody->GetPosition().y << ")" << std::endl;
                    break;
                }
            }
        }
        else {
            std::cout << "Player or staircases pointer is null!" << std::endl;
        }
    
    }

    // Телепортация через двери
    if (userDataA == DOOR_USER_DATA && IsPlayer(userDataB)) {
        // Находим дверь среди всех дверей
        Door* door = nullptr;
        for (auto& d : *doors) {
            if (d.getBody() == bodyA) {
                door = &d;
                break;
            }
        }
        if (door && door->isActive()) {
            Door* linkedDoor = door->getLinkedDoor();
            if (linkedDoor) {
                shouldTeleport = true;
                teleportPosition = linkedDoor->getBody()->GetPosition();
                std::cout << "Teleport scheduled to (" << teleportPosition.x << ", " << teleportPosition.y << ")" << std::endl;
            }
        }
    }
    // Столкновение с дверью (bodyB — дверь, bodyA — игрок)
    else if (userDataB == DOOR_USER_DATA && IsPlayer(userDataA)) {
        // Находим дверь среди всех дверей
        Door* door = nullptr;
        for (auto& d : *doors) {
            if (d.getBody() == bodyB) {
                door = &d;
                break;
            }
        }
        if (door && door->isActive()) {
            Door* linkedDoor = door->getLinkedDoor();
            if (linkedDoor) {
                shouldTeleport = true;
                teleportPosition = linkedDoor->getBody()->GetPosition();
                std::cout << "Teleport scheduled to (" << teleportPosition.x << ", " << teleportPosition.y << ")" << std::endl;
            }
        }
    }

    if ((IsPlayer(userDataA) && userDataB == LAVA_USER_DATA) || (IsPlayer(userDataB) && userDataA == LAVA_USER_DATA)) { // Новая проверка для лавы
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        if (player && !player->isDead()) {
            inLava = true; // Устанавливаем флаг "в лаве"
            setOnGround(true);
            std::cout << "Player entered lava!" << std::endl;
        }
        return;
    }

    if ((IsPlayer(userDataA) && userDataB == CLOUDS_USER_DATA) || (IsPlayer(userDataB) && userDataA == CLOUDS_USER_DATA)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        if (player || !player->isDead() || !player->isOnStaircase()) { // Проверяем, что игрок не на лестнице
            if (!onClouds) { // Отскок только при первом касании
                b2Vec2 velocity = player->getBody()->GetLinearVelocity();
                velocity.y = -7.0f; // Сила отскока вверх
                player->getBody()->SetLinearVelocity(velocity);
                onClouds = true; // Устанавливаем флаг
                std::cout << "Player bounced on cloud at (" << player->getBody()->GetPosition().x << ", " << player->getBody()->GetPosition().y << ")" << std::endl;
            }
        }
        return;
    }
}
void ContactListener::EndContact(b2Contact* contact) {
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();

    uintptr_t userDataA = bodyA->GetUserData().pointer;
    uintptr_t userDataB = bodyB->GetUserData().pointer;

    //std::cout << "End contact detected between objects with userData: " << userDataA << " and " << userDataB << std::endl;

    if (IsPlayerStaircaseContact(userDataA, userDataB)) {
        Player* player = GetPlayerFromContact(userDataA, userDataB);
        b2Body* staircaseBody = (userDataA == STAIRCASE_USER_DATA) ? bodyA : bodyB;
        if (player && staircases) {
            for (auto& staircase : *staircases) {
                if (staircase.getBody() == staircaseBody) {
                    player->setOnStaircase(false);
                    staircase.setPlayerOnStaircase(false);
                    std::cout << "Player off staircase at (" << staircaseBody->GetPosition().x << ", " 
                              << staircaseBody->GetPosition().y << ")" << std::endl;
                    break;
                }
            }
        }

        if ((IsPlayer(userDataA) && userDataB == LAVA_USER_DATA) || (IsPlayer(userDataB) && userDataA == LAVA_USER_DATA)) {
            inLava = false; // Сбрасываем флаг "в лаве"
            std::cout << "Player exited lava!" << std::endl;
        }
    }

    if (IsPlayerPlatformContact(userDataA, userDataB) || 
        (IsPlayer(userDataA) && userDataB == MOVE_PLATFORM_USER_DATA) || 
        (IsPlayer(userDataB) && userDataA == MOVE_PLATFORM_USER_DATA)) {
        platformContactCount--;
        if (platformContactCount <= 0) {
            platformContactCount = 0;
            //std::cout << "Player left platform, contact count: " << platformContactCount << ", setting onGround to false" << std::endl;
            setOnGround(false);
        } else {
            //std::cout << "Player still on platform, contact count: " << platformContactCount << ", onGround remains true" << std::endl;
        }
    }

    if ((IsPlayer(userDataA) && userDataB == LAVA_USER_DATA) || (IsPlayer(userDataB) && userDataA == LAVA_USER_DATA)) {
        inLava = false; 
        setOnGround(false);
        std::cout << "Player exited lava!" << std::endl;
    }

    if ((IsPlayer(userDataA) && userDataB == CLOUDS_USER_DATA) || (IsPlayer(userDataB) && userDataA == CLOUDS_USER_DATA)) {
        onClouds = false; // Сбрасываем флаг при выходе из контакта
        std::cout << "Player left cloud!" << std::endl;
    }
}

void ContactListener::cleanupDestroyedBodies(b2World& world) {
    if (bodiesToDestroy.empty()) return;
    std::cout << "Cleaning up " << bodiesToDestroy.size() << " destroyed bodies" << std::endl;
    for (auto it = bodiesToDestroy.begin(); it != bodiesToDestroy.end();) {
        b2Body* body = *it;
        if (body && world.GetBodyCount() > 0) {
            std::cout << "Destroying body at: " << body << std::endl;
            world.DestroyBody(body);
            it = bodiesToDestroy.erase(it);
        } else {
            ++it;
        }
    }
    bodiesToDestroy.clear();
}

void ContactListener::setOnGround(bool value) {
    onGround = value;
    //std::cout << "onGround set to: " << (value ? "true" : "false") << std::endl;
}