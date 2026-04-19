#include "Game.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

Game* g_game = nullptr;

void MainLoop()
{
    if (g_game )
    {

        g_game->update();
        g_game->draw();
    }
}

int main()
{
    g_game = new Game();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(MainLoop, 0, 1);
#else
    while (!WindowShouldClose() )
    {
        MainLoop();
    }
    delete g_game;
#endif

    return 0;
}