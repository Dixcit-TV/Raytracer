#include "Triangle.h"
#include "Utils.h"
#include "Quaternion.h"

Triangle::Triangle(const Elite::FPoint3& v0, const Elite::FPoint3& v1, const Elite::FPoint3& v2, const Elite::FMatrix4& transform, Material* pMaterial, CullMode cullMode)
	: Object(transform, pMaterial)
	, m_Vertices{ v0, v1, v2 }
	, m_CullMode{ cullMode }
{}

void Triangle::Update(float deltaT)
{
	Rotate(m_Transform, Quaternion<float>(deltaT, Utils::GetWorldY<float>()));
}

bool Triangle::HitCheck(HitRecord & hitRecord, bool isShadowTest) const
{
	Elite::FPoint3 worldV0 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[0]));
	Elite::FPoint3 worldV1 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[1]));
	Elite::FPoint3 worldV2 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[2]));

	bool result{ Math::RayTriangleInterestionTest(hitRecord, worldV0, worldV1, worldV2, m_CullMode, isShadowTest) };
	if (result)
		hitRecord.pMaterial = m_pMaterial;

	return result;
}