#include "CaptureList.hpp"

CaptureList* CaptureList::create(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo) {
    CaptureList* instance = new CaptureList(size, cellHeight, switchInfo);

    if (instance && instance->init()) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

CaptureList::CaptureList(const CCSize& size, const float cellHeight, const std::function<void(HttpInfo*)>& switchInfo) : m_itemSeparation(cellHeight), m_switchInfo(switchInfo) {
    this->setContentSize(size);
}

void CaptureList::createCells() {
    const float width = this->getContentWidth();
    CCArrayExt<CaptureCell*> entries;
    bool active = false;

    for (HttpInfo* request : HttpInfo::requests) {
        CaptureCell* capture = CaptureCell::create(request, { width, this->m_itemSeparation }, [this, request](CaptureCell* cell) {
            m_switchInfo(request);
            this->switchCell(cell);
        });

        if (request->isActive() && !active) {
            capture->activate();
            active = true;
        }

        entries.push_back(capture);
    }

    if (!active) {
        entries[0]->activate();
    }

    ListView* list = ListView::create(entries.inner(), m_itemSeparation, width, this->getContentHeight());

    OPT(this->getChildByID("capture_list"_spr))->removeFromParentAndCleanup(true);

    list->setID("capture_list"_spr);
    this->addChild(list);
}

void CaptureList::switchCell(CaptureCell* cell) {
    ListView* list = as<ListView*>(this->getChildByID("capture_list"_spr));

    if (list != nullptr) {
        CCArrayExt<CaptureCell*> entries(list->m_entries);

        for (CaptureCell* entry : entries) {
            if (entry != cell) {
                entry->deactivate();
            }
        }
    }
}

bool CaptureList::init() {
    this->createCells();

    return CCNode::init();
}