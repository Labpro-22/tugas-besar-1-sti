#pragma once
#include "Screen.hpp"

class ConfigScreen: public Screen {
protected:
    int width = 0;
    int height = 0;
public:
    ConfigScreen(int width, int height);
    virtual ~ConfigScreen();
    void render() override;
};