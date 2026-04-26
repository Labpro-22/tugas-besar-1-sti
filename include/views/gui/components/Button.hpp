#pragma once

#include "raylib.h"

#include <string>
#include <utility>

class Button {
private:
    Rectangle bounds_{0, 0, 0, 0};
    Texture2D texture_{};
    bool hasTexture_ = false;
    std::string label_;

    void unload() {
        if (hasTexture_) {
            UnloadTexture(texture_);
            hasTexture_ = false;
        }
    }

public:
    Button() = default;

    Button(Rectangle bounds, std::string label)
        : bounds_(bounds), label_(std::move(label)) {}

    Button(Rectangle bounds, std::string texturePath, std::string label)
        : bounds_(bounds), label_(std::move(label)) {
        setTexture(std::move(texturePath));
    }

    Button(const Button&) = delete;
    Button& operator=(const Button&) = delete;

    Button(Button&& other) noexcept
        : bounds_(other.bounds_),
          texture_(other.texture_),
          hasTexture_(other.hasTexture_),
          label_(std::move(other.label_)) {
        other.texture_ = {};
        other.hasTexture_ = false;
    }

    Button& operator=(Button&& other) noexcept {
        if (this != &other) {
            unload();
            bounds_ = other.bounds_;
            texture_ = other.texture_;
            hasTexture_ = other.hasTexture_;
            label_ = std::move(other.label_);

            other.texture_ = {};
            other.hasTexture_ = false;
        }
        return *this;
    }

    ~Button() {
        unload();
    }

    void setTexture(std::string texturePath) {
        unload();

        if (FileExists(texturePath.c_str())) {
            texture_ = LoadTexture(texturePath.c_str());
            hasTexture_ = texture_.id != 0;
        }
    }

    void setBounds(Rectangle bounds) {
        bounds_ = bounds;
    }

    const Rectangle& getBounds() const {
        return bounds_;
    }

    bool isHovered(Vector2 mouse) const {
        return CheckCollisionPointRec(mouse, bounds_);
    }

    bool isClicked(Vector2 mouse) const {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHovered(mouse);
    }

    void draw(Vector2 mouse) const {
        const bool hovered = isHovered(mouse);
        const Rectangle drawBounds = hovered
            ? Rectangle{bounds_.x - 4, bounds_.y - 4, bounds_.width + 8, bounds_.height + 8}
            : bounds_;

        if (hasTexture_) {
            DrawTexturePro(
                texture_,
                {0, 0, static_cast<float>(texture_.width), static_cast<float>(texture_.height)},
                drawBounds,
                {0, 0},
                0.0f,
                hovered ? Color{255, 255, 255, 255} : Color{238, 238, 238, 255}
            );
            return;
        }

        DrawRectangleRec(drawBounds, hovered ? Color{255, 218, 95, 255} : Color{250, 185, 45, 255});
        DrawRectangleLinesEx(drawBounds, 3, BLACK);

        const int fontSize = 30;
        DrawText(
            label_.c_str(),
            static_cast<int>(drawBounds.x + drawBounds.width / 2 - MeasureText(label_.c_str(), fontSize) / 2),
            static_cast<int>(drawBounds.y + drawBounds.height / 2 - fontSize / 2),
            fontSize,
            BLACK
        );
    }
};
