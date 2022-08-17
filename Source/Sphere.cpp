#include "Sphere.h"

Sphere::Sphere(const Elite::FMatrix4& transform, float radius, Material* pMaterial)
	: Object(transform, pMaterial)
	, m_Radius{ radius }
{}

bool Sphere::HitCheck(HitRecord& hitRecord, bool isShadowTest) const
{
	//Change to a ref frame centered on the shpere
	Elite::FPoint3 centerPos{ m_Transform[3][0], m_Transform[3][1], m_Transform[3][2] };
	Elite::FVector3 rayToCenter{ hitRecord.ray.origin - centerPos };

	float a = Dot(hitRecord.ray.direction, hitRecord.ray.direction);
	float b = 2 * (Dot(hitRecord.ray.direction, rayToCenter));
	float c = Dot(rayToCenter, rayToCenter) - m_Radius * m_Radius;

	float d = b * b - 4 * a * c;

	if (d < 0.f)
		return false;

	float sqrtD{ sqrtf(d) };
	float t1{ (-b - sqrtD) / (a * 2) };
	if (t1 >= hitRecord.ray.minT && t1 <= hitRecord.t)
	{
		if (isShadowTest)
			return true;

		hitRecord.t = t1;
	}
	else
	{
		if (isShadowTest)
			return false;

		float t2{ (-b + sqrtD) / (a * 2) };
		if (t2 >= hitRecord.ray.minT && t2 <= hitRecord.t)
		{
			hitRecord.t = t2;
		}
		else
		{
			return false;
		}
	}

	Elite::FVector3 centerToHitPoint{ hitRecord.ray.GetPoint(hitRecord.t) - centerPos };
	Normalize(centerToHitPoint);

	hitRecord.pMaterial = m_pMaterial;
	hitRecord.normal = centerToHitPoint;

	return true;

	//Elite::FVector3 rayToCenter{ centerPos - hitRecord.ray.origin };
	//float dist = Dot(rayToCenter, hitRecord.ray.direction);
	//Elite::FPoint3 closestPointCenter{ hitRecord.ray.origin + dist * hitRecord.ray.direction };
	//float distFromCenter2{ Elite::SqrDistance(closestPointCenter, centerPos) };
	//float radius2{ m_Radius * m_Radius };

	//if (distFromCenter2 > radius2)
	//	return false;

	//float intersectOffset{ sqrtf(radius2 - distFromCenter2) };
}