#include "PerspectiveCamera.h"
#include "SDL.h"
#include "Utils.h"
#include "Quaternion.h"

//#define DEBUG

PerspectiveCamera::PerspectiveCamera(const Elite::FPoint3& position, const Elite::FVector3& nForward, float aspectRation, float fovAngleDeg)
	: m_AspectRatio(aspectRation)
	, m_FOV(tanf(fovAngleDeg* float(E_TO_RADIANS) / 2))
	, m_LookAtMatrix()
{
	Elite::FVector3 right{ Cross(Utils::GetWorldY<float>(), nForward) };
	Normalize(right);
	Elite::FVector3 up{ Cross(nForward, right) };

	m_LookAtMatrix = Elite::FMatrix4(right.x, up.x, nForward.x, position.x
		, right.y, up.y, nForward.y, position.y
		, right.z, up.z, nForward.z, position.z
		, 0.f, 0.f, 0.f, 1.f);
}

Ray PerspectiveCamera::CastRay(const Elite::IPoint2& pixel, uint32_t windWidth, uint32_t winHeight)
{
	float xScreenSpace{ ((pixel.x + .5f) * 2.f / windWidth - 1) * m_AspectRatio * m_FOV };
	float yScreenSpace{ (-(pixel.y + .5f) * 2.f / winHeight + 1) * m_FOV };

	Elite::FVector4 direction = m_LookAtMatrix * Elite::FVector4(xScreenSpace, yScreenSpace, -1.f, 0.f);
	return Ray{ Elite::FPoint3{ m_LookAtMatrix(0, 3), m_LookAtMatrix(1, 3), m_LookAtMatrix(2, 3) }, Elite::FVector3(direction) };
}

void PerspectiveCamera::Update(float deltaT)
{
	Elite::FVector3 offset{};
	const float linearVelocity{ 5.f };
	const float mouseSensitivity{ 0.3f };

	float roll{ 0.f }, pitch{ 0.f };
	int zoom{};
	bool updated{ false };

	int mouseOffsetX{ 0 }, mouseOffsetY{ 0 };
	Uint32 mouseRelativeSate{ SDL_GetRelativeMouseState(&mouseOffsetX, &mouseOffsetY) };
	const Uint8* pScanCode{ SDL_GetKeyboardState(NULL) };
	bool mouseLeft{ bool(mouseRelativeSate & SDL_BUTTON(SDL_BUTTON_LEFT)) };
	bool mouseRight{ bool(mouseRelativeSate & SDL_BUTTON(SDL_BUTTON_RIGHT)) };

	if (pScanCode[SDL_SCANCODE_I] || pScanCode[SDL_SCANCODE_O])
	{
		updated = true;
		zoom = pScanCode[SDL_SCANCODE_O] - 1 * pScanCode[SDL_SCANCODE_I] * 1;
	}

	if (mouseLeft || mouseRight)
	{
		if (mouseOffsetY || mouseOffsetX)
		{
			float offsetAngle{ float(atan2(mouseOffsetY, mouseOffsetX)) };
			if (mouseLeft && mouseRight)
			{
				updated = true;
				offset.x += cosf(offsetAngle) * linearVelocity;
				offset.y += sinf(offsetAngle) * -linearVelocity;
			}
			else if (mouseLeft)
			{
				updated = true;
				offset.z += sin(offsetAngle) * linearVelocity;
				pitch -= mouseOffsetX;
			}
			else if (mouseRight)
			{
				updated = true;
				roll -= mouseOffsetY;
				pitch -= mouseOffsetX;
			}
		}

		Elite::FVector3 direction{ (pScanCode[SDL_SCANCODE_A] || pScanCode[SDL_SCANCODE_LEFT]) * -1.f + (pScanCode[SDL_SCANCODE_D] || pScanCode[SDL_SCANCODE_RIGHT]) * 1.f
								, (pScanCode[SDL_SCANCODE_S] || pScanCode[SDL_SCANCODE_DOWN]) * -1.f + (pScanCode[SDL_SCANCODE_W] || pScanCode[SDL_SCANCODE_UP]) * 1.f };

		if (direction != Elite::FVector3::ZeroVector())
		{
			updated = true;
			float offsetAngle{ atan2f(direction.y, direction.x) };
			offset.x += cosf(offsetAngle) * linearVelocity;
			offset.z += sinf(offsetAngle) * -linearVelocity;
		}
	}

	if (updated)
	{
		if (zoom != 0)
		{
			Zoom(zoom);
		}

		if (roll || pitch)
		{
			RotateCamera(roll * mouseSensitivity, pitch * mouseSensitivity, 0.f);
		}

		if (offset != Elite::FVector3::ZeroVector())
		{
			offset *= deltaT;
			Translate(offset);
		}
#if defined DEBUG
		Utils::PrintMatrix(m_LookAtMatrix);
#endif
	}
}

void PerspectiveCamera::Zoom(int zoomDirection)
{
	//10º fov angle
	const float minFOV{ 0.0874887f };
	const float maxFOV{ 3.14159265359f };

	m_FOV += zoomDirection * 0.01f;
	if (m_FOV < minFOV)
		m_FOV = minFOV;

	if (m_FOV > maxFOV)
		m_FOV = maxFOV;
}

void PerspectiveCamera::Translate(const Elite::FVector3& offset)
{
	m_LookAtMatrix *= MakeTranslation(offset);
}

void PerspectiveCamera::RotateCamera(float roll, float pitch, float)
{
	float rollRad{ roll * float(E_TO_RADIANS) }, pitchRad{ pitch * float(E_TO_RADIANS) };

	Rotate(m_LookAtMatrix, Quaternion<float>(pitchRad, Utils::GetWorldY<float>()) * Quaternion<float>(rollRad, Elite::FVector3(m_LookAtMatrix[0])));
}