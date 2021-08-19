#pragma once
#include <vector>

class Object;
class Light;
struct Ray;
struct HitRecord;

class SceneGraph
{
public:
	explicit SceneGraph() : m_pObjects{}, m_pLights{}{};
	SceneGraph(const SceneGraph& other) = delete;
	SceneGraph(SceneGraph&& other) noexcept;
	SceneGraph& operator=(const SceneGraph& other) = delete;
	SceneGraph& operator=(SceneGraph&& other) noexcept;
	~SceneGraph();

	const std::vector<Object*>& GetObjects() const;
	const std::vector<Light*>& GetLights() const;
	void AddObject(Object* pNewObject);
	void AddLight(Light* pNewLight);

	void Update(float deltaT);

private:
	std::vector<Object*> m_pObjects;
	std::vector<Light*> m_pLights;
};

