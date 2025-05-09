#include "game.hpp"
#include <vec2.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>
#include <vector>
#include <string>

#include "../Resources/ResourceManager.hpp"
#include "../Renderer/Shaderprogram.hpp"
#include "../Renderer/AnimatedSprite.hpp"


game::game(const glm::ivec2& windowSize) 
    : m_eCurrentGameState(EGameState::Active)
    , m_windowSize(windowSize){

	m_Keys.fill(false);
}

game::~game() {

}

void game::render() {
    ResourceManager::getAnimatedSprite("NewAnimatedSprite")->render();
}

void game::update(const uint64_t delta) {
    ResourceManager::getAnimatedSprite("NewAnimatedSprite")->update(delta);
}

void game::setKey(const int key, const int action) {
	m_Keys[key] = action;
}

bool game::init() {
    auto pDefaultShaderProgram = ResourceManager::loadShaders("DefaultShader", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
    if (!pDefaultShaderProgram) {
        std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
        return -1;
    }

    auto pSpriteShaderProgram = ResourceManager::loadShaders("SpriteShader", "res/shaders/vSprite.txt", "res/shaders/fSprite.txt");
    if (!pSpriteShaderProgram) {
        std::cerr << "Can't create sprite shader program: " << "DefaultSprite" << std::endl;
        return -1;
    }

    auto tex = ResourceManager::loadTexture("DefaultTexture", "res/textures/map_16x16.png");

    std::vector<std::string> subTexturesNames = { // см в map16x16        
        "block",
        "topBlock",
        "bottomBlock",
        "leftBlock",
        "rightBlock",
        "topLeftBlock",
        "topRightBlock",
        "bottomLeftBlock",

        "bottomRightBlock",
        "beton",
        "topBeton",
        "bottomBeton",
        "leftBeton",
        "rightBeton",
        "topLeftBeton",
        "topRightBeton",

        "bottomLeftBeton",
        "bottomRightBeton",
        "water1",
        "water2",
        "water3",
        "trees",
        "ice",
        "wall",

        "eagle",
        "deadEagle"
        "nothing",
        "respawn1",
        "respawn2",
        "respawn3",
        "respawn4"
    };

    auto pTextureAtlas = ResourceManager::loadTextureAtlas("DefaultTextureAtlas", "res/textures/map_16x16.png", std::move(subTexturesNames), 16, 16);

    auto pAnimatedSprite = ResourceManager::loadAnimatedSprite("NewAnimatedSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "water");
    pAnimatedSprite->setPosition(glm::vec2(400, 250));
    std::vector<std::pair<std::string, uint64_t>> waterStates;
    waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water1", 1000000000));
    waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water2", 1000000000));
    waterStates.emplace_back(std::make_pair<std::string, uint64_t>("water3", 1000000000));


    std::vector<std::pair<std::string, uint64_t>> eagleStates;
    eagleStates.emplace_back(std::make_pair<std::string, uint64_t>("eagle", 1000000000));
    eagleStates.emplace_back(std::make_pair<std::string, uint64_t>("deadEagle", 1000000000));

    pAnimatedSprite->insertState("waterState", std::move(waterStates));
    pAnimatedSprite->insertState("eagleState", std::move(eagleStates));
    pAnimatedSprite->setState("eagleState");

    pDefaultShaderProgram->use();
    pDefaultShaderProgram->setInt("tex", 0);

    glm::mat4 modelMatrix1 = glm::mat4(1.f); // единичная матрица (она не выполняет никаких преобразований)
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(100.f, 200.f, 0.f)); // переместили нашу modelMatrix на vec3

    glm::mat4 modelMatrix2 = glm::mat4(1.f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(540.f, 200.f, 0.f));

    // теперь перемножаем на projection matrix
    glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(m_windowSize.x), 0.f, static_cast<float>(m_windowSize.y), -100.f, 100.f);

    pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

    pSpriteShaderProgram->use();
    pSpriteShaderProgram->setInt("tex", 0);
    pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

    return true;
}

