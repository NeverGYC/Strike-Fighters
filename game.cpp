#include <ctime>
#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>

#include <path_config.h>

#include "sprite.h"
#include "shader.h"
#include "player_game_object.h"
#include "game.h"
#include "shader.h" 
#include "game_timer.h"    
#include <ctime>
#include "space_dust.h"
#include <stdexcept>



namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "Strike Fighters";
const unsigned int window_width_g = 1280;
const unsigned int window_height_g = 720;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);



// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


void Game::SetupGameWorld(void)
{
    // Setup the game world
    // Load all the textures that we will need
    std::vector<std::string> textures;

    textures.push_back("/textures/player_ship.png");            // tex_player = 0
    textures.push_back("/textures/enemy_ship_2.png");           // tex_enemy1 = 1
    textures.push_back("/textures/enemy_ship_1.png");           // tex_enemy2 = 2
    textures.push_back("/textures/enemy_fast.png");             // tex_enemy_fast = 3
    textures.push_back("/textures/enemy_heavy.png");            // tex_enemy_heavy = 4
    textures.push_back("/textures/stars.png");                  // tex_stars = 5
    textures.push_back("/textures/orb.png");                    // tex_orb = 6
    textures.push_back("/textures/explosion.png");              // tex_explosion = 7
    textures.push_back("/textures/player_ship_invincible.png"); // tex_player_invincible = 8
    textures.push_back("/textures/bullet.png");                 // tex_projectile = 9
    textures.push_back("/textures/particle.png");               // tex_particle = 10
    textures.push_back("/textures/missile.png");                // tex_missile = 11
    textures.push_back("/textures/enemy_bullet.png");           // tex_enemy_projectile = 12
    textures.push_back("/textures/asteroid_blue.png");          // tex_asteroid = 13
    textures.push_back("/textures/font.png");                   // tex_font = 14
    textures.push_back("/textures/collectible_speed.png");      // tex_collectible_speed = 15
    textures.push_back("/textures/collectible_power.png");      // tex_collectible_power = 16
    textures.push_back("/textures/health_bar_background.png");   // tex_health_bar_background = 17
    textures.push_back("/textures/health_bar_fill.png");         // tex_health_bar_fill = 18
    textures.push_back("/textures/stage_2.png");                 // tex_stage_2 = 19
    textures.push_back("/textures/base.png");                    // tex_base = 20
    textures.push_back("/textures/lower_part2.png");                    // tex_lower_part = 21
    textures.push_back("/textures/upper_part2.png");                    // tex_upper_part = 22
    textures.push_back("/textures/particle.png");             // tex_dust_particle = 23
    textures.push_back("/textures/flame_particle.png");       // tex_flame_particle = 24






    try {
        LoadTextures(textures);

        // Verify textures loaded properly before proceeding
        for (int i = 0; i < textures.size(); i++) {
            if (tex_[i] == 0) {
                throw std::runtime_error("Failed to load texture at index " + std::to_string(i));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to load textures: " << e.what() << std::endl;
        throw; // Re-throw to prevent game from starting with missing textures
    }

    // Create player object (first in vector)
    PlayerGameObject* player = new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f),
        sprite_, &sprite_shader_, tex_[tex_player]);
    player->SetExplosionTexture(tex_[tex_explosion]);
    player->SetInvincibleTexture(tex_[tex_player_invincible]);
    player->SetProjectileTexture(tex_[tex_projectile]);     // Set projectile texture
    player->SetRotation(glm::pi<float>() / 2.0f);
    player->SetMissileTexture(tex_[tex_missile]);
    game_objects_.push_back(player);


    //time text
    GameTimer* time_obj = new GameTimer(glm::vec3(-3.5f, 3.5f, 0.0f), sprite_, &text_shader_, tex_[14]);
    time_obj->SetScale(1.6 * 0.8, 0.4 * 0.8);
    time_obj->SetReferenceTime(glfwGetTime());
    game_objects_.push_back(time_obj);

    //enemy count text
    EnemyCounter* enemy_counter = new EnemyCounter(glm::vec3(-3.5f, 2.5f, 0.0f), sprite_, &text_shader_, tex_[14]);
    enemy_counter->SetScale(2.5 * 0.8, 0.4 * 0.8);
    enemy_count_ = enemy_counter;
    game_objects_.push_back(enemy_counter);

    // SpawnBoss();

    // Create space dust particle system
    space_dust_ = new SpaceDust(
        glm::vec3(0.0f, 0.0f, 0.0f),
        sprite_,
        &dust_shader_,
        tex_[tex_particle],  // Use the particle texture or create a new one for dust
        3000  // Number of dust particles
    );

    space_dust_->SetBounds(40.0f, 40.0f);  // Set bounds larger than game area
    game_objects_.push_back(space_dust_);

    // Setup background (will be the last in vector for proper rendering)
    GameObject* background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f),
        sprite_, &sprite_shader_, tex_[tex_stars]);

    // Set a large scale for the background
    background->SetScale(100.0f);

    game_objects_.push_back(background);

    SpawnAsteroid(5);  // Spawn 5 asteroids

    // Enable shader to set uniform
    sprite_shader_.Enable();
    sprite_shader_.SetUniform1f("texture_scale", 15.0f);
    sprite_shader_.Disable();

    // EnemyCounter* enemy_counter = new EnemyCounter(
    //     glm::vec3(-3.5f, 2.5f, 0.0f),  // Position (will be updated to follow player)
    //     sprite_,                        // Use the same sprite geometry
    //     &text_shader_,                  // Use text shader
    //     tex_[tex_font]                  // Use font texture
    // );

    // Configure enemy counter
    // enemy_counter->SetScale(3.3f, 0.5f);  // Set appropriate scale for text
    // enemy_count_ = enemy_counter;         // Store reference in class member

    game_objects_.insert(game_objects_.end() - 1, enemy_counter);


    // NOW spawn collectibles and enemies AFTER both player and background are added
    SpawnCollectibles(5);
    SpawnInitialEnemies(6);


    enemy_projectile_texture_ = tex_[tex_enemy_projectile];

    InitUI();


    // Initialize mini map with dedicated sprite
    mini_map_.Init(minimap_sprite_, &minimap_shader_);
    mini_map_.SetPosition(30.0f, 30.0f);
    mini_map_.SetSize(300.0f, 300.0f);
    mini_map_.SetWorldBounds(40.0f, 40.0f);

    std::cout << "Minimap initialized with dedicated sprite" << std::endl;
}

void Game::GameOver(const std::string& reason){
    if (game_over_) return;
    game_over_ = true;
    std::cout << "Game Over: " << reason << std::endl;
    game_over_timer_.Start(3.0f);
}


void Game::LoadTextures(std::vector<std::string>& textures) {
    // Allocate a buffer for all texture references
    num_textures = textures.size();  // Store in class member
    tex_ = new GLuint[num_textures];
    glGenTextures(num_textures, tex_);

    // Load each texture
    for (int i = 0; i < num_textures; i++) {
        std::string full_path = resources_directory_g + textures[i];
        SetTexture(tex_[i], full_path.c_str());

        // Verify texture was loaded successfully
        if (tex_[i] == 0) {
            throw std::runtime_error("Failed to load texture: " + full_path);
        }
    }

    // Set first texture in the array as default
    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}

