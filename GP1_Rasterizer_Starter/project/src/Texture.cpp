#include "Texture.h"

#include <iostream>

#include "Vector2.h"
#include <SDL_image.h>
#include <stdexcept>

namespace dae
{
	Texture::Texture(SDL_Surface* pSurface) :
		m_pSurface{ pSurface },
		m_pSurfacePixels{ static_cast<uint32_t*>(m_pSurface->pixels) }
	{
	}

	Texture::~Texture()
	{
		if (m_pSurface)
		{
			SDL_FreeSurface(m_pSurface);
			m_pSurface = nullptr;
		}
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
		if(!pSurface)
		{
			std::cout << "Texture did not load in with file path: " + path + "\n";
			return nullptr;
		}

		return new Texture(pSurface);
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		int texWidth = m_pSurface->w;
		int texHeight = m_pSurface->h;

		int x = static_cast<int>(uv.x * (texWidth - 1));
		int y = static_cast<int>(uv.y * (texHeight - 1));

		uint32_t pixelColor = m_pSurfacePixels[x + y * texWidth];

		uint8_t r,g,b;
		SDL_GetRGB(pixelColor, m_pSurface->format, &r, &g, &b);

		return ColorRGB{r / 255.f, g / 255.f, b / 255.f};
	}
}
