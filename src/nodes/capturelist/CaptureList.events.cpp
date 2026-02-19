#include "CaptureList.hpp"

void CaptureList::setup() {
    this->bind("next_packet", [this](){
        this->tabCell(true);
    }, true);
    this->bind("previous_packet", [this](){
        this->tabCell(false);
    }, true);
}

void CaptureList::tabCell(const bool forward) {
    const size_t cellAmount = this->m_cells.size();

    for (size_t i = 0; i < cellAmount; i++) {
        CONTINUE_WHEN(this->m_cells[i]->getInfo()->getID() != CaptureList::ACTIVE);

        if (forward && i == cellAmount - 1) {
            m_cells[0]->activate();
        } else if (!forward && i == 0) {
            m_cells[m_cells.size() - 1]->activate();
        } else {
            m_cells[i + (forward ? 1 : -1)]->activate();
        }

        break;
    }
}

void CaptureList::onHover() {
    ListView* list = reinterpret_cast<ListView*>(this->getNode());

    if (list->m_tableView->getContentHeight() <= list->m_tableView->m_contentLayer->getContentHeight()) {
        list->m_tableView->setMouseEnabled(true);
    }
}

void CaptureList::unHover() {
    reinterpret_cast<ListView*>(this->getNode())->m_tableView->setMouseEnabled(false);
}