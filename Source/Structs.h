#pragma once
#include "EMath.h"
#include "ERGBColor.h"
#include "Enums.h"

class Material;

struct Bound
{
	Elite::FPoint3 min;
	Elite::FVector3 size;

	bool IsValid() const { return size.x > 0.f && size.y > 0.f && size.z > 0.f; }
};

struct Ray
{
	Elite::FPoint3 origin;
	Elite::FVector3 direction;
	float minT;
	float maxT;

	explicit Ray(const Elite::FPoint3& origin, const Elite::FVector3& direction, float minT = 0.0001f, float maxT = FLT_MAX);

	Elite::FPoint3 GetPoint(float t) const;
};

struct HitRecord
{
	explicit HitRecord(const Ray& ray);

	Ray ray;
	const Material* pMaterial;
	Elite::FPoint3 hitPosition;
	Elite::FVector3 normal;
	//Elite::FPoint3 coordinates;
	//Elite::RGBColor color;
	float t;
};

