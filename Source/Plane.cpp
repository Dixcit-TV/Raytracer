#include "Plane.h"
#include "Utils.h"

Plane::Plane(const Elite::FMatrix4& transform, Material* pMaterial)
	: Object(transform, pMaterial)
	, m_Normal{ transform * Elite::FVector4(Utils::GetWorldY<float>()) }
{}

bool Plane::HitCheck(HitRecord& hitRecord, bool isShadowTest) const
{
	float dDotN{ Dot(hitRecord.ray.direction, m_Normal) };

	if (dDotN >= -0.00001f)
		return false;

	float t = Dot((Elite::FPoint3(m_Transform[3][0], m_Transform[3][1], m_Transform[3][2]) - hitRecord.ray.origin), m_Normal) / dDotN;

	if (t <= hitRecord.ray.minT || t >= hitRecord.t)
		return false;

	if (isShadowTest)
		return true;

	hitRecord.t = t;
	hitRecord.pMaterial = m_pMaterial;
	hitRecord.normal = m_Normal;

	return true;
}