#pragma once

namespace proxy::enums {
    enum ContentType {
        FORM,
        JSON,
        XML,
        ROBTOP,
        BINARY,
        UNKNOWN_CONTENT
    };

    enum Origin {
        GD,
        GD_CDN,
        ROBTOP_GAMES,
        NEWGROUNDS,
        GEODE,
        LOCALHOST,
        OTHER
    };

    enum Protocol {
        HTTP,
        HTTPS,
        UNKNOWN_PROTOCOL
    };

    enum State {
        PAUSED,
        IN_PROGRESS,
        COMPLETED,
        FAULTY,
        CANCELLED
    };

    enum OriginFilter {
        GD_FILTER,
        GD_CDN_FILTER,
        ROBTOP_GAMES_FILTER,
        NEWGROUNDS_FILTER,
        GEODE_FILTER,
        LOCALHOST_FILTER,
        OTHER_FILTER,
        ALL_FILTER
    };
}