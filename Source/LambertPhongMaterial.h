#pragma once
#include "LambertMaterial.h"

class LambertPhongMaterial final : public LambertMaterial
{
public:
	explicit LambertPhongMaterial(const Elite::RGBColor& diffuseColor, float specularReflectance, int phongExponent);
	~LambertPhongMaterial() override = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const override;

private:
	float m_SpecularReflectance;
	int m_PhongExponent;
};

