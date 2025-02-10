#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <Box2D/Box2D.h>

class ContactListener : public b2ContactListener {
    public:
        void BeginContact(b2Contact* contact) override;
        void EndContact(b2Contact* contact) override;
    
        bool isJumping = false; // Флаг для прыжка
        bool isGameOver = false; // Флаг завершения игры
    };

#endif
