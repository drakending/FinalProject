#pragma once
#include"glm/glm.hpp"
const float fric = 0.10;
class ClothPoint {
public:
	float weight;
	bool fixed;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 position;
	glm::vec3 vel;
	glm::vec3 force;
	glm::vec3 accer;
	ClothPoint(glm::vec3 pos = glm::vec3(0)) {
		weight = 1.0;
		fixed = false;
		vel = glm::vec3(0);
		force = glm::vec3(0);
		accer = glm::vec3(0);
		position = pos;
	}
	~ClothPoint() = default;
	void update(float timeDur)
	{
		if (fixed)
		{
			force = glm::vec3(0);
			return;
		}
		force *= (1 - fric);
		accer = force / weight;
		vel += accer * timeDur;
		position += vel * timeDur;
		force = glm::vec3(0);

	}
};
