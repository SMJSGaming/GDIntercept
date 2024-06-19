#include "CaptureCell.hpp"

CaptureCell* CaptureCell::create(HttpInfo* request, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell) {
    CaptureCell* instance = new CaptureCell(request, size, switchCell);

    if (instance && instance->init(size)) {
        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CaptureCell::CaptureCell(HttpInfo* request, const CCSize& size, const std::function<void(CaptureCell*)>& switchCell) : GenericListCell("", size),
    m_request(request),
    m_switchCell(switchCell) {
    this->setID("capture"_spr);
}

bool CaptureCell::init(const CCSize& size) {
    const std::string method(m_request->formatMethod());
    std::string path(m_request->getPath());
    std::string cutoffPath(path.substr(path.find_last_of('/')));

    if (cutoffPath.size() == 1) {
        cutoffPath = m_request->getHost();
    }

    CCLabelBMFont* label = CCLabelBMFont::create((method + ' ' + cutoffPath).c_str(), "bigFont.fnt", 0, kCCTextAlignmentLeft);
    ButtonSprite* button = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png");
    CCMenu* menu = CCMenu::createWithItem(CCMenuItemSpriteExtra::create(
        button,
        this,
        menu_selector(CaptureCell::onView)
    ));

    menu->setAnchorPoint(BOTTOM_LEFT);
    menu->setPosition({ size.width - 20, size.height / 2 });
    menu->setScale(0.3f);
    button->setID("view"_spr);
    label->setAnchorPoint(CENTER_LEFT);
    label->setPosition({ 5, size.height / 2 });
    label->setScale(0.2f);
    m_mainLayer->addChild(label);
    m_mainLayer->addChild(menu);

    for (size_t i = 0; i < cutoffPath.size(); i++) {
        if (cocos::getChild<CCSprite>(label, i)->getPositionX() * label->getScale() > menu->getPositionX() - button->getContentWidth() * menu->getScale() / 2 - 10) {
            label->setString((std::string(label->getString()).substr(0, i - 3) + "...").c_str());

            break;
        }
    }

    for (size_t i = 0; i < method.size(); i++) {
        cocos::getChild<CCSprite>(label, i)->setColor(m_request->colorForMethod());
    }

    return true;
}

void CaptureCell::activate() {
    m_request->setActive(true);

    m_switchCell(this);
    as<ButtonSprite*>(m_mainLayer->getChildByIDRecursive("view"_spr))->updateBGImage("GJ_button_05.png");
}

void CaptureCell::deactivate() {
    m_request->setActive(false);

    as<ButtonSprite*>(m_mainLayer->getChildByIDRecursive("view"_spr))->updateBGImage("GJ_button_01.png");
}

void CaptureCell::onView(CCObject* obj) {
    this->activate();
}