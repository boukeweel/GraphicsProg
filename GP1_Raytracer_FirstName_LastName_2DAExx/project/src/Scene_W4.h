#pragma once
#include "Scene.h"

namespace dae
{
	class Scene_W4 final : public Scene
	{
	public:
		Scene_W4() = default;
		~Scene_W4() override = default;

		Scene_W4(const Scene_W4&) = delete;
		Scene_W4(Scene_W4&&) noexcept = delete;
		Scene_W4& operator=(const Scene_W4&) = delete;
		Scene_W4& operator=(Scene_W4&&) noexcept = delete;

		void Initialize() override;
		void Update(dae::Timer* pTimer) override;
	private:
		TriangleMesh* pMesh{ nullptr };
	};

}
