#include "TriangleMesh.h"
#include "Utils.h"

TriangleMesh::TriangleMesh(const std::string& objPath, const Elite::FMatrix4& transform, Material* pMaterial, bool isStatic, CullMode cullMode)
	: Object(transform, pMaterial)
	, m_vBuffer()
	, m_iBuffer()
	, m_CullMode(cullMode)
	, m_IsStatic(isStatic)
{
	ObjReader::LoadModel(objPath, m_vBuffer, m_iBuffer);

	if (isStatic)
	{
		for (Elite::FPoint3& vertex : m_vBuffer)
		{
			vertex = Elite::FPoint3(m_Transform * Elite::FPoint4(vertex));
		}
	}
}

bool TriangleMesh::HitCheck(HitRecord& hitRecord, bool isShadowTest) const
{
	//Möller-Trumblore algorithm: 
	//http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/raytri_tam.pdf
	//https://www.youtube.com/watch?v=fK1RPmF_zjQ

	bool result{ false };
	const size_t idxCount{ m_iBuffer.size() };
	for (size_t idx{}; idx < idxCount; idx += 3)
	{
		Elite::FPoint3 worldV0, worldV1, worldV2;
		Elite::FVector3 edge01, edge02, V0ToRay, rayDir_x_edge02, V0ToRay_x_edge01;
		float det, inv_det, u, v, t;
		const float epsilon{ 0.0001f };

		//Dynamic meshes: Calculate vertex position each frame
		//Static meshes: Use pre-calculated vertex position
		int vIdx{ m_iBuffer[idx] };
		worldV0 = m_IsStatic ? m_vBuffer[vIdx] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx]));
		vIdx = m_iBuffer[idx + 1];
		worldV1 = m_IsStatic ? m_vBuffer[vIdx] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx]));
		vIdx = m_iBuffer[idx + 2];
		worldV2 = m_IsStatic ? m_vBuffer[vIdx] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx]));

		edge01 = worldV1 - worldV0;
		edge02 = worldV2 - worldV0;
		rayDir_x_edge02 = Elite::Cross(hitRecord.ray.direction, edge02);
		det = Elite::Dot(rayDir_x_edge02, edge01);

		//Allow Cull mode handling
		switch (m_CullMode)
		{
		case CullMode::BACKFACE:
			if ((!isShadowTest && det < epsilon) || (isShadowTest && det > -epsilon))
				continue;
			break;
		case CullMode::FRONTFACE:
			if ((isShadowTest && det < epsilon) || (!isShadowTest && det > -epsilon))
				continue;
			break;
		default:
			if (abs(det) < epsilon)
				continue;
			break;
		}

		inv_det = 1 / det;
		V0ToRay = hitRecord.ray.origin - worldV0;
		u = Elite::Dot(rayDir_x_edge02, V0ToRay) * inv_det;

		//Check validity of first baricentric coordinate, discard triangle if invalid
		if (u < 0.f || u > 1.f)
			continue;

		V0ToRay_x_edge01 = Elite::Cross(V0ToRay, edge01);
		v = Elite::Dot(V0ToRay_x_edge01, hitRecord.ray.direction) * inv_det;

		//Check validity of second baricentric coordinate, discard triangle if invalid
		if (v < 0.f || u + v > 1.f)
			continue;

		t = Elite::Dot(V0ToRay_x_edge01, edge02) * inv_det;

		//Check if the hit point is neither too far or if no closer hit point was already found
		//discard triangle if it's the case
		if (t < hitRecord.ray.minT || t > hitRecord.t)
			continue;

		//In case of shadow tracing, just stop any further check as soon as one obstructing hit was found
		if (isShadowTest)
		{
			result = true;
			break;
		}

		hitRecord.t = t;
		hitRecord.pMaterial = m_pMaterial;
		hitRecord.normal = Elite::GetNormalized(Elite::Cross(edge01, edge02));

		if (m_CullMode == CullMode::FRONTFACE)
			hitRecord.normal *= -1.f;

		result = true;
	}

	return result;
}