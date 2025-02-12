#include "Menu.h"
#include <iostream>

// Конструктор
Menu::Menu(float width, float height) : selectedItemIndex(0) {
    // Загрузка шрифта
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        throw std::runtime_error("Font loading failed");
    }

    // Константы для настройки меню
    const int fontSize = 40;
    const float xOffset = 100.0f;
    const std::string menuTexts[] = {"Start Game", "Options", "Exit"};

    // Настройка пунктов меню
    for (size_t i = 0; i < 3; ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(menuTexts[i]);
        text.setCharacterSize(fontSize);
        text.setPosition(width / 2.0f - xOffset, height / (3 + 1.0f) * (i + 1));
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White); // Выделение первого пункта
        menuItems.push_back(text); // Добавляем текст в вектор
    }
}

// Деструктор
Menu::~Menu() {}

// Отрисовка меню
void Menu::draw(sf::RenderWindow& window) {
    for (const auto& item : menuItems) {
        window.draw(item);
    }
}

// Перемещение вверх по меню
void Menu::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        --selectedItemIndex;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

// Перемещение вниз по меню
void Menu::moveDown() {
    if (selectedItemIndex < static_cast<int>(menuItems.size()) - 1) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::White);
        ++selectedItemIndex;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}