#include <Geode/loader/SettingEvent.hpp>
#include "include.hpp"
#include "objects/HttpInfo.hpp"
#include "scenes/InterceptPopup.hpp"

#ifdef GEODE_IS_WINDOWS
    #include <geode.custom-keybinds/include/Keybinds.hpp>

    $execute {
        using namespace keybinds;

        BindManager* manager = BindManager::get();

        manager->registerBindable({
            "open_capture_menu"_spr,
            "Open Capture Menu",
            "Opens the menu with the captured packets",
            { Keybind::create(KEY_I, Modifier::Alt) },
            "GD Intercept"
        });
        manager->registerBindable({
            "copy_code_block"_spr,
            "Copy Code Block",
            "Copies the contents of the intercept menu code block to the clipboard",
            {
                Keybind::create(KEY_C, Modifier::Control),
                // This will only work if this extension finally gets MacOS support
                Keybind::create(KEY_C, Modifier::Command)
            },
            "GD Intercept"
        });

        new EventListener([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                InterceptPopup::scene();
            }

            return ListenerResult::Propagate;
        }, InvokeBindFilter(nullptr, "open_capture_menu"_spr));

        new EventListener([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                OPT(InterceptPopup::get())->copyCode();
            }

            return ListenerResult::Propagate;
        }, InvokeBindFilter(nullptr, "copy_code_block"_spr));
    }
#endif

$execute {
    listenForSettingChanges("remember-requests", +[](const bool value) {
        if (!value) {
            for (size_t i = 1; i < HttpInfo::requests.size(); i++) {
                delete HttpInfo::requests.at(i);
            }

            if (HttpInfo::requests.size() > 1) {
                HttpInfo::requests.resize(1);
            }

            OPT(InterceptPopup::get())->reload();
        }
    });

    listenForSettingChanges("cache", +[](const bool value) {
        if (!value) {
            for (HttpInfo* request : HttpInfo::requests) {
                request->resetCache();
            }
        }
    });

    listenForAllSettingChanges(+[](SettingValue* event) {
        OPT(InterceptPopup::get())->reload();
    });
}