#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

#include "Tank.hpp"
#include "game.hpp"


game::game(const glm::ivec2& windowSize)
    : m_eCurrentGameState(EGameState::Active)
    , m_windowSize(windowSize) {

    m_Keys.fill(false);
}

game::~game() {

}

void game::render() {
    ResourceManager::getAnimatedSprite("NewAnimatedHuman")->render();
    if (m_pTank) {
        m_pTank->render();
    }
}

void game::update(const uint64_t delta) {
    ResourceManager::getAnimatedSprite("NewAnimatedHuman")->update(delta);
    if (m_pTank) {
        if (m_Keys[GLFW_KEY_W]) {
            m_pTank->setOrientation(Tank::eOrientation::Top);
            m_pTank->move(true);
        }
        else if (m_Keys[GLFW_KEY_A]){
            m_pTank->setOrientation(Tank::eOrientation::left);
            m_pTank->move(true);
        }
        else if (m_Keys[GLFW_KEY_S]){
            m_pTank->setOrientation(Tank::eOrientation::Bottom);
            m_pTank->move(true);
        }
        else if (m_Keys[GLFW_KEY_D]){
            m_pTank->setOrientation(Tank::eOrientation::right);
            m_pTank->move(true);
        }else {
            m_pTank->move(false);
        }
        m_pTank->update(delta);
    }
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

   // auto tex = ResourceManager::loadTexture("DefaultTexture", "res/textures/map_16x16.png");
    auto tex = ResourceManager::loadTexture("HumanAnimations", "res/textures/character_base_16x16.png");
/*
    std::vector<std::string> subTexturesNames = {
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
        "deadEagle",        
        "nothing",
        "respawn1",
        "respawn2",
        "respawn3",
        "respawn4"
    };


    auto pTextureAtlas = ResourceManager::loadTextureAtlas("DefaultTextureAtlas", "res/textures/map_16x16.png", std::move(subTexturesNames), 4, 8);

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
    pAnimatedSprite->setState("waterState");
*/

// ¬“ќ–ќ… ј“Ћј—     
    std::vector<std::string> subTexturesNames2 = {
        // ѕервый р€д Ч движение вниз
        "down_idle", "down_walk1", "down_walk2", "down_walk3",

        // ¬торой р€д Ч движение влево
        "left_idle", "left_walk1", "left_walk2", "left_walk3",

        // “ретий р€д Ч движение вправо
        "right_idle", "right_walk1", "right_walk2", "right_walk3",

        // „етвЄртый р€д Ч движение вверх
        "up_idle", "up_walk1", "up_walk2", "up_walk3",

        // ѕ€тый р€д Ч прочее, можно под свои нужды
        "misc1", "misc2", "misc3", "misc4"
    };

    auto pTextureAtlas2 = ResourceManager::loadTextureAtlas("HumanAnimations", "res/textures/character_base_16x16.png", std::move(subTexturesNames2), 16, 16);

    auto pAnimatedHuman = ResourceManager::loadAnimatedSprite("NewAnimatedHuman", "HumanAnimations", "SpriteShader", 100, 100, "down_idle");
    pAnimatedHuman->setPosition(glm::vec2(400, 250));
    std::vector<std::pair<std::string, uint64_t>> downWalkStates;
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_idle", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk1", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk2", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk3", 1000000000));


    pAnimatedHuman->insertState("downWalkStates", std::move(downWalkStates));
    pAnimatedHuman->setState("downWalkStates");

    pDefaultShaderProgram->use();
    pDefaultShaderProgram->setInt("tex", 0);

    glm::mat4 modelMatrix1 = glm::mat4(1.f); // единична€ матрица (она не выполн€ет никаких преобразований)
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(100.f, 200.f, 0.f)); // переместили нашу modelMatrix на vec3

    glm::mat4 modelMatrix2 = glm::mat4(1.f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(540.f, 200.f, 0.f));

    // теперь перемножаем на projection matrix
    glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(m_windowSize.x), 0.f, static_cast<float>(m_windowSize.y), -100.f, 100.f);

    pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

    pSpriteShaderProgram->use();
    pSpriteShaderProgram->setInt("tex", 0);
    pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);



    std::vector<std::string> TanksSubTexturesNames = {
        "tankTop1",
        "tankTop2",
        "tankLeft1",
        "tankLeft2",
        "tankBottom1",
        "tankBottom2",
        "tankRight1",
        "tankRight2"
    };

    auto pTanksTextureAtlas = ResourceManager::loadTextureAtlas("TanksTextureAtlas", "res/textures/tanks.png", std::move(TanksSubTexturesNames), 16, 16);
    auto pTanksAnimatedSprite = ResourceManager::loadAnimatedSprite("TanksAnimatedSprite", "TanksTextureAtlas", "SpriteShader", 100, 100, "tankTop1");

    std::vector<std::pair<std::string, uint64_t>> tankTopStates;
    tankTopStates.emplace_back(std::make_pair<std::string, uint64_t>("tankTop1", 500000000));
    tankTopStates.emplace_back(std::make_pair<std::string, uint64_t>("tankTop2", 500000000));

    std::vector<std::pair<std::string, uint64_t>> tankLeftStates;
    tankLeftStates.emplace_back(std::make_pair<std::string, uint64_t>("tankLeft1", 500000000));
    tankLeftStates.emplace_back(std::make_pair<std::string, uint64_t>("tankLeft2", 500000000));

    std::vector<std::pair<std::string, uint64_t>> tankBottomStates;
    tankBottomStates.emplace_back(std::make_pair<std::string, uint64_t>("tankBottom1", 500000000));
    tankBottomStates.emplace_back(std::make_pair<std::string, uint64_t>("tankBottom2", 500000000));

    std::vector<std::pair<std::string, uint64_t>> tankRightStates;
    tankRightStates.emplace_back(std::make_pair<std::string, uint64_t>("tankRight1", 500000000));
    tankRightStates.emplace_back(std::make_pair<std::string, uint64_t>("tankRight2", 500000000));

    pTanksAnimatedSprite->insertState("tankTopState", std::move(tankTopStates));
    pTanksAnimatedSprite->insertState("tankLeftState", std::move(tankLeftStates));
    pTanksAnimatedSprite->insertState("tankBottomState", std::move(tankBottomStates));
    pTanksAnimatedSprite->insertState("tankRightState", std::move(tankRightStates));

    pTanksAnimatedSprite->setState("tankTopState");
    m_pTank = std::make_unique<Tank>(pTanksAnimatedSprite, 0.0000001f, glm::vec2(100.f, 100.f));

    return true;
}

