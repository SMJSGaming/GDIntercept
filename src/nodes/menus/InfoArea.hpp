#pragma once

#include "../../include.hpp"
#include "../CopyButton.hpp"

class InfoArea : public Border {
public:
    static InfoArea* create(const CCSize& size);

    void updateInfo(const HttpInfo* info);
private:
    SimpleTextArea* m_textArea;

    bool init(const CCSize& size);
    void onCopy();
};