#include"ResourceManager.hpp"
#include"../Renderer/ShaderProgram.hpp"
#include"../Renderer/Texture2D.hpp"
#include"../Renderer/Sprite.hpp"
#include"../Renderer/AnimatedSprite.hpp"

#include<sstream>
#include<fstream>
#include<iostream>

#define STB_IMAGE_IMPLEMENTATION
//#define	STBI_ONLY_JP
#include "stb_image.h"

ResourceManager::ShaderProgramsMap ResourceManager::m_shaderPrograms;
ResourceManager::TexturesMap ResourceManager::m_textures;
ResourceManager::SpritesMap ResourceManager::m_sprites;
std::string ResourceManager::m_Path;
ResourceManager::AnimatedSpritesMap ResourceManager::m_animatedSprites;

void ResourceManager::unloadAllResources() {
	m_shaderPrograms.clear();
 	m_textures.clear();
	m_animatedSprites.clear();
 	m_Path.clear();
	m_sprites.clear();
	m_animatedSprites.clear();
}

void ResourceManager::setExecutablePath(const std::string& executablePath) {

	// берем путь к экзешнику и берем из этой строки подстроку с 0 символа до последнего слэша
	size_t found = executablePath.find_last_of("/\\");
	m_Path = executablePath.substr(0, found);

}

std::string ResourceManager::getFileString(const std::string& relativePath) {
	std::ifstream f;
	f.open(m_Path + "/" + relativePath.c_str(), std::ios::in | std::ios::binary);
	if (!f.is_open()) {
		std::cerr << "Failed to open file: " << relativePath << std::endl; 
		return std::string();
	}

	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

std::shared_ptr<RendererEngine::ShaderProgram> ResourceManager::loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath) {
	std::string vertexString = getFileString(vertexPath);
	if (vertexPath.empty()) {
		std::cerr << "No vertex shader!" << std::endl;
		return nullptr;
	}

	std::string fragmentString = getFileString(fragmentPath);
	if (fragmentPath.empty()) {
		std::cerr << "No fragment shader!" << std::endl;
		return nullptr;
	}

	std::shared_ptr<RendererEngine::ShaderProgram>& newShader = m_shaderPrograms.emplace(shaderName, std::make_shared<RendererEngine::ShaderProgram>(vertexString, fragmentString)).first->second;
	if (newShader->isCompiled()) {
		return newShader;
	}

	std::cerr << "Can't load shader program:\n"
		<< "Vertex: " << vertexPath << "\nFragment: "
		<< fragmentPath << std::endl;

	return nullptr;
}


std::shared_ptr<RendererEngine::ShaderProgram> ResourceManager::getShaderProgram(const std::string& shaderName) {
	ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName);
	if (it != m_shaderPrograms.end()) {
		return it->second;
	}

	std::cerr << "Can't find the shader program: " << shaderName << std::endl;
	return nullptr;
}


std::shared_ptr<RendererEngine::Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& texturePath) {
	int channels = 0;
	int width = 0;
	int height = 0;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* pixels = stbi_load(std::string(m_Path + "/" + texturePath).c_str(), &width, &height, &channels, 0);
	
	if (!pixels) {
		std::cerr << "Cant load image: " << texturePath << std::endl;
		return nullptr;
	}
	//std::cout << "Texture loaded: " << texturePath << ", width: " << width << ", height: " << height << ", channels: " << channels << std::endl;

	std::shared_ptr<RendererEngine::Texture2D> newTexture = m_textures.emplace(textureName, std::make_shared<RendererEngine::Texture2D>(width, height, pixels, channels, GL_NEAREST, GL_CLAMP_TO_EDGE)).first->second;

	stbi_image_free(pixels);
	return newTexture;
}


std::shared_ptr<RendererEngine::Texture2D> ResourceManager::getTexture(const std::string& textureName) {
    TexturesMap::const_iterator it = m_textures.find(textureName);
	if (it != m_textures.end()) {
		return it->second;
	}

	std::cerr << "Can't find the texture: " << textureName << std::endl;
	return nullptr;

}

