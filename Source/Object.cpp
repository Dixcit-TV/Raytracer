#include "Object.h"

Object::Object(const Elite::FMatrix4& transform, Material* pMaterial)
	: m_Transform{ transform }
	, m_pMaterial{ pMaterial }
{}