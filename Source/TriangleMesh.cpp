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
	bool result{ false };
	const size_t idxCount{ m_iBuffer.size() };
	for (size_t idx{}; idx < idxCount; idx += 3)
	{
		//Dynamic meshes: Calculate vertex position each frame
		//Static meshes: Use pre-calculated vertex position
		int vIdx0{ m_iBuffer[idx] };
		int vIdx1{ m_iBuffer[idx + 1] };
		int vIdx2{ m_iBuffer[idx + 2] };
		Elite::FPoint3 worldV0 = m_IsStatic ? m_vBuffer[vIdx0] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx0]));
		Elite::FPoint3 worldV1 = m_IsStatic ? m_vBuffer[vIdx1] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx1]));
		Elite::FPoint3 worldV2 = m_IsStatic ? m_vBuffer[vIdx2] : Elite::FPoint3(m_Transform * Elite::FPoint4(m_vBuffer[vIdx2]));

		if (Math::RayTriangleInterestionTest(hitRecord, worldV0, worldV1, worldV2, m_CullMode, isShadowTest))
		{
			result = true;
			hitRecord.pMaterial = m_pMaterial;
			if (isShadowTest)
				break;
		}
	}

	return result;
}