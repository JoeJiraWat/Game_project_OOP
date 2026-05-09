#pragma once
#include <string>
#include "Card.hpp"

class player{
    private:
        std::string name;
        bool isActive ;
        bool isturn ;
        bool isalive ;
        std::vector <Card> Mycard;
    public:
        void draw();
        void useCard();
};