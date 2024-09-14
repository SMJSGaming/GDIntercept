#include "CaptureList.hpp"

void CaptureList::setup() {
    this->bind("next_packet"_spr, [this](){
        this->tabCell(true);
    });
    this->bind("previous_packet"_spr, [this](){
        this->tabCell(false);
    });
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