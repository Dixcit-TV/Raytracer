#pragma once
#include "Object.h"

class Sphere final : public Object
{
public:
	explicit Sphere(const Elite::FMatrix4& transform, float radius, Material* pMaterial);
	~Sphere() override = default;

	void Update(float) override {};
	bool HitCheck(HitRecord& hitRecord, bool isShadowTest = false) const override;

private:
	float m_Radius;
};

