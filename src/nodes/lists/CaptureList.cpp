#include "CaptureList.hpp"

size_t CaptureList::active = 1;

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
                std::vector<ProxyHandler*> proxies = ProxyHandler::getFilteredProxies();

                for (size_t i = 0; i < proxies.size(); i++) {
                    if (proxies[i]->getInfo()->getID() == CaptureList::active) {
                        if (i == proxies.size() - 1) {
                            m_cells[0]->activate();
                        } else {
                            m_cells[i + 1]->activate();
                        }

                        break;
                    }
                }
            }

            return ListenerResult::Propagate;
        }, "next_packet"_spr);

        this->addEventListener<InvokeBindFilter>([=, this](const InvokeBindEvent* event) {
            if (event->isDown()) {
                std::vector<ProxyHandler*> proxies = ProxyHandler::getFilteredProxies();

                for (size_t i = 0; i < proxies.size(); i++) {
                    if (proxies[i]->getInfo()->getID() == CaptureList::active) {
                        if (i == 0) {
                            m_cells[m_cells.size() - 1]->activate();
                        } else {
                            m_cells[i - 1]->activate();
                        }

                        break;
                    }
                }
            }

            return ListenerResult::Propagate;
        }, "previous_packet"_spr);
    #endif

    const CCSize listSize = size - 2;
    CCTouchDispatcher* dispatcher = CCTouchDispatcher::get();
    bool activated = false;

    if (!Border::init(LIGHTER_BROWN_4B, size)) {
        return false;
    }

    this->setPadding(1);

    for (ProxyHandler* proxy : ProxyHandler::getFilteredProxies()) {
        HttpInfo* info = proxy->getInfo();
        CaptureCell* capture = CaptureCell::create(info->getRequest().getURL(), {
            listSize.width,
            cellHeight
        }, [this, info, switchInfo](CaptureCell* cell) {
            CaptureList::active = info->getID();

            switchInfo(info);

            if (m_list) {
                CCArrayExt<CaptureCell*> entries(m_list->m_entries);

                for (CaptureCell* entry : entries) {
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

    if ((!active || !activated) && m_cells.size()) {
        m_cells[0]->activate();
    }

    this->setNode(m_list = ListView::create(m_cells.inner(), cellHeight, listSize.width, listSize.height));

    return true;
}