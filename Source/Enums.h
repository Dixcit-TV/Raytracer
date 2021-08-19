#pragma once

enum class CullMode
{
	NONE
	, BACKFACE
	, FRONTFACE
};

enum class LightType
{
	POINTLIGHT
	, DIRECTIONAL
};

enum class LightRenderMode
{
	ALL
	, BRDFONLY
	, LIGHTSOURCETONLY
	, COUNT = 3
};