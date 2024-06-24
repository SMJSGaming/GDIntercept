#pragma once

#include "../include.hpp"
#include "../objects/HttpInfo.hpp"

struct InfoArea : public Border {
    static InfoArea* create(const CCSize& size);
    void updateRequest(HttpInfo* request);
private:
    bool init(const CCSize& size);
};