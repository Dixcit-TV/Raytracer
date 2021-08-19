#include "Structs.h"

//----------RAY-----------//
Ray::Ray(const Elite::FPoint3& origin, const Elite::FVector3& direction, float minT, float maxT)
	: origin(origin)
	, direction(direction)
	, minT(minT)
	, maxT(maxT)
{}

Elite::FPoint3 Ray::GetPoint(float t) const
{
	return origin + direction * t;
}
//----------END RAY----------//

//----------HitRecord-----------//
HitRecord::HitRecord(const Ray& ray)
	: ray(ray)
	, pMaterial(nullptr)
	, t(ray.maxT)
	, normal()
{}
//----------END HitRecord----------//