#include "missile_game_object.h"

namespace game{
    //constructor
    MissileGameObject::MissileGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, const glm::vec3& direction)
        : GameObject(position, geom, shader, texture){
            direction_ = glm::normalize(direction);
            speed_ = 3.0f;
            initial_speed_ = 3.0f;
            max_speed_ = 8.0f;
            damage_= 0.5f;
            is_expired_ = false;
            has_target_ = false;
            turn_rate = 12.0f;
            acquisition_delay_ = 0.4f;

            SetScale(2.0f);
            SetRotation(atan2(direction_.y, direction_.x)); //match direction
            
            //start lifespan, and disappear in 7s
            lifespan_.Start(7.0f);
            //update direction per 0.02s
            steering_.Start(0.02f);
            acquisition_.Start(acquisition_delay_);
    }

    void MissileGameObject::SetTarget(const glm::vec3& target_position){
        if(acquisition_.Finished()){
            target_position_ = target_position; //get the enemy position
            has_target_ = true; //set finding state to true

            //calculate the distance from missile to target enemy
            float distance = glm::length(target_position_ - GetPosition());
            //calculate the direction vector from missile to target enemy
            glm::vec3 to_target = target_position - GetPosition();
            //calculate the angle difference
            float angle_to_target = atan2(to_target.y, to_target.x);
            float current_angle = atan2(direction_.y, direction_.x);
            float angle = angle_to_target - current_angle;
            //normalize angle difference
            if(angle > glm::pi<float>()){
                angle -=2.0f * glm::pi<float>();
            }
            if(angle < -glm::pi<float>()){
                angle +=2.0f * glm::pi<float>();
            }

        }
        //test message
        // std::cout << "Missile target set to: " << target_position_.x << ", " << target_position_.y << std::endl;
    }

    void MissileGameObject::ClearTarget(){
        has_target_ = false;
        // std::cout << "Missile target cleared" << std::endl;
    }
    
    void MissileGameObject::Update(double delta_time){
        // Check if the projectile has expired
        if (lifespan_.Finished()) {
            is_expired_ = true;
            SetActive(false);
            return;
        }

        if (!acquisition_.Finished()){
            glm::vec3 position = GetPosition();
            position +=direction_ *speed_ * (float)delta_time;
            SetPosition(position);

            GameObject::Update(delta_time);
            return;
        }

        // calculate direction
        if(has_target_ && steering_.Finished()){
            //calculate direction to enemy
            glm::vec3 to_target = target_position_ - GetPosition();
            float distance_to_target = glm::length(to_target);
            
            //steering the near enemy
            if(distance_to_target>0.05f){ //too close, don need to steer
                glm::vec3 target_direction = glm::normalize(to_target);

                float target_angle = atan2(target_direction.y, target_direction.x);
                float current_angle = atan2(direction_.y, direction_.x);

                //the angle difference between the start direction and enemy
                float angle = target_angle - current_angle;
                //normalize angle
                if(angle > glm::pi<float>()){
                    angle -=2.0f * glm::pi<float>();
                }
                if(angle < -glm::pi<float>()){
                    angle +=2.0f * glm::pi<float>();
                }

                //more close to the enemy, the speed and turn rate will faster
                float distance_factor = std::min(1.0f, 5.0f /distance_to_target);
                float max_turn = turn_rate * delta_time * distance_factor;

                float actual_turn = angle;
                if (actual_turn > max_turn){
                    actual_turn = max_turn;
                }
                if (actual_turn < -max_turn) {
                    actual_turn = -max_turn;
                }

                float new_angle = current_angle + actual_turn;
                direction_ = glm::vec3(cos(new_angle), sin(new_angle), 0.0f);

                SetRotation(new_angle);

                speed_ = initial_speed_ + (max_speed_ - initial_speed_) * (1.0f - std::min(1.0f, distance_to_target / 15.0f));
            }
            steering_.Start(0.02f);
        }

        // Update position based on direction and speed
        glm::vec3 position = GetPosition();
        position += direction_ * speed_ * (float)delta_time;
        SetPosition(position);

        // Call parent's update method
        GameObject::Update(delta_time);
    }
}