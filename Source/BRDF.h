#include "ERGBColor.h"
#include "EMath.h"

namespace BRDF
{
	Elite::RGBColor Lambert(const Elite::RGBColor& diffuseReflectance);

	Elite::RGBColor Phong(float specularReflectance, int phongExponent, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal);

	Elite::RGBColor Cook_Torrance(const Elite::RGBColor& dfg, const Elite::FVector3& lightVector, const Elite::FVector3& viewVector, const Elite::FVector3& normal);
}

namespace BRDFUtils
{
	float RoughnessNormalDistrubtion(const Elite::FVector3& normal, const Elite::FVector3& halfVector, float roughness);
	Elite::RGBColor FresnelReflectance(const Elite::FVector3& halfVector, const Elite::FVector3& viewVector, const Elite::RGBColor& albedo);
	float ShadowMaskIndex(const Elite::FVector3& normal, const Elite::FVector3& viewVector, float roughness);
}