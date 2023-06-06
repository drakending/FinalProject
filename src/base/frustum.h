#pragma once

#include "bounding_box.h"
#include "plane.h"
#include <iostream>

struct Frustum {
public:
    Plane planes[6];
    enum {
        LeftFace = 0,
        RightFace = 1,
        BottomFace = 2,
        TopFace = 3,
        NearFace = 4,
        FarFace = 5
    };

    bool intersect(const BoundingBox& aabb, const glm::mat4& modelMatrix) const {
        
        glm::vec4 min = modelMatrix * glm::vec4(aabb.min, 1.0);

        glm::vec4 max = modelMatrix * glm::vec4(aabb.max, 1.0);
        glm::vec3 center = glm::vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
        glm::vec3 extents = glm::vec3(max.x - center.x, max.y - center.y, max.z - center.z);
       
        // Get global scale thanks to our transform
        const glm::vec3 globalCenter = center;
       glm::vec3 right(1.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 front(0.0f, 0.0f, -1.0f);
        glm::vec3 gRight =glm::normalize(glm::vec3(modelMatrix * glm::vec4(right, 1.0f)));
        glm::vec3 gUp = glm::normalize(glm::vec3(modelMatrix * glm::vec4(up, 1.0f))); 
        glm::vec3 gFront = glm::normalize(glm::vec3(modelMatrix * glm::vec4(front, 1.0f)));
        // Scaled orientation
        glm::vec3 bright = gRight * extents.x;
        glm::vec3 bup = gUp * extents.y;
        glm::vec3 bforward = gFront * extents.z;

        float newIi = std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, bright))
                      + std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, bup))
                      + std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, bforward));

        float newIj = std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, bright))
                      + std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, bup))
                      + std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, bforward));

        float newIk = std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, bright))
                      + std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, bup))
                      + std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, bforward));

        // We not need to divise scale because it's based on the half extention of the AABB
        for (int i = 0; i < 6; i++) {
            const Plane* p = &planes[i];
            const float r = newIi * std::abs(p->normal.x) + newIj * std::abs(p->normal.y)
                            + newIk * std::abs(p->normal.z);
            if (-r > p->getSignedDistanceToPoint(globalCenter)) {
                return false;
            }
        }
        return true;
        // ------------------------------------------------------------
    }
};

inline std::ostream& operator<<(std::ostream& os, const Frustum& frustum) {
    os << "frustum: \n";
    os << "planes[Left]:   " << frustum.planes[0] << "\n";
    os << "planes[Right]:  " << frustum.planes[1] << "\n";
    os << "planes[Bottom]: " << frustum.planes[2] << "\n";
    os << "planes[Top]:    " << frustum.planes[3] << "\n";
    os << "planes[Near]:   " << frustum.planes[4] << "\n";
    os << "planes[Far]:    " << frustum.planes[5] << "\n";

    return os;
}