void Game::InitUI() {
    std::cout << "Initializing UI started" << std::endl;

    // CRITICAL: Verify font texture exists and was loaded correctly
    if (tex_ == nullptr) {
        std::cerr << "ERROR: Texture array is null!" << std::endl;
        return;
    }

    if (tex_font >= num_textures) {
        std::cerr << "ERROR: Font texture index out of bounds! tex_font=" << tex_font
            << ", num_textures=" << num_textures << std::endl;
        return;
    }

    if (tex_[tex_font] == 0) {
        std::cerr << "ERROR: Invalid font texture ID (0)" << std::endl;
        return;
    }

    std::cout << "Font texture ID: " << tex_[tex_font] << std::endl;

    // Ensure text shader is initialized
    text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(),
        (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());


    // Initialize health bar
    if (tex_[tex_health_bar_background] == 0 || tex_[tex_health_bar_fill] == 0) {
        std::cerr << "ERROR: Health bar textures not loaded correctly" << std::endl;
    }
    else {
        health_bar_.Init(sprite_, &sprite_shader_, tex_[tex_health_bar_background], tex_[tex_health_bar_fill]);
        health_bar_.SetSize(200.0f, 30.0f);
        std::cout << "Health bar initialized successfully" << std::endl;
    }

    std::cout << "UI initialization completed" << std::endl;
}

// Create homing missile
void Game::CreateHomingMissile(const glm::vec3& position, const glm::vec3& direction) {
    PlayerGameObject* player = GetPlayer();
    float power_multiplier = 1.0f;
    if (player) {
        power_multiplier = player->GetPowerBuff();
    }
    // Create a new homing missile
    HomingMissile* missile = new HomingMissile(
        position,
        sprite_,
        &sprite_shader_,
        tex_[tex_missile],
        direction
    );

    missile->SetDamage(2.0f * power_multiplier);

    // Find closest enemy as initial target
    GameObject* target = FindClosestEnemy(position);
    if (target) {
        missile->SetTarget(target);
    }

    // Add the missile to game objects
    game_objects_.insert(game_objects_.end() - 1, missile);

    // Create flame particle effect for the missile
    MissileFlame* flame = new MissileFlame(
        position,
        sprite_,
        &particle_shader_,
        tex_[24],
        missile
    );

    // Add flame to game objects (insert before background)
    game_objects_.insert(game_objects_.end() - 1, flame);

    // Play missile firing sound
    audio_system_.PlaySound(fire_sound_);
}

GameObject* Game::FindClosestEnemy(const glm::vec3& position, float max_distance) {
    GameObject* closest_enemy = nullptr;
    float closest_distance = max_distance;

    // Search through all game objects for enemies
    for (int i = 1; i < game_objects_.size() - 1; i++) {
        EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(game_objects_[i]);

        // Check if it's a valid enemy
        if (enemy && enemy->IsActive() && !enemy->IsExploding()) {
            float distance = glm::length(enemy->GetPosition() - position);

            // Check if this enemy is closer than the current closest
            if (distance < closest_distance) {
                closest_enemy = enemy;
                closest_distance = distance;
            }
        }
    }

    return closest_enemy;
}

// spawn collectibles method
// In SpawnCollectibles method:
void Game::SpawnCollectibles(int count) {
    // Safety check
    if (game_objects_.size() < 2) {  // Need at least player and background
        std::cerr << "Warning: Not enough game objects for spawning collectibles" << std::endl;
        return;
    }

    // Verify texture exists
    if (tex_ == nullptr || tex_[tex_orb] == 0) {
        std::cerr << "Warning: Invalid orb texture" << std::endl;
        return;
    }

    for (int i = 0; i < count; i++) {
        // Generate random position between -5 and 5 for both x and y
        const float BOUND = 20.0f;  // Match the boundary from HandleControls
        float rand_x = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;
        float rand_y = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;

        // Random collectible object type
        int random_type = rand() % 10;
        Type type;
        GLuint texture_use;

        // Change texture for different type
        if (random_type < 6) {
            // 60% INVINCIBLE
            type = Type::INVINCIBLE;
            texture_use = tex_[tex_orb];
        }
        else if (random_type < 8) {
            // 20% SPEED
            type = Type::SPEED;
            texture_use = tex_[tex_collectible_speed]; // Make sure to add this texture
        }
        else {
            // 20% POWER
            type = Type::POWER;
            texture_use = tex_[tex_collectible_power]; // Make sure to add this texture
        }

        CollectibleGameObject* collectible = new CollectibleGameObject(
            glm::vec3(rand_x, rand_y, 0.0f),
            sprite_,
            &sprite_shader_,
            texture_use,
            type
        );

        // Insert before the background (which is last)
        game_objects_.insert(game_objects_.end() - 1, collectible);
    }
}


void Game::CreateEnemyProjectile(const glm::vec3& position, const glm::vec3& direction, int enemy_type) {
    // Create the projectile with the appropriate texture and properties
    GLuint projectile_texture = enemy_projectile_texture_; // Default enemy bullet texture
    float damage = 1.0f;  // Default damage
    float scale = 0.4f;   // Default scale
    bool is_missile = false;

    // Heavy enemy uses missiles with a different texture, damage, and scale
    if (enemy_type == 2) {
        projectile_texture = tex_[tex_missile];
        damage = 2.0f; // Increased damage for heavy enemies
        scale = 0.5f;  // Larger projectile
        is_missile = true;
    }

    // Create a new projectile object with the specified direction
    Projectile* projectile = new Projectile(
        position,
        sprite_,
        &sprite_shader_,
        projectile_texture,
        direction
    );

    // Set the projectile rotation to match its direction
    projectile->SetRotation(atan2(direction.y, direction.x));

    // Set projectile properties
    projectile->SetIsEnemyProjectile(true);
    projectile->SetScale(scale);
    projectile->SetDamage(damage);

    // Add the projectile to the game objects vector
    game_objects_.insert(game_objects_.end() - 1, projectile);

    // Add flame particles only for missiles (enemy type 2)
    if (is_missile) {
        // Create flame particle effect for the missile
        MissileFlame* flame = new MissileFlame(
            position,
            sprite_,
            &particle_shader_,
            tex_[24],
            projectile
        );

        // Add flame to game objects (insert before background)
        game_objects_.insert(game_objects_.end() - 1, flame);
    }

    // Play appropriate firing sound
    audio_system_.PlaySound(fire_sound_);
}


FastEnemyGameObject* Game::CreateFastEnemy(const glm::vec3& position) {
    // Check if we already have the maximum number of fast enemies
    if (fast_enemy_count_ >= MAX_ENEMIES_PER_TYPE) {
        return nullptr;
    }

    // Create new fast enemy object
    FastEnemyGameObject* enemy = new FastEnemyGameObject(
        position,
        sprite_,
        &sprite_shader_,
        tex_[tex_enemy_fast]
    );

    // Setup enemy properties
    enemy->SetExplosionTexture(tex_[tex_explosion]);
    enemy->SetRotation(glm::pi<float>() / 2.0f);
    enemy->SetProjectileTexture(tex_[tex_enemy_projectile]);

    // Set patrol parameters (smaller, faster ellipse)
    float width = 0.8f + ((float)rand() / RAND_MAX) * 1.5f;  // 0.8 to 2.3
    float height = 0.8f + ((float)rand() / RAND_MAX) * 1.0f; // 0.8 to 1.8
    enemy->SetPatrolParameters(position, width, height);

    // Increment counter
    fast_enemy_count_++;

    return enemy;
}

