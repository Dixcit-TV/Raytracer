#pragma once
#include "Object.h"

class Plane final : public Object
{
public:
	explicit Plane(const Elite::FMatrix4& transform, Material* pMaterial);
	~Plane() override = default;

	void Update(float) override {};
	bool HitCheck(HitRecord& hitRecord, bool isShadowTest = false) const override;

private:
	Elite::FVector3 m_Normal;
};

