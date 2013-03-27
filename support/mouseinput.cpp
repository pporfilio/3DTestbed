#include "mouseinput.h"
#include <stdexcept>

vec2<int> MouseButton::Event::location() const {
    return _location;
}

MouseButton::Which MouseButton::Event::button() const {
    return _button;
}

MouseButton::EventType MouseButton::Event::event() const {
    return _type;
}

// no sensible values for default constructor so let stuff get
//default initialized, which for built-ins means garbage values
MouseButton::Event::Event() {}
MouseButton::Event::Event(MouseButton::Which b, MouseButton::EventType t, int x, int y) {
    _location = vec2<int>(x, y);
    _button = b;
    _type = t;
}
void MouseButton::Event::print() const {
    printf("Event button: ");
    if (_button == MouseButton::LEFT) {
        printf("left ");
    }
    if (_button == MouseButton::RIGHT) {
        printf("right ");
    }
    if (_button == MouseButton::MIDDLE) {
        printf("middle ");
    }
    printf(" type: ");
    if (_type == MouseButton::PRESS) {
        printf("press ");
    }
    if (_type == MouseButton::RELEASE) {
        printf("release ");
    }
    printf("  location = (%d, %d)\n", _location.x, _location.y);
}

MouseState::MouseState(): prev_loc(-1, -1), curr_loc(-1, -1), left_down(false),
    right_down(false), middle_down(false), dragging(false) {}

void MouseState::setNewLocation(int newX, int newY) {
    //avoid moves of distance 0 or duplicated calls
    if (newX == curr_loc.x && newY == curr_loc.y) {
        return;
    }
    prev_loc = curr_loc;
    curr_loc = vec2<int>(newX, newY);
    if (left_down || right_down || middle_down) {
        dragging = true;
    }
}

void MouseState::mousePressed(MouseButton::Which b, int posX, int posY) {
    setNewLocation(posX, posY);
    left_down = left_down || b == MouseButton::LEFT;
    right_down = right_down || b == MouseButton::RIGHT;
    middle_down = middle_down || b == MouseButton::MIDDLE;
    eventQueue.push_front(MouseButton::Event(b,
                                             MouseButton::PRESS,
                                             posX, posY));
}

void MouseState::mouseReleased(MouseButton::Which b, int posX, int posY) {
    setNewLocation(posX, posY);
    left_down = (b == MouseButton::LEFT) ? false : left_down;
    right_down = (b == MouseButton::RIGHT) ? false : right_down;
    middle_down = (b == MouseButton::MIDDLE) ? false : middle_down;
    eventQueue.push_front(MouseButton::Event(b,
                                             MouseButton::RELEASE,
                                             posX, posY));
}

bool MouseState::hasEvent() const {
    return !eventQueue.empty();
}

bool MouseState::hasEvent(MouseButton::Which button) const {
    typedef std::list<MouseButton::Event>::const_iterator iter;
    for (iter i = eventQueue.begin(); i != eventQueue.end(); ++i) {
        if (i->button() == button) {
            return true;
        }
    }
    return false;
}

MouseButton::Event MouseState::pollEvent() {
    if (eventQueue.empty()) {
        throw out_of_range("event queue is empty. Call hasEvent() before pollEvent()");
    }
    //store a copy of the Event object stored in the list
    MouseButton::Event result = eventQueue.front();
    //remove the first element (the one we just copied) from the front
    eventQueue.pop_front();
    return result;
}

MouseButton::Event MouseState::pollEvent(MouseButton::Which button,
                                             bool discard_others) {
    MouseButton::Event result;
    if (discard_others) {
        while (true) {
            result = pollEvent();
            if (result.button() == button) {
                return result;
            }
        }
    } else {
        typedef std::list<MouseButton::Event>::iterator iter;
        iter i = eventQueue.begin();
        while (i != eventQueue.end()) {
            if (i->button() == button) {
                break;
            }
            ++i;
        }
        if (i == eventQueue.end()) {
            throw out_of_range("event queue is empty. Call hasEvent() before pollEvent()");
        }
        //store copy of value at iterator (location where button was correct)
        result = *i;
        //remove that element from the list
        eventQueue.erase(i);
        //return stored copy
        return result;
    }
}

void MouseState::offerEvent(MouseButton::Event e) {
    eventQueue.push_front(e);
}

bool MouseState::isLeft() const {
    return left_down;
}

bool MouseState::isRight() const {
    return right_down;
}

bool MouseState::isMid() const {
    return middle_down;
}

bool MouseState::isDragging() const {
    return dragging;
}

int MouseState::getCurrX() const {
    return curr_loc.x;
}

int MouseState::getCurrY() const {
    return curr_loc.y;
}

vec2<int> MouseState::getCurrPos() const {
    return curr_loc;
}

int MouseState::getPrevX() const {
    return prev_loc.x;
}

int MouseState::getPrevY() const {
    return prev_loc.y;
}

vec2<int> MouseState::getPrevPos() const {
    return prev_loc;
}

void MouseState::print() const {
    printf("MouseState: down: ");
    if (left_down) {
        printf("left ");
    }
    if (right_down) {
        printf("right ");
    }
    if (middle_down) {
        printf("middle ");
    }
    if (dragging) {
        printf("dragging");
    }
    printf("\n");

    typedef std::list<MouseButton::Event>::const_iterator iter;
    for (iter i = eventQueue.begin(); i != eventQueue.end(); ++i) {
        i->print();
    }
}

vec2<int> MouseState::getDelta(bool clear) {
    vec2<int> result = vec2<int>(curr_loc.x - prev_loc.x,
                                 curr_loc.y - prev_loc.y);
    //logic hear is that since mouse location updates only when the mouse moves,
    //if the mouse isn't moving then you could read the same delta frame after frame.

    //TODO: should the delta accumulate until it is read?
    if (clear) {
        prev_loc = curr_loc;
    }

    return result;
}

