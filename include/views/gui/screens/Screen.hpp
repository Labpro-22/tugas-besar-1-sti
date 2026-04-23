#pragma once

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
};