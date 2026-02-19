#pragma once

#include <Geode/modify/CCEGLView.hpp>
#include "../include.hpp"

class $modify(ModCCEGLView, CCEGLView) {
    void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
};