#include "include.hpp"
#include "nodes/InterceptPopup.hpp"
#include "settings/DynamicEnum.hpp"

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
            "collapse_side_bar"_spr,
            "Collapse Side Bar",
            "Toggles the side bar between collapsed and expanded",
            {
                Keybind::create(KEY_Enter, Modifier::Control)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "open_save_files"_spr,
            "Open Save Files",
            "Opens the save folder of GDIntercept",
            {
                Keybind::create(KEY_O, Modifier::Control),
                Keybind::create(KEY_O, Modifier::Command)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "open_theme_files"_spr,
            "Open Theme Files",
            "Opens the theme folder of GDIntercept",
            {
                Keybind::create(KEY_O, Modifier::Alt)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "save_packet"_spr,
            "Save",
            "Saves the selected packet to a file",
            {
                Keybind::create(KEY_S, Modifier::Control),
                Keybind::create(KEY_S, Modifier::Command)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "copy_code_block"_spr,
            "Copy",
            "Copies the contents of the current code in view",
            {
                Keybind::create(KEY_C, Modifier::Control),
                Keybind::create(KEY_C, Modifier::Command)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "resend_packet"_spr,
            "Resend",
            "Resends the selected packet",
            {
                Keybind::create(KEY_R, Modifier::Control),
                Keybind::create(KEY_R, Modifier::Command)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "pause_packet"_spr,
            "Pause/Resume",
            "Either pauses or resumes the selected packet",
            {
                Keybind::create(KEY_Space, Modifier::Control),
                Keybind::create(KEY_Space, Modifier::Command)
            },
            "GD Intercept/Side Bar"
        });
        manager->registerBindable({
            "raw_code_block"_spr,
            "Raw/Formatted Code",
            "Either shows the raw or formatted code",
            {
                Keybind::create(KEY_L, Modifier::Control),
                Keybind::create(KEY_L, Modifier::Command)
            },
            "GD Intercept/Side Bar"
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
            "code_line_down"_spr,
            "Line Down",
            "Scrolls one line down in the code block",
            {
                Keybind::create(KEY_Down)
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
        manager->registerBindable({
            "code_page_left"_spr,
            "Page Left",
            "Scrolls one page left in the code block",
            {
                Keybind::create(KEY_Left, Modifier::Control),
                Keybind::create(KEY_Left, Modifier::Command)
            },
            "GD Intercept/Code Block"
        });
        manager->registerBindable({
            "code_page_right"_spr,
            "Page Right",
            "Scrolls one page right in the code block",
            {
                Keybind::create(KEY_Right, Modifier::Control),
                Keybind::create(KEY_Right, Modifier::Command)
            },
            "GD Intercept/Code Block"
        });

        new EventListener([](const InvokeBindEvent* event) {
            if (event->isDown()) {
                InterceptPopup::scene();
            }

            return ListenerResult::Propagate;
        }, InvokeBindFilter(nullptr, "open_capture_menu"_spr));
    }
#endif

$execute {
    new EventListener([](const RequestEvent* event) {
        OPT(InterceptPopup::get())->reloadList();

        if (Mod::get()->getSettingValue<bool>("log-requests")) {
            const HttpInfo::Request& request = event->getRequest();
            const URL& url = request.getURL();

            log::info("Sending request:\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nHeaders: {}\nBody: {}",
                request.getMethod(),
                url.getProtocol(),
                url.getHost(),
                url.getPath(),
                url.stringifyQuery(),
                request.getHeaderList(true).contents,
                request.getBodyContent().contents
            );
        }

        return ListenerResult::Propagate;
    }, RequestFilter());

    new EventListener([](const ResponseEvent* event) {
        InterceptPopup* popup = InterceptPopup::get();

        if (popup != nullptr) {
            popup->reloadList();
            popup->reloadSideBar();
        }

        if (Mod::get()->getSettingValue<bool>("log-http-messages")) {
            const HttpInfo::Request& request = event->getRequest();
            const HttpInfo::Response& response = event->getResponse();
            const URL& url = request.getURL();

            log::info("HTTP message:\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nHeaders: {}\nBody: {}\nResponse: {}\nResponse Headers: {}",
                request.getMethod(),
                url.getProtocol(),
                url.getHost(),
                url.getPath(),
                url.stringifyQuery(),
                request.getHeaderList(false).contents,
                request.getBodyContent().contents,
                response.getResponse(),
                response.getHeaderList(false).contents
            );
        }

        return ListenerResult::Propagate;
    }, ResponseFilter());

    new EventListener([](const CancelEvent* event) {
        InterceptPopup* popup = InterceptPopup::get();

        if (popup != nullptr) {
            popup->reloadList();
            popup->reloadSideBar();
        }

        return ListenerResult::Propagate;
    }, CancelFilter());

    listenForAllSettingChanges([](std::shared_ptr<SettingV3> setting) {
        static const Stream<std::string> listReloads = {
            "filter",
            "cache-limit",
            "hide-badges"
        };
        static const Stream<std::string> codeBlockReloads = {
            "theme"
        };
        static const Stream<std::string> codeReloads = {
            "filter",
            "cache-limit",
            "censor-data",
            "raw-data",
            "decode-data",
            "max-characters-per-line"
        };
        static const Stream<std::string> sideMenuReloads = {
            "minimize-side-menu"
        };
        const std::string key = setting->getKey();

        if (key == "cache-limit") {
            ProxyHandler::setCacheLimit(Mod::get()->getSettingValue<int64_t>(key));
        }

        const bool reloadsList = listReloads.includes(key);

        if (reloadsList) {
            OPT(InterceptPopup::get())->reloadList();
        }

        if (codeBlockReloads.includes(key)) {
            OPT(InterceptPopup::get())->reloadCodeBlock(!reloadsList);
        }

        if (codeReloads.includes(key)) {
            OPT(InterceptPopup::get())->reloadCode();
        }

        if (sideMenuReloads.includes(key)) {
            OPT(InterceptPopup::get())->reloadSideBar();
        }
    });
}

$on_mod(Loaded) {
    DynamicEnum::registerLoader("theme"_spr, Theme::Theme::load);
    DynamicEnum::registerLoader("theme"_spr, []{
        OPT(InterceptPopup::get())->reloadCodeBlock(true);
    });
    DynamicEnum::reloadDynamicEnums("theme"_spr);
}