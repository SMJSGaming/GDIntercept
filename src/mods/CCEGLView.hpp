#pragma once

#ifdef GEODE_IS_WINDOWS
    #include <Geode/modify/CCEGLView.hpp>
    #include "../include.hpp"

    class $modify(ModCCEGLView, CCEGLView) {
        static void onModify(auto& self) {
            (void) self.setHookPriority("cocos2d::CCEGLView::onGLFWMouseScrollCallback", Priority::Replace + 1);
        }

        void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
    };
#endif