#pragma once
#include "EffectBase.h"

namespace dae {
	class EffectPartialCoverage final : public EffectBase 
	{
	public:
		EffectPartialCoverage(ID3D11Device* pDevice, const std::wstring& path);
	
	};

}