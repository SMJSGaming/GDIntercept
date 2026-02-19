#include "CCEGLView.hpp"

void ModCCEGLView::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y) {
    CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(
        -12 * std::clamp<int>(y, -1, 1),
        12 * std::clamp<int>(x, -1, 1)
    );
}