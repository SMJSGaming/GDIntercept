#include <Geode/loader/SettingEvent.hpp>
#include "include.hpp"
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
    new EventListener([=](RequestEvent* event) {
        if (Mod::get()->getSettingValue<bool>("log-requests")) {
            HttpInfo* request = event->getRequest();

            log::info("Sending request:\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nHeaders: {}\nBody: {}",
                request->stringifyMethod(),
                request->stringifyProtocol(),
                request->getHost(),
                request->getPath(),
                request->stringifyQuery(),
                request->stringifyHeaders(),
                request->getBodyContent(false).contents
            );
        }

        return ListenerResult::Propagate;
    }, RequestFilter(GD));

    new EventListener([=](ResponseEvent* event) {
        log::debug("This is the response my server is giving back: {}", event->getResponse().contents);

        return ListenerResult::Propagate;
    }, ResponseFilter({ "https://myveryspecific.endpoint/with_path" }));

    listenForSettingChanges("remember-requests", +[](const bool value) {
        if (!value) {
            ProxyHandler::forgetProxies();
            OPT(InterceptPopup::get())->reload();
        }
    });

    listenForSettingChanges("cache", +[](const bool value) {
        if (!value) {
            ProxyHandler::resetCache();
        }
    });

    listenForAllSettingChanges(+[](SettingValue* event) {
        std::vector<std::string> blacklist({ "remember-requests", "cache" });

        if (std::find(blacklist.begin(), blacklist.end(), event->getKey()) == blacklist.end()) {
            OPT(InterceptPopup::get())->reload();
        }
    });
}