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
    const Theme::Theme theme = Theme::getTheme();
    ESCAPE_WHEN(!KeybindNode::init(), false);
    ESCAPE_WHEN(!Border::init(theme.code.background, size), false);

    this->setPaddingY(1);
    this->setPaddingRight(theme.code.scrollbar.size + 1);

    const float fullPaddingY = this->getPaddingY() * 2;
    const float paddingBottom = this->getPaddingBottom();
    CullingList* list = CullingList::create(size - ccp(this->getPaddingX() * 2, fullPaddingY));
    TracklessScrollbar* scrollbarY = TracklessScrollbar::create(true, {
        theme.code.scrollbar.size,
        this->getContentHeight() - fullPaddingY
    }, list->getView());
    m_scrollbars = { nullptr, scrollbarY };
    m_bar = SideBar::create(this, size.height - fullPaddingY, CodeBlock::views, CodeBlock::actions);

    scrollbarY->setAnchorPoint(BOTTOM_LEFT);
    scrollbarY->setPosition({ this->getContentWidth() - this->getPaddingRight(), paddingBottom });
    m_bar->setPosition({ 1, paddingBottom });

    this->setNode(list);
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
    const Theme::Theme theme = Theme::getTheme();
    TracklessScrollbar* scrollbarX = std::get<0>(m_scrollbars);

    m_bar->reloadState();

    if (Mod::get()->getSettingValue<bool>("minimize-side-menu")) {
        m_bar->scaleToMin();
        this->setPaddingLeft(m_bar->getMinWidth() + 1);
    } else {
        m_bar->scaleToMax();
        this->setPaddingLeft(m_bar->getMaxWidth() + 1);
    }

    if (scrollbarX) {
        const float paddingLeft = this->getPaddingLeft();

        scrollbarX->setPositionX(paddingLeft);
        scrollbarX->setContentWidth(this->getContentWidth() - paddingLeft - this->getPaddingRight());
    }
}

void CodeBlock::updateInfo(HttpInfo* info) {
    if (info != m_info) {
        m_info = info;

        this->reloadCode();
    }
}

void CodeBlock::setCode(const HttpInfo::HttpContent& code) {
    const Theme::Theme theme = Theme::getTheme();
    const CCSize fontSize = theme.code.font.getTrueFontSize();
    const float lineNumberWidth = fontSize.width * std::to_string(
        std::count(code.contents.begin(), code.contents.end(), '\n') + 1
    ).size() + theme.code.paddingLeft;
    TracklessScrollbar* scrollbarX = std::get<0>(m_scrollbars);
    TracklessScrollbar* scrollbarY = std::get<1>(m_scrollbars);
    CullingList* list = as<CullingList*>(this->getNode());
    std::stringstream stream(m_code = code.contents);
    std::vector<CullingCell*> cells;
    std::string line;
    JSONTokenizer tokenizer;

    for (size_t i = 1; std::getline(stream, line) || i == 1; i++) {
        cells.push_back(CodeLineCell::create({
            code.type == ContentType::BINARY ?
                list->getContentWidth() :
                lineNumberWidth + theme.code.paddingCenter + fontSize.width * std::min<size_t>(2000, line.size()) + theme.code.paddingRight,
            fontSize.height + theme.code.font.lineHeight
        }, i, lineNumberWidth, { code.type, line }, tokenizer));
    }

    list->setCells(cells);

    if (list->isHorizontalLocked()) {
        if (scrollbarX) {
            m_scrollbars = { nullptr, scrollbarY };

            this->setPaddingBottom(1);
            m_corner->removeFromParentAndCleanup(true);
            scrollbarX->removeFromParentAndCleanup(true);
            scrollbarY->setPositionY(this->getPaddingBottom());
            scrollbarY->setContentHeight(this->getContentHeight() - theme.code.scrollbar.size + this->getPaddingY() * 2);
        }
    } else if (!scrollbarX) {
        const float contentWidth = this->getContentWidth();
        const float paddingLeft = this->getPaddingLeft();
        scrollbarX = TracklessScrollbar::create(false, {
            contentWidth - paddingLeft - this->getPaddingRight(),
            theme.code.scrollbar.size
        }, list->getView());
        m_scrollbars = { scrollbarX, scrollbarY };
        m_corner = CCLayerColor::create(theme.code.scrollbar.border);

        this->setPaddingBottom(theme.code.scrollbar.size + 1);
        m_corner->setContentSize({ theme.code.scrollbar.size, theme.code.scrollbar.size });
        m_corner->setAnchorPoint(BOTTOM_RIGHT);
        m_corner->ignoreAnchorPointForPosition(false);
        m_corner->setPosition({ contentWidth - 1, 1 });
        scrollbarX->setPosition({ paddingLeft, 1 });
        scrollbarX->setAnchorPoint(BOTTOM_LEFT);
        scrollbarY->setPositionY(this->getPaddingBottom());
        scrollbarY->setContentHeight(this->getContentHeight() - this->getPaddingY() * 2);
        this->addChild(scrollbarX);
        this->addChild(m_corner);
        list->moveToTop();
    }

    this->setBackgroundColor(theme.code.background);
}

HttpInfo* CodeBlock::getActiveInfo() const {
    return m_info;
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