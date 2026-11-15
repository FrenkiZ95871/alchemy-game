#include <iostream>
#include <string>
#include <windows.h>
#include "AlchemySystem.h"

// Вспомогательная функция: печатает текст нужным цветом
void printColored(const std::string& text, int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << text;
    SetConsoleTextAttribute(hConsole, 7); // возвращаем обычный серый
}

int main() {
#ifdef _WIN32
    system("chcp 1251 > nul"); // консоль и файлы в одной кодировке
#endif

    AlchemySystem alchemy;
    alchemy.isCrit = false;

    // Пробуем загрузить сохранение
    if (alchemy.loadGame("save.txt")) {
        printColored("Прогресс загружен из файла save.txt!\n", 11);
    }
    else {
        // Сохранения нет — начинаем с нуля
        alchemy.addIngredient(Ingredient("Трава А", 5));
        alchemy.addIngredient(Ingredient("Минерал Б", 3));
        alchemy.addIngredient(Ingredient("Корень В", 10));
        std::cout << "Новая игра: инвентарь заполнен.\n";
    }

    // Рецепты создаются всегда — это часть мира, а не прогресса
    Recipe healingPotion;
    healingPotion.potionName = "Зелье лечения";
    healingPotion.price = 15;
    healingPotion.bonusPrice = 30;
    healingPotion.ingredients.push_back({ "Трава А", 2 });
    healingPotion.ingredients.push_back({ "Минерал Б", 1 });
    alchemy.addRecipe(healingPotion);

    Recipe strengthPotion;
    strengthPotion.potionName = "Зелье силы";
    strengthPotion.price = 25;
    strengthPotion.bonusPrice = 50;
    strengthPotion.ingredients.push_back({ "Корень В", 4 });
    strengthPotion.ingredients.push_back({ "Минерал Б", 2 });
    alchemy.addRecipe(strengthPotion);

    // Игровой цикл
    bool running = true;
    while (running) {
        std::cout << "\n===== АЛХИМИЧЕСКАЯ ЛАБОРАТОРИЯ =====\n";
        std::cout << "1 - Посмотреть инвентарь\n";
        std::cout << "2 - Сварить зелье\n";
        std::cout << "3 - Показать рецепты\n";
        std::cout << "4 - Продать зелье\n";
        std::cout << "5 - Купить ингредиент\n";
        std::cout << "6 - Сохранить и выйти\n";
        std::cout << "Выбери действие: ";

        int choice = 0;
        std::cin >> choice;

        if (std::cin.fail()) {
            // Пользователь ввёл не число
            std::cin.clear();             // снимаем ошибку
            std::cin.ignore(10000, '\n'); // выбрасываем мусор из буфера
            printColored("Нужно ввести число от 1 до 6!\n", 12);
            continue;
        }

        if (choice == 1) {
            std::cout << "\n--- Текущий инвентарь ---\n";
            alchemy.showInventory();
            alchemy.showGold();
        }
        else if (choice == 2) {
            std::cout << "\n";
            alchemy.showRecipes();
            std::cout << "\nКакое зелье сварить?\n";
            std::cout << "1 - Зелье лечения\n";
            std::cout << "2 - Зелье силы\n";
            std::cout << "Выбери рецепт: ";

            int recipeChoice = 0;
            std::cin >> recipeChoice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                printColored("Нужно ввести число!\n", 12);
                continue;
            }

            std::string potionName;
            if (recipeChoice == 1) potionName = "Зелье лечения";
            else if (recipeChoice == 2) potionName = "Зелье силы";
            else {
                std::cout << "Такого рецепта нет.\n";
                continue;
            }

            std::cout << "Пытаемся сварить \"" << potionName << "\"...\n";
            if (alchemy.tryCraft(potionName)) {
                printColored("Успех! \"" + potionName + "\" создано.\n", 10);
                if (alchemy.isCrit) {
                    printColored("*** УДАРНОЕ ЗЕЛЬЕ! Оно стоит вдвое дороже! ***\n", 14);
                }
            }
            else {
                printColored("Неудача: не хватает ингредиентов.\n", 12);
            }
        }
        else if (choice == 3) {
            std::cout << "\n";
            alchemy.showRecipes();
        }
        else if (choice == 4) {
            std::cout << "\nЧто продать?\n";
            std::cout << "1 - Зелье лечения\n";
            std::cout << "2 - Зелье силы\n";
            std::cout << "Выбери: ";

            int sellChoice = 0;
            std::cin >> sellChoice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                printColored("Нужно ввести число!\n", 12);
                continue;
            }

            std::string name;
            if (sellChoice == 1) name = "Зелье лечения";
            else if (sellChoice == 2) name = "Зелье силы";
            else {
                std::cout << "Такого зелья нет.\n";
                continue;
            }

            if (alchemy.sellPotion(name)) {
                printColored("Продано! Монеты в кошельке.\n", 10);
            }
            else {
                printColored("У тебя нет такого зелья. Сначала свари!\n", 12);
            }
            alchemy.showGold();
        }
        else if (choice == 5) {
            std::cout << "\n";
            alchemy.showGold();
            std::cout << "Ассортимент лавки:\n";
            std::cout << "1 - Трава А, 1 шт - 5 монет\n";
            std::cout << "2 - Минерал Б, 1 шт - 8 монет\n";
            std::cout << "3 - Корень В, 1 шт - 4 монеты\n";
            std::cout << "Выбери: ";

            int buyChoice = 0;
            std::cin >> buyChoice;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                printColored("Нужно ввести число!\n", 12);
                continue;
            }

            if (buyChoice == 1) {
                if (alchemy.buyIngredient("Трава А", 1, 5))
                    printColored("Куплено: Трава А x1 за 5 монет.\n", 10);
                else
                    printColored("Не хватает монет!\n", 12);
            }
            else if (buyChoice == 2) {
                if (alchemy.buyIngredient("Минерал Б", 1, 8))
                    printColored("Куплено: Минерал Б x1 за 8 монет.\n", 10);
                else
                    printColored("Не хватает монет!\n", 12);
            }
            else if (buyChoice == 3) {
                if (alchemy.buyIngredient("Корень В", 1, 4))
                    printColored("Куплено: Корень В x1 за 4 монеты.\n", 10);
                else
                    printColored("Не хватает монет!\n", 12);
            }
            else std::cout << "Такого товара нет.\n";

            alchemy.showGold();
        }
        else if (choice == 6) {
            alchemy.saveGame("save.txt");
            printColored("Прогресс сохранён в файл save.txt.\n", 11);
            std::cout << "Лаборатория закрывается. До встречи!\n";
            running = false;
        }
        else {
            std::cout << "Нет такого пункта меню.\n";
        }
    }

    return 0;
}
