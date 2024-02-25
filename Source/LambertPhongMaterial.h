#pragma once
#include "LambertMaterial.h"

class LambertPhongMaterial final : public LambertMaterial
{
public:
	explicit LambertPhongMaterial(const Elite::RGBColor& diffuseReflectance, float specularReflectance, float phongExponent);
	~LambertPhongMaterial() override = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const override;
	virtual Elite::RGBColor GetReflectance() const override { return Elite::RGBColor{ m_SpecularReflectance, m_SpecularReflectance, m_SpecularReflectance }; }

private:
	float m_SpecularReflectance;
	float m_PhongExponent;
};

