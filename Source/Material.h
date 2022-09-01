#pragma once
#include "EMath.h"
#include "ERGBColor.h"

struct HitRecord;

class Material
{
public:
	virtual ~Material() = default;

	virtual Elite::RGBColor Shade(const HitRecord& hitRecord, const Elite::FVector3& lightVector) const = 0;
	virtual Elite::RGBColor GetReflectance() const = 0;

protected:
	Material() = default;
};

