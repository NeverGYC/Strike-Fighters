#ifndef ENEMY_COUNTER_H_
#define ENEMY_COUNTER_H_

#include <string>
#include <sstream>
#include <iomanip>
#include "text_game_object.h"
#include <GLFW/glfw3.h>

namespace game {

    // Enemy counter class for tracking destroyed enemies
    class EnemyCounter : public TextGameObject {

    public:
        EnemyCounter(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        // Increment the enemy counter
        void IncrementCount();
        void ResetCount();

        // Getter
        int GetCount() const;

        void Update(double delta_time) override;

        // Render for UI
        void RenderUI(glm::mat4 ui_projection_matrix, double current_time);

    private:
        int enemy_count_;
        std::string count_text_;

        // Helper to format the counter display
        void UpdateDisplayText();
    };

}

#endif