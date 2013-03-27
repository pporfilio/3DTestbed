#ifndef MOUSEINPUT_H
#define MOUSEINPUT_H

/*
 * MouseInput is an experimental design that I wanted to play around with. I
 * was initially motivated by wanting to make it as easy as possible to move
 * away from the Qt framework so I wanted to convert Qt Events to my own format.
 * It provides some convenient ways to query events that have happened and
 * I tried using it in a couple projects for classes. I kind of like it
 * but it's probably better to just handle events directly when they arise,
 * rather than packing them into this class.
 *
 *
 */

#include <list>
#include "CS123Algebra.h"

class MouseState;

namespace MouseButton {

enum Which {
    LEFT,
    MIDDLE,
    RIGHT
};

enum EventType {
    PRESS,
    RELEASE
};


//The MouseButton::Event class can only be instantiated by MouseState
//so that any event offered back to the queue was an actual event, although
//events can be reordered by users. Exisiting ones can be copied. Not allowing
//this seems like it could cause issues. Just don't put back an event that
//wasn't an actual event, okay? :)
class Event {
    //since we're in a different namespace than MouseState, have to explicitly
    //say which namespace MouseState is in.
    friend class ::MouseState;

public:
    vec2<int> location() const;
    MouseButton::Which button() const;
    MouseButton::EventType event() const;
    void print() const;

private:
    vec2<int> _location;
    MouseButton::Which _button;
    MouseButton::EventType _type;

    Event();
    Event(MouseButton::Which b, MouseButton::EventType t, int x, int y);
};

}

class MouseState
{

private:
    vec2<int> prev_loc;
    vec2<int> curr_loc;
    bool left_down;
    bool right_down;
    bool middle_down;
    bool dragging;
    std::list<MouseButton::Event> eventQueue;

public:

    MouseState();
    void setNewLocation(int newX, int newY);
    void mousePressed(MouseButton::Which b, int posX, int posY);
    void mouseReleased(MouseButton::Which b, int posX, int posY);
    bool hasEvent() const;
    bool hasEvent(MouseButton::Which button) const;
    MouseButton::Event pollEvent();
    MouseButton::Event pollEvent(MouseButton::Which button, bool discard_others = true);
    void offerEvent(MouseButton::Event e);
    bool isLeft() const;
    bool isRight() const;
    bool isMid() const;
    bool isDragging() const;
    int getCurrX() const;
    int getCurrY() const;
    vec2<int> getCurrPos() const;
    int getPrevX() const;
    int getPrevY() const;
    vec2<int> getPrevPos() const;
    void print() const;
    vec2<int> getDelta(bool clear = true);
};


#endif // MOUSEINPUT_H
