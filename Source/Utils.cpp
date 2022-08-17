#include "Utils.h"

void ObjReader::LoadModel(const std::string& objPath, std::vector<Elite::FPoint3>& vertexBuffer, std::vector<int>& indexBuffer)
{
	std::ifstream objStream{ objPath, std::ios::in | std::ios::binary };

	if (!objStream.is_open())
		std::cout << "Error: Could not open obj file \"" << objPath << "\".";

	std::string line{};

	while (!objStream.eof())
	{
		//std::regex_search(line, color_match, color_regex)
		std::getline(objStream, line, '\n');

		if (!line.empty())
		{
			float values[3];
     		if (sscanf_s(line.c_str(), "%*s %f %f %f", values, values + 1, values + 2) == 3)
			{
				if (line._Starts_with("v "))
				{
					vertexBuffer.push_back(Elite::FPoint3{ values[0], values[1], values[2] });
				}
				else if (line._Starts_with("f "))
				{
					indexBuffer.push_back(static_cast<int>(values[0]) - 1);
					indexBuffer.push_back(static_cast<int>(values[1]) - 1);
					indexBuffer.push_back(static_cast<int>(values[2]) - 1);
				}
			}
		}
	}

	auto vertexHash = [](const Elite::FPoint3& v)
	{
		return std::hash<std::string>()(std::to_string(v.x) + std::to_string(v.y) + std::to_string(v.z));
	};

	std::unordered_set<Elite::FPoint3, decltype(vertexHash)> s{ vertexBuffer.size(), vertexHash };
	std::unordered_map<Elite::FPoint3, int, decltype(vertexHash)> originals{ vertexBuffer.size(), vertexHash };
	int idx{ 0 };
	auto end = std::copy_if(vertexBuffer.begin(), vertexBuffer.end(), vertexBuffer.begin(),
		[&s, &originals, &idx, &indexBuffer](const Elite::FPoint3 v)
		{
			if (s.insert(v).second)
			{
				originals.emplace(v, idx);
				++idx;
				return true;
			}
			else
			{
				std::replace(indexBuffer.begin(), indexBuffer.end(), idx, originals[v]);
				return false;
			}
		});

	vertexBuffer.erase(end, vertexBuffer.end());
}