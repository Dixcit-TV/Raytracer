#include "LambertPhongMaterial.h"
#include "Structs.h"
#include "BRDF.h"

LambertPhongMaterial::LambertPhongMaterial(const Elite::RGBColor& diffuseReflectance, float specularReflectance, float phongExponent)
	: LambertMaterial(diffuseReflectance * (1.f - specularReflectance))
	, m_SpecularReflectance(specularReflectance)
	, m_PhongExponent(phongExponent)
{}

Elite::RGBColor LambertPhongMaterial::Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const
{
	return LambertMaterial::Shade(hitRecord, lightVector) + BRDF::Phong(m_SpecularReflectance, m_PhongExponent, lightVector, -hitRecord.ray.direction, hitRecord.normal);
}