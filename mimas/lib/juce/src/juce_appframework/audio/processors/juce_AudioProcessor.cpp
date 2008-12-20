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

#include "../../../juce_core/basics/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_AudioProcessor.h"
#include "../../../juce_core/threads/juce_ScopedLock.h"
#include "../../../juce_core/text/juce_XmlDocument.h"


//==============================================================================
AudioProcessor::AudioProcessor()
    : playHead (0),
      activeEditor (0),
      sampleRate (0),
      blockSize (0),
      numInputChannels (0),
      numOutputChannels (0),
      latencySamples (0),
      suspended (false),
      nonRealtime (false)
{
}

AudioProcessor::~AudioProcessor()
{
    // ooh, nasty - the editor should have been deleted before the filter
    // that it refers to is deleted..
    jassert (activeEditor == 0);

#ifdef JUCE_DEBUG
    // This will fail if you've called beginParameterChangeGesture() for one
    // or more parameters without having made a corresponding call to endParameterChangeGesture...
    jassert (changingParams.countNumberOfSetBits() == 0);
#endif
}

void AudioProcessor::setPlayHead (AudioPlayHead* const newPlayHead) throw()
{
    playHead = newPlayHead;
}

void AudioProcessor::addListener (AudioProcessorListener* const newListener) throw()
{
    const ScopedLock sl (listenerLock);
    listeners.addIfNotAlreadyThere (newListener);
}

void AudioProcessor::removeListener (AudioProcessorListener* const listenerToRemove) throw()
{
    const ScopedLock sl (listenerLock);
    listeners.removeValue (listenerToRemove);
}

void AudioProcessor::setPlayConfigDetails (const int numIns,
                                           const int numOuts,
                                           const double sampleRate_,
                                           const int blockSize_) throw()
{
    numInputChannels = numIns;
    numOutputChannels = numOuts;
    sampleRate = sampleRate_;
    blockSize = blockSize_;
}

void AudioProcessor::setNonRealtime (const bool nonRealtime_) throw()
{
    nonRealtime = nonRealtime_;
}

void AudioProcessor::setLatencySamples (const int newLatency)
{
    if (latencySamples != newLatency)
    {
        latencySamples = newLatency;
        updateHostDisplay();
    }
}

void AudioProcessor::setParameterNotifyingHost (const int parameterIndex,
                                                const float newValue)
{
    setParameter (parameterIndex, newValue);
    sendParamChangeMessageToListeners (parameterIndex, newValue);
}

void AudioProcessor::sendParamChangeMessageToListeners (const int parameterIndex, const float newValue)
{
    jassert (((unsigned int) parameterIndex) < (unsigned int) getNumParameters());

    for (int i = listeners.size(); --i >= 0;)
    {
        listenerLock.enter();
        AudioProcessorListener* const l = (AudioProcessorListener*) listeners [i];
        listenerLock.exit();

        if (l != 0)
            l->audioProcessorParameterChanged (this, parameterIndex, newValue);
    }
}

void AudioProcessor::beginParameterChangeGesture (int parameterIndex)
{
    jassert (((unsigned int) parameterIndex) < (unsigned int) getNumParameters());

#ifdef JUCE_DEBUG
    // This means you've called beginParameterChangeGesture twice in succession without a matching
    // call to endParameterChangeGesture. That might be fine in most hosts, but better to avoid doing it.
    jassert (! changingParams [parameterIndex]);
    changingParams.setBit (parameterIndex);
#endif

    for (int i = listeners.size(); --i >= 0;)
    {
        listenerLock.enter();
        AudioProcessorListener* const l = (AudioProcessorListener*) listeners [i];
        listenerLock.exit();

        if (l != 0)
            l->audioProcessorParameterChangeGestureBegin (this, parameterIndex);
    }
}

