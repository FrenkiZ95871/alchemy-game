#include "AlchemySystem.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

void AlchemySystem::addIngredient(const Ingredient& ing) {
    inventory.push_back(ing);
}

void AlchemySystem::addRecipe(const Recipe& r) {
    recipes.push_back(r);
}

bool AlchemySystem::rollCrit() const {
    // Инициализируем генератор случайных чисел один раз
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }
    // Шанс 20%: случайное число от 0 до 99 меньше 20
    return std::rand() % 100 < 20;
}

bool AlchemySystem::tryCraft(const std::string& potionName) {
    // 1. Ищем рецепт по имени
    for (const auto& r : recipes) {
        if (r.potionName == potionName) {
            // 2. Проверяем, что всех ингредиентов хватает
            for (const auto& req : r.ingredients) {
                bool found = false;
                for (const auto& invItem : inventory) {
                    if (invItem.name == req.first && invItem.hasEnough(req.second)) {
                        found = true;
                        break;
                    }
                }
                if (!found) return false; // чего-то нет или слишком мало
            }

            // 3. Все ингредиенты есть — тратим их
            for (const auto& req : r.ingredients) {
                for (auto& invItem : inventory) {
                    if (invItem.name == req.first) {
                        invItem.consume(req.second);
                    }
                }
            }

            // 4. Бросаем кубик: может, получится ударное зелье?
            isCrit = rollCrit();
            if (isCrit) {
                craftedPotions.push_back(potionName + " (ударное)");
            }
            else {
                craftedPotions.push_back(potionName);
            }
            return true;
        }
    }
    return false; // рецепт не найден
}

void AlchemySystem::showInventory() const {
    std::cout << "Инвентарь:\n";
    for (const auto& item : inventory) {
        std::cout << "- " << item.name << ": " << item.amount << "\n";
    }

    if (!craftedPotions.empty()) {
        std::cout << "\nСваренные зелья:\n";
        for (const auto& p : craftedPotions) {
            std::cout << "* " << p << "\n";
        }
    }
}

void AlchemySystem::showRecipes() const {
    std::cout << "Доступные рецепты:\n";
    for (const auto& r : recipes) {
        std::cout << "- " << r.potionName << " (нужно: ";
        for (size_t i = 0; i < r.ingredients.size(); ++i) {
            std::cout << r.ingredients[i].second << " x " << r.ingredients[i].first;
            if (i + 1 < r.ingredients.size()) std::cout << ", ";
        }
        std::cout << ")\n";
    }
}

bool AlchemySystem::sellPotion(const std::string& potionName) {
    for (size_t i = 0; i < craftedPotions.size(); ++i) {
        std::string current = craftedPotions[i];

        // Определяем, ударное ли зелье и его "чистое" имя
        bool crit = (current.find("(ударное)") != std::string::npos);
        std::string cleanName = current;
        size_t pos = cleanName.find(" (ударное)");
        if (pos != std::string::npos) cleanName = cleanName.substr(0, pos);

        if (cleanName == potionName) {
            for (const auto& r : recipes) {
                if (r.potionName == potionName) {
                    gold += crit ? r.bonusPrice : r.price;
                    craftedPotions.erase(craftedPotions.begin() + i);
                    return true;
                }
            }
        }
    }
    return false; // такого зелья нет в наличии
}

bool AlchemySystem::buyIngredient(const std::string& name, int amount, int cost) {
    if (gold >= cost) {
        gold -= cost;
        inventory.push_back(Ingredient(name, amount));
        return true;
    }
    return false; // не хватает монет — сообщение печатает main
}

void AlchemySystem::showGold() const {
    std::cout << "Монет: " << gold << "\n";
}

void AlchemySystem::saveGame(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) {
        std::cout << "Не удалось открыть файл для сохранения.\n";
        return;
    }

    out << "GOLD " << gold << "\n";
    out << "INVENTORY " << inventory.size() << "\n";
    // Разделитель | защищает названия с пробелами
    for (const auto& item : inventory) {
        out << item.name << "|" << item.amount << "\n";
    }
    out << "POTIONS " << craftedPotions.size() << "\n";
    for (const auto& p : craftedPotions) {
        out << p << "\n";
    }
}

bool AlchemySystem::loadGame(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false; // файла ещё нет — играем с нуля

    inventory.clear();
    craftedPotions.clear();

    std::string tag;
    int count = 0;

    in >> tag >> gold;              // GOLD
    in >> tag >> count;             // INVENTORY
    in.ignore(10000, '\n');         // добираем остаток строки
    for (int i = 0; i < count; ++i) {
        std::string line;
        std::getline(in, line);
        size_t pos = line.find('|');
        if (pos != std::string::npos) {
            std::string name = line.substr(0, pos);
            int amount = std::stoi(line.substr(pos + 1));
            inventory.push_back(Ingredient(name, amount));
        }
    }
    in >> tag >> count;             // POTIONS
    in.ignore(10000, '\n');
    for (int i = 0; i < count; ++i) {
        std::string p;
        std::getline(in, p);
        craftedPotions.push_back(p);
    }
    return true;
}