HeavyEnemyGameObject* Game::CreateHeavyEnemy(const glm::vec3& position) {
    // Check if we already have the maximum number of heavy enemies
    if (heavy_enemy_count_ >= MAX_ENEMIES_PER_TYPE) {
        return nullptr;
    }

    // Create new heavy enemy object
    HeavyEnemyGameObject* enemy = new HeavyEnemyGameObject(
        position,
        sprite_,
        &sprite_shader_,
        tex_[tex_enemy_heavy]
    );

    // Setup enemy properties
    enemy->SetExplosionTexture(tex_[tex_explosion]);
    enemy->SetRotation(glm::pi<float>() / 2.0f);
    enemy->SetProjectileTexture(tex_[tex_enemy_projectile]);

    // Set patrol parameters (larger, slower ellipse)
    float width = 1.5f + ((float)rand() / RAND_MAX) * 2.5f;  // 1.5 to 4.0
    float height = 1.5f + ((float)rand() / RAND_MAX) * 2.0f; // 1.5 to 3.5
    enemy->SetPatrolParameters(position, width, height);

    // Increment counter
    heavy_enemy_count_++;

    return enemy;
}



void Game::DestroyGameWorld(void) {
    // Clear out any special pointers first to avoid double deletion
    boss_ = nullptr;
    enemy_count_ = nullptr;
    time_ui_ = nullptr;
    space_dust_ = nullptr;

    // Free memory for all game objects
    for (auto obj : game_objects_) {
        delete obj;
    }

    // Clear the vector after deleting all objects
    game_objects_.clear();

    // Reset counters
    regular_enemy_count_ = 0;
    fast_enemy_count_ = 0;
    heavy_enemy_count_ = 0;
}

void Game::CheckLevelProgression(){
    if(current_level_ == 1 && enemy_count_ && enemy_count_->GetCount() >=ENEMITES_TO_NEXT_LEVEL){
        std::cout << "Starting transition to Level 2!" << std::endl;
        level_transition_active_ = true;
        level_transition_timer_.Start(2.0f);
    }
}

void Game::ClearGameObjects(bool keep_player_and_background){
    if (keep_player_and_background) {
        PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
        GameObject* background = game_objects_.back();
        std::vector<GameObject*> ui_elements;
    
        for (int i = 1; i < game_objects_.size() - 1; i++) {
            if (dynamic_cast<GameTimer*>(game_objects_[i]) || dynamic_cast<HealthBar*>(game_objects_[i])) {
                ui_elements.push_back(game_objects_[i]);
            } else {
                delete game_objects_[i];
            }
        }
        game_objects_.clear();
        game_objects_.push_back(player);
        for (auto ui : ui_elements) {
            game_objects_.push_back(ui);
        }
        game_objects_.push_back(background);
    }else {
        for (auto obj : game_objects_) {
            delete obj;
        }
        game_objects_.clear();
    }
    regular_enemy_count_ = 0;
    fast_enemy_count_ = 0;
    heavy_enemy_count_ = 0;
}

void Game::SpawnBoss(){
    boss_ = new BossGameObject(
        glm::vec3(0.0f, 0.0f, 0.0f),
        sprite_,
        &sprite_shader_,
        tex_[20]
    );

    boss_->SetExplosionTexture(tex_[tex_explosion]);
    boss_->SetArmTexture(tex_[21]);
    boss_->SetJointTexture(tex_[22]);
    boss_->SetProjectileTexture(tex_[tex_enemy_projectile]);
    boss_->SetMissileTexture(tex_[tex_missile]);

    boss_->SetScale(3.0f);
    game_objects_.insert(game_objects_.end() -1, boss_);

    std::cout << "Boss spawned!" << std::endl;
}

void Game::ProcessBossPlayerCollision(double delta_time) {
    // Only process if boss exists and is active
    if (!boss_ || !boss_->IsActive() || boss_->IsExploding()) {
        return;
    }

    // Get player game object
    PlayerGameObject* player = GetPlayer();
    if (!player || !player->IsActive() || player->IsExploding()) {
        return;
    }

    // Get positions
    glm::vec3 player_pos = player->GetPosition();
    float player_radius = 0.8f;

    // Get boss parts positions and collision data
    struct BossPart {
        glm::vec3 position;
        float radius;
        std::string name;
    };

    // Collect all boss parts for collision checking
    std::vector<BossPart> boss_parts;

    // Main base part
    boss_parts.push_back({
        boss_->GetPosition(),
        1.5f,
        "Base"
        });

    // Get boss arm positions - using the boss methods to access arm data
    std::vector<glm::vec3> arm_positions = boss_->GetArmPositions();
    std::vector<float> arm_radii = boss_->GetArmRadii();

    // Add arm parts to collision check list
    if (arm_positions.size() == arm_radii.size()) {
        for (size_t i = 0; i < arm_positions.size(); i++) {
            boss_parts.push_back({
                arm_positions[i],
                arm_radii[i],
                "Arm part " + std::to_string(i + 1)
                });
        }
    }

    // Check collision with each part
    bool collision = false;
    glm::vec3 collision_pos;
    glm::vec3 push_direction;
    float push_force = 12.0f;
    std::string collision_part = "";

    for (const auto& part : boss_parts) {
        // Calculate distance between player and this boss part
        float distance = glm::length(player_pos - part.position);

        // Check for collision
        if (distance < (part.radius + player_radius)) {
            collision = true;
            collision_pos = part.position;
            push_direction = glm::normalize(player_pos - part.position);
            collision_part = part.name;

            // We can break on first collision or process all and use the strongest one
            break;
        }
    }

    // Handle collision if it occurred
    if (collision) {
        // If player is not invincible, apply damage
        if (!player->IsInvincible()) {
            player->TakeDamage(1.0f);
            audio_system_.PlaySound(hit_sound_);
        }

        // Calculate push velocity
        glm::vec3 push_velocity = push_direction * push_force;

        // Move player outside collision radius to prevent sticking
        float safe_distance = player_radius + 1.2f; // Extra margin
        glm::vec3 safe_position = collision_pos + push_direction * safe_distance;
        player->SetPosition(safe_position);

        // Apply sliding effect to player (bounce off the boss)
        player->StartSliding(push_velocity);
        player->LockInputForTime(0.3f); // Brief input lock for feedback

        std::cout << "Player collided with boss " << collision_part << "!" << std::endl;
    }
}



void Game::TransitionToLevel2(){
    if(transitioning_to_level2_){
        return;
    }
    std::cout << "Transitioning to Level 2!" << std::endl;
    transitioning_to_level2_ = true;
    current_level_ = 2;
    GameObject* background = game_objects_.back();

    if(background){
        background->SetTexture(tex_[19]);
        background->SetScale(100.0f);
    }

    transitioning_to_level2_ = false;
}


