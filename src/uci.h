#ifndef UCI_H
#define UCI_H
#include "engine.h"
#include <sstream>

class UCI
{
    Engine engine;

public:
    void runner();
    void position(std::istringstream &is);
    void go(std::istringstream &is);
};

#endif // #ifndef UCI_H
