#pragma once

#include "Screen.hpp"
#include "../components/Button.hpp"
#include "raylib.h"

#include <string>
#include <utility>
#include <vector>

class TransactionLogScreen : public Screen {
private:
    Texture2D background{};
    Texture2D title{};
    
    Button closeBtn{};
    bool hasBackground = false;
    bool hasTitle = false;
    bool hasCloseBtn = false;
    std::vector<std::string> logs;

public:
    TransactionLogScreen(int width = 1000, int height = 800) : Screen(width, height) {
        hasBackground = loadTexture(background, "assets/ui/ScreenBackground.png");
        hasTitle = loadTexture(title, "assets/ui/LogScreen/TransactionLogText.png");
        closeBtn = Button({static_cast<float>(width - 118), 46, 90, 76}, "assets/ui/LogScreen/CancelBtn.png", "Close");
        hasCloseBtn = true;
        logs = {
            "[INFO] something",
            "[INFO] something"
        };
    }

    ~TransactionLogScreen() override {
        if (hasBackground) {
            UnloadTexture(background);
        }
        if (hasTitle) {
            UnloadTexture(title);
        }
    }

    void setLogs(std::vector<std::string> newLogs) {
        logs = std::move(newLogs);
    }

    void addLog(std::string log) {
        logs.push_back(std::move(log));
    }

    void clearLogs() {
        logs.clear();
    }

    bool isCloseBtnClicked(Vector2 mouse) const {
        return closeBtn.isClicked(mouse);
    }

    void render() override {
        const Vector2 mouse = GetMousePosition();
        ClearBackground(Color{188, 207, 219, 255});

        if (hasBackground) {
            drawTextureFit(background, {0, 0, static_cast<float>(width), static_cast<float>(height)});
        } else {
            DrawRectangle(0, 0, width, height, Color{188, 207, 219, 255});
            DrawRectangle(0, height / 2 + 95, width, height / 2, Color{180, 189, 122, 255});
        }

        if (hasTitle) {
            drawTextureFit(title, {70, 62, 435, 72});
        } else {
            DrawText("Transaction Log", 70, 62, 48, BLACK);
        }

        closeBtn.draw(mouse);

        Rectangle panel{72, 145, static_cast<float>(width - 144), static_cast<float>(height - 205)};
        DrawRectangleRounded(panel, 0.025f, 8, Color{255, 255, 255, 115});

        int y = static_cast<int>(panel.y + panel.height - 145);
        const int fontSize = 34;
        const int lineHeight = 48;
        const int maxWidth = static_cast<int>(panel.width - 70);

        for (const std::string& log : logs) {
            const std::vector<std::string> lines = wrapText(log, fontSize, maxWidth);
            for (const std::string& line : lines) {
                if (y > panel.y + panel.height - lineHeight) {
                    return;
                }
                DrawText(line.c_str(), static_cast<int>(panel.x + 36), y, fontSize, BLACK);
                y += lineHeight;
            }
        }
    }
};