void Game::HandleControls(double delta_time) {
    // Get player game object
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);

    // Don't process controls if player is exploding or inactive
    if (!player || player->IsExploding() || !player->IsActive()) {
        return;
    }

    // Get current position and angle
    glm::vec3 curpos = player->GetPosition();
    float angle = player->GetRotation();

    // Compute current bearing direction
    glm::vec3 dir = player->GetBearing();
    glm::vec3 right = player->GetRight();

    // SIGNIFICANTLY INCREASED speed for faster movement
    float speed = delta_time * 1600.0 * player->GetSpeedBuff(); // Doubled from 800.0
    float motion_increment = 0.004 * speed; // Doubled from 0.002
    float angle_increment = (glm::pi<float>() / 400.0f) * speed; // Increased turn rate

    // Add position bounds
    const float BOUND = 20.0f;
    glm::vec3 new_pos;

    // Process controls with bounds checking
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        new_pos = curpos + motion_increment * dir;
        if (abs(new_pos.x) <= BOUND && abs(new_pos.y) <= BOUND) {
            player->SetPosition(new_pos);
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        new_pos = curpos - motion_increment * dir;
        if (abs(new_pos.x) <= BOUND && abs(new_pos.y) <= BOUND) {
            player->SetPosition(new_pos);
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        player->SetRotation(angle - angle_increment);
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        player->SetRotation(angle + angle_increment);
    }
    if (glfwGetKey(window_, GLFW_KEY_Z) == GLFW_PRESS) {
        new_pos = curpos - motion_increment * right;
        if (abs(new_pos.x) <= BOUND && abs(new_pos.y) <= BOUND) {
            player->SetPosition(new_pos);
        }
    }
    if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS) {
        new_pos = curpos + motion_increment * right;
        if (abs(new_pos.x) <= BOUND && abs(new_pos.y) <= BOUND) {
            player->SetPosition(new_pos);
        }
    }
    // Add Q key for weapon switching
    static bool q_pressed = false;
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        if (!q_pressed) {
            player->SwitchWeapon();
            q_pressed = true;
        }
    }
    else {
        q_pressed = false;
    }

    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
}

PlayerGameObject* Game::GetPlayer() const {
    if (!game_objects_.empty()) {
        return dynamic_cast<PlayerGameObject*>(game_objects_[0]);
    }
    return nullptr;
}

void Game::CreateProjectile(const glm::vec3& position, const glm::vec3& direction) {
    PlayerGameObject* player = GetPlayer();
    float power_multiplier = 1.0f;
    if (player) {
        power_multiplier = player->GetPowerBuff();
    }
    // Create a new projectile object
    Projectile* projectile = new Projectile(
        position,
        sprite_,
        &sprite_shader_,
        tex_[tex_projectile],
        direction
    );

    projectile->SetDamage(1.0f * power_multiplier);

    // Add the projectile to the game objects vector
    // Insert before the background (which is the last object)
    game_objects_.insert(game_objects_.end() - 1, projectile);

    // Play firing sound if available
    if (fire_sound_ != 0) {
        audio_system_.PlaySound(fire_sound_);
    }
}


// Function to spawn initial enemies
void Game::SpawnInitialEnemies(int count) {
    if (game_objects_.size() < 2) {
        std::cerr << "Warning: Not enough game objects for spawning enemies" << std::endl;
        return;
    }

    // Reset counters
    regular_enemy_count_ = 0;
    fast_enemy_count_ = 0;
    heavy_enemy_count_ = 0;

    // Distribution of initial enemies
    for (int i = 0; i < MAX_ENEMIES_PER_TYPE; i++) {
        // Create each type of enemy, positioned in different areas
        float x, y;
        GameObject* enemy = nullptr;

        // Fast enemies in a circular pattern
        float angle = ((float)i / MAX_ENEMIES_PER_TYPE) * 2.0f * glm::pi<float>();
        float radius = 8.0f + ((float)rand() / RAND_MAX) * 4.0f; // 8-12 units from center
        x = radius * cos(angle);
        y = radius * sin(angle);
        enemy = CreateFastEnemy(glm::vec3(x, y, 0.0f));
        if (enemy) {
            game_objects_.insert(game_objects_.end() - 1, enemy);
        }

        // Heavy enemies in the corners
        int corner = i % 4;
        float offset = 3.0f + ((float)rand() / RAND_MAX) * 4.0f; // Random offset
        switch (corner) {
        case 0: x = 15.0f - offset; y = 15.0f - offset; break; // Top-right
        case 1: x = -15.0f + offset; y = 15.0f - offset; break; // Top-left
        case 2: x = -15.0f + offset; y = -15.0f + offset; break; // Bottom-left
        case 3: x = 15.0f - offset; y = -15.0f + offset; break; // Bottom-right
        }
        enemy = CreateHeavyEnemy(glm::vec3(x, y, 0.0f));
        if (enemy) {
            game_objects_.insert(game_objects_.end() - 1, enemy);
        }

        // Regular enemies in random positions
        const float BOUND = 18.0f;
        x = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;
        y = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;
        enemy = CreateEnemy(glm::vec3(x, y, 0.0f));
        if (enemy) {
            game_objects_.insert(game_objects_.end() - 1, enemy);
        }
    }

    std::cout << "Spawned " << regular_enemy_count_ << " regular enemies, "
        << fast_enemy_count_ << " fast enemies, and "
        << heavy_enemy_count_ << " heavy enemies." << std::endl;

    // Initialize spawn timer with a shorter interval
    enemy_spawn_timer_.Start(5.0f);
}


// Process projectile-enemy collisions using ray-circle collision detection
// Full corrected method
void Game::ProcessProjectileEnemyCollisions(Projectile* projectile, EnemyGameObject* enemy, double delta_time) {
    // Get necessary data for ray-circle intersection
    glm::vec3 projectile_pos = projectile->GetPosition();
    glm::vec3 projectile_dir = projectile->GetDirection();
    float ray_length = projectile->GetSpeed() * delta_time * 2.0f;

    glm::vec3 enemy_pos = enemy->GetPosition();
    float enemy_radius = 0.8f;

    // For heavy enemies, increase collision radius
    if (enemy->GetEnemyType() == 2) {
        enemy_radius = 1.2f;
    }

    // Check for ray-circle intersection
    if (CollisionDetection::RayCircleIntersection(
        projectile_pos,
        projectile_dir,
        ray_length,
        enemy_pos,
        enemy_radius)) {

        // Get player for power buff
        PlayerGameObject* player = GetPlayer();
        float power_multiplier = 1.0f;
        if (player) {
            power_multiplier = player->GetPowerBuff();
        }

        // Apply damage with power multiplier
        float damage = projectile->GetDamage() * power_multiplier;

        // Apply damage to enemy
        enemy->TakeDamage(damage);

        // If enemy is still alive, don't start explosion yet
        if (enemy->GetHealth() <= 0) {
            // Start enemy explosion
            enemy->StartExplosion();
            enemy->SetScale(2.5f);  // Make explosion bigger

            // Increment enemy counter ONLY when enemy is destroyed
            if (enemy_count_) {
                enemy_count_->IncrementCount();
            }

            // Play explosion sound
            audio_system_.PlaySound(explosion_sound_);

            std::cout << "Enemy destroyed by projectile!" << std::endl;
        }
        else {
            std::cout << "Enemy hit! Health: " << enemy->GetHealth() << "/" << enemy->GetMaxHealth() << std::endl;
        }

        // Deactivate projectile
        projectile->SetActive(false);
    }
}

