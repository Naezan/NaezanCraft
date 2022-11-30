#pragma once

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind();
	inline const unsigned int GetTextureID() { return textureID; }

	static std::shared_ptr<Texture> CreateTexture(const std::string& path)
	{
		return std::make_shared<Texture>(path);
	}
private:
	unsigned int textureID;
};