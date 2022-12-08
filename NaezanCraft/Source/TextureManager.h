#pragma once

class Texture;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	static void AddTexture(const std::string& key, const std::string& path);
	static std::shared_ptr<Texture> GetTexture(const std::string& key);
	static void BindTexture(const std::string& key);

private:
	static std::map<std::string, std::shared_ptr<Texture>> textures;
};