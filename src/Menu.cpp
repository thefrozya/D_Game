#include "Menu.h"
#include <iostream>



// Конструктор
Menu::Menu(float width, float height) : selectedItemIndex(0) {
    // Загрузка шрифта
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        throw std::runtime_error("Font loading failed");
    }

   
    // Загрузка фонового изображения (JPEG)
    if (!backgroundTexture.loadFromFile("assets/background/BackGroundMenu.jpg")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        throw std::runtime_error("Background texture loading failed");
    }
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(width / backgroundTexture.getSize().x, height / backgroundTexture.getSize().y);

    // Загрузка фоновой музыки
    if (!backgroundMusic.openFromFile("assets/music/BackMusicMenu.ogg")) {
        std::cerr << "Failed to load background music!" << std::endl;
        throw std::runtime_error("Background music loading failed");
    }

    // Настройка параметров музыки
    backgroundMusic.setVolume(50); // Устанавливаем громкость (от 0 до 100)
    backgroundMusic.setLoop(true); // Зацикливаем музыку

   
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
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::Black); // Выделение первого пункта
        menuItems.push_back(text); // Добавляем текст в вектор
    }

    playBackgroundMusic();
}

// Деструктор
Menu::~Menu() {}

// Метод для запуска музыки
void Menu::playBackgroundMusic() {
    if (backgroundMusic.getStatus() != sf::SoundSource::Playing) { // Проверяем, играет ли музыка
        backgroundMusic.play(); // Запускаем воспроизведение
    }
}

// Метод для остановки музыки
void Menu::stopBackgroundMusic() {
    if (backgroundMusic.getStatus() == sf::SoundSource::Playing) { // Проверяем, играет ли музыка
        backgroundMusic.stop(); // Останавливаем воспроизведение
    }
}


// Отрисовка меню
void Menu::draw(sf::RenderWindow& window) {

        window.draw(backgroundSprite);
    for (const auto& item : menuItems) {
        window.draw(item);
    }
}

// Перемещение вверх по меню
void Menu::moveUp() {
    if (selectedItemIndex > 0) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::Black);
        --selectedItemIndex;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}

// Перемещение вниз по меню
void Menu::moveDown() {
    if (selectedItemIndex < static_cast<int>(menuItems.size()) - 1) {
        menuItems[selectedItemIndex].setFillColor(sf::Color::Black);
        ++selectedItemIndex;
        menuItems[selectedItemIndex].setFillColor(sf::Color::Red);
    }
}