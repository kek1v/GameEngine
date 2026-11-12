#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "Scoreboard.hpp"
#include "../Renderer/Sprite.hpp"
#include "../Renderer/Texture2D.hpp"
#include <vector>
#include <stdexcept>

std::shared_ptr<RendererEngine::Sprite> makeScoreboard(
    const std::shared_ptr<RendererEngine::ShaderProgram>& shader,
    const std::wstring& title, const std::wstring& subtitle,
    int width, int height, bool victory) {
    HDC dc = CreateCompatibleDC(nullptr);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dc || !bitmap) {
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        throw std::runtime_error("Cannot create scoreboard texture");
    }
    auto oldBitmap = SelectObject(dc, bitmap);
    RECT bounds{0,0,width,height};
    auto background = CreateSolidBrush(RGB(17,26,30));
    FillRect(dc, &bounds, background);
    DeleteObject(background);
    auto border = CreateSolidBrush(victory ? RGB(95,225,155) : RGB(231,189,94));
    FrameRect(dc, &bounds, border);
    DeleteObject(border);
    SetBkMode(dc, TRANSPARENT);
    auto drawText = [&](const std::wstring& text, int top, int size, COLORREF color) {
        HFONT font = CreateFontW(-size,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH,L"Segoe UI");
        auto oldFont = SelectObject(dc, font);
        SetTextColor(dc, color);
        RECT line{12,top,width-12,top+size+14};
        DrawTextW(dc, text.c_str(), -1, &line, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        SelectObject(dc, oldFont);
        DeleteObject(font);
    };
    if (height < 60) drawText(title, 2, 20, RGB(240,238,221));
    else {
        drawText(title, 35, 36, victory ? RGB(112,239,169) : RGB(255,201,105));
        drawText(subtitle, 105, 23, RGB(237,241,242));
        drawText(L"R - \u043d\u043e\u0432\u044b\u0439 \u0431\u043e\u0439    |    Esc - \u0432\u044b\u0445\u043e\u0434", 175, 20, RGB(173,189,193));
    }
    GdiFlush();
    std::vector<unsigned char> rgba(width * height * 4);
    const auto* bgra = static_cast<unsigned char*>(bits);
    for (int y = 0; y < height; ++y) for (int x = 0; x < width; ++x) {
        const int source = (y * width + x) * 4;
        const int dest = ((height - 1 - y) * width + x) * 4;
        rgba[dest] = bgra[source+2]; rgba[dest+1] = bgra[source+1];
        rgba[dest+2] = bgra[source]; rgba[dest+3] = 255;
    }
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    auto texture = std::make_shared<RendererEngine::Texture2D>(width,height,rgba.data(),4,GL_LINEAR);
    texture->addSubTexture("panel", {0.f,0.f}, {1.f,1.f});
    return std::make_shared<RendererEngine::Sprite>(texture,"panel",shader,glm::vec2(0.f),glm::vec2(width,height));
}
