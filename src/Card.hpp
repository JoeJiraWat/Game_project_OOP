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
class Boom: public card{
    private:
        Boom(std::string t);
    public://รอเพื่อนเขียน game manage
        void Onuse();
};
class Default: public card{
    private:
        Default(std::string t);
    public:
        void Onuse();
};
class Skip: public card{
    private:
        Skip(std::string t);
    public:
        void Onuse();
}
class Favor: public card{
    private:
        Favor(std::string t);
    public:
        void Onuse();
};
class See_the_future: public card{
    private:
        See_the_future(std::string t);
    public:
        void Onuse();
};
class Shuffle: public card{
    private:
        Shuffle(std::string t);
    public:
        void Onuse();
};
class Nope: public card{
    private:
        Nope(std::string t);
    public:
        void Onuse();
};
class Draw_from_the_bottom: public card{
    private:
        Draw_from_the_bottom(std::string t);
    public:
        void Onuse();
};
class Cat_card: public card{
    private:
        int quantity;
        enum cat_card_type{Tacocat,Hairy_Potato_Cat,Beard_cat,Rainbow_Ralphing_Cat,Cattermelon} type;
    public:
        void Onuse();
};