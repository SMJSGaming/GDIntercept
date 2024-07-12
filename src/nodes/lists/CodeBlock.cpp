#include "CodeBlock.hpp"

std::vector<std::pair<char, SEL_MenuHandler>> CodeBlock::dataTypes({
    { 'B', menu_selector(CodeBlock::onBody) },
    { 'Q', menu_selector(CodeBlock::onQuery) },
    { 'H', menu_selector(CodeBlock::onHeaders) },
    { 'R', menu_selector(CodeBlock::onResponse) }
});

size_t CodeBlock::buttonCount = CodeBlock::dataTypes.size();

char CodeBlock::currentDataType = CodeBlock::dataTypes.at(0).first;

CodeBlock* CodeBlock::create(const CCSize& size, const CCSize& buttonBarSize) {
    CodeBlock* instance = new CodeBlock();

    if (instance && instance->init(size, buttonBarSize)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CodeBlock::init(const CCSize& size, const CCSize& buttonBarSize) {
    #ifdef KEYBINDS_ENABLED
        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                TextAlertPopup* alert = TextAlertPopup::create("Code Copied", 0.5f, 0.6f, 150, "");

                utils::clipboard::write(this->m_code);
                alert->setPosition(this->getContentSize() / 2);
                this->addChild(alert, 100);
            }

            return ListenerResult::Propagate;
        }, "copy_code_block"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->scroll(-this->getCellHeight());
            }

            return ListenerResult::Propagate;
        }, "code_line_up"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->scroll(-as<ListView*>(this->getNode())->m_height);
            }

            return ListenerResult::Propagate;
        }, "code_page_up"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->scroll(this->getCellHeight());
            }

            return ListenerResult::Propagate;
        }, "code_line_down"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->scroll(as<ListView*>(this->getNode())->m_height);
            }

            return ListenerResult::Propagate;
        }, "code_page_down"_spr);
    #endif

    const size_t buttonCount = CodeBlock::dataTypes.size();
    std::vector<CCMenuItemSpriteExtra*> buttons;

    if (!Border::init({ 0, 0, 0, FULL_OPACITY }, size)) {
        return false;
    }

    this->setPaddingY(PADDING / 2);
    this->setPaddingRight(PADDING * 1.2f);
    this->setCode({ HttpInfo::UNKNOWN_CONTENT, "" });

    for (size_t i = 0; i < buttonCount; i++) {
        const auto& [key, selector] = CodeBlock::dataTypes.at(i);
        std::string keyStr(1, key);
        CCLabelBMFont* label = CCLabelBMFont::create(keyStr.c_str(), "consola.fnt"_spr);

        buttons.push_back(CCMenuItemSpriteExtra::create(label, this, selector));
        label->setScale(0.8f);
        label->setAnchorPoint(CENTER);
        label->setColor(ThemeStyle::getTheme().lineNum);
        label->setPosition(ccp(buttonBarSize.width / buttonCount, buttonBarSize.height) / 2);
        m_buttons.insert({ key, label });

        cocos::getChild<CCSprite>(label, 0)->setPositionY(buttonBarSize.height / 2);
    }

    ButtonBar* buttonBar = ButtonBar::create("square02_001.png", 0.2f, buttonBarSize, buttons);

    buttonBar->setPosition({ size.width - PADDING * 1.5f, size.height - PADDING });
    buttonBar->setAnchorPoint(TOP_RIGHT);
    this->addChild(buttonBar);

    return true;
}

void CodeBlock::setCode(const HttpInfo::HttpContent& code) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const CCSize size = this->getContentSize() - ccp(this->getPaddingX(), this->getPaddingY()) * 2;
    const float cellHeight = this->getCellHeight();
    const float lineNumberWidth = this->getTrueFontSize().width * 4;
    CCTouchDispatcher* dispatcher = CCTouchDispatcher::get();
    CCArray* cells = CCArray::create();
    std::stringstream stream(m_code = code.contents);
    std::string line;
    JSONColor color;

    OPT(this->getChildByID("scrollbar"_spr))->removeFromParentAndCleanup(true);

    for (size_t i = 1; std::getline(stream, line) || i == 1; i++) {
        if ((code.type == HttpInfo::BINARY || code.type == HttpInfo::UNKNOWN_CONTENT) && i == 999) {
            cells->addObject(CodeLineCell::create({ HttpInfo::UNKNOWN_CONTENT, "..." }, i, lineNumberWidth, color));

            break;
        } else {
            cells->addObject(CodeLineCell::create({ code.type, line }, i, lineNumberWidth, color));
        }
    }

    ListView* list = ListView::create(cells, cellHeight, size.width, size.height);
    TracklessScrollbar* scrollbar = TracklessScrollbar::create({ PADDING, this->getContentHeight() - 2 }, list);

    list->setCellOpacity(0);
    list->setCellBorderColor({ 0, 0, 0, 0 });
    scrollbar->setID("scrollbar"_spr);
    scrollbar->setAnchorPoint(BOTTOM_LEFT);
    scrollbar->setPosition({ this->getContentWidth() - PADDING, 1 });

    for (size_t i = 0; i < cells->count(); i++) {
        as<CodeLineCell*>(cells->objectAtIndex(i))->setContentSize({ size.width, cellHeight });
    }

    this->setNode(list);
    this->addChild(scrollbar);
    this->setBackgroundColor({ theme.background.r, theme.background.g, theme.background.b, FULL_OPACITY });
}

