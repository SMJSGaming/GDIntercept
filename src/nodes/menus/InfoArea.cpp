#include "InfoArea.hpp"

InfoArea* InfoArea::create(const CCSize& size) {
    InfoArea* instance = new InfoArea();

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool InfoArea::init(const CCSize& size) {
    #ifdef KEYBINDS_ENABLED
        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                TextAlertPopup* alert = TextAlertPopup::create("Code Copied", 0.5f, 0.6f, 150, "");

                utils::clipboard::write(m_textArea->getText());
                alert->setPosition(this->getContentSize() / 2);
                this->addChild(alert, 100);
            }

            return ListenerResult::Propagate;
        }, "copy_info_block"_spr);
    #endif

    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");
    m_textArea = SimpleTextArea::create("", "chatFont.fnt", 0.5f, size.width - PADDING * 4);

    if (!Border::init(infoBg, LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPadding(PADDING);
    infoBg->addChild(m_textArea);
    infoBg->setColor(LIGHT_BROWN_3B);
    m_textArea->setAnchorPoint(CENTER_LEFT);
    m_textArea->setPosition({ PADDING, infoBg->getContentHeight() / 2 });
    m_textArea->setMaxLines(5);
    m_textArea->setLinePadding(2);
    m_textArea->setWrappingMode(CUTOFF_WRAP);

    return true;
}

void InfoArea::updateInfo(HttpInfo* info) {
    const HttpInfo::URL url = info->getRequest().getURL();

    m_textArea->setText(fmt::format("Status Code: {}\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}",
        info->getResponse().stringifyStatusCode(),
        url.getMethod(),
        url.stringifyProtocol(),
        url.getPortHost(),
        url.getPath()
    ));
}