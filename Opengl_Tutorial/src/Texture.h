#pragma once

struct TextureParameters {
	unsigned int wrapS;
	unsigned int wrapT;
	unsigned int minFilter;
	unsigned int magFilter;
};

class Texture
{
private:
	unsigned int m_ID;
public:
	Texture(const char* path, const TextureParameters& textureParams);
	~Texture();
	void Bind() const;
	void Unbind() const;
	unsigned int GetFormat(unsigned int nChannels) const;
};
