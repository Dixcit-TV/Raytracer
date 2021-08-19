#include "ProjectSettings.h"
#include "SceneManager.h"

ProjectSettings* ProjectSettings::m_Instance = nullptr;

ProjectSettings* ProjectSettings::GetInstance() 
{
	return m_Instance ? m_Instance : (m_Instance = new ProjectSettings());
}

void ProjectSettings::OnKeyDownEvent(SDL_Event& e)
{
	switch (e.key.keysym.scancode)
	{
	case SDL_SCANCODE_J:
		m_DirectionalLightsEnabled = !m_DirectionalLightsEnabled;
		break;
	case SDL_SCANCODE_K:
		m_PointLights1Enabled = !m_PointLights1Enabled;
		break;
	case SDL_SCANCODE_L:
		m_PointLights2Enabled = !m_PointLights2Enabled;
		break;
	case SDL_SCANCODE_Z:
		m_HardShadowEnabled = !m_HardShadowEnabled;
		break;
	case SDL_SCANCODE_T:
		m_LightMode = LightRenderMode((int(m_LightMode) + 1) % int(LightRenderMode::COUNT));
		break;
	case SDL_SCANCODE_SPACE:
		SceneManager::GetInstance()->ToggleActiveScene();
		break;
	}
}

LightRenderMode ProjectSettings::GetLightMode() const
{
	return m_LightMode;
}

bool ProjectSettings::IsDirectionalLightEnabled() const
{
	return m_DirectionalLightsEnabled;
}

bool ProjectSettings::IsPointLight1Enabled() const
{
	return m_PointLights1Enabled;
}

bool ProjectSettings::IsPointLight2Enabled() const
{
	return m_PointLights2Enabled;
}

bool ProjectSettings::IsHardShadowEnabled() const
{
	return m_HardShadowEnabled;
}