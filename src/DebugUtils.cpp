#include "DebugUtils.h"
#include "Constants.h"
#include "ContactListener.h"
#include <iostream>


// Вспомогательная функция для настройки полигона
void setupPolygon(sf::ConvexShape& polygon, const sf::Color& color, float outlineThickness = 2.0f) {
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineThickness(outlineThickness);
    polygon.setOutlineColor(color);
}

void debugDrawPhysics(sf::RenderWindow& window, b2World& world, float scale) {
    if (!DEBUG_DRAW_ENABLED) {
        return; // Если визуализация отключена, выходим из функции
    }

    for (b2Body* body = world.GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            if (fixture->GetType() == b2Shape::e_polygon) {
                b2PolygonShape* shape = (b2PolygonShape*)fixture->GetShape();
                int vertexCount = shape->m_count;
                sf::ConvexShape polygon(vertexCount);

                for (int i = 0; i < vertexCount; ++i) {
                    b2Vec2 vertex = body->GetWorldPoint(shape->m_vertices[i]);
                    polygon.setPoint(i, sf::Vector2f(vertex.x * scale, vertex.y * scale));
                }

                if (body->GetUserData().pointer == DEATH_USER_DATA) {
                    setupPolygon(polygon, sf::Color::Blue); // Синий для триггеров смерти
                } else if (body->GetUserData().pointer == PLATFORM_USER_DATA) {
                    setupPolygon(polygon, sf::Color::Black); // Чёрный для платформ
                } else if (body->GetUserData().pointer == COIN_USER_DATA) {
                    setupPolygon(polygon, sf::Color::Yellow); // Жёлтый для монет
                } else if (body->GetType() == b2_staticBody) {
                    setupPolygon(polygon, sf::Color::Green); // Зелёный для статических тел
                } else if (body->GetUserData().pointer == 0) {
                    setupPolygon(polygon, sf::Color::Magenta); // Магента для тел без UserData
                }else {
                    setupPolygon(polygon, sf::Color::Red); // Красный для остальных тел
                }
                

                window.draw(polygon);
            }
        }
    }
}