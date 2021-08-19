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
	//Möller-Trumblore algorithm: 
	//http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/raytri_tam.pdf
	//https://www.youtube.com/watch?v=fK1RPmF_zjQ

	Elite::FPoint3 worldV0, worldV1, worldV2;
	Elite::FVector3 edge01, edge02, V0ToRay, rayDir_x_edge02, V0ToRay_x_edge01;
	float det, inv_det, u, v, t;
	const float epsilon{ 0.0001f };

	worldV0 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[0]));
	worldV1 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[1]));
	worldV2 = Elite::FPoint3(m_Transform * Elite::FPoint4(m_Vertices[2]));

	edge01 = worldV1 - worldV0;
	edge02 = worldV2 - worldV0;
	rayDir_x_edge02 = Elite::Cross(hitRecord.ray.direction, edge02);
	det = Elite::Dot(rayDir_x_edge02, edge01);

	switch (m_CullMode)
	{
	case CullMode::BACKFACE:
		if ((!isShadowTest && det < epsilon) || (isShadowTest && det > -epsilon))
			return false;
		break;
	case CullMode::FRONTFACE:
		if ((isShadowTest && det < epsilon) || (!isShadowTest && det > -epsilon))
			return false;
		break;
	default:
		if (abs(det) < epsilon)
			return false;
		break;
	}

	inv_det = 1 / det;
	V0ToRay = hitRecord.ray.origin - worldV0;
	u = Elite::Dot(rayDir_x_edge02, V0ToRay) * inv_det;

	if (u < 0.f || u > 1.f)
		return false;

	V0ToRay_x_edge01 = Elite::Cross(V0ToRay, edge01);
	v = Elite::Dot(V0ToRay_x_edge01, hitRecord.ray.direction) * inv_det;

	if (v < 0.f || u + v > 1.f)
		return false;

	t = Elite::Dot(V0ToRay_x_edge01, edge02) * inv_det;

	if (t < hitRecord.ray.minT || t > hitRecord.t)
		return false;
	
	if (isShadowTest)
		return true;

	hitRecord.t = t;
	hitRecord.pMaterial = m_pMaterial;
	hitRecord.normal = Elite::GetNormalized(Elite::Cross(edge01, edge02));

	if (m_CullMode == CullMode::FRONTFACE)
		hitRecord.normal *= -1.f;

	return true;
}