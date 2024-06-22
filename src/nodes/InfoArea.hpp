#pragma once

#include "BorderFix.hpp"
#include "../include.hpp"
#include "../objects/HttpInfo.hpp"

struct InfoArea : public BorderFix {
    static InfoArea* create(const CCSize& size);
    void updateRequest(HttpInfo* request);
private:
    bool init(const CCSize& size);
};