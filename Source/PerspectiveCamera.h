#pragma once
#include "EMath.h"
#include "Structs.h"

class PerspectiveCamera
{
public:
	explicit PerspectiveCamera() = default;
	explicit PerspectiveCamera(const Elite::FPoint3& position, const Elite::FVector3& nForward, float aspectRation, float fovAngleDeg = 45.f);
	~PerspectiveCamera() = default;

	Ray CastRay(const Elite::IPoint2& pixel, uint32_t windWidth, uint32_t winHeight);
	void Update(float deltaT);

private:
	Elite::FMatrix4 m_LookAtMatrix;
	float m_AspectRatio;
	float m_FOV;

	void Zoom(int zoomStrength);
	void Translate(const Elite::FVector3& offset);
	void RotateCamera(float yaw, float pitch, float roll);
};