void AudioProcessor::endParameterChangeGesture (int parameterIndex)
{
    jassert (((unsigned int) parameterIndex) < (unsigned int) getNumParameters());

#ifdef JUCE_DEBUG
    // This means you've called endParameterChangeGesture without having previously called
    // endParameterChangeGesture. That might be fine in most hosts, but better to keep the
    // calls matched correctly.
    jassert (changingParams [parameterIndex]);
    changingParams.clearBit (parameterIndex);
#endif

    for (int i = listeners.size(); --i >= 0;)
    {
        listenerLock.enter();
        AudioProcessorListener* const l = (AudioProcessorListener*) listeners [i];
        listenerLock.exit();

        if (l != 0)
            l->audioProcessorParameterChangeGestureEnd (this, parameterIndex);
    }
}

void AudioProcessor::updateHostDisplay()
{
    for (int i = listeners.size(); --i >= 0;)
    {
        listenerLock.enter();
        AudioProcessorListener* const l = (AudioProcessorListener*) listeners [i];
        listenerLock.exit();

        if (l != 0)
            l->audioProcessorChanged (this);
    }
}

bool AudioProcessor::isParameterAutomatable (int /*index*/) const
{
    return true;
}

void AudioProcessor::suspendProcessing (const bool shouldBeSuspended)
{
    const ScopedLock sl (callbackLock);
    suspended = shouldBeSuspended;
}

//==============================================================================
void AudioProcessor::editorBeingDeleted (AudioProcessorEditor* const editor) throw()
{
    const ScopedLock sl (callbackLock);

    jassert (activeEditor == editor);

    if (activeEditor == editor)
        activeEditor = 0;
}

AudioProcessorEditor* AudioProcessor::createEditorIfNeeded()
{
    if (activeEditor != 0)
        return activeEditor;

    AudioProcessorEditor* const ed = createEditor();

    if (ed != 0)
    {
        // you must give your editor comp a size before returning it..
        jassert (ed->getWidth() > 0 && ed->getHeight() > 0);

        const ScopedLock sl (callbackLock);
        activeEditor = ed;
    }

    return ed;
}

//==============================================================================
void AudioProcessor::getCurrentProgramStateInformation (JUCE_NAMESPACE::MemoryBlock& destData)
{
    getStateInformation (destData);
}

void AudioProcessor::setCurrentProgramStateInformation (const void* data, int sizeInBytes)
{
    setStateInformation (data, sizeInBytes);
}

//==============================================================================
// magic number to identify memory blocks that we've stored as XML
const uint32 magicXmlNumber = 0x21324356;

void AudioProcessor::copyXmlToBinary (const XmlElement& xml,
                                      JUCE_NAMESPACE::MemoryBlock& destData)
{
    const String xmlString (xml.createDocument (String::empty, true, false));
    const int stringLength = xmlString.length();

    destData.setSize (stringLength + 10);

    char* const d = (char*) destData.getData();
    *(uint32*) d = swapIfBigEndian ((const uint32) magicXmlNumber);
    *(uint32*) (d + 4) = swapIfBigEndian ((const uint32) stringLength);

    xmlString.copyToBuffer (d + 8, stringLength);
}

XmlElement* AudioProcessor::getXmlFromBinary (const void* data,
                                              const int sizeInBytes)
{
    if (sizeInBytes > 8
         && littleEndianInt ((const char*) data) == magicXmlNumber)
    {
        const uint32 stringLength = littleEndianInt (((const char*) data) + 4);

        if (stringLength > 0)
        {
            XmlDocument doc (String (((const char*) data) + 8,
                                     jmin ((sizeInBytes - 8), stringLength)));

            return doc.getDocumentElement();
        }
    }

    return 0;
}

//==============================================================================
void AudioProcessorListener::audioProcessorParameterChangeGestureBegin (AudioProcessor*, int)
{
}

void AudioProcessorListener::audioProcessorParameterChangeGestureEnd (AudioProcessor*, int)
{
}


END_JUCE_NAMESPACE
