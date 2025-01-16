#pragma once
#include "pch.h"
#include <string>
#include "SDL_surface.h"

namespace dae {
	class Texture final{
	public:
		static Texture* LoadFromFile(ID3D11Device* devicePtr,const std::string& path);

		~Texture();

		ColorRGB Sample(const Vector2& uv) const;

		[[nodiscard]] ID3D11ShaderResourceView* GetShaderResource() const
		{
			return m_pShaderResource;
		}
	private:
		Texture(ID3D11Device* pDivice,SDL_Surface* pSurface);

		//DirectX
		ID3D11Texture2D* m_pResource;
		ID3D11ShaderResourceView* m_pShaderResource;

		//sdl
		SDL_Surface* m_pSurface{ nullptr };
		uint32_t* m_pSurfacePixels{ nullptr };
	};
}
