#include <iostream>
#include <string>

// Base class for Coffee
class Coffee {
public:
    virtual void prepare() = 0;
    virtual ~Coffee() {}
};

// Derived class for Espresso
class Espresso : public Coffee {
public:
    void prepare() override {
        std::cout << "Preparing an Espresso" << std::endl;
    }
};

// Derived class for Latte
class Latte : public Coffee {
public:
    void prepare() override {
        std::cout << "Preparing a Latte with milk" << std::endl;
    }
};

// Derived class for Cappuccino
class Cappuccino : public Coffee {
public:
    void prepare() override {
        std::cout << "Preparing a Cappuccino with foam" << std::endl;
    }
};

// CoffeeFactory class
class CoffeeFactory {
public:
    // Factory method to create Coffee
    static Coffee* createCoffee(const std::string& type) {
        if (type == "Espresso") {
            return new Espresso();
        } else if (type == "Latte") {
            return new Latte();
        } else if (type == "Cappuccino") {
            return new Cappuccino();
        } else {
            std::cout << "Unknown coffee type: " << type << std::endl;
            return nullptr;
        }
    }
};

int main() {
    // Create an Espresso
    Coffee* espresso = CoffeeFactory::createCoffee("Espresso");
    if (espresso) {
        espresso->prepare();
        delete espresso;
    }

    // Create a Latte
    Coffee* latte = CoffeeFactory::createCoffee("Latte");
    if (latte) {
        latte->prepare();
        delete latte;
    }

    // Create a Cappuccino
    Coffee* cappuccino = CoffeeFactory::createCoffee("Cappuccino");
    if (cappuccino) {
        cappuccino->prepare();
        delete cappuccino;
    }

    // Attempt to create an unknown coffee type
    Coffee* unknown = CoffeeFactory::createCoffee("Mocha");
    if (unknown) {
        unknown->prepare();
        delete unknown;
    }

    return 0;
}