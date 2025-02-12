#include "DebugUtils.h"
#include "Constants.h"


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

                // Определяем цвет в зависимости от типа тела
                if (body->GetType() == b2_dynamicBody) {
                    setupPolygon(polygon, sf::Color::Green);
                } else if (body->GetUserData().pointer == MASHROOM_USER_DATA) {
                    setupPolygon(polygon, sf::Color::Blue);
                } else {
                    setupPolygon(polygon, sf::Color::Red);
                }

                window.draw(polygon);
            }
        }
    }
}