#pragma once
#include "Light.h"

class DirectionalLight final : public Light
{
public:
	explicit DirectionalLight(const Elite::FVector3& direction, float intensity, const Elite::RGBColor& color, bool isOn = true);
	~DirectionalLight() override = default;

	Elite::FVector3 GetDirection(const Elite::FPoint3& lookAt) const override;

	Elite::RGBColor CalculateIllumination(const Elite::FPoint3& hitPoint) const override;

private:
	Elite::FVector3 m_Direction;
};

