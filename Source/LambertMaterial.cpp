#include "LambertMaterial.h"
#include "BRDF.h"

LambertMaterial::LambertMaterial(const Elite::RGBColor& diffuseReflectance)
	: Material()
	, m_DiffuseReflectance(diffuseReflectance)
{}

Elite::RGBColor LambertMaterial::Shade(const HitRecord&, const Elite::FVector3&) const
{
	return BRDF::Lambert(m_DiffuseReflectance);
}