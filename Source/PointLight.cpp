#include "PointLight.h"

PointLight::PointLight(const Elite::FPoint3& pos, float intensity, const Elite::RGBColor& color, bool isOn)
	: Light(intensity, color, LightType::POINTLIGHT, isOn)
	, m_Pos(pos)
{}

Elite::FVector3 PointLight::GetDirection(const Elite::FPoint3& lookAt) const
{
	return lookAt - m_Pos;
}

bool PointLight::IsOutOfRange(const Elite::FPoint3& hitPoint, const Elite::FVector3& hitNormal) const
{
	return Dot(hitNormal, m_Pos - hitPoint) <= FLT_EPSILON || (m_Intensity / SqrDistance(hitPoint, m_Pos)) < 0.001f;
}

Elite::RGBColor PointLight::CalculateIllumination(const Elite::FPoint3& hitPoint) const
{
	return m_Color * m_Intensity / SqrDistance(hitPoint, m_Pos);
}