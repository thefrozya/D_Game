// Menu.h

#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu(float width, float height);
    ~Menu();

    void draw(sf::RenderWindow& window);
    void moveUp();
    void moveDown();
    int getPressedItem() const { return selectedItemIndex; }

private:
    int selectedItemIndex;
    sf::Font font;
    sf::Text menuItems[3]; // Массив пунктов меню
};

#endif // MENU_H