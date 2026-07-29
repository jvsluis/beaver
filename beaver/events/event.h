#pragma once

namespace bvr::event {

class Event {
public:
    virtual ~Event() = default;

    bool handled = false;
};

}  // namespace bvr::event
