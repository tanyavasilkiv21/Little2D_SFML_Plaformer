#include "GameEngine.h"

#include "ProfileTimer.hpp"

int main() 
{
    PROFILING_FUNCTION();
    GameEngine g("levels_config/assets.txt");
    g.run();
    return 0;
}
