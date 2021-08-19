#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::m_Instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	return m_Instance ? m_Instance : (m_Instance = new SceneManager());
}

void SceneManager::AddScene(SceneGraph&& newScene)
{
	m_Scenes.push_back(std::move(newScene));
}

SceneGraph& SceneManager::GetScene(int idx)
{
	assert(idx >= 0 && idx < m_Scenes.size());
	return m_Scenes[idx];
}

const SceneGraph& SceneManager::GetScene(int idx) const
{
	assert(idx >= 0 && idx < m_Scenes.size());
	return m_Scenes[idx];
}

SceneGraph& SceneManager::operator[](int idx)
{
	assert(idx >= 0 && idx < m_Scenes.size());
	return m_Scenes[idx];
}

const SceneGraph& SceneManager::operator[](int idx) const
{
	assert(idx >= 0 && idx < m_Scenes.size());
	return m_Scenes[idx];
}

void SceneManager::SetActiveScene(int idx)
{
	assert(idx >= 0 && idx < m_Scenes.size());
	m_ActiveSceneIdx = idx;
}

void SceneManager::ToggleActiveScene()
{
	m_ActiveSceneIdx = (m_ActiveSceneIdx + 1) % int(m_Scenes.size());
}

const SceneGraph& SceneManager::GetActiveScene() const
{
	assert(m_ActiveSceneIdx >= 0 && m_ActiveSceneIdx < m_Scenes.size());
	return m_Scenes[m_ActiveSceneIdx];
}

SceneGraph& SceneManager::GetActiveScene()
{
	assert(m_ActiveSceneIdx >= 0 && m_ActiveSceneIdx < m_Scenes.size());
	return m_Scenes[m_ActiveSceneIdx];
}