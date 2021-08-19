#include "Utils.h"

void ObjReader::LoadModel(const std::string& objPath, std::vector<Elite::FPoint3>& vertexBuffer, std::vector<int>& indexBuffer)
{
	std::ifstream objStream{ objPath, std::ios::in | std::ios::binary };

	if (!objStream.is_open())
		std::cout << "Error: Could not open obj file \"" << objPath << "\".";

	std::string line{}, header{};
	std::smatch match{};

	while (!objStream.eof())
	{
		//std::regex_search(line, color_match, color_regex)
		std::getline(objStream, line, '\n');

		if (!line.empty())
		{
     		if (std::regex_search(line, match, std::regex("^(v|f)\\s*(-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?) (-?\\d+(?:\\.\\d+)?)")))
			{
				if (match[1] == "v")
				{
					vertexBuffer.push_back(Elite::FPoint3{ std::stof(match[2]), std::stof(match[3]), std::stof(match[4]) });
				}
				else if (match[1] == "f")
				{
					indexBuffer.push_back(std::stoi(match[2]) - 1);
					indexBuffer.push_back(std::stoi(match[3]) - 1);
					indexBuffer.push_back(std::stoi(match[4]) - 1);
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