#include "Light.h"

Light::Light(float intensity, const Elite::RGBColor& color, LightType type, bool isOn)
	: m_Intensity{ intensity }
	, m_Color{ color }
	, m_Type{ type }
	, m_IsOn{ isOn }
{}

LightType Light::GetType() const
{
	return m_Type;
}

bool Light::IsOn() const
{
	return m_IsOn;
}

void Light::SetIsOn(bool isOn)
{
	m_IsOn = isOn;
}