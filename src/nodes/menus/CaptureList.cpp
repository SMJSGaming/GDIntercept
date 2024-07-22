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
    #ifdef KEYBINDS_ENABLED
        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->tabCell(true);
            }

            return ListenerResult::Propagate;
        }, "next_packet"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                this->tabCell(false);
            }

            return ListenerResult::Propagate;
        }, "previous_packet"_spr);
    #endif

    const CCSize listSize = size - 1;
    CCTouchDispatcher* dispatcher = CCTouchDispatcher::get();
    bool activated = false;

    if (!Border::init(LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPaddingTop(1);
    this->setPaddingLeft(1);

    for (ProxyHandler* proxy : ProxyHandler::getFilteredProxies()) {
        HttpInfo* info = proxy->getInfo();
        CaptureCell* capture = CaptureCell::create(info->getRequest().getURL(), {
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

    if ((!active || !activated) && m_cells.size()) {
        m_cells[0]->activate();
    }

    // The code block takes priority over the capture list
    list->m_tableView->setMouseEnabled(false);

    this->setNode(list);

    return true;
}

void CaptureList::tabCell(const bool forward) {
    std::deque<ProxyHandler*> proxies = ProxyHandler::getFilteredProxies();

    for (size_t i = 0; i < proxies.size(); i++) {
        if (proxies[i]->getInfo()->getID() == CaptureList::active) {
            if (!forward && i == 0) {
                m_cells[m_cells.size() - 1]->activate();
            } else if (forward && i == proxies.size() - 1) {
                m_cells[0]->activate();
            } else {
                m_cells[i + (forward ? 1 : -1)]->activate();
            }

            break;
        }
    }
}