#include "BRDF.h"

Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffuseReflectance)
{
	return diffuseReflectance / float(E_PI);
}

Elite::RGBColor BRDF::Phong(float specularReflectance, float phongExponent, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal)
{
	const Elite::FVector3 reflect{ Reflect(lightVector, normal) };
	const float rDotV = std::max(Elite::Dot(reflect, viewVector), 0.f);
	return Elite::RGBColor(specularReflectance, specularReflectance, specularReflectance) * powf(rDotV, phongExponent);
}

Elite::RGBColor BRDF::Cook_Torrance(const Elite::RGBColor& dfg, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal)
{
	const float vDotN{ std::max(Elite::Dot(viewVector, normal), 0.f) };
	const float lDotN{ std::max(Elite::Dot(lightVector, normal), 0.f) };
	return dfg / std::max((4.f * vDotN * lDotN), 0.000001f);
}

float BRDFUtils::RoughnessNormalDistrubtion(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float roughness)
{
	float nDotH{ std::max(Elite::Dot(normal, halfVector), 0.f) };
	float roughnessSqr{ roughness * roughness };
	float d{ nDotH * nDotH * (roughnessSqr - 1) + 1 };
	return roughnessSqr / (float(E_PI) * d * d);
}

Elite::RGBColor BRDFUtils::FresnelReflectance(const Elite::FVector3& halfVector, const Elite::FVector3& viewVector, const Elite::RGBColor& albedo)
{
	const float hDotV{ std::max(Elite::Dot(halfVector, viewVector), 0.f) };
	return albedo + (Elite::RGBColor(1.f, 1.f, 1.f) - albedo) * powf(1 - hDotV, 5.f);
}

float BRDFUtils::ShadowMaskIndex(const Elite::FVector3& normal, const Elite::FVector3& v, float roughness)
{
	float nDotV{ std::max(Elite::Dot(normal, v), 0.f) };
	float k{ (roughness + 1.f) * (roughness + 1.f) / 8.f };
	return nDotV / (nDotV * (1 - k) + k);
}