std::shared_ptr<RendererEngine::Sprite> ResourceManager::loadSprite(const std::string& spriteName,
															  const std::string& textureName,
															  const std::string& shaderName,
														      const unsigned int spriteWidth,
															  const unsigned int spriteHeight,
	                                                          const std::string& subTextureName) {
		
	auto pTexture = getTexture(textureName);
	if (!pTexture) {
		std::cerr << "Can't find the sprite: " << spriteName << std::endl;
	}

	auto pShader = getShaderProgram(shaderName);
	if (!pShader) {
		std::cerr << "Can't find the shader: " << shaderName << std::endl;
	}


	std::shared_ptr<RendererEngine::Sprite> newSprite = m_sprites.emplace(spriteName, 
																	std::make_shared<RendererEngine::Sprite>(pTexture, subTextureName, pShader,
																	glm::vec2(0.f, 0.f), 
																	glm::vec2(spriteWidth, spriteHeight))).first->second;
	return newSprite;
}


std::shared_ptr<RendererEngine::Sprite> ResourceManager::getSprite(const std::string& spriteName) {
	SpritesMap::const_iterator it = m_sprites.find(spriteName);
	if (it != m_sprites.end()) {
		return it->second;
	}

	std::cerr << "Can't find the sprite: " << spriteName<< std::endl;
	return nullptr;
}

std::shared_ptr<RendererEngine::AnimatedSprite> ResourceManager::loadAnimatedSprite(const std::string& spriteName,
	const std::string& textureName,
	const std::string& shaderName,
	const unsigned int spriteWidth,
	const unsigned int spriteHeight,
	const std::string& subTextureName) {

	auto pTexture = getTexture(textureName);
	if (!pTexture) {
		std::cerr << "Can't find the sprite: " << spriteName << std::endl;
	}

	auto pShader = getShaderProgram(shaderName);
	if (!pShader) {
		std::cerr << "Can't find the shader: " << shaderName << std::endl;
	}


	std::shared_ptr<RendererEngine::AnimatedSprite> newAnimatedSprite = m_animatedSprites.emplace(spriteName,
		std::make_shared<RendererEngine::AnimatedSprite>(pTexture, subTextureName, pShader,
			glm::vec2(0.f, 0.f),
			glm::vec2(spriteWidth, spriteHeight))).first->second;
	return newAnimatedSprite;
}


std::shared_ptr<RendererEngine::AnimatedSprite> ResourceManager::getAnimatedSprite(const std::string& spriteName) {
	auto it = m_animatedSprites.find(spriteName);
	if (it != m_animatedSprites.end()) {
		return it->second;
	}

	std::cerr << "Can't find animated sprite: " << spriteName << std::endl;
	return nullptr;
}


std::shared_ptr<RendererEngine::Texture2D>  ResourceManager::loadTextureAtlas(const std::string& textureName,
																		const std::string& texturePath,
																		std::vector<std::string> subTextures,
																		const unsigned int subTextureWidth,
																		const unsigned int subTextureHeiht) {

	auto pTexture = loadTexture(std::move(textureName), std::move(texturePath));
	if (!pTexture) {
		std::cerr << "cant load textute atlas" << std::endl;
		return nullptr;
	}

	const unsigned int textureWidth = pTexture->width();
	const unsigned int textureHeight = pTexture->height();
	unsigned int currentTextureOffsetX = 0;
	unsigned int currentTextureOffsetY = textureHeight;
	for (auto& currentSubTextureName : subTextures) {
		glm::vec2 leftBottomUV(static_cast<float>(currentTextureOffsetX) / textureWidth ,static_cast<float>(currentTextureOffsetY - subTextureHeiht) / textureHeight);
		glm::vec2 rightTopUV(static_cast<float>(currentTextureOffsetX + subTextureWidth) / textureWidth, static_cast<float>(currentTextureOffsetY) / textureHeight);

		pTexture->addSubTexture(std::move(currentSubTextureName), leftBottomUV, rightTopUV);
		currentTextureOffsetX += subTextureWidth;
		if (currentTextureOffsetX >= textureWidth) {
			currentTextureOffsetX = 0;
			currentTextureOffsetY -= subTextureHeiht;
		}
	}
	
	return pTexture;
}
