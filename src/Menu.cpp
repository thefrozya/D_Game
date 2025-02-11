// Menu.cpp

#include <iostream>
#include "Menu.h"

Menu::Menu(float width, float height) {
    // Загрузка шрифта
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Настройка пунктов меню
    std::string menuTexts[3] = {"Start Game", "Options", "Exit"};
    for (int i = 0; i < 3; ++i) {
        menuItems[i].setFont(font);
        menuItems[i].setString(menuTexts[i]);
        menuItems[i].setCharacterSize(40);
        menuItems[i].setPosition(width / 2.0f - 100, height / (3 + 1.0f) * (i + 1));
        menuItems[i].setFillColor(i == 0 ? sf::Color::Red : sf::Color::White); // Выделение первого пункта
    }

    selectedItemIndex = 0;
}

Menu::~Menu() {}

void Menu::draw(sf::RenderWindow& window) {
    // Отрисовка пунктов меню
    for (int i = 0; i < 3; ++i) {
        window.draw(menuItems[i]);
    }
}

void Menu::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex--;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

void Menu::moveDown() {
    if (selectedItemIndex < 2) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        selectedItemIndex++;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}