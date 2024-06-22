#include "TouchFixList.hpp"
#include "Geode/ui/ListView.hpp"

TouchFixList* TouchFixList::create(CCArray* cells, const float cellHeight, const float width, const float height) {
    TouchFixList* instance = new TouchFixList();

    if (instance) {
        instance->m_itemSeparation = cellHeight;
        instance->m_primaryCellColor = BROWN_3B;
        instance->m_secondaryCellColor = BROWN_3B;
        instance->m_cellOpacity = 0xFF;
        instance->m_cellBorderColor = ccc4(0x00, 0x00, 0x00, 0x4B);

        if (instance->init(cells, BoomListType::Default, width, height)) {
            instance->autorelease();

            return instance;
        }
    }

    CC_SAFE_DELETE(instance);

    return nullptr;
}

void TouchFixList::draw() {
    CCTouchDispatcher* dispatcher = CCTouchDispatcher::get();
    CCTouchHandler* popupHandler = dispatcher->findHandler(as<Popup<>*>(CCDirector::sharedDirector()->getRunningScene()->getChildByID("intercept_popup"_spr)));
    CCTouchHandler* listHandler = dispatcher->findHandler(m_tableView);

    ListView::draw();

    if (popupHandler && listHandler) {
        dispatcher->setPriority(popupHandler->getPriority(), listHandler->getDelegate());
    }
}