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

#include "juce_StretchableObjectResizer.h"


//==============================================================================
StretchableObjectResizer::StretchableObjectResizer()
{
}

StretchableObjectResizer::~StretchableObjectResizer()
{
}

void StretchableObjectResizer::addItem (const double size,
                                        const double minSize, const double maxSize,
                                        const int order)
{
    jassert (order >= 0 && order < INT_MAX); // the order must be >= 0 and less than INT_MAX

    Item* const item = new Item();
    item->size = size;
    item->minSize = minSize;
    item->maxSize = maxSize;
    item->order = order;
    items.add (item);
}

double StretchableObjectResizer::getItemSize (const int index) const throw()
{
    const Item* const it = items [index];
    return it != 0 ? it->size : 0;
}

void StretchableObjectResizer::resizeToFit (const double targetSize)
{
    int order = 0;

    for (;;)
    {
        double currentSize = 0;
        double minSize = 0;
        double maxSize = 0;

        int nextHighestOrder = INT_MAX;

        for (int i = 0; i < items.size(); ++i)
        {
            const Item* const it = items.getUnchecked(i);
            currentSize += it->size;

            if (it->order <= order)
            {
                minSize += it->minSize;
                maxSize += it->maxSize;
            }
            else
            {
                minSize += it->size;
                maxSize += it->size;
                nextHighestOrder = jmin (nextHighestOrder, it->order);
            }
        }

        const double thisIterationTarget = jlimit (minSize, maxSize, targetSize);

        if (thisIterationTarget >= currentSize)
        {
            const double availableExtraSpace = maxSize - currentSize;
            const double targetAmountOfExtraSpace = thisIterationTarget - currentSize;
            const double scale = targetAmountOfExtraSpace / availableExtraSpace;

            for (int i = 0; i < items.size(); ++i)
            {
                Item* const it = items.getUnchecked(i);

                if (it->order <= order)
                    it->size = jmin (it->maxSize, it->size + (it->maxSize - it->size) * scale);
            }
        }
        else
        {
            const double amountOfSlack = currentSize - minSize;
            const double targetAmountOfSlack = thisIterationTarget - minSize;
            const double scale = targetAmountOfSlack / amountOfSlack;

            for (int i = 0; i < items.size(); ++i)
            {
                Item* const it = items.getUnchecked(i);

                if (it->order <= order)
                    it->size = jmax (it->minSize, it->minSize + (it->size - it->minSize) * scale);
            }
        }

        if (nextHighestOrder < INT_MAX)
            order = nextHighestOrder;
        else
            break;
    }
}


END_JUCE_NAMESPACE
