#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"
#include "geometry.h"
#include "audio_system.h"
#include "collectible_game_object.h"
#include "enemy_game_object.h"
#include "player_game_object.h"
#include "projectile_game_object.h"
#include "ray_circle_collision.h"
#include "fast_enemy_game_object.h"
#include "heavy_enemy_game_object.h"
#include "homing_missile.h"
#include "asteroid_game_object.h"
#include "enemy_counter.h"
#include "game_timer.h"
#include "health_bar.h"
#include "boss_game_object.h"
#include "space_dust.h"  // Add space dust header
// #include "missile_game_object.h"
#include "missile_flame.h"
#include "mini_map.h"
#include "minimap_sprite.h" 


namespace game {

    // Forward declarations
    class MissileFlame;

    // A class for holding the main game objects
    class Game {

    public:
        // Constructor and destructor
        Game(void);
        ~Game();

        // Call Init() before calling any other method
        // Initialize graphics libraries and main window
        void Init(void);

        // Set up the game world (scene, game objects, etc.)
        void SetupGameWorld(void);

        // Destroy the game world
        void DestroyGameWorld(void);

        // Run the game (keep the game active)
        void MainLoop(void);

        // Enemy count tracking
        int regular_enemy_count_;
        int fast_enemy_count_;
        int heavy_enemy_count_;
        const int MAX_ENEMIES_PER_TYPE = 2;

        inline int GetCurrentLevel() const { return current_level_; }

        void SetupBoss(void);
        void UpdateBoss(double delta_time);
        void TransitionToLevel2(void);

        void GameOver(const std::string& reason);



    private:
        // void CreateMissile(const glm::vec3& position, const glm::vec3& direction);

        bool game_over_;
        Timer game_over_timer_;

        //stage 2
        int current_level_;
        bool level_transition_active_;
        Timer level_transition_timer_;
        bool transitioning_to_level2_;
        int enemy_kills_;
        const int ENEMITES_TO_NEXT_LEVEL = 10;
        GLuint boss_level_background_texture_;
        BossGameObject* boss_;
        void CheckLevelProgression();
        void ClearGameObjects(bool keep_player_and_background = true);
        void SpawnBoss();
        void ProcessBossProjectileCollisions(Projectile* projectile, BossGameObject* boss, double delta_time);

        // Main window: pointer to the GLFW window structure
        GLFWwindow* window_;

        // Sprite geometry
        Geometry* sprite_;
        void SpawnAsteroid(int count);
        void ProcessAsteroidCollisions();


        // Shader for rendering sprites in the scene
        Shader sprite_shader_;

        // timer for spawning enemies
        Timer enemy_spawn_timer_;

        // References to textures
        // This needs to be a pointer
        GLuint* tex_;

        // List of game objects
        std::vector<GameObject*> game_objects_;

        Shader particle_shader_;  // Shader for particle systems
        Shader dust_shader_;      // Shader for space dust particles
        SpaceDust* space_dust_;   // Space dust particle system

        void CreateHomingMissile(const glm::vec3& position, const glm::vec3& direction);
        GameObject* FindClosestEnemy(const glm::vec3& position, float max_distance = 15.0f);

        // Keep track of time
        double current_time_;

        // Handle user input
        void HandleControls(double delta_time);

        // Update all the game objects
        void Update(double delta_time);

        // Helper function to spawn an enemy at a random position
        void SpawnEnemy();

        // Spawn initial enemies with a more distributed pattern
        void SpawnInitialEnemies(int count);

        // Create fast enemy
        FastEnemyGameObject* CreateFastEnemy(const glm::vec3& position);

        // Create heavy enemy
        HeavyEnemyGameObject* CreateHeavyEnemy(const glm::vec3& position);

        // Helper function to create a new enemy object
        EnemyGameObject* CreateEnemy(const glm::vec3& position);

        // Get reference to player object
        PlayerGameObject* GetPlayer() const;

        // Create a projectile from the player
        void CreateProjectile(const glm::vec3& position, const glm::vec3& direction);

        // Process collisions between projectiles and enemies using ray-circle collision
        void ProcessProjectileEnemyCollisions(Projectile* projectile, EnemyGameObject* enemy, double delta_time);

        // Audio system
        AudioSystem audio_system_;      // Audio system
        ALuint background_music_;       // Background music
        ALuint explosion_sound_;        // Explosion sound
        ALuint pickup_sound_;           // Pickup orb sound
        ALuint invincible_start_sound_; // Invincible start sound
        ALuint invincible_end_sound_;   // Invincible end sound
        ALuint fire_sound_;             // Projectile firing sound
        ALuint enemy_fire_sound_; // Sound for enemy firing
        ALuint hit_sound_;        // Sound for player being hit


        // Render the game world
        void Render(void);

        // Callback for when the window is resized
        static void ResizeCallback(GLFWwindow* window, int width, int height);

        // Set a specific texture
        void SetTexture(GLuint w, const char* fname);

        // Load all textures
        void LoadTextures(std::vector<std::string>& textures);

        // Spawn collectibles

        void SpawnCollectibles(int count);

        GLuint enemy_projectile_texture_; // Add to texture enum section or as a separate variable
        void CreateEnemyProjectile(const glm::vec3& position, const glm::vec3& direction, int enemy_type = 0);

        void ProcessBossPlayerCollision(double delta_time);

        MiniMap mini_map_;
        Shader minimap_shader_;

        // UI elements
        HealthBar health_bar_;
        EnemyCounter* enemy_count_;
        GameTimer* time_ui_;
        Shader text_shader_;

        void RenderUI();
        void InitUI(void);

        MinimapSprite* minimap_sprite_; // Use a dedicated sprite for minimap


        int num_textures;

        enum {
            tex_player = 0,
            tex_enemy1 = 1,
            tex_enemy2 = 2,
            tex_enemy_fast = 3,         // New texture for fast enemy
            tex_enemy_heavy = 4,        // New texture for heavy enemy
            tex_stars = 5,
            tex_orb = 6,
            tex_explosion = 7,
            tex_player_invincible = 8,
            tex_projectile = 9,
            tex_particle = 10,          // Texture for particle effects
            tex_missile = 11,           // Texture for homing missiles
            tex_enemy_projectile = 12,  // Texture for enemy projectiles
            tex_asteroid = 13,          // Texture for asteroids
            tex_font = 14,
            tex_collectible_speed = 15,
            tex_collectible_power = 16,
            tex_health_bar_background = 17,
            tex_health_bar_fill = 18,
            tex_stage_2 = 19,
            tex_base = 20,
            tex_lower_part = 21,
            tex_upper_part = 22,
            tex_dust_particle = 23,     // Texture for space dust particles
            TEXTURE_COUNT
        };
    }; // class Game

} // namespace game
#endif // GAME_H