#include "CodeBlock.hpp"

using namespace SideBarCell;

const std::vector<SideBarView> CodeBlock::views({
    { 'I', { 0xFF, 0x69, 0x61 }, "Info", [](CodeBlock* block) { block->onInfo(); } },
    { 'B', { 0xFF, 0xEE, 0x8C }, "Body", [](CodeBlock* block) { block->onBody(); } },
    { 'Q', { 0xA2, 0xBF, 0xFE }, "Query", [](CodeBlock* block) { block->onQuery(); } },
    { 'H', { 0xB1, 0x9C, 0xD9 }, "Request Headers", [](CodeBlock* block) { block->onRequestHeaders(); } },
    { 'R', { 0xB2, 0xFB, 0xA5 }, "Response", [](CodeBlock* block) { block->onResponse(); } },
    { 'H', { 0xB1, 0x9C, 0xD9 }, "Response Headers", [](CodeBlock* block) { block->onResponseHeaders(); } }
});

const SideBar::Categories CodeBlock::actions({
    { { "Local Files", "localfiles.png"_spr }, {
        { { "Open Files", "folder.png"_spr, [](CodeBlock* block) { return block->onOpenFiles(); } }, [](CodeBlock* block) {
            return GEODE_ANDROID(!)true;
        } },
        { { "Save", "download.png"_spr, [](CodeBlock* block) { return block->onSave(); } }, [](CodeBlock* block) {
            return block->m_info && GEODE_ANDROID(!)true;
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
        { "pause-requests", { "Resume", "Pause" }, { "resume.png"_spr, "pause.png"_spr }, true, {
            [](CodeBlock* block, const OriginalCallback& original) {
                return block->onPause(original);
            },
            [](CodeBlock* block, const OriginalCallback& original) {
                return block->onResume(original);
            }
        } },
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

void CodeBlock::setup() {
    this->bind("open_files"_spr, [this]() {
        this->onOpenFiles();
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
        Mod* mod = Mod::get();

        if (mod->getSettingValue<bool>("pause-requests")) {
            this->onResume([mod]() {
                mod->setSettingValue("pause-requests", false);

                return true;
            });
        } else {
            this->onPause([mod]() {
                mod->setSettingValue("pause-requests", true);

                return true;
            });
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
        this->scroll(0, -this->getCellHeight());
    });
    this->bind("code_line_down"_spr, [this]() {
        this->scroll(0, this->getCellHeight());
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

bool CodeBlock::onOpenFiles() {
    utils::file::openFolder(Mod::get()->getSettingValue<std::filesystem::path>("save-dir").make_preferred());

    return true;
}

bool CodeBlock::onSave() {
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
    const std::filesystem::path path = Mod::get()->getSettingValue<std::filesystem::path>("save-dir");

    (void) utils::file::createDirectoryAll(path);

    if (utils::file::writeString(path / filename, contents).isOk()) {
        this->showMessage("File Saved");
    } else {
        this->showMessage("Error Saving File", { 0xFF, 0x00, 0x00 });
    }

    return true;
}

bool CodeBlock::onPause(const SideBarCell::OriginalCallback& original) {
    if (Mod::get()->getSavedValue<bool>("accepted-pauses", false)) {
        this->showMessage("Requests Paused");

        return original();
    } else {
        FLAlertLayer::create(this, "Pausing Requests",
            "Requests will be <cy>paused</c> until you resume them. "
            "This means that you will no longer be able to send or receive data from the server.\n\n"
            "Are you sure you want to <cy>pause</c> requests?"
        , "Cancel", "Ok")->show();

        return false;
    }
}

bool CodeBlock::onResume(const SideBarCell::OriginalCallback& original) {
    this->showMessage("Requests Resumed");

    return original();
}

bool CodeBlock::onRaw(const SideBarCell::OriginalCallback& original) {
    this->showMessage("Raw Data Enabled");

    return original();
}

bool CodeBlock::onFormatted(const SideBarCell::OriginalCallback& original) {
    this->showMessage("Formatted Data Enabled");

    return original();
}

void CodeBlock::onInfo() {
    if (!m_info) {
        this->setCode({ ContentType::UNKNOWN_CONTENT, "" });
    } else {
        const HttpInfo::URL url = m_info->getRequest().getURL();

        this->setCode({ ContentType::UNKNOWN_CONTENT, fmt::format("Status Code: {}\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}",
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
        Mod* mod = Mod::get();

        mod->setSettingValue("pause-requests", true);
        mod->setSavedValue("accepted-pauses", true);
        m_bar->reloadState();

        this->showMessage("Requests Paused");
    }
}