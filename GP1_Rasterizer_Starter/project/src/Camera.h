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

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{};
		float totalYaw{};

		const float MoveSpeedKey{ 5.f };
		const float MoveSpeedMouse{ 0.05f };
		const float RotatedSpeed{ 0.01f };

		Matrix invViewMatrix{};
		Matrix viewMatrix{};

		void Initialize(float _fovAngle = 90.f, Vector3 _origin = {0.f,0.f,0.f})
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);

			origin = _origin;
		}

		void CalculateViewMatrix()
		{
			invViewMatrix = Matrix::CreateLookAtLH(origin, forward, up);
			viewMatrix = Matrix::Inverse(invViewMatrix);
		}

		void CalculateProjectionMatrix()
		{
			//TODO W3

			//ProjectionMatrix => Matrix::CreatePerspectiveFovLH(...) [not implemented yet]
			//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			Vector3 InputVector{};

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			if (pKeyboardState[SDL_SCANCODE_S])
				InputVector.z -= 1;

			if (pKeyboardState[SDL_SCANCODE_W])
				InputVector.z += 1;

			if (pKeyboardState[SDL_SCANCODE_A])
				InputVector.x -= 1;

			if (pKeyboardState[SDL_SCANCODE_D])
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
				InputVector.y -= mouseY * MoveSpeedMouse;
			}
			else if (!MouseRightPressed && MouseLeftPressed)
			{
				InputVector.z -= mouseY * MoveSpeedMouse;
				totalYaw -= mouseX * RotatedSpeed;
			}
			else if (MouseRightPressed && !MouseLeftPressed)
			{
				totalPitch -= mouseY * RotatedSpeed;
				totalYaw -= mouseX * RotatedSpeed;
			}


			const Matrix pitchYawRotation
			{
				Vector3{cosf(totalYaw), 0, sinf(totalYaw)},
				Vector3{sinf(totalYaw) * sinf(totalPitch), cosf(totalPitch), -sinf(totalPitch) * cosf(totalYaw)},
				Vector3{-cosf(totalPitch) * sinf(totalYaw), sinf(totalPitch), cosf(totalPitch) * cosf(totalYaw)},
				Vector3::Zero
			};

			forward = pitchYawRotation.TransformVector(Vector3::UnitZ);

			InputVector = pitchYawRotation.TransformVector(InputVector);

			origin += InputVector * deltaTime * MoveSpeedKey;

			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	};
}
