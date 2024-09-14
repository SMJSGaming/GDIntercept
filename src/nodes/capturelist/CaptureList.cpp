#include "CaptureList.hpp"

size_t CaptureList::active = 0;

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
    const CCSize listSize = size - 1;
    std::deque<ProxyHandler*> proxies = ProxyHandler::getFilteredProxies();
    bool activated = false;

    ESCAPE_WHEN(!KeybindNode::init(), false);
    ESCAPE_WHEN(!Border::init(LIGHTER_BROWN_4B, size), false);

    this->setPaddingTop(1);
    this->setPaddingLeft(1);

    for (size_t i = 0; i < proxies.size(); i++) {
        HttpInfo* info = proxies.at(i)->getInfo();
        CaptureCell* capture = CaptureCell::create(i, info, {
            listSize.width,
            cellHeight
        }, [this, info, switchInfo](CaptureCell* cell) {
            CaptureList::active = info->getID();

            switchInfo(info);

            if (this->getNode()) {
                for (CaptureCell* entry : m_cells) {
                    if (entry != cell) {
                        entry->deactivate();
                    }
                }
            }
        });

        if (CaptureList::active == info->getID()) {
            activated = true;
            capture->activate();
        }

        m_cells.push_back(capture);
    }

    ListView* list = ListView::create(m_cells.inner(), cellHeight, listSize.width, listSize.height);

    if (m_cells.size()) {
        if (!active || !activated) {
            m_cells[0]->activate();
        }
    } else {
        switchInfo(nullptr);
    }

    // The code block takes priority over the capture list
    list->m_tableView->setMouseEnabled(false);

    this->setNode(list);

    return true;
}