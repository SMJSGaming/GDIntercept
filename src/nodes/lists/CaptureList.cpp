#include "CaptureList.hpp"

HttpInfo* CaptureList::active = nullptr;

CaptureList* CaptureList::create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo) {
    CaptureList* instance = new CaptureList();

    if (instance && instance->init(size, cellHeight, switchInfo)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CaptureList::init(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo) {
    if (!CCNode::init()) {
        return false;
    }

    CCTouchDispatcher* dispatcher = CCTouchDispatcher::get();
    CCArrayExt<CaptureCell*> entries;
    bool activated = false;

    for (ProxyHandler* proxy : ProxyHandler::getFilteredProxies()) {
        HttpInfo* info = proxy->getInfo();
        CaptureCell* capture = CaptureCell::create(info->getRequest().getURL(), {
            size.width,
            cellHeight
        }, [this, info, switchInfo](CaptureCell* cell) {
            switchInfo(CaptureList::active = info);

            if (m_list) {
                CCArrayExt<CaptureCell*> entries(m_list->m_entries);

                for (CaptureCell* entry : entries) {
                    if (entry != cell) {
                        entry->deactivate();
                    }
                }
            }
        });

        if (info == CaptureList::active) {
            activated = true;
            capture->activate();
        }

        entries.push_back(capture);
    }

    if ((!active || !activated) && entries.size()) {
        entries[0]->activate();
    }

    this->setContentSize(size);
    this->addChild(m_list = ListView::create(entries.inner(), cellHeight, size.width, this->getContentHeight()));

    return true;
}