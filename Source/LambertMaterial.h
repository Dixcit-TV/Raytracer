#pragma once
#include "Material.h"

class LambertMaterial : public Material
{
public:
	explicit LambertMaterial(float diffuseReflectance, const Elite::RGBColor& diffuseColor);
	virtual ~LambertMaterial() override = default;

	virtual Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const override;
	virtual Elite::RGBColor GetReflectance() const override { return Elite::RGBColor{}; }

private:
	Elite::RGBColor m_DiffuseColor;
	float m_DiffuseReflectance;
};

