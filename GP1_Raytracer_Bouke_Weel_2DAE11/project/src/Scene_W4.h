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

	class Scene_W4_ReferenceScene final : public Scene
	{
	public:
		Scene_W4_ReferenceScene() = default;
		~Scene_W4_ReferenceScene() override = default;

		Scene_W4_ReferenceScene(const Scene_W4_ReferenceScene&) = delete;
		Scene_W4_ReferenceScene(Scene_W4_ReferenceScene&&) noexcept = delete;
		Scene_W4_ReferenceScene& operator=(const Scene_W4_ReferenceScene&) = delete;
		Scene_W4_ReferenceScene& operator=(Scene_W4_ReferenceScene&&) noexcept = delete;

		void Initialize() override;
		void Update(dae::Timer* pTimer) override;
	private:
		TriangleMesh* m_pMeshes[3]{};
	};

	class Scene_W4_Bunny final : public Scene
	{
	public:
		Scene_W4_Bunny() = default;
		~Scene_W4_Bunny() override = default;

		Scene_W4_Bunny(const Scene_W4_Bunny&) = delete;
		Scene_W4_Bunny(Scene_W4_Bunny&&) noexcept = delete;
		Scene_W4_Bunny& operator=(const Scene_W4_Bunny&) = delete;
		Scene_W4_Bunny& operator=(Scene_W4_Bunny&&) noexcept = delete;

		void Initialize() override;
	private:
		TriangleMesh* pMesh{};
	};

}
