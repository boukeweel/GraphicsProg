#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle, float _aspectRatio):
			m_Origin{_origin},
			M_FovAngle{_fovAngle},
			m_AspectRatio{_aspectRatio}
		{
		}

		Vector3 m_Origin{};
		float M_FovAngle{90.f};
		float m_Fov{ tanf((M_FovAngle * TO_RADIANS) / 2.f) };
		float m_AspectRatio{};

		float m_NearClippingPlane = .1f;
		float m_FarClippingPlane = 1000.f;

		Vector3 m_Forward{Vector3::UnitZ};
		Vector3 m_Up{Vector3::UnitY};
		Vector3 m_Right{Vector3::UnitX};

		float m_TotalPitch{};
		float m_TotalYaw{};

		const float m_MoveSpeedKey{ 5.f };
		const float m_MoveSpeedMouse{ 0.5f };
		const float m_RotatedSpeed{ 0.01f };

		Matrix m_InvViewMatrix{};
		Matrix m_ViewMatrix{};

		Matrix m_ProjectionMatrix;

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f}, float _aspectRatio = 0)
		{
			M_FovAngle = _fovAngle;
			m_Fov = tanf((M_FovAngle * TO_RADIANS) / 2.f);
			m_AspectRatio = _aspectRatio;
			m_Origin = _origin;
		}

		void CalculateViewMatrix()
		{
			m_InvViewMatrix = Matrix::CreateLookAtLH(m_Origin, m_Forward, m_Up);
			m_ViewMatrix = m_InvViewMatrix.Inverse();
		}

		void CalculateProjectionMatrix()
		{
			const float A{ m_FarClippingPlane / (m_FarClippingPlane - m_NearClippingPlane) };
			const float B{ -(m_FarClippingPlane * m_NearClippingPlane) / (m_FarClippingPlane - m_NearClippingPlane) };

			m_ProjectionMatrix = Matrix{
				{1.f / (m_AspectRatio * m_Fov),0,0,0},
				{0,1.f / m_Fov,0,0},
				{0,0,A,1},
				{0,0,B,0}
			};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			Vector3 InputVector{};

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
				InputVector.z -= 1;

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
				InputVector.z += 1;

			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
				InputVector.x -= 1;

			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
				InputVector.x += 1;

			if (pKeyboardState[SDL_SCANCODE_Q])
				InputVector.y -= 1;

			if (pKeyboardState[SDL_SCANCODE_E])
				InputVector.y += 1;

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const bool MouseRightPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0 };
			const bool MouseLeftPressed{ (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0 };

			if (MouseRightPressed && MouseLeftPressed)
			{
				InputVector.y -= mouseY * m_MoveSpeedMouse;
			}
			else if (!MouseRightPressed && MouseLeftPressed)
			{
				InputVector.z -= mouseY * m_MoveSpeedMouse;
				m_TotalYaw -= mouseX * m_RotatedSpeed;
			}
			else if (MouseRightPressed && !MouseLeftPressed)
			{
				m_TotalPitch -= mouseY * m_RotatedSpeed;
				m_TotalYaw -= mouseX * m_RotatedSpeed;
			}

			const Matrix pitchYawRotation
			{
				Vector3{cosf(m_TotalYaw), 0, sinf(m_TotalYaw)},
				Vector3{sinf(m_TotalYaw) * sinf(m_TotalPitch), cosf(m_TotalPitch), -sinf(m_TotalPitch) * cosf(m_TotalYaw)},
				Vector3{-cosf(m_TotalPitch) * sinf(m_TotalYaw), sinf(m_TotalPitch), cosf(m_TotalPitch) * cosf(m_TotalYaw)},
				Vector3::Zero
			};

			m_Forward = pitchYawRotation.TransformVector(Vector3::UnitZ);
			m_Right = Vector3::Cross(Vector3::UnitY, m_Forward).Normalized();
			m_Up = Vector3::Cross(m_Forward, m_Right).Normalized();

			InputVector = pitchYawRotation.TransformVector(InputVector);

			m_Origin += InputVector * deltaTime * m_MoveSpeedKey;

			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when m_Fov/aspectRatio changes
		}
	};
}
