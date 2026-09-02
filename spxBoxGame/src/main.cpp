#include <Core/spxBoxWorld.h>

int main()
{
    spxBoxWorld world;
   // world.Initialize();

    if (!world.Initialize())
        return -1;

    world.Run();

    world.Shutdown();

    return 0;
}