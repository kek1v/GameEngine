#pragma once
#include <memory>
#include <string>
namespace RendererEngine { class Sprite; class ShaderProgram; }
std::shared_ptr<RendererEngine::Sprite> makeScoreboard(
    const std::shared_ptr<RendererEngine::ShaderProgram>& shader,
    const std::wstring& title, const std::wstring& subtitle,
    int width, int height, bool victory = false);
