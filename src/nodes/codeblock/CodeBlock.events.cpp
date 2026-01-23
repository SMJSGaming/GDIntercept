#include "CodeBlock.hpp"

using namespace SideBarCell;

const std::vector<SideBarView> CodeBlock::VIEWS({
    { 'I', [](const auto& icons) { return icons.info; }, "Info", [](CodeBlock* block) { block->onInfo(); } },
    { 'B', [](const auto& icons) { return icons.body; }, "Body", [](CodeBlock* block) { block->onBody(); } },
    { 'Q', [](const auto& icons) { return icons.query; }, "Query", [](CodeBlock* block) { block->onQuery(); } },
    { 'H', [](const auto& icons) { return icons.header; }, "Request Headers", [](CodeBlock* block) { block->onRequestHeaders(); } },
    { 'R', [](const auto& icons) { return icons.response; }, "Response", [](CodeBlock* block) { block->onResponse(); } },
    { 'H', [](const auto& icons) { return icons.header; }, "Response Headers", [](CodeBlock* block) { block->onResponseHeaders(); } }
});

const SideBar::Categories CodeBlock::ACTIONS {
    { { "Themes", "themes.png"_spr }, {
        { { "Open Theme Files", "folder.png"_spr, [](CodeBlock* block) { return block->onOpenThemeFiles(); } } },
        { { "Open Theme Docs", "docs.png"_spr, [](CodeBlock* block) { return block->onDocs(); } } }
    } },
    { { "Save Files", "saves.png"_spr }, {
        { { "Open Save Files", "folder.png"_spr, [](CodeBlock* block) { return block->onOpenSaveFiles(); } } },
        { { "Save", "download.png"_spr, [](CodeBlock* block) { return block->onSave(); } }, [](CodeBlock* block) {
            return block->m_info;
        } }
    } },
    { { "Current Request", "link.png"_spr }, {
        { { "Copy", "copy.png"_spr, [](CodeBlock* block) { return block->onCopy(); } }, [](CodeBlock* block) {
            return block->m_info;
        } },
        { { "Resend", "redo.png"_spr, [](CodeBlock* block) { return block->onSend(); } }, [](CodeBlock* block) {
            return block->m_info;
        } },
        { { "Cancel", "cancel.png"_spr, [](CodeBlock* block) { return block->onCancel(); } }, [](CodeBlock* block) {
            return block->m_info && (block->m_info->isInProgress() || block->m_info->isPaused());
        } }
    } },
    { {"Settings", "settings.png"_spr }, {
        {
            { "Resume All", "resume.png"_spr, [](CodeBlock* block) { return block->onResume(); } },
            { "Pause All", "pause.png"_spr, [](CodeBlock* block) { return block->onPause(); } },
            [](CodeBlock* block) { return ProxyHandler::isPaused(); }
        },
        { "raw-data", { "Raw", "Formatted" }, { "raw.png"_spr, "simplified.png"_spr }, false, {
            [](CodeBlock* block, const OriginalCallback& original) {
                return block->onRaw(original);
            },
            [](CodeBlock* block, const OriginalCallback& original) {
                return block->onFormatted(original);
            }
        } }
    } }
};

bool CodeBlock::ACCEPTED_PAUSES = false;

