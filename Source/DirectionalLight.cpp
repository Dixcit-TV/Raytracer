#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const Elite::FVector3& direction, float intensity, const Elite::RGBColor& color, bool isOn)
	: Light(intensity, color, LightType::DIRECTIONAL, isOn)
	, m_Direction{ direction }
{}

Elite::FVector3 DirectionalLight::GetDirection(const Elite::FPoint3&) const
{
	return m_Direction;
}

Elite::RGBColor DirectionalLight::CalculateIllumination(const Elite::FPoint3&) const
{
	return m_Color * m_Intensity;
}