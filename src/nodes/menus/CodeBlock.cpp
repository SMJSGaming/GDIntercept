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
                this->onCopy();
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
    const float buttonOffset = size.width - PADDING * 1.5f;
    std::vector<CCMenuItemSpriteExtra*> buttons;

    if (!Border::init({ 0, 0, 0, FULL_OPACITY }, size)) {
        return false;
    }

    this->setPaddingY(PADDING / 2);
    this->setPaddingRight(PADDING * 1.2f);
    this->setNode(CullingList::create(this->getContentSize() - ccp(this->getPaddingX(), this->getPaddingY()) * 2));
    this->setCode({ ContentType::UNKNOWN_CONTENT, "" });

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
    TracklessScrollbar* scrollbar = TracklessScrollbar::create({
        PADDING,
        this->getContentHeight() - 2
    }, as<CullingList*>(this->getNode())->getView());

    scrollbar->setAnchorPoint(BOTTOM_LEFT);
    scrollbar->setPosition({ this->getContentWidth() - PADDING, 1 });
    buttonBar->setPosition({ buttonOffset, size.height - PADDING });
    buttonBar->setAnchorPoint(TOP_RIGHT);
    this->addChild(buttonBar);
    this->addChild(scrollbar);

    if (!Mod::get()->getSettingValue<bool>("hide-copy-buttons")) {
        CopyButton* copyButton = CopyButton::create({ 15, 15 }, [this]() { this->onCopy(); });
        CCMenu* copyMenu = CCMenu::createWithItem(copyButton);
        const CCSize copyButtonSize = copyButton->getContentSize();

        copyButton->setPosition(copyButtonSize / 2);
        copyMenu->setAnchorPoint(BOTTOM_RIGHT);
        copyMenu->setContentSize(copyButtonSize);
        copyMenu->ignoreAnchorPointForPosition(false);
        copyMenu->setPosition({ buttonOffset, PADDING });
        this->addChild(copyMenu);
    }

    return true;
}

void CodeBlock::setCode(const HttpInfo::HttpContent& code) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    CullingList* list = as<CullingList*>(this->getNode());
    const CCSize cellSize = { list->getContentWidth(), this->getCellHeight() };
    const float lineNumberWidth = this->getTrueFontSize().width * std::to_string(
        std::count(code.contents.begin(), code.contents.end(), '\n') + 1
    ).size() + PADDING;
    std::stringstream stream(m_code = code.contents);
    std::vector<CullingCell*> cells;
    std::string line;
    JSONTokenizer tokenizer;

    for (size_t i = 1; std::getline(stream, line) || i == 1; i++) {
        CodeLineCell* cell = CodeLineCell::create(cellSize, i, lineNumberWidth, { code.type, line }, tokenizer);

        cells.push_back(cell);
    }

    list->setCells(cells);
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

void CodeBlock::updateInfo(const HttpInfo* info) {
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

void CodeBlock::onCopy() {
    TextAlertPopup* alert = TextAlertPopup::create("Code Copied", 0.5f, 0.6f, 150, "");

    utils::clipboard::write(this->m_code);
    alert->setPosition(this->getContentSize() / 2);
    this->addChild(alert, 100);
}

void CodeBlock::onBody(CCObject* sender) {
    if (m_info) {
        this->setCode(m_info->getRequest().getBodyContent(Mod::get()->getSettingValue<bool>("raw-data")));
        this->updateDataTypeColor('B');
    }
}

void CodeBlock::onQuery(CCObject* sender) {
    if (m_info) {
        this->setCode({ ContentType::JSON, m_info->getRequest().getURL().stringifyQuery() });
        this->updateDataTypeColor('Q');
    }
}

void CodeBlock::onHeaders(CCObject* sender) {
    if (m_info) {
        this->setCode({ ContentType::JSON, m_info->getRequest().stringifyHeaders() });
        this->updateDataTypeColor('H');
    }
}

void CodeBlock::onResponse(CCObject* sender) {
    if (m_info) {
        if (m_info->responseReceived()) {
            this->setCode(m_info->getResponse().getResponseContent(Mod::get()->getSettingValue<bool>("raw-data")));
        } else {
            this->setCode({ ContentType::UNKNOWN_CONTENT, m_info->getResponse().stringifyStatusCode() });
        }

        this->updateDataTypeColor('R');
    }
}

void CodeBlock::draw() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const float x = this->getContentWidth() - PADDING;

    Border::draw();

    ccDrawColor4B(theme.scrollBorder.r, theme.scrollBorder.g, theme.scrollBorder.b, FULL_OPACITY);
    glLineWidth(2);
    ccDrawLine({ x, 1 }, { x, this->getContentHeight() - 1 });
}