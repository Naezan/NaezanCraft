#include "pch.h"
#include "TextureManager.h"
#include "Renderer/Texture.h"

std::map<std::string, std::shared_ptr<Texture>> TextureManager::textures;

TextureManager::TextureManager() = default;

TextureManager::~TextureManager() = default;

void TextureManager::AddTexture(const std::string& key, const std::string& path)
{
	textures.emplace(key, std::make_shared<Texture>(path));
}

std::weak_ptr<Texture> TextureManager::GetTexture(const std::string& key)
{
	return textures[key];
}

void TextureManager::BindTexture(const std::string& key)
{
	textures[key]->Bind();
}
