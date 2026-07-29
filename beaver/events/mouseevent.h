#pragma once

#include "beaver/events/event.h"

namespace bvr::event {

class MouseMovedEvent : public Event {
public:
    MouseMovedEvent(double x, double y) : x_(x), y_(y) {}
    ~MouseMovedEvent() = default;

    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_;
    double y_;
};

}  // namespace bvr::event
