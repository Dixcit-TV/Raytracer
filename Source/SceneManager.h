#pragma once
#include <vector>
#include "SceneGraph.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();
	~SceneManager() = default;

	void SetActiveScene(int idx);
	void ToggleActiveScene();

	void AddScene(SceneGraph&& newScene);
	SceneGraph& GetScene(int idx);
	const SceneGraph& GetScene(int idx) const;

	SceneGraph& operator[](int idx);
	const SceneGraph& operator[](int idx) const;

	const SceneGraph& GetActiveScene() const;
	SceneGraph& GetActiveScene();

private:
	SceneManager() : m_Scenes{}, m_ActiveSceneIdx{ 0 } {};

	static SceneManager* m_Instance;

	std::vector<SceneGraph> m_Scenes;
	int m_ActiveSceneIdx;
};

