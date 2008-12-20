/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "../../../../juce_core/basics/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_MouseEvent.h"
#include "../juce_Component.h"


//==============================================================================
MouseEvent::MouseEvent (const int x_,
                        const int y_,
                        const ModifierKeys& mods_,
                        Component* const originator,
                        const Time& eventTime_,
                        const int mouseDownX_,
                        const int mouseDownY_,
                        const Time& mouseDownTime_,
                        const int numberOfClicks_,
                        const bool mouseWasDragged) throw()
    : x (x_),
      y (y_),
      mods (mods_),
      eventComponent (originator),
      originalComponent (originator),
      eventTime (eventTime_),
      mouseDownX (mouseDownX_),
      mouseDownY (mouseDownY_),
      mouseDownTime (mouseDownTime_),
      numberOfClicks (numberOfClicks_),
      wasMovedSinceMouseDown (mouseWasDragged)
{
}

MouseEvent::~MouseEvent() throw()
{
}

//==============================================================================
bool MouseEvent::mouseWasClicked() const throw()
{
    return ! wasMovedSinceMouseDown;
}

int MouseEvent::getMouseDownX() const throw()
{
    return mouseDownX;
}

int MouseEvent::getMouseDownY() const throw()
{
    return mouseDownY;
}

int MouseEvent::getDistanceFromDragStartX() const throw()
{
    return x - mouseDownX;
}

int MouseEvent::getDistanceFromDragStartY() const throw()
{
    return y - mouseDownY;
}

int MouseEvent::getDistanceFromDragStart() const throw()
{
    return roundDoubleToInt (juce_hypot (getDistanceFromDragStartX(),
                                         getDistanceFromDragStartY()));
}

int MouseEvent::getLengthOfMousePress() const throw()
{
    if (mouseDownTime.toMilliseconds() > 0)
        return jmax (0, (int) (eventTime - mouseDownTime).inMilliseconds());

    return 0;
}

//==============================================================================
int MouseEvent::getScreenX() const throw()
{
    int sx = x, sy = y;
    eventComponent->relativePositionToGlobal (sx, sy);
    return sx;
}

int MouseEvent::getScreenY() const throw()
{
    int sx = x, sy = y;
    eventComponent->relativePositionToGlobal (sx, sy);
    return sy;
}

int MouseEvent::getMouseDownScreenX() const throw()
{
    int sx = mouseDownX, sy = mouseDownY;
    eventComponent->relativePositionToGlobal (sx, sy);
    return sx;
}

int MouseEvent::getMouseDownScreenY() const throw()
{
    int sx = mouseDownX, sy = mouseDownY;
    eventComponent->relativePositionToGlobal (sx, sy);
    return sy;
}

//==============================================================================
const MouseEvent MouseEvent::getEventRelativeTo (Component* const otherComponent) const throw()
{
    if (otherComponent == 0)
    {
        jassertfalse
        return *this;
    }

    MouseEvent me (*this);

    eventComponent->relativePositionToOtherComponent (otherComponent, me.x, me.y);
    eventComponent->relativePositionToOtherComponent (otherComponent, me.mouseDownX, me.mouseDownY);
    me.eventComponent = otherComponent;

    return me;
}

//==============================================================================
static int doubleClickTimeOutMs = 400;

void MouseEvent::setDoubleClickTimeout (const int newTime) throw()
{
    doubleClickTimeOutMs = newTime;
}

int MouseEvent::getDoubleClickTimeout() throw()
{
    return doubleClickTimeOutMs;
}

END_JUCE_NAMESPACE
