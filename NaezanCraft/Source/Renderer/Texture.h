#pragma once

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind();
	inline const unsigned int GetTextureID() { return textureID; }

private:
	unsigned int textureID;
};