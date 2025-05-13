#ifndef GAME_TIMER_H
#define GAME_TIMER_H

#include <GL/glew.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <GLFW/glfw3.h>
#include "text_game_object.h"

namespace game {
    class GameTimer : public TextGameObject {
    public:
        // Constructor
        GameTimer(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

        void Update(double delta_time) override;

        void Render(glm::mat4 view_matrix, double current_time) override;

        void RenderUI(glm::mat4 ui_projection_matrix, double current_time);

        // Setter
        inline void SetReferenceTime(double time) { start_time_ = time; }
        // Getter
        inline double GetReferenceTime() const { return start_time_; }

        std::string FormatTime(double seconds) const;

    private:
        double start_time_;
        std::string last_time_string_;
        int last_update_seconds_;
    };
}

#endif // GAME_TIMER_H