#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
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

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		const float movementSpeed{ 7.0f };
		const float rotationSpeed{ 20.0f };
		const float keyboardRotationSpeed{ 80.0f };

		bool updateONB{ true };


		Matrix CalculateCameraToWorld()
		{
			if (!updateONB)
			{
				return cameraToWorld;
			}

			//todo: W2
			/*assert(false && "Not Implemented Yet");
			return {};*/

			//No Roll
			Matrix rotationMatrix = Matrix::CreateRotationX(totalPitch * TO_RADIANS) * Matrix::CreateRotationY(totalYaw * TO_RADIANS);

			forward = rotationMatrix.GetAxisZ();
			right = rotationMatrix.GetAxisX();
			up = rotationMatrix.GetAxisY();

			// This is for when we have a static forward
			/*Vector3 right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			Vector3 up = Vector3::Cross(forward, right).Normalized();*/

			//ONB
			cameraToWorld = Matrix(
				Vector4(right,0),
				Vector4(up,0),
				Vector4(forward,0),
				Vector4(origin, 1)
			);

			updateONB = false;
			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			InputLogic(pTimer);
			
			//const float deltaTime = pTimer->GetElapsed();

			////Keyboard Input
			//const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			////Mouse Input
			//int mouseX{}, mouseY{};
			//const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			//float movementSpeed{1.f};

			//if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			//{
			//	movementSpeed *= 4;
			//}

			//if (pKeyboardState[SDL_SCANCODE_LEFT])
			//{
			//	fovAngle -= movementSpeed * deltaTime * 50.f;
			//	fovAngle = fmax(0.00001f, fovAngle);
			//}
			//if (pKeyboardState[SDL_SCANCODE_RIGHT])
			//{
			//	fovAngle += movementSpeed * deltaTime * 50.f;
			//	fovAngle = fmin(179.99999f, fovAngle);
			//}

			//if (pKeyboardState[SDL_SCANCODE_W])
			//{
			//	origin += forward * movementSpeed * deltaTime;
			//}
			//if (pKeyboardState[SDL_SCANCODE_S])
			//{
			//	origin -= forward * movementSpeed * deltaTime;
			//}
			//if (pKeyboardState[SDL_SCANCODE_D])
			//{
			//	origin += right * movementSpeed * deltaTime;
			//}
			//if (pKeyboardState[SDL_SCANCODE_A])
			//{
			//	origin -= right * movementSpeed * deltaTime;
			//}


			//if ((mouseState & SDL_BUTTON_LMASK) != 0)
			//{
			//	if ((mouseState & SDL_BUTTON_RMASK) != 0)
			//	{
			//		// up and down
			//		origin.y -= mouseY / 50.f;
			//		origin.x += mouseX / 50.f;
			//	}
			//	else
			//	{
			//		//I don't really want naything here for just left clicking tbh
			//		//But this doesn't run automatically
			//	}
			//}

			//if ((mouseState & SDL_BUTTON_RMASK) != 0)
			//{
			//	if (!(mouseState & SDL_BUTTON_LMASK) != 0)
			//	{
			//		updateONB = true;
			//		//rotate yaw
			//		totalYaw += mouseX;
			//		//rotate pitch
			//		totalPitch -= mouseY;
			//	}
			//}
		}
		
		void InputLogic(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			// Keyboard movement of the camera
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_SPACE])
			{
				origin += up * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_LSHIFT])
			{
				origin -= up * movementSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_UP])
			{
				totalPitch += keyboardRotationSpeed * deltaTime;
				updateONB = true;
			}
			if (pKeyboardState[SDL_SCANCODE_DOWN])
			{
				totalPitch -= keyboardRotationSpeed * deltaTime;
				updateONB = true;
			}
			if (pKeyboardState[SDL_SCANCODE_LEFT])
			{
				totalYaw -= keyboardRotationSpeed * deltaTime;
				updateONB = true;
			}
			if (pKeyboardState[SDL_SCANCODE_RIGHT])
			{
				totalYaw += keyboardRotationSpeed * deltaTime;
				updateONB = true;
			}
			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			// Mouse movements / rotation of the camera
			if ((mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) && mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				// mouseX yaw left & right, mouse Y moves forwards & backwards
				const float upwards = -mouseY * movementSpeed * deltaTime;
				origin += up * upwards;
				updateONB = true;
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
			{
				// mouseX yaw left & right, mouse Y moves forwards & backwards
				const float forwards = -mouseY * deltaTime;
				const float yaw = mouseX * deltaTime;

				origin += forward * forwards;
				totalYaw += yaw;
				updateONB = true;
			}
			else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
			{
				// Look around the current origin
				const float pitch = -mouseY * rotationSpeed * deltaTime;
				const float yaw = mouseX * rotationSpeed * deltaTime;

				totalPitch += pitch;
				totalYaw += yaw;
				updateONB = true;
			}
		}
	};
}
