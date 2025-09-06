#include "uci.h"
#include <string>
#include <iostream>
#include <sstream>

void UCI::runner()
{
    std::string input;
    std::string word = "";
    while (word != "quit")
    {
        getline(std::cin, input);
        std::istringstream is(input);

        word.clear();
        is >> std::skipws >> word;
        if (word == "quit")
        {
            engine.quit();
        }
        else if (word == "uci")
        {
            std::cout << "id name slonce" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (word == "go")
        {
            go(is);
        }
        else if (word == "position")
        {
            position(is);
        }
        else if (word == "ucinewgame")
        {
            engine.new_game();
        }
        else if (word == "isready")
        {
            engine.initialize();
        }
    }
}

void UCI::position(std::istringstream &is)
{
    engine.set_position(is);
}

void UCI::go(std::istringstream &is)
{
    engine.go(is);
}
