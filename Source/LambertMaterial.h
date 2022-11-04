#pragma once
#include "Material.h"

class LambertMaterial : public Material
{
public:
	explicit LambertMaterial(const Elite::RGBColor& diffuseReflectance);
	virtual ~LambertMaterial() override = default;

	virtual Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const override;
	virtual Elite::RGBColor GetReflectance() const override { return Elite::RGBColor{}; }

private:
	Elite::RGBColor m_DiffuseReflectance;
};

