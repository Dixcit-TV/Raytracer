#include "BRDF.h"

Elite::RGBColor BRDF::Lambert(const Elite::RGBColor& diffuseReflectance, const Elite::RGBColor& diffuseColor)
{
	return diffuseColor * diffuseReflectance / float(E_PI);
}

Elite::RGBColor BRDF::Phong(float specularReflectance, int phongExponent, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal)
{
	float lDotN = Elite::Dot(lightVector, normal);
	return Elite::RGBColor(1.f, 1.f, 1.f) * specularReflectance * powf(Elite::Dot(lightVector - (lDotN + lDotN) * normal, -viewVector), float(phongExponent));
}

Elite::RGBColor BRDF::Cook_Torrance(const Elite::RGBColor& dfg, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal, const Elite::RGBColor& albedo)
{
	return dfg / (4 * Elite::Dot(viewVector, normal) * Elite::Dot(lightVector, normal));
}

float BRDFUtils::RoughnessNormalDistrubtion(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float roughness)
{
	float nDotH{ Elite::Dot(normal, halfVector) };
	float roughnessSqr{ roughness * roughness };
	float d{ nDotH * nDotH * (roughnessSqr - 1) + 1 };
	return roughnessSqr / (float(E_PI) * d * d);
}

Elite::RGBColor BRDFUtils::FresnelReflectance(const Elite::FVector3& halfVector, const Elite::FVector3& viewVector, const Elite::RGBColor& albedo)
{
	return albedo + (Elite::RGBColor(1.f, 1.f, 1.f) - albedo) * powf(1 - Elite::Dot(halfVector, viewVector), 5.f);
}

float BRDFUtils::ShadowMaskIndex(const Elite::FVector3& normal, const Elite::FVector3& viewVector, float roughness)
{
	float nDotV{ Elite::Dot(normal, viewVector) };
	float k{ (roughness + 1) * (roughness + 1) / 8 };
	return nDotV / (nDotV * (1 - k) + k);
}