#pragma once
#include <string>

struct Card_data{
    std::string name;
    int quantity;
    enum card_type{Boom,Default,Skip,Favor,See_the_future
                    ,Shuffle,Nope,Draw_from_the_bottom,Cat_card} type;
    std::string description;
    enum Effect{KaBoom,Defuse_a_boom,Skip_turn,Favor,see_Card,Shuffle_deck,Cancel_effect,Draw_from_the_Bottom,Normal_Card} effect;                
};
class card{
    private:
        std::string type;
    public:
        card(std::string t);
        std::string getType();
};
class skip: public card{
    public:
        skip(std::string t);
};