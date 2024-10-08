#include "CodeBlock.hpp"

using namespace SideBarCell;

const std::vector<SideBarView> CodeBlock::views({
    { 'I', [](const auto& icons) { return icons.info; }, "Info", [](CodeBlock* block) { block->onInfo(); } },
    { 'B', [](const auto& icons) { return icons.body; }, "Body", [](CodeBlock* block) { block->onBody(); } },
    { 'Q', [](const auto& icons) { return icons.query; }, "Query", [](CodeBlock* block) { block->onQuery(); } },
    { 'H', [](const auto& icons) { return icons.header; }, "Request Headers", [](CodeBlock* block) { block->onRequestHeaders(); } },
    { 'R', [](const auto& icons) { return icons.response; }, "Response", [](CodeBlock* block) { block->onResponse(); } },
    { 'H', [](const auto& icons) { return icons.header; }, "Response Headers", [](CodeBlock* block) { block->onResponseHeaders(); } }
});

const SideBar::Categories CodeBlock::actions({
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
});

bool CodeBlock::acceptedPauses = false;

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
        const CullingList* list = as<CullingList*>(this->getNode());
        const std::vector<CullingCell*> cells = list->getCells();

        this->scroll(cells.empty() ? 0 : list->getContentWidth() - as<CodeLineCell*>(cells.front())->getCodeLineWidth(), 0);
    });
    this->bind("code_page_right"_spr, [this]() {
        const CullingList* list = as<CullingList*>(this->getNode());
        const std::vector<CullingCell*> cells = list->getCells();

        this->scroll(cells.empty() ? 0 : -(list->getContentWidth() - as<CodeLineCell*>(cells.front())->getCodeLineWidth()), 0);
    });
}

bool CodeBlock::onCopy() {
    utils::clipboard::write(m_code);
    this->showMessage("Code Copied");

    return true;
}

bool CodeBlock::onSend() {
    const HttpInfo::Request original = m_info->getRequest();
    const HttpInfo::URL url = original.getURL();
    const std::string method = url.getMethod();
    const std::string body = original.getBody();
    web::WebRequest request;

    if (body.size()) {
        request.bodyString(body);
    }

    request.header(ProxyHandler::getCopyHandshake(), "true");

    for (const auto& [name, value] : original.getHeaders().items()) {
        request.header(name, value.get<std::string>());
    }

    (void) request.send(method, url.getOriginal());

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
    const HttpInfo::Request request = m_info->getRequest();
    const HttpInfo::URL url = request.getURL();
    const HttpInfo::Response response = m_info->getResponse();
    const std::string host = url.getPortHost();
    const std::string bodyContent = request.getBodyContent().contents;
    const std::string responseContent = response.getResponseContent().contents;
    const std::string filename = fmt::format("{:%d-%m-%Y %H.%M.%S} - {}.txt", fmt::localtime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())), host);
    const std::string contents = fmt::format("Method: {}\nProtocol: {}\nHost: {}\nPath: {}\nQuery: {}\nRequest Headers: {}\nBody:{}\nResponse Headers: {}\nResponse:{}",
        url.getMethod(),
        url.stringifyProtocol(),
        host,
        url.getPath(),
        url.stringifyQuery(),
        request.stringifyHeaders(),
        bodyContent.empty() ? "" : fmt::format("\n{}", bodyContent),
        response.stringifyHeaders(),
        responseContent.empty() ? "" : fmt::format("\n{}", responseContent)
    );

    utils::file::pick(file::PickMode::OpenFolder, {
        .defaultPath = mod->getSavedValue("default-path", mod->getSaveDir())
    }).listen([this, mod, filename, contents](const Result<std::filesystem::path>* path) {
        Loader::get()->queueInMainThread([this, mod, path, filename, contents]{
            if (path->isErr()) {
                this->showMessage("Error Picking File", { 0xFF, 0x00, 0x00 });
            } else if (utils::file::writeString(path->unwrap() / filename, contents).isOk()) {
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
    if (CodeBlock::acceptedPauses) {
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
        const HttpInfo::URL url = m_info->getRequest().getURL();

        this->setCode({ ContentType::UNKNOWN_CONTENT, fmt::format("Client: {}\nStatus Code: {}\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}",
            m_info->getClient() == Client::COCOS ? "Cocos2D-X" : "Geode",
            m_info->getResponse().stringifyStatusCode(),
            url.getMethod(),
            url.stringifyProtocol(),
            url.getPortHost(),
            url.getPath()
        ) });
    }
}

void CodeBlock::onBody() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else {
        this->setCode(m_info->getRequest().getBodyContent(Mod::get()->getSettingValue<bool>("raw-data")));
    }
}

void CodeBlock::onQuery() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else {
        this->setCode({ ContentType::JSON, m_info->getRequest().getURL().stringifyQuery(Mod::get()->getSettingValue<bool>("raw-data")) });
    }
}

void CodeBlock::onRequestHeaders() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else {
        this->setCode({ ContentType::JSON, m_info->getRequest().stringifyHeaders(Mod::get()->getSettingValue<bool>("raw-data")) });
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
        this->setCode({ ContentType::JSON, m_info->getResponse().stringifyHeaders(Mod::get()->getSettingValue<bool>("raw-data")) });
    } else {
        this->setCode({ ContentType::UNKNOWN_CONTENT, m_info->getResponse().stringifyStatusCode() });
    }
}

void CodeBlock::FLAlert_Clicked(FLAlertLayer* alert, const bool state) {
    if (state) {
        CodeBlock::acceptedPauses = true;

        Warning::show();
        this->showMessage("Requests Paused");
        m_bar->reloadState();
    }
}