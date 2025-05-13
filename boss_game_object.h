#ifndef BOSS_GAME_OBJECT_H_
#define BOSS_GAME_OBJECT_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "game_object.h"
#include "timer.h"

namespace game{
    enum class Phase{
        PHASE_ONE,
        PHASE_TWO,
        DEFEATED
    };

    class BossGameObject : public GameObject{
        public:

            //constructor
            BossGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);
            
            ~BossGameObject();
            //Update function
            void Update(double delta_time) override;

            void UpdateTransforms(double delta_time);
            void UpdateAnimation(double delta_time);
            void UpdatePhase();

            void Render(glm::mat4 view_matrix, double current_time) override;

            void FireProjectiles();
            void FireMissile();

            //Setter
            void SetArmTexture(GLuint texture);
            void SetJointTexture(GLuint texture);
            void SetProjectileTexture(GLuint texture);
            void SetMissileTexture(GLuint texture);
            // void SetExplosionTexture(GLuint texture);

            //Getter
            inline Phase GetPhase() const {return phase_;}
            inline float GetHealth() const {return health_;}
            inline float GetBaseRotation() const { return base_rotation_; }
            inline float GetLowerArmAngle() const { return lower_arm_angle_; }
            inline float GetUpperArmAngle() const { return upper_arm_angle_; }

            std::vector<glm::vec3> GetProjectileSpawnPoints() const;
            std::vector<glm::vec3> GetProjectileDirections() const;
            glm::vec3 GetMissileSpawnPoint() const;

            bool TakeDamage(float damage);

            //in phase one, the boss can only fire projectile
            inline bool CanProjectiles() const {return projectile_timer_.Finished();}
            //in phase two, the boss will fire missile
            inline bool CanMissile() const {return missile_timer_.Finished() && phase_ == Phase::PHASE_TWO;}
            

            // Get positions of all boss parts for collision detection
            std::vector<glm::vec3> GetArmPositions() const;

            // Get collision radii for all boss parts
            std::vector<float> GetArmRadii() const;

        private:
            float base_rotation_;
            //make three gameobject for each part
            GameObject* base_;
            GameObject* lower_arm_;
            GameObject* upper_arm_;

            GLuint arm_texture_;
            GLuint joint_texture_;
            GLuint projectile_texture_;
            GLuint missile_texture_;

            Phase phase_;
            float health_;
            float max_health_;

            //base
            float base_rotation_speed_;

            //lower arm
            float lower_arm_rotation_speed_;
            float lower_arm_angle_;
            float lower_arm_angle_min_;
            float lower_arm_angle_max_;
            float lower_arm_direction_;

            //upper arm
            float upper_arm_rotation_speed_;
            float upper_arm_angle_;
            float upper_arm_angle_min_;
            float upper_arm_angle_max_;
            float upper_arm_direction_;

            Timer projectile_timer_;
            Timer missile_timer_;
            float projectile_speed_;
            float missile_speed_;
    };
}

#endif