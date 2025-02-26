#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

class Menu {
public:
    Menu(float width, float height);
    ~Menu();

    void draw(sf::RenderWindow& window);
    void moveUp();
    void moveDown();

    int getPressedItem() const { return selectedItemIndex; }

        
    void playBackgroundMusic(); // Запуск музыки
    void stopBackgroundMusic(); // Остановка музыки

private:
    int selectedItemIndex;
    sf::Font font;
    std::vector<sf::Text> menuItems; // Вектор пунктов меню

    // Фоновое изображение
    sf::Texture backgroundTexture;  // Добавляем текстуру фона
    sf::Sprite backgroundSprite;    // Добавляем спрайт фона

    // Фоновая музыка
    sf::Music backgroundMusic; // Добавляем объект для фоновой музыки
};

#endif // MENU_H