void Game::UpdateBoss(double delta_time) {
    if (!boss_ || !boss_->IsActive()) {
        return;
    }
    if (boss_->CanProjectiles()) {
        std::vector<glm::vec3> spawn_points = boss_->GetProjectileSpawnPoints();
        std::vector<glm::vec3> directions = boss_->GetProjectileDirections();
        for (int i = 0;i < spawn_points.size();i++) {
            if (i < directions.size()) {
                CreateEnemyProjectile(spawn_points[i], directions[i], 0);
            }
        }
        boss_->FireProjectiles();
    }
    // if(boss_->CanMissile()){
    //     glm::vec3 spawn_point = boss_->GetMissileSpawnPoint();
    //     PlayerGameObject* player = GetPlayer();
    //     if(player && player->IsActive()){
    //         glm::vec3 direction = glm::normalize(player->GetPosition() - spawn_point);
    //         // CreateMissile(spawn_point, direction);
    //     }
    //     boss_->FireMissile();
    // }
    for (int i = 1;i < game_objects_.size() - 1;i++) {
        Projectile* projectile = dynamic_cast<Projectile*>(game_objects_[i]);
        if (projectile && projectile->IsActive() && !projectile->IsEnemyProjectile()) {
            float distance = glm::length(projectile->GetPosition() - boss_->GetPosition());
            if (distance < 1.5f) {
                // Get player for power buff
                PlayerGameObject* player = GetPlayer();
                float power_multiplier = 1.0f;
                if (player) {
                    power_multiplier = player->GetPowerBuff();
                }

                // Apply damage with power multiplier
                float damage = projectile->GetDamage() * power_multiplier;

                bool phase_changed = boss_->TakeDamage(damage);
                projectile->SetActive(false);
            }
        }
    }
}

void Game::SetupBoss(){
    std::cout << "Setting up boss for level 2..." << std::endl;
    for (int i = 1; i < game_objects_.size() - 1; i++) {
        EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(game_objects_[i]);
        Projectile* projectile = dynamic_cast<Projectile*>(game_objects_[i]);
        
        if (enemy || projectile) {
            delete game_objects_[i];
            game_objects_.erase(game_objects_.begin() + i);
            i--;
        }
    }
    
    regular_enemy_count_ = 0;
    fast_enemy_count_ = 0;
    heavy_enemy_count_ = 0;
    
    SpawnBoss();
}

// void Game::CreateMissile(const glm::vec3& position, const glm::vec3& direction) {
//     // Create a new missile object
//     MissileGameObject* missile = new MissileGameObject(
//         position,
//         sprite_,
//         &sprite_shader_,
//         tex_[11],
//         direction
//     );

//     // Add the missile to the game objects vector
//     // Insert before the background (which is the last object)
//     game_objects_.insert(game_objects_.end() - 1, missile);

//     // Play firing sound if available
//     if (fire_sound_ != 0) {
//         audio_system_.PlaySound(fire_sound_);
//     }
// }


void Game::ProcessBossProjectileCollisions(Projectile* projectile, BossGameObject* boss, double delta_time) {
    if (projectile->IsEnemyProjectile()) {
        return;
    }

    glm::vec3 projectile_pos = projectile->GetPosition();
    glm::vec3 projectile_dir = projectile->GetDirection();
    float ray_length = projectile->GetSpeed() * delta_time * 2.0f;

    glm::vec3 boss_pos = boss->GetPosition();
    float boss_radius = 1.5f;

    if (CollisionDetection::RayCircleIntersection(
        projectile_pos,
        projectile_dir,
        ray_length,
        boss_pos,
        boss_radius)) {

        // Get player for power buff
        PlayerGameObject* player = GetPlayer();
        float power_multiplier = 1.0f;
        if (player) {
            power_multiplier = player->GetPowerBuff();
        }

        float damage = projectile->GetDamage() * power_multiplier;

        bool phase_changed = boss->TakeDamage(damage);

        if (phase_changed) {
            std::cout << "Boss entered Phase Two!" << std::endl;
        }
        projectile->SetActive(false);
    }
}

// spawn an enemy at a random position
void Game::SpawnEnemy() {
    if (game_objects_.size() < 2) {
        std::cerr << "Warning: Not enough game objects for spawning enemies" << std::endl;
        return;
    }

    // Generate random position using the full map boundaries
    const float BOUND = 20.0f;
    float rand_x = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;
    float rand_y = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;

    // Try to spawn a random enemy type, prioritizing types under the limit
    GameObject* enemy = nullptr;

    // Create a list of enemy types that haven't reached their limit
    std::vector<int> available_types;
    if (regular_enemy_count_ < MAX_ENEMIES_PER_TYPE) available_types.push_back(0);
    if (fast_enemy_count_ < MAX_ENEMIES_PER_TYPE) available_types.push_back(1);
    if (heavy_enemy_count_ < MAX_ENEMIES_PER_TYPE) available_types.push_back(2);

    // If all types have reached their limit, don't spawn
    if (available_types.empty()) {
        std::cout << "All enemy types at maximum capacity!" << std::endl;
        enemy_spawn_timer_.Start(5.0f);
        return;
    }

    // Choose a random available type
    int enemy_type = available_types[rand() % available_types.size()];

    switch (enemy_type) {
    case 0:
        enemy = CreateEnemy(glm::vec3(rand_x, rand_y, 0.0f));
        break;
    case 1:
        enemy = CreateFastEnemy(glm::vec3(rand_x, rand_y, 0.0f));
        break;
    case 2:
        enemy = CreateHeavyEnemy(glm::vec3(rand_x, rand_y, 0.0f));
        break;
    }

    // Insert before the background
    if (enemy) {
        game_objects_.insert(game_objects_.end() - 1, enemy);
    }

    // Restart spawn timer
    enemy_spawn_timer_.Start(5.0f);
}

// create a new enemy object
EnemyGameObject* Game::CreateEnemy(const glm::vec3& position) {
    // Check if we already have the maximum number of regular enemies
    if (regular_enemy_count_ >= MAX_ENEMIES_PER_TYPE) {
        return nullptr;
    }

    // Create new enemy object
    EnemyGameObject* enemy = new EnemyGameObject(
        position,
        sprite_,
        &sprite_shader_,
        tex_[tex_enemy1]
    );

    // Setup enemy properties
    enemy->SetExplosionTexture(tex_[tex_explosion]);
    enemy->SetRotation(glm::pi<float>() / 2.0f);
    enemy->SetProjectileTexture(tex_[tex_enemy_projectile]);

    // Set patrol parameters (random ellipse size)
    float width = 1.0f + ((float)rand() / RAND_MAX) * 2.0f;  // 1.0 to 3.0
    float height = 1.0f + ((float)rand() / RAND_MAX) * 1.5f; // 1.0 to 2.5
    enemy->SetPatrolParameters(position, width, height);

    // Increment counter
    regular_enemy_count_++;

    return enemy;
}

// Add the SpawnAsteroid method
void Game::SpawnAsteroid(int count) {


    for (int i = 0; i < count; i++) {
        // Generate random position
        const float BOUND = 20.0f;
        float rand_x = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;
        float rand_y = ((float)rand() / RAND_MAX) * (2 * BOUND) - BOUND;

        Asteroid* asteroid = new Asteroid(
            glm::vec3(rand_x, rand_y, 0.0f),
            sprite_,
            &sprite_shader_,
            tex_[tex_asteroid]
        );

        // Insert before the background
        game_objects_.insert(game_objects_.end() - 1, asteroid);
    }
}

