#pragma once
#include "raylib.h"
#include <string>
#include <vector>


/** ABC untuk Kelas Screen -- buat rendering
 * Fungsi: layout + flow control, navigation for trigger
 */
class Screen{
protected:
    int width = 0;
    int height = 0;
public:
    Screen(int width = 1000, int height = 800) : width(width), height(height) {}
    virtual ~Screen() = default;
    virtual void update() {}
    virtual void render() = 0;

    static bool loadTexture(Texture2D& target, const std::string& path) {
        if (!FileExists(path.c_str())) {
            return false;
        }

        target = LoadTexture(path.c_str());
        if (target.id != 0) {
            SetTextureFilter(target, TEXTURE_FILTER_POINT);
        }
        return target.id != 0;
    }

    static void drawTextureFit(Texture2D texture, Rectangle destination, Color tint = WHITE) {
        DrawTexturePro(
            texture,
            {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
            destination,
            {0, 0},
            0.0f,
            tint
        );
    }

    static std::vector<std::string> wrapText(const std::string& text, int fontSize, int maxWidth) {
        std::vector<std::string> lines;
        std::string current;
        std::string word;

        for (char ch : text + " ") {
            if (ch != ' ') {
                word += ch;
                continue;
            }

            if (word.empty()) {
                continue;
            }

            const std::string candidate = current.empty() ? word : current + " " + word;
            if (MeasureText(candidate.c_str(), fontSize) <= maxWidth) {
                current = candidate;
            } else {
                if (!current.empty()) {
                    lines.push_back(current);
                }
                current = word;
            }
            word.clear();
        }

        if (!current.empty()) {
            lines.push_back(current);
        }
        return lines;
    }
};