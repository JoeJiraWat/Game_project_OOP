#include "GameUi.h"

#include "raylib.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1400, 900, "แมวระเบิด");
    InitAudioDevice();
    SetTargetFPS(60);

    {
        CatBombApp app;
        while (!WindowShouldClose()) {
            app.updateAndDraw();
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
