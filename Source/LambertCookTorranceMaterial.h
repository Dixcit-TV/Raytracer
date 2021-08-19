#pragma once
#include "Material.h"
#include "Enums.h"

class LambertCookTorranceMaterial final : public Material
{
public:
	explicit LambertCookTorranceMaterial(const Elite::RGBColor& albedo, float roughness, bool isMetal);
	~LambertCookTorranceMaterial() override = default;

	Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const override;

private:
	Elite::RGBColor m_Albedo;
	float m_Roughness;
	bool m_IsMetal;
};

