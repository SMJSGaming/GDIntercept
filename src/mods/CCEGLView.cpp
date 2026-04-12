#include "CCEGLView.hpp"

#ifdef GEODE_IS_WINDOWS
    void ModCCEGLView::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y) {
        CCDirector::sharedDirector()->getMouseDispatcher()->dispatchScrollMSG(-12 * y, 12 * x);
    }
#endif