// Add the ProcessAsteroidCollisions method
void Game::ProcessAsteroidCollisions() {
    // Get the player
    PlayerGameObject* player = GetPlayer();
    if (!player || !player->IsActive()) {
        return;
    }

    glm::vec3 player_pos = player->GetPosition();

    // Check for collisions with all asteroids
    for (int i = 1; i < game_objects_.size() - 1; i++) {
        Asteroid* asteroid = dynamic_cast<Asteroid*>(game_objects_[i]);
        if (asteroid) {
            float distance = glm::length(asteroid->GetPosition() - player_pos);
            float asteroid_radius = asteroid->GetScale().x * 0.4f;
            float player_radius = player->GetScale().x * 0.4f;

            // If distance is less than combined radii, collision occurred
            if (distance < (asteroid_radius + player_radius)) {
                asteroid->CollisionWithPlayer(player);
            }
        }
    }
}


// Update method to handle game logic
void Game::Update(double delta_time) {

    static int debug_counter = 0;
    if (debug_counter++ % 60 == 0) {
        std::cout << "DEBUG: Level=" << current_level_ 
                << ", EnemyCount=" << (enemy_count_ ? enemy_count_->GetCount() : -1)
                << ", TransitioningFlag=" << transitioning_to_level2_ << std::endl;
    }

    if (game_over_) {
        for (auto obj : game_objects_) {
            if (obj->IsActive()) {
                obj->Update(delta_time);
            }
        }
        return;
    }

    // Add boss-player collision detection when boss exists
    if (boss_ && boss_->IsActive() && !boss_->IsExploding()) {
        ProcessBossPlayerCollision(delta_time);
    }

    if (current_level_ == 1 && enemy_count_ && enemy_count_->GetCount()>=5 && !transitioning_to_level2_) {
        TransitionToLevel2();
        return;
    }

    if (current_level_ == 2 && enemy_count_ && enemy_count_->GetCount() >= 5 && !boss_) {
        SpawnBoss();
        std::cout << "Boss spawned after defeating 5 enemies in level 2!" << std::endl;
    }

    // Cast first object to PlayerGameObject (it's always the player)
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);

    GameTimer* time_obj = nullptr;
    EnemyCounter* enemy_counter = nullptr;

    if (player && player->IsExploding()) {
        GameOver("Player destroyed! Game over. You lose.");
    }

    if (boss_ && boss_->IsExploding() && boss_->GetPhase() == Phase::DEFEATED && enemy_count_->GetCount() == 5) {
        GameOver("Boss defeated! You win!");
    }

    for(int i = 1; i < game_objects_.size() - 1; i++) {
        if(!time_obj) {
            time_obj = dynamic_cast<GameTimer*>(game_objects_[i]);
        }
        if(!enemy_counter) {
            enemy_counter = dynamic_cast<EnemyCounter*>(game_objects_[i]);
        }
        if(time_obj && enemy_counter) break;
    }

    if(player && time_obj) {
        glm::vec3 player_pos = player->GetPosition();
        
        glm::vec3 time_pos = player_pos + glm::vec3(0.0f, 6.0f, 0.0f);
        time_obj->SetPosition(time_pos);
    }
    
    if(player && enemy_counter) {
        glm::vec3 player_pos = player->GetPosition();
        
        glm::vec3 counter_pos = player_pos + glm::vec3(0.0f, 5.0f, 0.0f);
        enemy_counter->SetPosition(counter_pos);
    }

    // if(current_level_ == 1 || (current_level_ == 2 && !boss_)){
        // Check enemy spawn timer and spawn if needed
        if (enemy_spawn_timer_.Finished()) {
            SpawnEnemy();
            enemy_spawn_timer_.Start(5.0f); // Reset timer for next spawn (every 5 seconds)
        }
    // }

    // if(current_level_ == 2 && boss_ && boss_->IsActive() && !boss_->IsExploding()){
    //     if (boss_->CanProjectiles()) {
    //         std::vector<glm::vec3> spawn_points = boss_->GetProjectileSpawnPoints();
    //         std::vector<glm::vec3> directions = boss_->GetProjectileDirections();
            
    //         size_t num_projectiles = std::min(spawn_points.size(), directions.size());
    //         for (size_t i = 0; i < num_projectiles; i++) {
    //             CreateEnemyProjectile(spawn_points[i], directions[i], 0);
    //         }
    //         boss_->FireProjectiles();
    //     }

    //     if (boss_->CanMissile()) {
    //         glm::vec3 missile_spawn = boss_->GetMissileSpawnPoint();
    //         if (player && player->IsActive()) {
    //             glm::vec3 player_pos = player->GetPosition();
    //             glm::vec3 direction = glm::normalize(player_pos - missile_spawn);
    //             CreateEnemyProjectile(missile_spawn, direction, 2);
    //         }
    //         boss_->FireMissile();
    //     }
    // }

    if(boss_){
        UpdateBoss(delta_time);
    }

    ProcessAsteroidCollisions();


    // Check if player fired a weapon
    if (player && player->IsActive() && !player->IsExploding()) {
        // Check for firing when the space key is pressed
        bool space_pressed = glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS;

        if (space_pressed) {
            // Handle different weapon types
            if (player->GetCurrentWeapon() == PlayerGameObject::LASER) {
                if (player->Fire()) {
                    // Get player position and direction
                    glm::vec3 player_pos = player->GetPosition();
                    glm::vec3 direction = player->GetBearing();

                    // Create a projectile
                    CreateProjectile(player_pos, direction);
                }
            }
            else { // MISSILE
                if (player->FireMissile()) {
                    // Get player position and direction
                    glm::vec3 player_pos = player->GetPosition();
                    glm::vec3 direction = player->GetBearing();

                    // Create a homing missile
                    CreateHomingMissile(player_pos, direction);
                }
            }
        }
    }

    // Check if invincibility just ended
    if (player && player->IsInvincible() && player->GetInvincibilityTimer().Finished()) {
        audio_system_.PlaySound(invincible_end_sound_);
    }

    // Update enemy states based on player position with improved logic
    if (player && player->IsActive()) {
        glm::vec3 player_pos = player->GetPosition();

        for (int i = 1; i < game_objects_.size() - 1; i++) {
            EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(game_objects_[i]);
            if (enemy && enemy->IsActive() && !enemy->IsExploding()) {
                // Calculate distance to player
                float distance = glm::length(enemy->GetPosition() - player_pos);

                // Update enemy target and state based on player proximity
                // Use the enemy's specific detection range
                if (distance < enemy->GetDetectionRange()) {
                    // If within firing range, set to ATTACKING state
                    if (distance < enemy->GetDetectionRange() * 0.7f) {
                        enemy->SetState(EnemyState::ATTACKING);
                    }
                    else {
                        enemy->SetState(EnemyState::INTERCEPTING);
                    }

                    // Add slight prediction to player position for more challenging gameplay
                    glm::vec3 predicted_pos = player_pos + player->GetVelocity() * 0.5f;
                    enemy->SetTarget(predicted_pos);

                    // Handle enemy firing when in ATTACKING state
                    if (enemy->GetState() == EnemyState::ATTACKING && enemy->CanAttack()) {
                        // Get the enemy type
                        int enemy_type = enemy->GetEnemyType();

                        // Get enemy's position and rotation
                        glm::vec3 enemy_pos = enemy->GetPosition();
                        float enemy_rotation = enemy->GetRotation();

                        // Calculate enemy's forward vector based on its rotation
                        glm::vec3 enemy_forward(cos(enemy_rotation), sin(enemy_rotation), 0.0f);

                        // Slightly adjust spawn position to be at the front of the ship
                        glm::vec3 firing_position = enemy_pos + enemy_forward * 0.5f;

                        if (enemy_type == 0) {  // Regular enemy - shotgun pattern
                            // Create a 180-degree spread (π radians)
                            float spread_angle = glm::pi<float>();

                            // Fire 5 bullets in the spread pattern
                            for (int i = 0; i < 5; i++) {
                                // Calculate angle for each projectile in the spread
                                // This maps i=0 to -spread/2, i=2 to 0, and i=4 to +spread/2
                                float angle_offset = spread_angle * (i / 4.0f - 0.5f);

                                // Calculate the final direction for this projectile
                                float projectile_angle = enemy_rotation + angle_offset;
                                glm::vec3 projectile_direction(cos(projectile_angle), sin(projectile_angle), 0.0f);

                                // Create the projectile with the calculated direction
                                CreateEnemyProjectile(firing_position, projectile_direction, enemy_type);
                            }
                        }
                        else {
                            // Fast or heavy enemy - single projectile
                            // Use enemy's forward direction for accurate firing
                            CreateEnemyProjectile(firing_position, enemy_forward, enemy_type);
                        }

                        // Reset attack cooldown based on enemy type
                        float cooldown = 2.0f;  // Default cooldown
                        if (enemy_type == 1) {  // Fast enemy
                            cooldown = 0.8f;
                        }
                        else if (enemy_type == 2) {  // Heavy enemy
                            cooldown = 3.0f;
                        }

                        enemy->ResetAttackCooldown(cooldown);
                    }
                }
                else if (enemy->GetState() == EnemyState::INTERCEPTING) {
                    // Return to patrol if player is out of range
                    if (distance > enemy->GetDetectionRange() * 1.5f) {
                        enemy->SetState(EnemyState::PATROLLING);
                    }
                    else {
                        // Update target position while still intercepting
                        enemy->SetTarget(player_pos);
                    }
                }
            }
        }
    }

    // Main game object update loop
    for (int i = 0; i < game_objects_.size(); i++) {
        GameObject* current_game_object = game_objects_[i];

        // Skip processing for inactive objects
        if (!current_game_object->IsActive()) {
            continue;
        }

        // Update current object's state (movement, timers, etc.)
        current_game_object->Update(delta_time);

        // Skip collision detection for objects that are already exploding
        if (current_game_object->IsExploding()) {
            continue;
        }

        // Process projectile collisions with enemies
        Projectile* projectile = dynamic_cast<Projectile*>(current_game_object);
        if (projectile && !projectile->IsExpired() && !projectile->IsEnemyProjectile()) {
            // Check this projectile against all enemies
            for (int j = 1; j < game_objects_.size() - 1; j++) {
                // Skip comparing the projectile with itself
                if (i == j) continue;

                // Try to cast to an enemy
                EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(game_objects_[j]);

                // Only process active, non-exploding enemies
                if (enemy && enemy->IsActive() && !enemy->IsExploding()) {
                    // Process ray-circle collision between projectile and enemy
                    ProcessProjectileEnemyCollisions(projectile, enemy, delta_time);

                    // If projectile was deactivated by collision, stop checking other enemies
                    if (!projectile->IsActive()) break;
                }
            }
            if (current_level_ == 2 && boss_ && projectile->IsActive() && boss_->IsActive() && !boss_->IsExploding()){
                ProcessBossProjectileCollisions(projectile, boss_, delta_time);
            }
        }

        // Process enemy projectile collisions with player
        if (projectile && !projectile->IsExpired() && projectile->IsEnemyProjectile() && player && player->IsActive()) {
            // Get player position and radius for collision detection
            glm::vec3 player_pos = player->GetPosition();
            float player_radius = 0.8f; // Match the collision radius used for enemies

            // Get projectile data for ray-circle collision
            glm::vec3 projectile_pos = projectile->GetPosition();
            glm::vec3 projectile_dir = projectile->GetDirection();
            float ray_length = projectile->GetSpeed() * delta_time * 2.0f;

            // Check for collision between enemy projectile and player
            if (CollisionDetection::RayCircleIntersection(
                projectile_pos,
                projectile_dir,
                ray_length,
                player_pos,
                player_radius)) {

                // Only apply damage if player is not invincible
                if (!player->IsInvincible()) {
                    player->TakeDamage(1.0f);

                    // If player is now exploding, scale up the explosion
                    if (player->IsExploding()) {
                        player->SetScale(2.5f);
                    }

                    // Play hit sound
                    audio_system_.PlaySound(hit_sound_);
                }
                else {
                    // Player is invincible - show message
                    std::cout << "Projectile deflected by invincibility shield!" << std::endl;
                }

                // Deactivate projectile regardless of player invincibility
                projectile->SetActive(false);
            }
        }

        // Player collisions (player is always index 0)
        if (i == 0) {
            // Check against all other objects
            for (int j = 1; j < game_objects_.size() - 1; j++) {
                GameObject* other_object = game_objects_[j];

                // Skip inactive or exploding objects
                if (!other_object->IsActive() || other_object->IsExploding()) {
                    continue;
                }

                // Calculate distance for simple circle-circle collision
                float distance = glm::length(player->GetPosition() - other_object->GetPosition());

                // Collision threshold check
                if (distance < 0.8f) {
                    // Check if it's a collectible
                    CollectibleGameObject* collectible = dynamic_cast<CollectibleGameObject*>(other_object);
                    if (collectible && !collectible->IsCollected()) {
                        collectible->Collect();
                        Type type = collectible->GetType(); // Get the type
                        bool became_invincible = player->CollectItem(type); // Pass the type

                        // Play appropriate sound effect
                        if (became_invincible) {
                            audio_system_.PlaySound(invincible_start_sound_);
                        }
                        else {
                            audio_system_.PlaySound(pickup_sound_);
                        }

                        // Spawn new collectible
                        SpawnCollectibles(1);
                        std::cout << "Collectible obtained!" << std::endl;
                    }
                    // Check if it's an enemy
                    else if (dynamic_cast<EnemyGameObject*>(other_object)) {
                        if (player->IsInvincible()) {
                            // While invincible, we still explode the enemy but don't take damage
                            other_object->StartExplosion();
                            other_object->SetScale(2.5f);  // Make explosion bigger
                            audio_system_.PlaySound(explosion_sound_);
                            std::cout << "Enemy destroyed while invincible!" << std::endl;
                        }
                        else {
                            // Regular collision handling when not invincible
                            other_object->StartExplosion();
                            other_object->SetScale(2.5f);  // Make explosion bigger

                            // Apply damage to player
                            player->TakeDamage(1.0f);

                            // If player is now exploding, scale up their explosion too
                            if (player->IsExploding()) {
                                player->SetScale(2.5f);
                            }

                            // Play explosion sound
                            audio_system_.PlaySound(explosion_sound_);
                        }
                    }
                }
            }
        }
    }

    // Cleanup loop - remove inactive objects
    // Skip player (i=0) and background (last object)
    for (int i = 1; i < game_objects_.size() - 1; i++) {
        if (!game_objects_[i]->IsActive()) {
            // Check enemy type and decrement appropriate counter
            EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(game_objects_[i]);
            if (enemy) {
                int type = enemy->GetEnemyType();
                if (type == 0) regular_enemy_count_--;
                else if (type == 1) fast_enemy_count_--;
                else if (type == 2) heavy_enemy_count_--;
            }

            delete game_objects_[i];
            game_objects_.erase(game_objects_.begin() + i);
            i--; // Adjust index since we removed an element
        }
    }
}
void Game::RenderUI() {
    // Get player for health information
    PlayerGameObject* player = GetPlayer();
    if (!player || !player->IsActive()) return;

    // Get window dimensions
    int window_width, window_height;
    glfwGetWindowSize(window_, &window_width, &window_height);

    // CRITICAL: Create a completely separate rendering context for UI
    // Clear depth buffer only (keep color from main rendering)
    glClear(GL_DEPTH_BUFFER_BIT);

    // Set up fresh 2D orthographic projection
    glm::mat4 ui_projection = glm::ortho(
        0.0f, static_cast<float>(window_width),
        0.0f, static_cast<float>(window_height),
        -1.0f, 1.0f
    );

    // Setup rendering state
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render health bar - make it SIGNIFICANTLY larger
    if (health_bar_.IsInitialized()) {
        float bar_width = 300.0f;  // Much wider
        float bar_height = 40.0f;  // Much taller

        // Position in top-right corner with margin
        float bar_x = window_width - bar_width - 20.0f;
        float bar_y = window_height - bar_height - 20.0f;

        health_bar_.SetPosition(glm::vec3(bar_x, bar_y, 0.0f));
        health_bar_.SetSize(bar_width, bar_height);

        // Calculate health (ensure it's not 0)
        // float health = std::max(0.1f, player->GetHealth() / 10.0f);
        float health = player->GetHealth() / 20.0f;
        health_bar_.Render(ui_projection, health);
    }

    // Restore rendering state
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

// Modify Game::Render method to add minimap rendering at the end:

void Game::Render(void) {
    // Clear background
    glClearColor(viewport_background_color_g.r,
        viewport_background_color_g.g,
        viewport_background_color_g.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get window size for scaling
    int width, height;
    glfwGetWindowSize(window_, &width, &height);

    // Compute a window scale matrix based on the aspect ratio
    glm::mat4 window_scale_matrix = glm::mat4(1.0f);
    if (width > height) {
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect, 1.0f, 1.0f));
    }
    else {
        float aspect = static_cast<float>(height) / static_cast<float>(width);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f / aspect, 1.0f));
    }

    // Get player's position for camera following
    glm::vec3 camera_position(0.0f, 0.0f, 0.0f);
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
    if (player && player->IsActive()) {
        camera_position = player->GetPosition();
    }

    // Create camera translation matrix that follows the player
    glm::mat4 camera_translation = glm::translate(glm::mat4(1.0f), -camera_position);

    // Set view to zoom out, centered on the player
    float camera_zoom = 0.15f;
    glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));

    // Combine matrices: window scaling, zoom, and camera translation
    glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translation;

    // Render all game objects using the final view matrix
    for (int i = 0; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }

    // Call the separate UI rendering function
    RenderUI();

    // Render the mini map
    mini_map_.Render(game_objects_, boss_);
}