void CodeBlock::setup() {
    this->bind("open_save_files"_spr, [this]() {
        this->onOpenSaveFiles();
    });
    this->bind("open_theme_files"_spr, [this]() {
        this->onOpenThemeFiles();
    });
    this->bind("save_packet"_spr, [this]() {
        this->onSave();
    });
    this->bind("copy_code_block"_spr, [this]() {
        this->onCopy();
    });
    this->bind("resend_packet"_spr, [this]() {
        this->onSend();
    });
    this->bind("pause_packet"_spr, [this]() {
        if (ProxyHandler::isPaused()) {
            this->onResume();
        } else {
            this->onPause();
        }
    });
    this->bind("raw_code_block"_spr, [this]() {
        Mod* mod = Mod::get();

        if (mod->getSettingValue<bool>("raw-data")) {
            this->onFormatted([mod]() {
                mod->setSettingValue("raw-data", false);

                return true;
            });
        } else {
            this->onRaw([mod]() {
                mod->setSettingValue("raw-data", true);

                return true;
            });
        }
    });

    this->bind("code_line_up"_spr, [this]() {
        const Theme::Theme theme = Theme::getTheme();

        this->scroll(0, -(theme.code.font.getTrueFontSize().height + theme.code.font.lineHeight));
    });
    this->bind("code_line_down"_spr, [this]() {
        const Theme::Theme theme = Theme::getTheme();

        this->scroll(0, theme.code.font.getTrueFontSize().height + theme.code.font.lineHeight);
    });
    this->bind("code_page_up"_spr, [this]() {
        this->scroll(0, -this->getNode()->getContentHeight());
    });
    this->bind("code_page_down"_spr, [this]() {
        this->scroll(0, this->getNode()->getContentHeight());
    });
    this->bind("code_page_left"_spr, [this]() {
        const CullingList* list = typeinfo_cast<CullingList*>(this->getNode());
        const std::vector<CullingCell*>& cells = list->getCells();

        this->scroll(cells.empty() ? 0 : list->getContentWidth() - typeinfo_cast<CodeLineCell*>(cells.front())->getCodeLineWidth(), 0);
    });
    this->bind("code_page_right"_spr, [this]() {
        const CullingList* list = typeinfo_cast<CullingList*>(this->getNode());
        const std::vector<CullingCell*>& cells = list->getCells();

        this->scroll(cells.empty() ? 0 : -(list->getContentWidth() - typeinfo_cast<CodeLineCell*>(cells.front())->getCodeLineWidth()), 0);
    });
}

bool CodeBlock::onCopy() {
    utils::clipboard::write(m_code);
    this->showMessage("Code Copied");

    return true;
}

bool CodeBlock::onSend() {
    const HttpInfo::Request& original = m_info->getRequest();
    const std::string& method = original.getMethod();
    const std::string& body = original.getBody();
    web::WebRequest request;

    if (body.size()) {
        request.bodyString(body);
    }

    request.header(ProxyHandler::getCopyHandshake(), "true");

    for (const auto& [name, list] : original.getHeaders()) {
        for (const std::string_view value : list) {
            request.header(name, value);
        }
    }

    (void) request.send(method, original.getURL().getReconstruction());

    this->showMessage("Request Resent");

    return true;
}

bool CodeBlock::onCancel() {
    m_info->cancel();
    this->showMessage("Request Cancelled");

    return true;
}

bool CodeBlock::onOpenSaveFiles() {
    Mod* mod = Mod::get();

    utils::file::openFolder(mod->getSavedValue("default-path", mod->getSaveDir()));

    return true;
}

bool CodeBlock::onOpenThemeFiles() {
    utils::file::openFolder(Mod::get()->getConfigDir() / "themes");

    return true;
}

bool CodeBlock::onSave() {
    Mod* mod = Mod::get();
    const HttpInfo::Request& request = m_info->getRequest();
    const URL& url = request.getURL();
    const HttpInfo::Response& response = m_info->getResponse();
    std::string host = url.getHost();
    std::string bodyContent = request.getBodyContent().contents;
    std::string responseContent = response.getResponseContent().contents;
    std::string filename = fmt::format("{:%d-%m-%Y %H.%M.%S} - {}.txt", fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), host);
    std::string contents = fmt::format("Method: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nRequest Headers: {}\nBody:{}\nResponse Headers: {}\nResponse:{}",
        request.getMethod(),
        url.getProtocol(),
        std::move(host),
        url.getPath(),
        url.stringifyQuery(),
        request.getHeaderList(true).contents,
        bodyContent.empty() ? "" : fmt::format("\n{}", std::move(bodyContent)),
        response.getHeaderList(true).contents,
        responseContent.empty() ? "" : fmt::format("\n{}", std::move(responseContent))
    );

    utils::file::pick(file::PickMode::OpenFolder, {
        .defaultPath = mod->getSavedValue("default-path", mod->getSaveDir())
    }).listen([this, mod, filename = std::move(filename), contents = std::move(contents)](const Result<std::filesystem::path>* path) {
        Loader::get()->queueInMainThread([this, mod, path, filename = std::move(filename), contents = std::move(contents)]{
            if (path->isErr()) {
                this->showMessage("Error Picking File", { 0xFF, 0x00, 0x00 });
            } else if (utils::file::writeString(path->unwrap() / std::move(filename), std::move(contents)).isOk()) {
                mod->setSavedValue("default-path", path->unwrap());

                this->showMessage("File Saved");
            } else {
                this->showMessage("Error Saving File", { 0xFF, 0x00, 0x00 });
            }
        });
    }, [](std::monostate*){}, [this]{
        Loader::get()->queueInMainThread([this]{
            this->showMessage("File Save Cancelled", { 0xFF, 0xFF, 0x00 });
        });
    });

    return true;
}

