// INI HANYA UI SEMENTARA, BELUM DIINTEGRASI DENGAN LOGIC GAME
// .cpp menyusul

#pragma once

#include "Screen.hpp"
#include "../components/Button.hpp"
#include "raylib.h"

#include <string>

class MenuScreen : public Screen {
private:
    Texture2D background{};
    Texture2D title{};
    bool hasBackground = false;
    bool hasTitle = false;

    Rectangle pannelMenu{0, 0, 0, 0};
    Button newGameBtn;
    Button loadGameBtn;
    Button settingsBtn;

public:
    MenuScreen(int width = 1000, int height = 800) : Screen(width, height) {
        hasBackground = loadTexture(background, "assets/ui/ScreenBackground.png");
        hasTitle = loadTexture(title, "assets/ui/MenuScreen/TitleText.png");

        const float buttonX = width / 2.0f - 137.0f;
        pannelMenu = {buttonX - 80, 350, 414, 350};
        newGameBtn = Button({buttonX, 390, 274, 74}, "assets/ui/MenuScreen/NewGameBtn.png", "New Game");
        loadGameBtn = Button({buttonX, 486, 274, 74}, "assets/ui/MenuScreen/LoadBtn.png", "Load");
        settingsBtn = Button({buttonX, 582, 274, 74}, "assets/ui/MenuScreen/SettingsBtn.png", "Settings");

    }

    ~MenuScreen() override {
        if (hasBackground) {
            UnloadTexture(background);
        }
        if (hasTitle) {
            UnloadTexture(title);
        }
    }

    bool isNewGameClicked(Vector2 mouse) const {
        return newGameBtn.isClicked(mouse);
    }

    bool isLoadGameClicked(Vector2 mouse) const {
        return loadGameBtn.isClicked(mouse);
    }

    bool isSettingsClicked(Vector2 mouse) const {
        return settingsBtn.isClicked(mouse);
    }

    void render() override {
        const Vector2 mouse = GetMousePosition();
        ClearBackground(Color{29, 31, 34, 255});

        if (hasBackground) {
            drawTextureFit(background, {0, 0, static_cast<float>(width), static_cast<float>(height)});
        } else {
            DrawRectangle(52, 90, width - 104, height - 185, Color{255, 184, 44, 255});
            DrawRectangleLinesEx({52, 90, static_cast<float>(width - 104), static_cast<float>(height - 185)}, 4, BLACK);
        }

        if (hasTitle) {
            drawTextureFit(title, {width / 2.0f - 359, 110, 718, 218});
        } else {
            const char* fallbackTitle = "NimonsPoli";
            const int fontSize = 70;
            DrawText(fallbackTitle, width / 2 - MeasureText(fallbackTitle, fontSize) / 2, 120, fontSize, BLACK);
        }
        
        DrawRectangleRounded(pannelMenu, 0.2f, 10, Fade(WHITE, 0.3f));

        newGameBtn.draw(mouse);
        loadGameBtn.draw(mouse);
        settingsBtn.draw(mouse);
    }
};


/**
 * JIKA INGIN MELAKUKAN TES, SILAHKAN PAKAI INI:
 */

// #include "views/gui/screens/GameScreen.hpp"

// #include "raylib.h"

// int main() {
//     const int width = 1000;
//     const int height = 800;

//     InitWindow(width, height, "GameScreen Preview");
//     SetTargetFPS(60);

//     GameScreen screen(width, height);

//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         screen.render();
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
