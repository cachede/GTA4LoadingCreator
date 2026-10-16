#include <iostream>

#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    const int screenWidth = 1680;
    const int screenHeight = 945;

    InitWindow(screenWidth, screenHeight, "GTAIV Loading Creator");
    ToggleFullscreen();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Image backgroundImage = LoadImage("./Backgrounds/1_1.png");
    if (!IsImageValid(backgroundImage)) {
        TraceLog(LOG_ERROR, "Could not load background image");
        return 1;
    }

    ImageResize(&backgroundImage, screenWidth, screenHeight);

    Texture2D backgroundTexture = LoadTextureFromImage(backgroundImage);
    if (!IsTextureValid(backgroundTexture)) {
        TraceLog(LOG_ERROR, "Could not load background texture");
        return 1;
    }

    float alphaValue{ 1.0f };

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        if (alphaValue <= 0.0f) {
            alphaValue = 1.0f; //ab hier ist tot, weil alles schwarz ist.
        } else {
            alphaValue -= 0.01f;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(backgroundTexture, 0, 0, Fade(WHITE, alphaValue));


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    UnloadTexture(backgroundTexture);
    UnloadImage(backgroundImage);
    //--------------------------------------------------------------------------------------

    return 0;
}