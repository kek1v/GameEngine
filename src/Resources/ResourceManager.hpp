#pragma once 

#include<string>
#include<memory>
#include<map>
#include<vector>

namespace RendererEngine {
	class ShaderProgram;
	class Texture2D;
	class Sprite;
	class AnimatedSprite;
}

// ��� ����������� ��� �� ����� ����  ������������ ����� � �� �������

class ResourceManager {
public:
	static void setExecutablePath(const std::string& executablPath);
	static void unloadAllResources();

	~ResourceManager() = delete;
	ResourceManager() = delete;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const  ResourceManager&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	
	static std::shared_ptr<RendererEngine::ShaderProgram> loadShaders(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);
	static std::shared_ptr<RendererEngine::ShaderProgram> getShaderProgram(const std::string& shaderName);

	static std::shared_ptr<RendererEngine::Texture2D> loadTexture(const std::string& textureName, const std::string& texturePath);
	static std::shared_ptr<RendererEngine::Texture2D> getTexture(const std::string& textureName);

	static std::shared_ptr<RendererEngine::Sprite> loadSprite(const std::string& spriteName,
												 const std::string& textureName, 
												 const std::string& shaderName,
												 const unsigned int spriteWidth,
												 const unsigned int spriteHeight,
												 const std::string& subTextureName = "default");
	static std::shared_ptr<RendererEngine::Sprite> getSprite(const std::string& spriteName);

	static std::shared_ptr<RendererEngine::AnimatedSprite> loadAnimatedSprite(const std::string& spriteName,
																 const std::string& textureName,
																 const std::string& shaderName,
																 const unsigned int spriteWidth,
																 const unsigned int spriteHeight,
				                                                 const std::string& subTextureName = "default");
	static std::shared_ptr<RendererEngine::AnimatedSprite> getAnimatedSprite(const std::string& spriteName);

	static std::shared_ptr<RendererEngine::Texture2D>  loadTextureAtlas(const std::string& textureName,
														   const std::string& texturePath,
		                                                   std::vector<std::string> subTextures,
			                                               const unsigned int subTextureWidth,
			                                               const unsigned int subTextureHeiht);	

	static bool loadJSONresources(const std::string& JSONpath);
private:

	static std::string getFileString(const std::string& relativePath);

	typedef std::map< const std::string, std::shared_ptr<RendererEngine::ShaderProgram>> ShaderProgramsMap;
	static ShaderProgramsMap m_shaderPrograms;

	typedef std::map<const std::string, std::shared_ptr<RendererEngine::Texture2D>> TexturesMap;
	static TexturesMap m_textures;


	typedef std::map<const std::string, std::shared_ptr<RendererEngine::Sprite>> SpritesMap;
	static SpritesMap m_sprites;

	static std::string m_Path;

	typedef std::map<const std::string, std::shared_ptr<RendererEngine::AnimatedSprite>> AnimatedSpritesMap;
	static AnimatedSpritesMap m_animatedSprites;
};
