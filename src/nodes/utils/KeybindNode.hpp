#pragma once

#include "../../include.hpp"

template<Node T = CCNode>
class KeybindNode : public T {
protected:
    bool init() {
        #ifdef KEYBINDS_ENABLED
            this->setup();
        #endif

        return true;
    }

    void bind(const std::string& key, const std::function<void()>& callback) {
        #ifdef KEYBINDS_ENABLED
            this->template addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
                if (event->isDown()) {
                    callback();
                }

                return ListenerResult::Propagate;
            }, key);
        #endif
    }

    virtual void setup() = 0;
};