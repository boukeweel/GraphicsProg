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


	Texture::Texture(ID3D11Device* pDivice, SDL_Surface* pSurface)
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
		SafeRelease(m_pResource)
		SafeRelease(m_pShaderResource)
	}
}
