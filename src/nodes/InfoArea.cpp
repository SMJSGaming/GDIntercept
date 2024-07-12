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

                utils::clipboard::write(as<SimpleTextArea*>(this->getNode()->getChildByID("info_text"_spr))->getText());
                alert->setPosition(this->getContentSize() / 2);
                this->addChild(alert, 100);
            }

            return ListenerResult::Propagate;
        }, "copy_info_block"_spr);
    #endif

    SimpleTextArea* infoText = SimpleTextArea::create("", "chatFont.fnt", 0.5f, size.width - PADDING * 4);
    CCScale9Sprite* infoBg = CCScale9Sprite::create("square02b_001.png");

    if (!Border::init(infoBg, LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPadding(PADDING);
    infoBg->addChild(infoText);
    infoBg->setColor(LIGHT_BROWN_3B);
    infoText->setID("info_text"_spr);
    infoText->setAnchorPoint(CENTER_LEFT);
    infoText->setPosition({ PADDING, infoBg->getContentHeight() / 2 });
    infoText->setMaxLines(5);
    infoText->setLinePadding(2);
    infoText->setWrappingMode(CUTOFF_WRAP);

    return true;
}

void InfoArea::updateInfo(HttpInfo* info) {
    const HttpInfo::URL url = info->getRequest().getURL();
    const std::string status = info->responseReceived() ? info->getResponse().stringifyStatusCode() : "No Response";

    as<SimpleTextArea*>(this->getNode()->getChildByID("info_text"_spr))->setText(fmt::format("Status Code: {}\nMethod: {}\nProtocol: {}\nHost: {}\nPath: {}",
        status,
        url.getMethod(),
        url.stringifyProtocol(),
        url.getPortHost(),
        url.getPath()
    ));
}