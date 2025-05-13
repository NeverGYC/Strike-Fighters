#include "boss_game_object.h"

namespace game{
    //constructor
    BossGameObject::BossGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture){
        phase_ = Phase::PHASE_ONE; //set the phase to phase one
        max_health_ = 40.0f;
        health_ = max_health_;

        arm_texture_ = texture;
        joint_texture_ = texture;
        projectile_texture_ = texture;
        missile_texture_ = texture;

        base_rotation_speed_ = 0.3f;
        lower_arm_rotation_speed_ = 0.5f;
        upper_arm_rotation_speed_ = 0.8f;
        lower_arm_angle_min_ = -glm::pi<float>() / 4.0f;
        lower_arm_angle_max_ = glm::pi<float>() / 4.0f;
        upper_arm_angle_min_ = -glm::pi<float>() / 3.0f;
        upper_arm_angle_max_ = glm::pi<float>() / 3.0f;

        lower_arm_angle_ = 0.0f;
        upper_arm_angle_ = 0.0f;
        lower_arm_direction_ = 1.0f;
        upper_arm_direction_ = 1.0f;

        //start timer
        projectile_timer_.Start(2.0f);
        missile_timer_.Start(5.0f);

        projectile_speed_ = 5.0f;
        missile_speed_ = 3.0f;
        base_rotation_ = 0.0f;

        float base_height = 1.0f * 2.0f;

        base_ = new GameObject(glm::vec3(0.0f, base_height, 0.0f), geom, shader, texture);
        base_->SetScale(1.0f, 1.0f);
        lower_arm_ = new GameObject(glm::vec3(0.0f, base_height/2, 0.0f), geom, shader, arm_texture_);
        lower_arm_->SetScale(1.0f, 1.5f);
        upper_arm_ = new GameObject(glm::vec3(0.0f, lower_arm_->GetScale().y, 0.0f), geom, shader, arm_texture_);
        upper_arm_->SetScale(0.8f, 1.6f);
        SetScale(2.0f);
    }



    BossGameObject::~BossGameObject() {
        delete base_;
        delete lower_arm_;
        delete upper_arm_;
    }

    std::vector<glm::vec3> BossGameObject::GetArmPositions() const {
        std::vector<glm::vec3> positions;

        // Add base position
        if (base_) {
            positions.push_back(base_->GetPosition());
        }

        // Add lower arm position
        if (lower_arm_) {
            positions.push_back(lower_arm_->GetPosition());
        }

        // Add upper arm position
        if (upper_arm_) {
            positions.push_back(upper_arm_->GetPosition());
        }

        return positions;
    }

    std::vector<float> BossGameObject::GetArmRadii() const {
        std::vector<float> radii;

        // Base collision radius (larger)
        if (base_) {
            radii.push_back(base_->GetScale().x * 0.5f);
        }

        // Lower arm collision radius
        if (lower_arm_) {
            radii.push_back(lower_arm_->GetScale().x * 0.4f);
        }

        // Upper arm collision radius
        if (upper_arm_) {
            radii.push_back(upper_arm_->GetScale().x * 0.4f);
        }

        return radii;
    }


    void BossGameObject::UpdateAnimation(double delta_time){
        lower_arm_angle_ += lower_arm_direction_ * lower_arm_rotation_speed_ * delta_time;
        if (lower_arm_angle_ > lower_arm_angle_max_ || 
            lower_arm_angle_ < lower_arm_angle_min_) {
            lower_arm_direction_ *= -1;
        }
        
        upper_arm_angle_ += upper_arm_direction_ * upper_arm_rotation_speed_ * delta_time;
        if (upper_arm_angle_ > upper_arm_angle_max_ || 
            upper_arm_angle_ < upper_arm_angle_min_) {
            upper_arm_direction_ *= -1;
        }
        
        base_rotation_ += base_rotation_speed_ * delta_time;
        if (phase_ == Phase::PHASE_TWO) {
            base_rotation_speed_ = 0.6f; 
        }
    }

    void BossGameObject::UpdateTransforms(double delta_time){

        glm::vec3 boss_position = GetPosition();
        float boss_rotation = GetRotation();
        
        glm::mat4 base_transform = glm::translate(glm::mat4(1.0f), boss_position) * glm::rotate(glm::mat4(1.0f), boss_rotation + base_rotation_, glm::vec3(0.0f, 0.0f, 1.0f));
        
        float lower_arm_height = lower_arm_->GetScale().y; 
        float upper_arm_height = upper_arm_->GetScale().y; 

        // glm::vec3 lower_arm_local_offset(0.0f, 1.0f, 0.0f);
        glm::mat4 lower_arm_transform =base_transform *glm::rotate(glm::mat4(1.0f), lower_arm_angle_, glm::vec3(0.0f, 0.0f, 1.0f)) *glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, lower_arm_height / 1.7f, 0.0f));
        
        // glm::vec3 upper_arm_local_offset(0.0f, 1.2f, 0.0f);
        glm::mat4 upper_arm_transform =lower_arm_transform *glm::rotate(glm::mat4(1.0f), upper_arm_angle_, glm::vec3(0.0f, 0.0f, 1.0f)) *glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, upper_arm_height / 1.4f, 0.0f));
        
        base_->SetPosition(glm::vec3(base_transform[3]));
        base_->SetRotation(boss_rotation + base_rotation_);
        
        lower_arm_->SetPosition(glm::vec3(lower_arm_transform[3]));
        lower_arm_->SetRotation(lower_arm_angle_ + base_->GetRotation());
        
        upper_arm_->SetPosition(glm::vec3(upper_arm_transform[3]));
        upper_arm_->SetRotation(upper_arm_angle_ + lower_arm_->GetRotation());
    }

    void BossGameObject::UpdatePhase(){
        if(phase_ == Phase::PHASE_ONE && health_ <=max_health_ / 2.0f){
            phase_ = Phase::PHASE_TWO;
            std::cout << "Boss entering Phase Two." << std::endl;
            projectile_timer_.Start(1.0f);
            missile_timer_.Start(3.0f);
        }
        if(health_<=0.0f && phase_ !=Phase::DEFEATED){
            phase_ = Phase::DEFEATED;
            StartExplosion();
            std::cout << "Boss Dead. You win!" << std::endl;
        }
    }

    void BossGameObject::Update(double delta_time){
        //if boss is dead or is exploding
        //directly return
        if(!IsActive() || IsExploding()){
            GameObject::Update(delta_time);
            return;
        }
        UpdateAnimation(delta_time);
        UpdateTransforms(delta_time);
        UpdatePhase();
        GameObject::Update(delta_time);
    }

    void BossGameObject::Render(glm::mat4 view_matrix, double current_time){
        if(!IsActive()){
            return;
        }
        if(IsExploding()){
            GameObject::Render(view_matrix, current_time);
            return;
        }
        upper_arm_->Render(view_matrix, current_time);
        lower_arm_->Render(view_matrix, current_time);
        base_->Render(view_matrix, current_time);
    }

    bool BossGameObject::TakeDamage(float damage){
        health_ -=damage;
        std::cout << "HP:" << health_ << std::endl;
        bool phase_changed = false;
        if(health_ <= max_health_ / 2.0f && phase_ == Phase::PHASE_ONE){
            phase_changed = true;
        }
        if(health_ < 0.0f){
            health_ = 0.0f;
        }
        return phase_changed;
    }

    std::vector<glm::vec3> BossGameObject::GetProjectileSpawnPoints() const{
        std::vector<glm::vec3> spawn_points;
        glm::vec3 arm_end = upper_arm_->GetPosition();
        float arm_angle = upper_arm_->GetRotation();

        float arm_length = upper_arm_->GetScale().y;
        glm::vec3 spawn_pos = arm_end + glm::vec3(
            glm::sin(arm_angle) * arm_length * 0.5f,
            glm::cos(arm_angle) * arm_length * 0.5f,
            0.0f
        );

        if(phase_ == Phase::PHASE_ONE || phase_ == Phase::PHASE_TWO){
            for(int i=-2;i<=2;i++){
                float angle_offset = i * (glm::pi<float>() / 12.0f);
                float firing_angle = arm_angle + angle_offset;
                
                glm::vec3 adjusted_spawn = spawn_pos;
                spawn_points.push_back(adjusted_spawn);
            }
        }
        return spawn_points;
    }

    std::vector<glm::vec3> BossGameObject::GetProjectileDirections() const {
        std::vector<glm::vec3> directions;
        
        float arm_angle = upper_arm_->GetRotation();
        
        if(phase_ == Phase::PHASE_ONE || phase_ == Phase::PHASE_TWO) {
            for(int i=-2; i<=2; i++) {
                float angle_offset = i * (glm::pi<float>() / 12.0f);
                float firing_angle = arm_angle + angle_offset;
                
                glm::vec3 direction(
                    glm::sin(firing_angle),
                    glm::cos(firing_angle),
                    0.0f
                );
                
                directions.push_back(glm::normalize(direction));
            }
        }
        
        return directions;
    }

    glm::vec3 BossGameObject::GetMissileSpawnPoint() const {
        glm::vec3 base_pos = base_->GetPosition();
        return base_pos;
    }

    void BossGameObject::FireProjectiles() {
        projectile_timer_.Start(phase_ == Phase::PHASE_ONE ? 2.0f : 1.0f);
    }

    void BossGameObject::FireMissile() {
        missile_timer_.Start(3.0f);
    }

    void BossGameObject::SetArmTexture(GLuint texture) {
        arm_texture_ = texture;
        lower_arm_->SetTexture(texture);
    }

    void BossGameObject::SetJointTexture(GLuint texture) {
        upper_arm_->SetTexture(texture);
        joint_texture_ = texture;
    }
    
    void BossGameObject::SetProjectileTexture(GLuint texture) {
        projectile_texture_ = texture;
    }
    
    void BossGameObject::SetMissileTexture(GLuint texture) {
        missile_texture_ = texture;
    }    

    // void BossGameObject::SetExplosionTexture(GLuint texture){
    //     GameObject::SetExplosionTexture(texture);
    // }
    
}