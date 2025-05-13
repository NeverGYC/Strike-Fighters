#ifndef RAY_CIRCLE_COLLISION_H_
#define RAY_CIRCLE_COLLISION_H_

#include <glm/glm.hpp>
#include <cmath>

namespace game {

    // Ray-circle collision detection utility
    class CollisionDetection {
    public:
        // Check if a ray intersects with a circle
        // ray_origin: start point of the ray (projectile's current position)
        // ray_direction: direction of the ray (normalized)
        // ray_length: length of the ray (projectile's speed * delta_time or a fixed length)
        // circle_center: center of the circle (enemy's position)
        // circle_radius: radius of the circle (hit box radius)
        // returns: true if the ray intersects with the circle
        static bool RayCircleIntersection(
            const glm::vec3& ray_origin,
            const glm::vec3& ray_direction,
            float ray_length,
            const glm::vec3& circle_center,
            float circle_radius)
        {
            // Vector from ray origin to circle center
            glm::vec3 m = ray_origin - circle_center;

            // Coefficients of the quadratic equation
            float a = glm::dot(ray_direction, ray_direction); // Should be 1 if ray_direction is normalized
            float b = glm::dot(m, ray_direction);
            float c = glm::dot(m, m) - circle_radius * circle_radius;

            // Discriminant of the quadratic equation
            float discriminant = b * b - a * c;

            // If discriminant is negative, there is no intersection
            if (discriminant < 0)
                return false;

            // Calculate the two solutions (distances along the ray)
            float sqrt_discriminant = sqrt(discriminant);
            float t1 = (-b - sqrt_discriminant) / a;
            float t2 = (-b + sqrt_discriminant) / a;

            // Check if either solution is within the ray length
            // t1 < 0 means the intersection is behind the ray origin
            // t2 < 0 means the entire circle is behind the ray origin
            if (t2 < 0)
                return false;

            // If t1 < 0 and t2 >= 0, the ray origin is inside the circle
            if (t1 < 0)
                return true;

            // Otherwise, check if the intersection point is within the ray length
            return t1 <= ray_length;
        }
    };

} // namespace game

#endif // RAY_CIRCLE_COLLISION_H_