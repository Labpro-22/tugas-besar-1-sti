#pragma once

#include "Screen.hpp"
#include "raylib.h"

#include <string>

class LoadingScreen : public Screen {
private:
    int frames = 0;
    Texture2D background{};
    Texture2D loadingBuffer{};
    Texture2D loadingText{};
    bool hasBackground = false;
    bool hasLoadingBuffer = false;
    bool hasLoadingText = false;

public:
    LoadingScreen(int width = 1000, int height = 800) : Screen(width, height) {
        hasBackground = loadTexture(background, "assets/ui/ScreenBackground.png");
        hasLoadingBuffer = loadTexture(loadingBuffer, "assets/ui/LoadingScreen/LoadingBuffer.png");
        hasLoadingText = loadTexture(loadingText, "assets/ui/LoadingScreen/LoadingText.png");
    }

    ~LoadingScreen() override {
        if (hasBackground) {
            UnloadTexture(background);
        }
        if (hasLoadingBuffer) {
            UnloadTexture(loadingBuffer);
        }
        if (hasLoadingText) {
            UnloadTexture(loadingText);
        }
    }

    void update() override {
        frames++;
    }

    bool isFinished() const {
        return frames > 120;
    }

    void render() override {
        ClearBackground(Color{188, 207, 219, 255});

        if (hasBackground) {
            drawTextureFit(background, {0, 0, static_cast<float>(width), static_cast<float>(height)});
        } else {
            DrawRectangle(0, 0, width, height, Color{188, 207, 219, 255});
            DrawRectangle(0, height / 2 + 95, width, height / 2, Color{180, 189, 122, 255});
        }

        DrawRectangle(0, 0, width, height, Color{255, 255, 255, 105});

        if (hasLoadingBuffer) {
            const float bufferScale = 0.8f;
            const float bufferWidth = loadingBuffer.width * bufferScale;
            const float bufferHeight = loadingBuffer.height * bufferScale;
            drawTextureFit(
                loadingBuffer,
                {width / 2.0f - bufferWidth / 2.0f, height / 2.0f - 100.0f, bufferWidth, bufferHeight}
            );
        } else {
            DrawCircle(width / 2, height / 2 - 80, 54, YELLOW);
            DrawCircleLines(width / 2, height / 2 - 80, 54, BLACK);
        }

        if (hasLoadingText) {
            const float textScale = 0.9f;
            const float textWidth = loadingText.width * textScale;
            const float textHeight = loadingText.height * textScale;
            drawTextureFit(
                loadingText,
                {width / 2.0f - textWidth / 2.0f, height / 2.0f + 35.0f, textWidth, textHeight}
            );
        } else {
            const std::string text = "Loading...";
            const int fontSize = 48;
            DrawText(text.c_str(), width / 2 - MeasureText(text.c_str(), fontSize) / 2, height / 2 + 92, fontSize, BLACK);
        }
    }
};


/**
 * Jika ingin melakukan testing khusus GUI ini, gunakan kode berikut.
 */
// #include "views/gui/screens/LoadingScreen.hpp"

// #include "raylib.h"

// int main() {
//     const int width = 1000;
//     const int height = 800;

//     InitWindow(width, height, "LoadingScreen Preview");
//     SetTargetFPS(60);

//     LoadingScreen screen(width, height);

//     while (!WindowShouldClose()) {
//         screen.update();

//         BeginDrawing();
//         screen.render();
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
