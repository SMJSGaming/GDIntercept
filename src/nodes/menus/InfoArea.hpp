#pragma once

#include "../../include.hpp"

class InfoArea : public Border {
public:
    static InfoArea* create(const CCSize& size);

    void updateInfo(HttpInfo* info);
private:
    SimpleTextArea* m_textArea;

    bool init(const CCSize& size);
};