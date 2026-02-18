#include "include.hpp"
#include "nodes/InterceptPopup.hpp"
#include "settings/DynamicEnum.hpp"

$on_game(Loaded) {
    listenForKeybindSettingPresses("open_capture_menu", [](const Keybind& keybind, const bool down, const bool repeat, const double time) {
        if (down && !repeat) {
            InterceptPopup::scene();

            return ListenerResult::Stop;
        } else {
            return ListenerResult::Propagate;
        }
    });
}

$execute {
    ProxyEvent(EventState::REQUEST).listen([](std::shared_ptr<HttpInfo> event) {
        Loader::get()->queueInMainThread([]() { OPT(InterceptPopup::get())->reloadList(); });

        if (Mod::get()->getSettingValue<bool>("log-requests")) {
            log::info("Sending request:\n{}", event->toString());
        }

        return ListenerResult::Propagate;
    }).leak();

    ProxyEvent(EventState::RESPONSE).listen([](std::shared_ptr<HttpInfo> event) {
        Loader::get()->queueInMainThread([]() {
            InterceptPopup* popup = InterceptPopup::get();

            if (popup != nullptr) {
                popup->reloadList();
                popup->reloadSideBar();
            }
        });

        if (Mod::get()->getSettingValue<bool>("log-http-messages")) {
            log::info("HTTP message:\n{}", event->toString());
        }

        return ListenerResult::Propagate;
    }).leak();

    ProxyEvent(EventState::CANCEL).listen([](std::shared_ptr<HttpInfo> event) {
        Loader::get()->queueInMainThread([]() {
            InterceptPopup* popup = InterceptPopup::get();

            if (popup != nullptr) {
                popup->reloadList();
                popup->reloadSideBar();
            }
        });

        return ListenerResult::Propagate;
    }).leak();

    listenForAllSettingChanges([](const std::string_view key, std::shared_ptr<SettingV3> setting) {
        static const StringStream listReloads = {
            "filter",
            "cache-limit",
            "hide-badges"
        };
        static const StringStream codeBlockReloads = {
            "theme"
        };
        static const StringStream codeReloads = {
            "filter",
            "cache-limit",
            "censor-data",
            "raw-data",
            "decode-data",
            "max-characters-per-line"
        };
        static const StringStream sideMenuReloads = {
            "minimize-side-menu"
        };

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
    }, Mod::get());
}

$on_game(Loaded) {
    DynamicEnum::registerLoader("theme"_spr, Theme::Theme::load);
    DynamicEnum::registerLoader("theme"_spr, []{
        OPT(InterceptPopup::get())->reloadCodeBlock(true);
    });
    DynamicEnum::reloadDynamicEnums("theme"_spr);
}