bool CodeBlock::onPause() {
    if (CodeBlock::ACCEPTED_PAUSES) {
        Warning::show();
        this->showMessage("Requests Paused");

        return true;
    } else {
        FLAlertLayer::create(this, "Pausing Requests",
            "<cr>All requests</c> will be <cy>paused</c> until you resume them. "
            "This means that you will no longer be able to send or receive online data from any mod or GD.\n\n"
            "Are you sure you want to <cy>pause</c> <cr>all requests</c>?"
        , "Cancel", "Ok")->show();

        return false;
    }
}

bool CodeBlock::onResume() {
    ProxyHandler::resumeAll();
    Warning::hide();
    this->showMessage("Requests Resumed");

    return true;
}

bool CodeBlock::onRaw(const SideBarCell::OriginalCallback& original) {
    this->showMessage("Raw Data Enabled");

    return original();
}

bool CodeBlock::onFormatted(const SideBarCell::OriginalCallback& original) {
    this->showMessage("Formatted Data Enabled");

    return original();
}

bool CodeBlock::onDocs() {
    utils::web::openLinkInBrowser("https://bin.smjsproductions.com/smjs.gdintercept/pages");

    return false;
}

void CodeBlock::onInfo() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else {
        const HttpInfo::Request& request = m_info->getRequest();
        const HttpInfo::Response& response = m_info->getResponse();
        const URL& url = request.getURL();
        LookupTable<std::string, std::string> info {
            { "Client", m_info->getClient() == Client::COCOS ? "Cocos2D-X" : "Geode" },
            { "Status Code", response.stringifyStatusCode() },
            { "Method", request.getMethod() },
            { "Protocol", url.getProtocol() },
            { "Host", url.getHost() },
            { "Path", url.getPath() }
        };

        if (m_info->isCompleted()) {
            info.emplace("Method", "Response Time", fmt::format("{}ms", response.getResponseTime()));
        }

        if (url.getUsername().size() || url.getPassword().size()) {
            info.emplace("Host", {
                { "Username", url.getUsername() },
                { "Password", url.getPassword() }
            });
        }

        if (url.getHash().size()) {
            info.insert("Hash", url.getHash());
        }

        this->setCode({ ContentType::UNKNOWN_CONTENT, info.reduce<std::string>([](std::string acc, const auto& entry) {
            if (!acc.empty()) {
                acc.push_back('\n');
            }

            return acc.append(entry.first)
                .append(": ")
                .append(entry.second);
        }, "") });
    }
}

void CodeBlock::onBody() {
    if (m_info) {
        this->setCode(m_info->getRequest().getBodyContent(Mod::get()->getSettingValue<bool>("raw-data")));
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    }
}

void CodeBlock::onQuery() {
    if (m_info) {
        this->setCode({ ContentType::JSON, m_info->getRequest().getURL().stringifyQuery(Mod::get()->getSettingValue<bool>("raw-data")) });
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    }
}

void CodeBlock::onRequestHeaders() {
    if (m_info) {
        this->setCode(m_info->getRequest().getHeaderList(Mod::get()->getSettingValue<bool>("raw-data")));
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    }
}

void CodeBlock::onResponse() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else if (m_info->responseReceived()) {
        this->setCode(m_info->getResponse().getResponseContent(Mod::get()->getSettingValue<bool>("raw-data")));
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, m_info->getResponse().stringifyStatusCode() });
    }
}

void CodeBlock::onResponseHeaders() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else if (m_info->responseReceived()) {
        this->setCode(m_info->getResponse().getHeaderList(Mod::get()->getSettingValue<bool>("raw-data")));
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, m_info->getResponse().stringifyStatusCode() });
    }
}

void CodeBlock::FLAlert_Clicked(FLAlertLayer* alert, const bool state) {
    if (state) {
        CodeBlock::ACCEPTED_PAUSES = true;

        Warning::show();
        this->showMessage("Requests Paused");
        m_bar->reloadState();
    }
}