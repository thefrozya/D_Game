#ifndef OBJECTBASE_H
#define OBJECTBASE_H

#include <cstdint>

enum class ObjectType {
    Player,
    Enemy,
    Coin
};

class ObjectBase {
public:
    virtual ObjectType getType() const = 0; 
    virtual ~ObjectBase() = default;
};

#endif 