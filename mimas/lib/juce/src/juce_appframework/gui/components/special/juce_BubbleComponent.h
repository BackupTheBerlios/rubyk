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

#ifndef __JUCE_BUBBLECOMPONENT_JUCEHEADER__
#define __JUCE_BUBBLECOMPONENT_JUCEHEADER__

#include "../juce_Component.h"
#include "../../graphics/effects/juce_DropShadowEffect.h"


//==============================================================================
/**
    A component for showing a message or other graphics inside a speech-bubble-shaped
    outline, pointing at a location on the screen.

    This is a base class that just draws and positions the bubble shape, but leaves
    the drawing of any content up to a subclass. See BubbleMessageComponent for a subclass
    that draws a text message.

    To use it, create your subclass, then either add it to a parent component or
    put it on the desktop with addToDesktop (0), use setPosition() to
    resize and position it, then make it visible.

    @see BubbleMessageComponent
*/
class JUCE_API  BubbleComponent  : public Component
{
protected:
    //==============================================================================
    /** Creates a BubbleComponent.

        Your subclass will need to implement the getContentSize() and paintContent()
        methods to draw the bubble's contents.
    */
    BubbleComponent();

public:
    /** Destructor. */
    ~BubbleComponent();

    //==============================================================================
    /** A list of permitted placements for the bubble, relative to the co-ordinates
        at which it should be pointing.

        @see setAllowedPlacement
    */
    enum BubblePlacement
    {
        above   = 1,
        below   = 2,
        left    = 4,
        right   = 8
    };

    /** Tells the bubble which positions it's allowed to put itself in, relative to the
        point at which it's pointing.

        By default when setPosition() is called, the bubble will place itself either
        above, below, left, or right of the target area. You can pass in a bitwise-'or' of
        the values in BubblePlacement to restrict this choice.

        E.g. if you only want your bubble to appear above or below the target area,
        use setAllowedPlacement (above | below);

        @see BubblePlacement
    */
    void setAllowedPlacement (const int newPlacement);

    //==============================================================================
    /** Moves and resizes the bubble to point at a given component.

        This will resize the bubble to fit its content, then find a position for it
        so that it's next to, but doesn't overlap the given component.

        It'll put itself either above, below, or to the side of the component depending
        on where there's the most space, honouring any restrictions that were set
        with setAllowedPlacement().
    */
    void setPosition (Component* componentToPointTo);

    /** Moves and resizes the bubble to point at a given point.

        This will resize the bubble to fit its content, then position it
        so that the tip of the bubble points to the given co-ordinate. The co-ordinates
        are relative to either the bubble component's parent component if it has one, or
        they are screen co-ordinates if not.

        It'll put itself either above, below, or to the side of this point, depending
        on where there's the most space, honouring any restrictions that were set
        with setAllowedPlacement().
    */
    void setPosition (const int arrowTipX,
                      const int arrowTipY);

    /** Moves and resizes the bubble to point at a given rectangle.

        This will resize the bubble to fit its content, then find a position for it
        so that it's next to, but doesn't overlap the given rectangle. The rectangle's
        co-ordinates are relative to either the bubble component's parent component
        if it has one, or they are screen co-ordinates if not.

        It'll put itself either above, below, or to the side of the component depending
        on where there's the most space, honouring any restrictions that were set
        with setAllowedPlacement().
    */
    void setPosition (const Rectangle& rectangleToPointTo);


protected:
    //==============================================================================
    /** Subclasses should override this to return the size of the content they
        want to draw inside the bubble.
    */
    virtual void getContentSize (int& width, int& height) = 0;

    /** Subclasses should override this to draw their bubble's contents.

        The graphics object's clip region and the dimensions passed in here are
        set up to paint just the rectangle inside the bubble.
    */
    virtual void paintContent (Graphics& g, int width, int height) = 0;


public:
    //==============================================================================
    /** @internal */
    void paint (Graphics& g);

    juce_UseDebuggingNewOperator

private:
    Rectangle content;
    int side, allowablePlacements;
    float arrowTipX, arrowTipY;
    DropShadowEffect shadow;

    BubbleComponent (const BubbleComponent&);
    const BubbleComponent& operator= (const BubbleComponent&);
};


#endif   // __JUCE_BUBBLECOMPONENT_JUCEHEADER__
