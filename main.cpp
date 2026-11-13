#include <filesystem>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "raylib.h"


class BackgroundRing {

public:

    BackgroundRing(std::string root_path, int screenWidth, int screenHeight) : m_root_path(root_path), m_screenWidth(screenWidth),
                                                                                m_screenHeight(screenHeight), m_fileNamesIndex(0) {

        auto rd = std::random_device {};
        auto rng = std::default_random_engine { rd() };

        for (const auto& entry : std::filesystem::directory_iterator(m_root_path)) {

            m_fileNames.push_back(entry.path().filename().string());
        }

        std::ranges::shuffle(m_fileNames, rng);
    }

    ~BackgroundRing() {
        UnloadImage(m_backgroundImage);
        UnloadTexture(m_backgroundTexture);
    }

    int loadBackgroundTexture() {

        std::string fullFilename = m_root_path + m_fileNames.at(m_fileNamesIndex);
        m_fileNamesIndex = (m_fileNamesIndex + 1) % m_fileNames.size();
        m_backgroundImage = LoadImage(fullFilename.c_str());
        if (!IsImageValid(m_backgroundImage)) {
            TraceLog(LOG_ERROR, "Could not load background image");
            return -1;
        }
        ImageResize(&m_backgroundImage, m_screenWidth, m_screenHeight);
        m_backgroundTexture = LoadTextureFromImage(m_backgroundImage);
        if (!IsTextureValid(m_backgroundTexture)) {
            TraceLog(LOG_ERROR, "Could not load background texture");
            return -2;
        }

        return 0;
    }

    Texture2D& getBackgroundTexture() {
        return m_backgroundTexture;
    }


private:
    Image m_backgroundImage;
    Texture2D m_backgroundTexture;
    std::string m_root_path;
    int m_screenWidth;
    int m_screenHeight;
    size_t m_fileNamesIndex;
    std::vector<std::string> m_fileNames;


};


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------

    const int screenWidth = 1680;
    const int screenHeight = 945;
    const int MAX_STALE_TICKS = 100;

    InitWindow(screenWidth, screenHeight, "GTAIV Loading Creator");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    BackgroundRing backgroundRing("./Backgrounds/", screenWidth, screenHeight);
    int errorCodeLoading = backgroundRing.loadBackgroundTexture();

    if (errorCodeLoading < 0) {
        TraceLog(LOG_ERROR, "Could not load background texture");
        return -1;
    }

    float alphaValue{ 0.0f };
    int staleTicks{ 0 };
    enum BlendingState {BLEND_IN, STALE, BLEND_OUT};
    BlendingState currentBlendingState {BLEND_IN};
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        switch (currentBlendingState) {
            case BLEND_IN: {
                alphaValue += 0.01f;

                if (alphaValue >= 1.0f) currentBlendingState = STALE;
                break;

            }
            case STALE: {
                staleTicks += 1;

                if (staleTicks >= MAX_STALE_TICKS) {
                    staleTicks = 0;
                    currentBlendingState = BLEND_OUT;
                }
                break;
            }
            case BLEND_OUT: {
                alphaValue -= 0.01f;

                if (alphaValue <= 0.0f) {
                    int errorCode = backgroundRing.loadBackgroundTexture();
                    if (errorCode < 0) {
                        TraceLog(LOG_ERROR, "Could not load background texture");
                        return -1;
                    }
                    currentBlendingState = BLEND_IN;
                }
                break;
            }
            default:
                break;
        }

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(backgroundRing.getBackgroundTexture(), 0, 0, Fade(WHITE, alphaValue));


        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}