CCSize CodeBlock::getTrueFontSize() {
    const ThemeStyle& theme = ThemeStyle::getTheme();

    return CCLabelBMFont::create("0", theme.fontName)->getContentSize() * theme.fontSize;
}

float CodeBlock::getCellHeight() {
    const ThemeStyle& theme = ThemeStyle::getTheme();

    return this->getTrueFontSize().height + theme.lineHeight;
}

void CodeBlock::updateInfo(HttpInfo* info) {
    m_info = info;

    if (info) {
        switch (currentDataType) {
            case 'B': this->onBody(nullptr); break;
            case 'Q': this->onQuery(nullptr); break;
            case 'H': this->onHeaders(nullptr); break;
            case 'R': this->onResponse(nullptr); break;
        }
    } else {
        this->updateDataTypeColor('-');
    }
}

void CodeBlock::updateDataTypeColor(const char type) {
    const ThemeStyle& theme = ThemeStyle::getTheme();

    if (type != '-') {
        currentDataType = type;
    }

    for (const auto& [key, _] : CodeBlock::dataTypes) {
        m_buttons.at(key)->setColor(key == type ? theme.text : theme.lineNum);
    }
}

void CodeBlock::scroll(const float delta) {
    ListView* list = as<ListView*>(this->getNode());
    const float max = 0;
    const float min = -list->m_tableView->m_contentLayer->getContentHeight();

    list->m_tableView->m_contentLayer->setPositionY(std::min(max, std::max(min, list->m_tableView->m_contentLayer->getPositionY() + delta)));
}

void CodeBlock::onBody(CCObject* sender) {
    if (m_info) {
        this->setCode(m_info->getRequest().getBodyContent(Mod::get()->getSettingValue<bool>("raw-data")));
        this->updateDataTypeColor('B');
    }
}

void CodeBlock::onQuery(CCObject* sender) {
    if (m_info) {
        this->setCode({ HttpInfo::JSON, m_info->getRequest().getURL().stringifyQuery() });
        this->updateDataTypeColor('Q');
    }
}

void CodeBlock::onHeaders(CCObject* sender) {
    if (m_info) {
        this->setCode({ HttpInfo::JSON, m_info->getRequest().stringifyHeaders() });
        this->updateDataTypeColor('H');
    }
}

void CodeBlock::onResponse(CCObject* sender) {
    if (m_info) {
        if (m_info->responseReceived()) {
            this->setCode(m_info->getResponse().getResponseContent(Mod::get()->getSettingValue<bool>("raw-data")));
        } else {
            this->setCode({ HttpInfo::UNKNOWN_CONTENT, "No response available yet." });
        }

        this->updateDataTypeColor('R');
    }
}

void CodeBlock::draw() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const ListView* list = as<ListView*>(this->getNode());
    const CCSize& contentSize = this->getContentSize();
    const float x = contentSize.width - PADDING;
    const float listDelta = list->m_tableView->getContentHeight() - list->m_tableView->m_contentLayer->getContentHeight();

    if (list->m_tableView->m_contentLayer->getChildrenCount() == 1) {
        list->m_tableView->m_contentLayer->setPositionY(list->m_height);
    } else if (listDelta < 0) {
        list->m_tableView->m_contentLayer->setPositionY(std::min(0.0f, std::max(
            list->m_tableView->m_contentLayer->getPositionY(),
            list->m_tableView->getContentHeight() - list->m_tableView->m_contentLayer->getContentHeight()
        )));
    }

    Border::draw();

    ccDrawColor4B(theme.scrollBorder.r, theme.scrollBorder.g, theme.scrollBorder.b, FULL_OPACITY);
    glLineWidth(2);
    ccDrawLine({ x, 1 }, { x, this->getContentHeight() - 1 });
}