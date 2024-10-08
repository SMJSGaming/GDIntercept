#pragma once

#include "../../../include.hpp"
#include "CullingCell.hpp"
#include "../utils/MonospaceLabel.hpp"
#include "../../../objects/JSONTokenizer.hpp"

class CodeLineCell : public CullingCell {
public:
    static CodeLineCell* create(const CCSize& size, const size_t lineNumber, const float lineNumberWidth, const HttpInfo::HttpContent& code, JSONTokenizer& tokenizer);

    float getCodeLineWidth() const;
protected:
    void initRender() override;
private:
    HttpInfo::HttpContent m_code;
    size_t m_lineNumber;
    float m_lineNumberWidth;
    std::vector<JSONTokenizer::TokenOffset> m_tokens;

    CodeLineCell(const CCSize& size, const size_t lineNumber, const float lineNumberWidth, const HttpInfo::HttpContent& code, JSONTokenizer& tokenizer);
};