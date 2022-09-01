#include "Sphere.h"
#include "Utils.h"

Sphere::Sphere(const Elite::FMatrix4& transform, float radius, Material* pMaterial)
	: Object(transform, pMaterial)
	, m_Radius{ radius }
{
	m_Bound.min = Elite::FPoint3(m_Transform[3][0], m_Transform[3][1], m_Transform[3][2]) - Elite::FVector3(radius, radius, radius);
	m_Bound.size = Elite::FVector3(radius, radius, radius) * 2;
}

bool Sphere::HitCheck(HitRecord& hitRecord, bool isShadowTest) const
{
	//Change to a ref frame centered on the shpere
	Elite::FPoint3 centerPos{ m_Transform[3][0], m_Transform[3][1], m_Transform[3][2] };
	Elite::FVector3 cToO{ hitRecord.ray.origin - centerPos };

	//Ray Tracing Gems: High precision sphere intersection
	float b = Dot(-cToO, hitRecord.ray.direction);
	float r2 = m_Radius * m_Radius;
	Elite::FVector3 l = cToO + hitRecord.ray.direction * b;
	float d = r2 - Dot(l, l);

	if (d < 0.f)
		return false;

	float sqrtD{ sqrtf(d) };
	float c = Dot(cToO, cToO) - r2;

	int sign{ (b >= 0.f) - (b < 0.f) };
	float q{ b + sign * sqrtD };
	float t0{ c / q };

	// c >= 0 ? t0 (c / q) : t1 (q)
	float t = (t0 > 0.f) * t0 + (t0 <= 0.f) * q;
	bool isValidT = t > hitRecord.ray.minT && t < hitRecord.t;
	if (isShadowTest || !isValidT)
		return isShadowTest && isValidT;

	hitRecord.t = t;

	hitRecord.hitPosition = hitRecord.ray.GetPoint(t);
	Elite::FVector3 centerToHitPoint{ hitRecord.hitPosition - centerPos };
	Normalize(centerToHitPoint);

	hitRecord.pMaterial = m_pMaterial;
	hitRecord.normal = centerToHitPoint;

	return true;
}