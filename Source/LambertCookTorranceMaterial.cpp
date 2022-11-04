#include "LambertCookTorranceMaterial.h"
#include "Structs.h"
#include "BRDF.h"

LambertCookTorranceMaterial::LambertCookTorranceMaterial(const Elite::RGBColor& albedo, float roughness, bool isMetal)
	: m_Albedo(albedo)
	, m_Roughness(roughness * roughness)
	, m_IsMetal(isMetal)
{}

Elite::RGBColor LambertCookTorranceMaterial::Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const
{
	Elite::FVector3 viewVector{ -hitRecord.ray.direction };
	Elite::FVector3 halfVector{ Elite::GetNormalized(viewVector + lightVector) };

	Elite::RGBColor albedo{ m_IsMetal ? m_Albedo : Elite::RGBColor{ 0.004f, 0.004f, 0.004f } };
	float d{ BRDFUtils::RoughnessNormalDistrubtion(hitRecord.normal, halfVector, m_Roughness) };
	Elite::RGBColor f{ BRDFUtils::FresnelReflectance(halfVector, viewVector, albedo) };
	float g{ BRDFUtils::ShadowMaskIndex(hitRecord.normal, viewVector, m_Roughness) * BRDFUtils::ShadowMaskIndex(hitRecord.normal, lightVector, m_Roughness) };

	Elite::RGBColor brdfCT{ BRDF::Cook_Torrance((f * d * g), lightVector, viewVector, hitRecord.normal) };
	
	if (m_IsMetal)
		return brdfCT;
	
	return BRDF::Lambert((Elite::RGBColor(1.f, 1.f, 1.f) - f) * m_Albedo) + brdfCT;
}