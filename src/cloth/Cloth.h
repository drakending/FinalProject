#pragma once
#include<vector>
#include<cloth/ClothPoint.h>
#include<cloth/ClothSpring.h>
#include<base/transform.h>
class Cloth
{
public:
	const int nodesDensity = 7;
	const int iterationFreq = 10;
	const double structuralCoef = 1000.0;
	const double shearCoef = 50.0;
	const double bendingCoef = 400.0;
	Transform transform;
	int width, height;
	int nodesPerRow, nodesPerCol;

	std::vector<ClothPoint*> nodes;
	std::vector<ClothSpring*> springs;
	std::vector<ClothPoint*> faces;

	glm::vec2 pin1;
	glm::vec2 pin2;

	Cloth(glm::vec3 pos, float x, float y)
	{
		transform.position = pos;

		width = x;
		height = y;
		init();
	}
	~Cloth()
	{
		for (int i = 0; i < nodes.size(); i++) { delete nodes[i]; }
		for (int i = 0; i < springs.size(); i++) { delete springs[i]; }
		nodes.clear();
		springs.clear();
		faces.clear();
	}
	ClothPoint* getNode(int x, int y) {
		return nodes[y * nodesPerRow + x];
	}
	void pin(glm::vec2 index, glm::vec3 offset)
	{
		if (!(index.x < 0 || index.x >= nodesPerRow || index.y < 0 || index.y >= nodesPerCol)) {
			getNode(index.x, index.y)->position += offset;
			getNode(index.x, index.y)->fixed = true;
		}
	}
	void init()
	{
		nodesPerRow = width * nodesDensity;
		nodesPerCol = height * nodesDensity;

		pin1 = glm::vec2(0);
		pin2 = glm::vec2(nodesPerRow - 1, 0);

		for (int i = 0; i < nodesPerRow; i++) {
			for (int j = 0; j < nodesPerCol; j++) {
				/** Create node by position **/
				ClothPoint* node = new ClothPoint(glm::vec3((double)j / nodesDensity, -((double)i / nodesDensity), 0));
				/** Set texture coordinates **/
				node->texCoord.x = (double)j / (nodesPerRow - 1);
				node->texCoord.y = (double)i / (1 - nodesPerCol);
				/** Add node to cloth **/
				nodes.push_back(node);
			}
		}

		/** Add springs **/
		for (int i = 0; i < nodesPerRow; i++) {
			for (int j = 0; j < nodesPerCol; j++) {
				/** Structural **/
				if (i < nodesPerRow - 1) springs.push_back(new ClothSpring(getNode(i, j), getNode(i + 1, j), structuralCoef));
				if (j < nodesPerCol - 1) springs.push_back(new ClothSpring(getNode(i, j), getNode(i, j + 1), structuralCoef));
				/** Shear **/
				if (i < nodesPerRow - 1 && j < nodesPerCol - 1) {
					springs.push_back(new ClothSpring(getNode(i, j), getNode(i + 1, j + 1), shearCoef));
					springs.push_back(new ClothSpring(getNode(i + 1, j), getNode(i, j + 1), shearCoef));
				}
				/** Bending **/
				if (i < nodesPerRow - 2) springs.push_back(new ClothSpring(getNode(i, j), getNode(i + 2, j), bendingCoef));
				if (j < nodesPerCol - 2) springs.push_back(new ClothSpring(getNode(i, j), getNode(i, j + 2), bendingCoef));
			}
		}
		pin(pin1, glm::vec3(1.0, 0.0, 0.0));
		pin(pin2, glm::vec3(-1.0, 0.0, 0.0));

		/** Triangle faces **/
		for (int i = 0; i < nodesPerRow - 1; i++) {
			for (int j = 0; j < nodesPerCol - 1; j++) {
				// Left upper triangle
				faces.push_back(getNode(i + 1, j));
				faces.push_back(getNode(i, j));
				faces.push_back(getNode(i, j + 1));
				// Right bottom triangle
				faces.push_back(getNode(i + 1, j + 1));
				faces.push_back(getNode(i + 1, j));
				faces.push_back(getNode(i, j + 1));
			}
		}
	}
	glm::vec3 computeFaceNormal(ClothPoint* n1, ClothPoint* n2, ClothPoint* n3)
	{
		return glm::cross(n2->position - n1->position, n3->position - n1->position);
	}
	void computeNormal()
	{
		/** Reset nodes' normal **/
		glm::vec3 normal(0.0, 0.0, 0.0);
		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->normal = normal;
		}
		/** Compute normal of each face **/
		for (int i = 0; i < faces.size() / 3; i++) { // 3 nodes in each face
			ClothPoint* n1 = faces[3 * i + 0];
			ClothPoint* n2 = faces[3 * i + 1];
			ClothPoint* n3 = faces[3 * i + 2];

			// Face normal
			normal = computeFaceNormal(n1, n2, n3);
			// Add all face normal
			n1->normal += normal;
			n2->normal += normal;
			n3->normal += normal;
		}

		for (int i = 0; i < nodes.size(); i++) {
			nodes[i]->normal = glm::normalize(nodes[i]->normal);
		}

	}

	void addForce(glm::vec3 f)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->force += f;
		}
	}

	void computeForce(double timeStep, glm::vec3 gravity)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->force += (gravity * nodes[i]->weight);
		}
		for (int i = 0; i < springs.size(); i++)
		{
			springs[i]->addForce(timeStep);
		}
	}
	void update(double timeStep)
	{
		/** Node **/
		for (int i = 0; i < nodes.size(); i++)
		{
			nodes[i]->update(timeStep);
		}
	}
};