void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update window events like input handling
        glfwPollEvents();

        if (game_over_ && game_over_timer_.Finished()) {
            glfwSetWindowShouldClose(window_, true);
            continue;
        }

        // Handle user input
        HandleControls(delta_time);

        // Update all the game objects
        Update(delta_time);

		// Update audio system
		audio_system_.Update();

        // Render all the game objects
        Render();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }
}


Game::Game(void)
{
    // Initialize counters
    regular_enemy_count_ = 0;
    fast_enemy_count_ = 0;
    heavy_enemy_count_ = 0;

    current_level_ = 1;
    level_transition_active_ = false;
    boss_ = nullptr;

    game_over_ = false;
}



void Game::Init(void) {


    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }



    // Set whether window can be resized
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") +
            std::string((const char*)glewGetErrorString(err))));
    }

    // Initialize dust particle shader
    dust_shader_.Init(
        (resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(),
        (resources_directory_g + std::string("/dust_fragment_shader.glsl")).c_str()
    );


    // NOW it's safe to initialize shaders and OpenGL resources
    particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(),
        (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize text shader
    text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(),
        (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());


    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize minimap shader
    minimap_shader_.Init(
        (resources_directory_g + std::string("/minimap_vertex_shader.glsl")).c_str(),
        (resources_directory_g + std::string("/minimap_fragment_shader.glsl")).c_str()
    );

    // Create minimap sprite
    minimap_sprite_ = new MinimapSprite();
    minimap_sprite_->CreateGeometry();

    // Initialize time
    current_time_ = 0.0;

    current_level_ = 1;
    transitioning_to_level2_ = false;
    enemy_kills_ = 0;

    // Initialize spawn timer
    enemy_spawn_timer_.Start(10.0f); // Spawn first enemy after 10 seconds

    // Add randomization seed
    srand(static_cast<unsigned int>(time(nullptr)));
    // Initialize audio system
    try {
        audio_system_.Init();

        // Load audio files
        std::string audio_path  = resources_directory_g + "/audio/";
        background_music_       = audio_system_.LoadSound(audio_path + "background.wav");
        explosion_sound_        = audio_system_.LoadSound(audio_path + "explosion.wav");
        pickup_sound_           = audio_system_.LoadSound(audio_path + "RetroPickup10.wav");  
        invincible_start_sound_ = audio_system_.LoadSound(audio_path + "RetroMagicProtection01.wav");
        invincible_end_sound_   = audio_system_.LoadSound(audio_path + "RetroChargeOffStereoUP02.wav");
        fire_sound_ = audio_system_.LoadSound(audio_path + "RetroMissileLauncher01.wav");
        hit_sound_ = audio_system_.LoadSound(audio_path + "RetroImpactPunch07.wav");


        // Start background music
        audio_system_.PlayBackgroundMusic(background_music_);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Audio system initialization failed: " << e.what() << std::endl;
    }

}



Game::~Game() {
    // Stop all audio before destroying game objects
    audio_system_.StopAll();

    // Free memory used by game world
    DestroyGameWorld();

    // Cleanup textures
    if (tex_) {
        glDeleteTextures(num_textures, tex_);
        delete[] tex_;
        tex_ = nullptr;
    }

    // Free rendering resources
    delete sprite_;
    sprite_ = nullptr;

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();

    delete minimap_sprite_;
    minimap_sprite_ = nullptr;
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}

void Game::SetTexture(GLuint w, const char* fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image) {
        std::cout << "Cannot load texture " << fname << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Check if this is the stars/background texture and enable tiling
    std::string filename(fname);
    if (filename.find("stars.png") != std::string::npos || filename.find("stage_2.png") != std::string::npos) {
        // Enable texture tiling/wrapping for the background
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else {
        // Default texture wrapping for other textures
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


} // namespace game