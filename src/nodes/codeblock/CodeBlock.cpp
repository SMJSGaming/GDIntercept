#include "CodeBlock.hpp"

CodeBlock* CodeBlock::create(const CCSize& size) {
    CodeBlock* instance = new CodeBlock();

    if (instance && instance->init(size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CodeBlock::CodeBlock() : m_info(nullptr) { }

bool CodeBlock::init(const CCSize& size) {
    ESCAPE_WHEN(!KeybindNode::init(), false);
    ESCAPE_WHEN(!Border::init(ThemeStyle::getTheme().ui.background, size), false);

    this->setPaddingY(PADDING / 2);
    this->setPaddingRight(PADDING + 1);
    this->setNode(CullingList::create(this->getContentSize() - ccp(this->getPaddingX(), this->getPaddingY()) * 2));

    TracklessScrollbar* scrollbarY = TracklessScrollbar::create(true, {
        PADDING,
        this->getContentHeight() - 2
    }, as<CullingList*>(this->getNode())->getView());
    m_scrollbars = { nullptr, scrollbarY };
    m_bar = SideBar::create(this, size.height - 2, CodeBlock::views, CodeBlock::actions);

    scrollbarY->setAnchorPoint(BOTTOM_RIGHT);
    scrollbarY->setPosition({ this->getContentWidth(), 1 });
    m_bar->setPosition({ 0, 1 });

    this->reloadCode();
    this->reloadSideBar();
    this->addChild(scrollbarY);
    this->addChild(m_bar);

    return true;
}

void CodeBlock::reloadCode() {
    m_bar->reloadView();
}

void CodeBlock::reloadSideBar() {
    TracklessScrollbar* scrollbarX = std::get<0>(m_scrollbars);

    m_bar->reloadState();

    if (Mod::get()->getSettingValue<bool>("minimize-side-menu")) {
        m_bar->scaleToMin();
        this->setPaddingLeft(m_bar->getMinWidth());
    } else {
        m_bar->scaleToMax();
        this->setPaddingLeft(m_bar->getMaxWidth());
    }

    if (scrollbarX) {
        scrollbarX->setPositionX(m_bar->getContentWidth());
        scrollbarX->setContentWidth(this->getNode()->getContentWidth() + 1);
    }
}

void CodeBlock::updateInfo(HttpInfo* info) {
    if (info != m_info) {
        m_info = info;

        this->reloadCode();
    }
}

void CodeBlock::setCode(const HttpInfo::HttpContent& code) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const float cellHeight = this->getCellHeight();
    const float fontWidth = this->getTrueFontSize().width;
    const float lineNumberWidth = fontWidth * std::to_string(
        std::count(code.contents.begin(), code.contents.end(), '\n') + 1
    ).size() + PADDING;
    TracklessScrollbar* scrollbarX = std::get<0>(m_scrollbars);
    TracklessScrollbar* scrollbarY = std::get<1>(m_scrollbars);
    CullingList* list = as<CullingList*>(this->getNode());
    std::stringstream stream(m_code = code.contents);
    std::vector<CullingCell*> cells;
    std::string line;
    JSONTokenizer tokenizer;

    for (size_t i = 1; std::getline(stream, line) || i == 1; i++) {
        cells.push_back(CodeLineCell::create({
            code.type == ContentType::BINARY ? list->getContentWidth() : lineNumberWidth + PADDING * 2 + fontWidth * std::min<size_t>(2000, line.size()),
            cellHeight
        }, i, lineNumberWidth, { code.type, line }, tokenizer));
    }

    list->setCells(cells);

    if (list->isHorizontalLocked()) {
        if (scrollbarX) {
            m_scrollbars = { nullptr, scrollbarY };

            m_corner->removeFromParentAndCleanup(true);
            scrollbarX->removeFromParentAndCleanup(true);
            scrollbarY->setPositionY(1);
            scrollbarY->setContentHeight(this->getContentHeight() - 2);
            this->setPaddingBottom(PADDING / 2);
        }
    } else if (!scrollbarX) {
        scrollbarX = TracklessScrollbar::create(false, {
            list->getContentWidth() + 1,
            PADDING
        }, list->getView());
        m_scrollbars = { scrollbarX, scrollbarY };
        m_corner = CCLayerColor::create(theme.ui.scrollbar.border);

        m_corner->setContentSize({ PADDING, PADDING });
        m_corner->setAnchorPoint(BOTTOM_RIGHT);
        m_corner->ignoreAnchorPointForPosition(false);
        m_corner->setPosition({ this->getContentWidth(), 0 });
        scrollbarX->setPositionX(m_bar->getContentWidth());
        scrollbarX->setAnchorPoint(BOTTOM_LEFT);
        scrollbarY->setPositionY(PADDING);
        scrollbarY->setContentHeight(this->getContentHeight() - PADDING - 1);
        this->addChild(scrollbarX);
        this->addChild(m_corner);
        this->setPaddingBottom(PADDING + 1);
    }

    this->setBackgroundColor(theme.ui.background);
}

HttpInfo* CodeBlock::getActiveInfo() const {
    return m_info;
}

CCSize CodeBlock::getTrueFontSize() {
    const ThemeStyle& theme = ThemeStyle::getTheme();

    return CCLabelBMFont::create("0", theme.font.fontName)->getContentSize() * theme.font.fontScale;
}

float CodeBlock::getCellHeight() {
    return this->getTrueFontSize().height + ThemeStyle::getTheme().font.lineHeight;
}

void CodeBlock::showMessage(const std::string& message, const ccColor3B& color) {
    CCNode* node = this->getNode();
    TextAlertPopup* alert = TextAlertPopup::create(message, 0.5f, 0.6f, 150, "");

    alert->setPosition(node->getContentSize() / 2);
    alert->m_label->setColor(color);
    node->addChild(alert, 100);
}

void CodeBlock::scroll(const float x, const float y) {
    const CullingList* list = as<CullingList*>(this->getNode());
    const TableView* tableView = list->getView();
    const float max = 0;
    const CCSize min = -(tableView->m_contentLayer->getContentSize() - tableView->getContentSize());
    const CCPoint position = tableView->m_contentLayer->getPosition();

    tableView->m_contentLayer->setPosition({
        std::min(max, std::max(min.width, position.x + x)),
        std::min(max, std::max(min.height, position.y + y))
    });
}