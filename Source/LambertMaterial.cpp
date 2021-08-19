#include "LambertMaterial.h"
#include "BRDF.h"

LambertMaterial::LambertMaterial(float diffuseReflectance, const Elite::RGBColor& diffuseColor)
	: Material()
	, m_DiffuseColor(diffuseColor)
	, m_DiffuseReflectance(diffuseReflectance)
{}

Elite::RGBColor LambertMaterial::Shade(const HitRecord&, const Elite::FVector3&) const
{
	return BRDF::Lambert(Elite::RGBColor(m_DiffuseReflectance, m_DiffuseReflectance, m_DiffuseReflectance), m_DiffuseColor);
}