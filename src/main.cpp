#include <Geode/loader/SettingEvent.hpp>
#include "include.hpp"
#include "scenes/InterceptPopup.hpp"

#ifdef KEYBINDS_ENABLED
    $execute {
        BindManager* manager = BindManager::get();

        manager->registerBindable({
            "open_capture_menu"_spr,
            "Open Capture Menu",
            "Opens the menu with the captured packets",
            { Keybind::create(KEY_I, Modifier::Alt) },
            "GD Intercept"
        });
        manager->registerBindable({
            "next_packet"_spr,
            "Next Packet",
            "Selects the next packet in the capture menu",
            {
                Keybind::create(KEY_Tab),
                Keybind::create(KEY_Down, Modifier::Shift),
            },
            "GD Intercept"
        });
        manager->registerBindable({
            "previous_packet"_spr,
            "Previous Packet",
            "Selects the previous packet in the capture menu",
            {
                // This will clash with steam overlay, but I'll leave it to the user to fix that
                Keybind::create(KEY_Tab, Modifier::Shift),
                Keybind::create(KEY_Up, Modifier::Shift)
            },
            "GD Intercept"
        });
        manager->registerBindable({
            "toggle_pause"_spr,
            "Toggle Pause",
            "Pauses or resumes packets captured by the proxy",
            { Keybind::create(KEY_Space) },
            "GD Intercept"
        });
        manager->registerBindable({
            "resend"_spr,
            "Resend",
            "Resends the current packet shown in the capture menu",
            {
                Keybind::create(KEY_R, Modifier::Control),
                Keybind::create(KEY_R, Modifier::Command)
            },
            "GD Intercept"
        });
        manager->registerBindable({
            "copy_info_block"_spr,
            "Copy Info",
            "Copies the contents of the request info",
            {
                Keybind::create(KEY_C, Modifier::Alt)
            },
            "GD Intercept"
        });
        manager->registerBindable({
            "copy_code_block"_spr,
            "Copy",
            "Copies the contents of the intercept menu code block to the clipboard",
            {
                Keybind::create(KEY_C, Modifier::Control),
                // This will only work if this extension finally gets MacOS support
                Keybind::create(KEY_C, Modifier::Command)
            },
            "GD Intercept/Code Block"
        });
        manager->registerBindable({
            "code_line_up"_spr,
            "Line Up",
            "Scrolls one line up in the code block",
            {
                Keybind::create(KEY_Up)
            },
            "GD Intercept/Code Block"
        });
        manager->registerBindable({
            "code_page_up"_spr,
            "Page Up",
            "Scrolls one page up in the code block",
            {
                Keybind::create(KEY_PageUp),
                Keybind::create(KEY_Up, Modifier::Control),
                Keybind::create(KEY_Up, Modifier::Command)
            },
            "GD Intercept/Code Block"
        });
        manager->registerBindable({
            "code_line_down"_spr,
            "Line Down",
            "Scrolls one line down in the code block",
            {
                Keybind::create(KEY_Down)
            },
            "GD Intercept/Code Block"
        });
        manager->registerBindable({
            "code_page_down"_spr,
            "Page Down",
            "Scrolls one page down in the code block",
            {
                Keybind::create(KEY_PageDown),
                Keybind::create(KEY_Down, Modifier::Control),
                Keybind::create(KEY_Down, Modifier::Command)
            },
            "GD Intercept/Code Block"
        });

        new EventListener([=](const InvokeBindEvent* event) {
            if (event->isDown()) {
                InterceptPopup::scene();
            }

            return ListenerResult::Propagate;
        }, InvokeBindFilter(nullptr, "open_capture_menu"_spr));
    }
#endif

$execute {
    new EventListener([=](const RequestEvent* event) {
        OPT(InterceptPopup::get())->softReload();

        if (Mod::get()->getSettingValue<bool>("log-requests")) {
            const HttpInfo::Request request = event->getRequest();
            const HttpInfo::URL url = request.getURL();

            log::info("Sending request:\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nHeaders: {}\nBody: {}",
                url.getMethod(),
                url.stringifyProtocol(),
                url.getPortHost(),
                url.getPath(),
                url.stringifyQuery(),
                request.stringifyHeaders(),
                request.getBodyContent().contents
            );
        }

        return ListenerResult::Propagate;
    }, RequestFilter());

    new EventListener([=](const ResponseEvent* event) {
        OPT(InterceptPopup::get())->softReload();

        return ListenerResult::Propagate;
    }, ResponseFilter());

    listenForSettingChanges("cache-limit", +[](const int64_t value) {
        ProxyHandler::setCacheLimit(value);

        OPT(InterceptPopup::get())->reload();
    });

    listenForSettingChanges("pause-requests", +[](const bool value) {
        if (!value) {
            ProxyHandler::resumeAll();
        }
    });

    listenForAllSettingChanges(+[](SettingValue* event) {
        const static std::vector<std::string> softList = {
            "hide-badges"
        };
        const static std::vector<std::string> blacklist = {
            "cache-limit",
            "pause-requests",
            "log-requests"
        };

        if (std::find(blacklist.begin(), blacklist.end(), event->getKey()) == blacklist.end()) {
            if (std::find(softList.begin(), softList.end(), event->getKey()) != softList.end()) {
                OPT(InterceptPopup::get())->softReload();
            } else {
                OPT(InterceptPopup::get())->reload();
            }
        }
    });
}