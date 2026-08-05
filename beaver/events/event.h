#pragma once

namespace bvr::events {

class Event {
public:
    virtual ~Event() = default;

    bool handled = false;
};

}  // namespace bvr::events
