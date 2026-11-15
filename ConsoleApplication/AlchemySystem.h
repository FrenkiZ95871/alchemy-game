#pragma once
#include "Ingredient.h"
#include <vector>
#include <string>
#include <utility>

struct Recipe {
    std::string potionName;
    std::vector<std::pair<std::string, int>> ingredients;
    int price;      // цена обычного зелья
    int bonusPrice; // цена ударного зелья
};

class AlchemySystem {
private:
    std::vector<Ingredient> inventory;
    std::vector<Recipe> recipes;
    std::vector<std::string> craftedPotions;
    int gold = 20; // кошелёк алхимика

    bool rollCrit() const; // бросок кубика на ударное зелье

public:
    bool isCrit; // флаг: последнее сваренное зелье было ударным

    void addIngredient(const Ingredient& ing);
    void addRecipe(const Recipe& r);
    bool tryCraft(const std::string& potionName);
    void showInventory() const;
    void showRecipes() const;
    bool sellPotion(const std::string& potionName);
    bool buyIngredient(const std::string& name, int amount, int cost);
    void showGold() const;
    void saveGame(const std::string& filename) const;
    bool loadGame(const std::string& filename);
};
