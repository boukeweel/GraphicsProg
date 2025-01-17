#include "Texture.h"

#include <cassert>

namespace dae
{
	Texture* Texture::LoadFromFile(ID3D11Device* devicePtr, const std::string& path)
	{
		SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
		if (!pSurface)
		{
			std::cout << "Texture did not load in with file path: " + path + "\n";
			return nullptr;
		}

		Texture* pTexture = new Texture(devicePtr, pSurface);

		SDL_FreeSurface(pSurface);
		pSurface = nullptr;

		return pTexture;
	}

	Texture* Texture::LoadFromFile(const std::string& path)
	{
		SDL_Surface* pSurface{ IMG_Load(path.c_str()) };
		if (!pSurface)
		{
			std::cout << "Texture did not load in with file path: " + path + "\n";
			return nullptr;
		}

		return new Texture(pSurface);
	}

	Texture::Texture(SDL_Surface* pSurface):
		m_pSurface{ pSurface }, m_pSurfacePixels{ static_cast<uint32_t*>(m_pSurface->pixels) },m_UseDirectX{false}
	{
		m_UseDirectX = false;
	}


	Texture::Texture(ID3D11Device* pDivice, SDL_Surface* pSurface):
	m_UseDirectX{true}
	{
		HRESULT result;

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};

		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		result = pDivice->CreateTexture2D(&desc, &initData, &m_pResource);

		if (FAILED(result))
			assert(true && "Failed To make Texture2d");

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};

		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		result = pDivice->CreateShaderResourceView(m_pResource, &srvDesc, &m_pShaderResource);
		if (FAILED(result))
			assert(true && "Failed To make Shader Resource View");

	}

	Texture::~Texture()
	{
		if(m_UseDirectX)
		{
			SafeRelease(m_pResource)
			SafeRelease(m_pShaderResource)
		}
		else
		{
			if (m_pSurface)
			{
				SDL_FreeSurface(m_pSurface);
				m_pSurface = nullptr;
			}
		}
	}

	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		if (m_UseDirectX)
			return { 0,0,0 };

		int texWidth = m_pSurface->w;
		int texHeight = m_pSurface->h;

		int x = static_cast<int>(uv.x * (texWidth - 1));
		int y = static_cast<int>(uv.y * (texHeight - 1));

		uint32_t pixelColor = m_pSurfacePixels[x + y * texWidth];

		uint8_t r, g, b;
		SDL_GetRGB(pixelColor, m_pSurface->format, &r, &g, &b);

		return ColorRGB{ r / 255.f, g / 255.f, b / 255.f };
	}

	
}
