#include "CaptureList.hpp"

size_t CaptureList::ACTIVE = 0;

CaptureList* CaptureList::create(const CCSize& size, const float cellHeight, const std::function<void(std::shared_ptr<HttpInfo>)>& switchInfo) {
    CaptureList* instance = new CaptureList();

    if (instance && instance->init(size, cellHeight, switchInfo)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

        return nullptr;
    }
}

bool CaptureList::init(const CCSize& size, const float cellHeight, const std::function<void(std::shared_ptr<HttpInfo>)>& switchInfo) {
    const CCSize listSize = size - 1;
    Stream<ProxyHandler*> proxies = ProxyHandler::getFilteredProxies();
    bool activated = false;

    ESCAPE_WHEN(!HoverNode::init(), false);
    ESCAPE_WHEN(!KeybindNode::init(), false);
    ESCAPE_WHEN(!Border::init(LIGHTER_BROWN_4B, size), false);

    for (size_t i = 0; i < proxies.size(); i++) {
        std::shared_ptr<HttpInfo> info = proxies.at(i)->getInfo();
        CaptureCell* capture = CaptureCell::create(i, info, {
            listSize.width,
            cellHeight
        }, [this, info, switchInfo](CaptureCell* cell) {
            CaptureList::ACTIVE = info->getID();

            switchInfo(info);

            if (this->getNode()) {
                for (CaptureCell* entry : m_cells) {
                    if (entry != cell) {
                        entry->deactivate();
                    }
                }
            }
        });

        if (CaptureList::ACTIVE == info->getID()) {
            activated = true;
            capture->activate();
        }

        m_cells.push_back(capture);
    }

    ListView* list = ListView::create(m_cells.inner(), cellHeight, listSize.width, listSize.height);

    if (m_cells.size()) {
        if (!CaptureList::ACTIVE || !activated) {
            m_cells[0]->activate();
        }
    } else {
        switchInfo(nullptr);
    }

    list->m_tableView->setMouseEnabled(false);
    this->setPaddingTop(1);
    this->setPaddingLeft(1);
    this->setNode(list);

    return true;
}