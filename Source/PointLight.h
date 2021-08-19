#pragma once
#include "Light.h"

class PointLight final : public Light
{
public:
	explicit PointLight(const Elite::FPoint3& pos, float intensity, const Elite::RGBColor& color, bool isOn = true);
	~PointLight() override = default;

	Elite::FVector3 GetDirection(const Elite::FPoint3& lookAt) const override;

	Elite::RGBColor CalculateIllumination(const Elite::FPoint3& hitPoint) const override;

private:
	Elite::FPoint3 m_Pos;
};

