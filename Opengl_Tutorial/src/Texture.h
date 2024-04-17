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
	int m_Width, m_Height;
public:
	Texture(const char* path, const TextureParameters& textureParams);
	~Texture();
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
	unsigned int GetFormat(unsigned int nChannels) const;
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};
