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

    for (ProxyHandler* proxy : context::CACHED_PROXIES) {
        HttpInfo* request = proxy->getInfo();
        CaptureCell* capture = CaptureCell::create(request, { size.width, cellHeight }, [this, request, switchInfo](CaptureCell* cell) {
            active = request;
            switchInfo(request);

            if (m_list) {
                CCArrayExt<CaptureCell*> entries(m_list->m_entries);

                for (CaptureCell* entry : entries) {
                    if (entry != cell) {
                        entry->deactivate();
                    }
                }
            }
        });

        if (request == CaptureList::active) {
            capture->activate();
        }

        entries.push_back(capture);
    }

    if (!active) {
        entries[0]->activate();
    }

    this->setContentSize(size);
    this->addChild(m_list = ListView::create(entries.inner(), cellHeight, size.width, this->getContentHeight()));

    return true;
}