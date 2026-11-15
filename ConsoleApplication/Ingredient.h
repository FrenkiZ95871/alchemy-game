#pragma once
#include <string>

class Ingredient {
public:
    std::string name;
    int amount;

    Ingredient() = default;
    Ingredient(std::string n, int a) : name(n), amount(a) {}

    bool hasEnough(int needed) const;
    void consume(int count);
};
