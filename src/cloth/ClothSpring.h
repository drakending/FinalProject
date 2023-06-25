#pragma once
#include<glm/glm.hpp>
#include<cloth/ClothPoint.h>
class ClothSpring {
public:
	ClothPoint* node1, * node2;
	float len, hook, damp = 5.0;
	ClothSpring(ClothPoint* node1, ClothPoint* node2, float hook) {
		this->node1 = node1;
		this->node2 = node2;
		len = glm::length(node2->position - node1->position);
		this->hook = hook;
	}
	void addForce(float timeDur)
	{
		glm::vec3 dir = glm::normalize(node2->position - node1->position);
		glm::vec3 velDiff = node2->vel - node1->vel;
		glm::vec3 force = dir * ((glm::length(node2->position - node1->position) - len) * hook + glm::dot(velDiff, dir) * damp);
		node1->force += force;
		node2->force -= force;
	}

};
