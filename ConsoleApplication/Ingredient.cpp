#include "Ingredient.h"

bool Ingredient::hasEnough(int needed) const {
    return amount >= needed;
}

void Ingredient::consume(int count) {
    if (amount >= count) {
        amount -= count;
    }
}
