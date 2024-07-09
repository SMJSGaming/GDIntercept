#include "JSONCodeBlock.hpp"

bool JSONCodeBlock::init(const HttpInfo::HttpContent& code, const CCSize& size) {
    if (!Border::init({ 0, 0, 0, FULL_OPACITY }, size)) {
        return false;
    }

    this->setPaddingY(PADDING / 2);
    this->setPaddingRight(PADDING * 1.2f);
    this->setCode(code);

    return true;
}

void JSONCodeBlock::copyCode() {
    TextAlertPopup* alert = TextAlertPopup::create("Code Copied", 0.5f, 0.6f, 150, "");

    utils::clipboard::write(this->m_code);
    alert->setPosition(this->getContentSize() / 2);
    this->addChild(alert, 100);

}

void JSONCodeBlock::setCode(const HttpInfo::HttpContent& code) {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const CCSize fontSize = CCLabelBMFont::create("0", theme.fontName)->getContentSize() * theme.fontSize;
    const CCSize size = this->getContentSize() - ccp(this->getPaddingX(), this->getPaddingY()) * 2;
    const float cellHeight = fontSize.height + theme.lineHeight;
    const float lineNumberWidth = fontSize.width * 4;
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

void JSONCodeBlock::draw() {
    const ThemeStyle& theme = ThemeStyle::getTheme();
    const ListView* list = as<ListView*>(this->getNode());
    const CCSize& contentSize = this->getContentSize();
    const float x = contentSize.width - PADDING;
    const float listDelta = list->m_tableView->getContentHeight() - list->m_tableView->m_contentLayer->getContentHeight();

    if (listDelta < 0) {
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