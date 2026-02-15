#pragma once

#include "../../include.hpp"

template<Node T = CCNode>
class KeybindNode : public T {
protected:
    bool init() {
        this->setup();

        return true;
    }

    template<typename F> requires(std::invocable<F>)
    void bind(std::string keybindName, F&& callback, const bool repeatable = false) {
        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), std::move(keybindName)), [
            callback = std::forward<F>(callback),
            repeatable
        ](const Keybind& keybind, const bool down, const bool repeat) {
            if (down && (repeatable || !repeat)) {
                callback();

                return ListenerResult::Stop;
            } else {
                return ListenerResult::Propagate;
            }
       });
    }

    virtual void setup() = 0;
};