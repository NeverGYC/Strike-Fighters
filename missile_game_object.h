#ifndef MISSILE_GAME_OBJECT_H_
#define MISSILE_GAME_OBJECT_H_

#include <iostream>
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include "game_object.h"
#include "timer.h"

//A weapon that does less damage
//but automatically tracks the closest enemy

namespace game{
    class MissileGameObject : public GameObject{
        public:
            //constructor
            MissileGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, const glm::vec3& direction);
            
            //override the update method
            void Update(double delta_time) override;

            //Getter
            inline bool IsExpired() const {return is_expired_;}
            inline float GetSpeed() const {return speed_;}
            inline glm::vec3 GetDirection() const {return direction_;}
            inline float GetDamage() const {return damage_;}

            //Setter
            void SetTarget(const glm::vec3& target_position);
            void ClearTarget();

        private:
            glm::vec3 direction_; //direction vector
            glm::vec3 target_position_; //enemy position
            bool has_target_; //if there are no enemy around, then false
            bool is_expired_; //need to disappear
            float speed_;
            float initial_speed_; //when close to enemy
            float max_speed_; //add speed
            float damage_; //damage caused
            Timer lifespan_; //a timer for lifespan
            Timer steering_; //to update the steering state
            Timer acquisition_; //the delay time before obtaining the target
            float turn_rate;
            float acquisition_delay_;

    };
}

#endif