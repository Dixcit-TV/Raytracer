#pragma once
#include "Enums.h"
#include <SDL.h>

class ProjectSettings
{
public:
	static ProjectSettings* GetInstance();

	void OnKeyDownEvent(SDL_Event& e);

	bool IsDirectionalLightEnabled() const;
	bool IsPointLight1Enabled() const;
	bool IsPointLight2Enabled() const;
	bool IsHardShadowEnabled() const;
	LightRenderMode GetLightMode() const;

private:
	ProjectSettings()
		: m_LightMode(LightRenderMode::ALL)
		, m_DirectionalLightsEnabled(true)
		, m_PointLights1Enabled(true)
		, m_PointLights2Enabled(true)
		, m_HardShadowEnabled(true)
	{};

	static ProjectSettings* m_Instance;
	LightRenderMode m_LightMode;
	bool m_DirectionalLightsEnabled;
	bool m_PointLights1Enabled;
	bool m_PointLights2Enabled;
	bool m_HardShadowEnabled;
};

