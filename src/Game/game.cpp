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

    ResourceManager::loadJSONresources("res/resources.json");

    auto pSpriteShaderProgram = ResourceManager::getShaderProgram("SpriteShader");
    if (!pSpriteShaderProgram) {
        std::cerr << "cant find shader program: " << " shaderprogram" << std::endl;
    }

    auto pTextureAtlas2 = ResourceManager::getTexture("HumanAnimations");
    if (!pTextureAtlas2) {
        std::cerr << "cant find texture atlas: " << "HumanAnimations" << std::endl;
    }

    auto pTanksTextureAtlas = ResourceManager::getTexture("TanksTextureAtlas");
    if (!pTanksTextureAtlas) {
        std::cerr << "cant find texture atlas: " << "TanksTextureAtlas" << std::endl;
    }

    auto pAnimatedHuman = ResourceManager::loadAnimatedSprite("NewAnimatedHuman", "HumanAnimations", "SpriteShader", 100, 100, "down_idle");
    pAnimatedHuman->setPosition(glm::vec2(400, 250));
    std::vector<std::pair<std::string, uint64_t>> downWalkStates;
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_idle", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk1", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk2", 1000000000));
    downWalkStates.emplace_back(std::make_pair<std::string, uint64_t>("down_walk3", 1000000000));


    pAnimatedHuman->insertState("downWalkStates", std::move(downWalkStates));
    pAnimatedHuman->setState("downWalkStates");


    glm::mat4 modelMatrix1 = glm::mat4(1.f); // единичная матрица (она не выполняет никаких преобразований)
    modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(100.f, 200.f, 0.f)); // переместили нашу modelMatrix на vec3

    glm::mat4 modelMatrix2 = glm::mat4(1.f);
    modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(540.f, 200.f, 0.f));

    // теперь перемножаем на projection matrix
    glm::mat4 projectionMatrix = glm::ortho(0.f, static_cast<float>(m_windowSize.x), 0.f, static_cast<float>(m_windowSize.y), -100.f, 100.f);

    pSpriteShaderProgram->use();
    pSpriteShaderProgram->setInt("tex", 0);
    pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);

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

