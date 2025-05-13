#include <GLFW/glfw3.h>
#include "timer.h"

namespace game {

    Timer::Timer(void) : start_time_(0.0), end_time_(0.0), running_(false) {
    }

    Timer::~Timer() {
    }

    void Timer::Start(float end_time) {
        start_time_ = glfwGetTime();
        end_time_ = start_time_ + end_time;
        running_ = true;
    }

    bool Timer::Finished(void) const {
        if (!running_) return false;
        return glfwGetTime() >= end_time_;
    }

} // namespace game