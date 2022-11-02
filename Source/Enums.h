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
	ALL					= 0b11
	, BRDFONLY			= 0b01
	, LIGHTSOURCETONLY	= 0b10
	, COUNT = 3
};