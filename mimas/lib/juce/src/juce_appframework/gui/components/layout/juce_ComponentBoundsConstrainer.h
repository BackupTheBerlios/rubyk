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

#ifndef __JUCE_COMPONENTBOUNDSCONSTRAINER_JUCEHEADER__
#define __JUCE_COMPONENTBOUNDSCONSTRAINER_JUCEHEADER__

#include "../juce_Component.h"


//==============================================================================
/**
    A class that imposes restrictions on a Component's size or position.

    This is used by classes such as ResizableCornerComponent,
    ResizableBorderComponent and ResizableWindow.

    The base class can impose some basic size and position limits, but you can
    also subclass this for custom uses.

    @see ResizableCornerComponent, ResizableBorderComponent, ResizableWindow
*/
class JUCE_API  ComponentBoundsConstrainer
{
public:
    //==============================================================================
    /** When first created, the object will not impose any restrictions on the components. */
    ComponentBoundsConstrainer() throw();

    /** Destructor. */
    virtual ~ComponentBoundsConstrainer();

    //==============================================================================
    /** Imposes a minimum width limit. */
    void setMinimumWidth (const int minimumWidth) throw();

    /** Returns the current minimum width. */
    int getMinimumWidth() const throw()                         { return minW; }

    /** Imposes a maximum width limit. */
    void setMaximumWidth (const int maximumWidth) throw();

    /** Returns the current maximum width. */
    int getMaximumWidth() const throw()                         { return maxW; }

    /** Imposes a minimum height limit. */
    void setMinimumHeight (const int minimumHeight) throw();

    /** Returns the current minimum height. */
    int getMinimumHeight() const throw()                        { return minH; }

    /** Imposes a maximum height limit. */
    void setMaximumHeight (const int maximumHeight) throw();

    /** Returns the current maximum height. */
    int getMaximumHeight() const throw()                        { return maxH; }

    /** Imposes a minimum width and height limit. */
    void setMinimumSize (const int minimumWidth,
                         const int minimumHeight) throw();

    /** Imposes a maximum width and height limit. */
    void setMaximumSize (const int maximumWidth,
                         const int maximumHeight) throw();

    /** Set all the maximum and minimum dimensions. */
    void setSizeLimits (const int minimumWidth,
                        const int minimumHeight,
                        const int maximumWidth,
                        const int maximumHeight) throw();

    //==============================================================================
    /** Sets the amount by which the component is allowed to go off-screen.

        The values indicate how many pixels must remain on-screen when dragged off
        one of its parent's edges, so e.g. if minimumWhenOffTheTop is set to 10, then
        when the component goes off the top of the screen, its y-position will be
        clipped so that there are always at least 10 pixels on-screen. In other words,
        the lowest y-position it can take would be (10 - the component's height).

        If you pass 0 or less for one of these amounts, the component is allowed
        to move beyond that edge completely, with no restrictions at all.

        If you pass a very large number (i.e. larger that the dimensions of the
        component itself), then the component won't be allowed to overlap that
        edge at all. So e.g. setting minimumWhenOffTheLeft to 0xffffff will mean that
        the component will bump into the left side of the screen and go no further.
    */
    void setMinimumOnscreenAmounts (const int minimumWhenOffTheTop,
                                    const int minimumWhenOffTheLeft,
                                    const int minimumWhenOffTheBottom,
                                    const int minimumWhenOffTheRight) throw();

    //==============================================================================
    /** Specifies a width-to-height ratio that the resizer should always maintain.

        If the value is 0, no aspect ratio is enforced. If it's non-zero, the width
        will always be maintained as this multiple of the height.

        @see setResizeLimits
    */
    void setFixedAspectRatio (const double widthOverHeight) throw();

    /** Returns the aspect ratio that was set with setFixedAspectRatio().

        If no aspect ratio is being enforced, this will return 0.
    */
    double getFixedAspectRatio() const throw();


    //==============================================================================
    /** This callback changes the given co-ordinates to impose whatever the current
        constraints are set to be.

        @param x                the x position that should be examined and adjusted
        @param y                the y position that should be examined and adjusted
        @param w                the width that should be examined and adjusted
        @param h                the height that should be examined and adjusted
        @param previousBounds   the component's current size
        @param limits           the region in which the component can be positioned
        @param isStretchingTop      whether the top edge of the component is being resized
        @param isStretchingLeft     whether the left edge of the component is being resized
        @param isStretchingBottom   whether the bottom edge of the component is being resized
        @param isStretchingRight    whether the right edge of the component is being resized
    */
    virtual void checkBounds (int& x, int& y, int& w, int& h,
                              const Rectangle& previousBounds,
                              const Rectangle& limits,
                              const bool isStretchingTop,
                              const bool isStretchingLeft,
                              const bool isStretchingBottom,
                              const bool isStretchingRight);

    /** This callback happens when the resizer is about to start dragging. */
    virtual void resizeStart();

    /** This callback happens when the resizer has finished dragging. */
    virtual void resizeEnd();

    /** Checks the given bounds, and then sets the component to the corrected size. */
    void setBoundsForComponent (Component* const component,
                                int x, int y, int w, int h,
                                const bool isStretchingTop,
                                const bool isStretchingLeft,
                                const bool isStretchingBottom,
                                const bool isStretchingRight);

    /** Called by setBoundsForComponent() to apply a new constrained size to a
        component.

        By default this just calls setBounds(), but it virtual in case it's needed for
        extremely cunning purposes.
    */
    virtual void applyBoundsToComponent (Component* component,
                                         int x, int y, int w, int h);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    int minW, maxW, minH, maxH;
    int minOffTop, minOffLeft, minOffBottom, minOffRight;
    double aspectRatio;

    ComponentBoundsConstrainer (const ComponentBoundsConstrainer&);
    const ComponentBoundsConstrainer& operator= (const ComponentBoundsConstrainer&);
};


#endif   // __JUCE_COMPONENTBOUNDSCONSTRAINER_JUCEHEADER__
