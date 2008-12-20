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

#include "juce_AudioDeviceSelectorComponent.h"
#include "../buttons/juce_TextButton.h"
#include "../menus/juce_PopupMenu.h"
#include "../windows/juce_AlertWindow.h"
#include "../lookandfeel/juce_LookAndFeel.h"
#include "../../../../juce_core/text/juce_LocalisedStrings.h"


//==============================================================================
class AudioDeviceSelectorComponentListBox  : public ListBox,
                                             public ListBoxModel
{
public:
    enum BoxType
    {
        midiInputType,
        audioInputType,
        audioOutputType
    };

    //==============================================================================
    AudioDeviceSelectorComponentListBox (AudioDeviceManager& deviceManager_,
                                         const BoxType type_,
                                         const String& noItemsMessage_,
                                         const int minNumber_,
                                         const int maxNumber_)
        : ListBox (String::empty, 0),
          deviceManager (deviceManager_),
          type (type_),
          noItemsMessage (noItemsMessage_),
          minNumber (minNumber_),
          maxNumber (maxNumber_)
    {
        AudioIODevice* const currentDevice = deviceManager.getCurrentAudioDevice();

        if (type_ == midiInputType)
        {
            items = MidiInput::getDevices();
        }
        else if (type_ == audioInputType)
        {
            items = currentDevice->getInputChannelNames();
        }
        else if (type_ == audioOutputType)
        {
            items = currentDevice->getOutputChannelNames();
        }
        else
        {
            jassertfalse
        }

        setModel (this);
        setOutlineThickness (1);
    }

    ~AudioDeviceSelectorComponentListBox()
    {
    }

    int getNumRows()
    {
        return items.size();
    }

    void paintListBoxItem (int row,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected)
    {
        if (((unsigned int) row) < (unsigned int) items.size())
        {
            if (rowIsSelected)
                g.fillAll (findColour (TextEditor::highlightColourId)
                               .withMultipliedAlpha (0.3f));

            const String item (items [row]);
            bool enabled = false;

            if (type == midiInputType)
            {
                enabled = deviceManager.isMidiInputEnabled (item);
            }
            else if (type == audioInputType)
            {
                enabled = deviceManager.getInputChannels() [row];
            }
            else if (type == audioOutputType)
            {
                enabled = deviceManager.getOutputChannels() [row];
            }

            const int x = getTickX();
            const int tickW = height - height / 4;

            getLookAndFeel().drawTickBox (g, *this, x - tickW, (height - tickW) / 2, tickW, tickW,
                                          enabled, true, true, false);

            g.setFont (height * 0.6f);
            g.setColour (findColour (ListBox::textColourId, true).withMultipliedAlpha (enabled ? 1.0f : 0.6f));
            g.drawText (item, x, 0, width - x - 2, height, Justification::centredLeft, true);
        }
    }

    void listBoxItemClicked (int row, const MouseEvent& e)
    {
        selectRow (row);

        if (e.x < getTickX())
            flipEnablement (row);
    }

    void listBoxItemDoubleClicked (int row, const MouseEvent&)
    {
        flipEnablement (row);
    }

    void returnKeyPressed (int row)
    {
        flipEnablement (row);
    }

    void paint (Graphics& g)
    {
        ListBox::paint (g);

        if (items.size() == 0)
        {
            g.setColour (Colours::grey);
            g.setFont (13.0f);
            g.drawText (noItemsMessage,
                        0, 0, getWidth(), getHeight() / 2,
                        Justification::centred, true);
        }
    }

    int getBestHeight (const int preferredHeight)
    {
        const int extra = getOutlineThickness() * 2;

        return jmax (getRowHeight() * 2 + extra,
                     jmin (getRowHeight() * getNumRows() + extra,
                           preferredHeight));
    }

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    AudioDeviceManager& deviceManager;
    const BoxType type;
    const String noItemsMessage;
    StringArray items;
    int minNumber, maxNumber;

    void flipEnablement (const int row)
    {
        if (((unsigned int) row) < (unsigned int) items.size())
        {
            AudioIODevice* const audioDevice = deviceManager.getCurrentAudioDevice();

            const String item (items [row]);

            if (type == midiInputType)
            {
                deviceManager.setMidiInputEnabled (item, ! deviceManager.isMidiInputEnabled (item));
            }
            else
            {
                jassert (type == audioInputType || type == audioOutputType);

                if (audioDevice != 0)
                {
                    BitArray chans (type == audioInputType ? deviceManager.getInputChannels()
                                                           : deviceManager.getOutputChannels());

                    const BitArray oldChans (chans);

                    const bool newVal = ! chans[row];
                    const int numActive = chans.countNumberOfSetBits();

                    if (! newVal)
                    {
                        if (numActive > minNumber)
                            chans.setBit (row, false);
                    }
                    else
                    {
                        if (numActive >= maxNumber)
                        {
                            const int firstActiveChan = chans.findNextSetBit();

                            chans.setBit (row > firstActiveChan
                                             ? firstActiveChan : chans.getHighestBit(),
                                          false);
                        }

                        chans.setBit (row, true);
                    }

                    if (type == audioInputType)
                        deviceManager.setInputChannels (chans, true);
                    else
                        deviceManager.setOutputChannels (chans, true);
                }
            }
        }
    }

    int getTickX() const throw()
    {
        return getRowHeight() + 5;
    }

    AudioDeviceSelectorComponentListBox (const AudioDeviceSelectorComponentListBox&);
    const AudioDeviceSelectorComponentListBox& operator= (const AudioDeviceSelectorComponentListBox&);
};

//==============================================================================
AudioDeviceSelectorComponent::AudioDeviceSelectorComponent (AudioDeviceManager& deviceManager_,
                                                            const int minInputChannels_,
                                                            const int maxInputChannels_,
                                                            const int minOutputChannels_,
                                                            const int maxOutputChannels_,
                                                            const bool showMidiInputOptions,
                                                            const bool showMidiOutputSelector)
    : deviceManager (deviceManager_),
      minOutputChannels (minOutputChannels_),
      maxOutputChannels (maxOutputChannels_),
      minInputChannels (minInputChannels_),
      maxInputChannels (maxInputChannels_),
      sampleRateDropDown (0),
      inputChansBox (0),
      inputsLabel (0),
      outputChansBox (0),
      outputsLabel (0),
      sampleRateLabel (0),
      bufferSizeDropDown (0),
      bufferSizeLabel (0),
      launchUIButton (0)
{
    jassert (minOutputChannels >= 0 && minOutputChannels <= maxOutputChannels);
    jassert (minInputChannels >= 0 && minInputChannels <= maxInputChannels);

    audioDeviceDropDown = new ComboBox ("device");
    deviceManager_.addDeviceNamesToComboBox (*audioDeviceDropDown);
    audioDeviceDropDown->setSelectedId (-1, true);

    if (deviceManager_.getCurrentAudioDeviceName().isNotEmpty())
        audioDeviceDropDown->setText (deviceManager_.getCurrentAudioDeviceName(), true);

    audioDeviceDropDown->addListener (this);
    addAndMakeVisible (audioDeviceDropDown);

    Label* label = new Label ("l1", TRANS ("audio device:"));
    label->attachToComponent (audioDeviceDropDown, true);

    if (showMidiInputOptions)
    {
        addAndMakeVisible (midiInputsList
                            = new AudioDeviceSelectorComponentListBox (deviceManager,
                                                       AudioDeviceSelectorComponentListBox::midiInputType,
                                                       TRANS("(no midi inputs available)"),
                                                       0, 0));

        midiInputsLabel = new Label ("lm", TRANS ("active midi inputs:"));
        midiInputsLabel->setJustificationType (Justification::topRight);
        midiInputsLabel->attachToComponent (midiInputsList, true);
    }
    else
    {
        midiInputsList = 0;
        midiInputsLabel = 0;
    }

    if (showMidiOutputSelector)
    {
        addAndMakeVisible (midiOutputSelector = new ComboBox (String::empty));
        midiOutputSelector->addListener (this);

        midiOutputLabel = new Label ("lm", TRANS("Midi Output:"));
        midiOutputLabel->attachToComponent (midiOutputSelector, true);
    }
    else
    {
        midiOutputSelector = 0;
        midiOutputLabel = 0;
    }

    deviceManager_.addChangeListener (this);
    changeListenerCallback (0);
}

AudioDeviceSelectorComponent::~AudioDeviceSelectorComponent()
{
    deviceManager.removeChangeListener (this);
    deleteAllChildren();
}

void AudioDeviceSelectorComponent::resized()
{
    const int lx = proportionOfWidth (0.35f);
    const int w = proportionOfWidth (0.55f);
    const int h = 24;
    const int space = 6;
    const int dh = h + space;
    int y = 15;

    audioDeviceDropDown->setBounds (lx, y, w, h);
    y += dh;

    if (sampleRateDropDown != 0)
    {
        sampleRateDropDown->setBounds (lx, y, w, h);
        y += dh;
    }

    if (bufferSizeDropDown != 0)
    {
        bufferSizeDropDown->setBounds (lx, y, w, h);
        y += dh;
    }

    if (launchUIButton != 0)
    {
        launchUIButton->setBounds (lx, y, 150, h);
        ((TextButton*) launchUIButton)->changeWidthToFitText();
        y += dh;
    }

    VoidArray boxes;

    if (outputChansBox != 0)
        boxes.add (outputChansBox);

    if (inputChansBox != 0)
        boxes.add (inputChansBox);

    if (midiInputsList != 0)
        boxes.add (midiInputsList);

    const int boxSpace = getHeight() - y;

    for (int i = 0; i < boxes.size(); ++i)
    {
        AudioDeviceSelectorComponentListBox* const box = (AudioDeviceSelectorComponentListBox*) boxes.getUnchecked (i);

        const int bh = box->getBestHeight (jmin (h * 8, boxSpace / boxes.size()) - space);
        box->setBounds (lx, y, w, bh);
        y += bh + space;
    }

    if (midiOutputSelector != 0)
        midiOutputSelector->setBounds (lx, y, w, h);
}

void AudioDeviceSelectorComponent::buttonClicked (Button*)
{
    AudioIODevice* const device = deviceManager.getCurrentAudioDevice();

    if (device != 0 && device->hasControlPanel())
    {
        const String lastDevice (device->getName());

        if (device->showControlPanel())
        {
            deviceManager.setAudioDevice (String::empty, 0, 0, 0, 0, false);
            deviceManager.setAudioDevice (lastDevice, 0, 0, 0, 0, false);
        }

        getTopLevelComponent()->toFront (true);
    }
}

void AudioDeviceSelectorComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    AudioIODevice* const audioDevice = deviceManager.getCurrentAudioDevice();

    if (comboBoxThatHasChanged == audioDeviceDropDown)
    {
        if (audioDeviceDropDown->getSelectedId() < 0)
        {
            deviceManager.setAudioDevice (String::empty, 0, 0, 0, 0, true);
        }
        else
        {
            String error (deviceManager.setAudioDevice (audioDeviceDropDown->getText(),
                                                        0, 0, 0, 0, true));

            if (error.isNotEmpty())
            {
#if JUCE_WIN32
                if (deviceManager.getInputChannels().countNumberOfSetBits() > 0
                      && deviceManager.getOutputChannels().countNumberOfSetBits() > 0)
                {
                    // in DSound, some machines lose their primary input device when a mic
                    // is removed, and this also buggers up our attempt at opening an output
                    // device, so this is a workaround that doesn't fail in that case.
                    BitArray noInputs;
                    error = deviceManager.setAudioDevice (audioDeviceDropDown->getText(),
                                                          0, 0, &noInputs, 0, false);
                }
#endif
                if (error.isNotEmpty())
                    AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                                 T("Error while opening \"")
                                                    + audioDeviceDropDown->getText()
                                                    + T("\""),
                                                 error);
            }
        }

        if (deviceManager.getCurrentAudioDeviceName().isNotEmpty())
            audioDeviceDropDown->setText (deviceManager.getCurrentAudioDeviceName(), true);
        else
            audioDeviceDropDown->setSelectedId (-1, true);
    }
    else if (comboBoxThatHasChanged == midiOutputSelector)
    {
        deviceManager.setDefaultMidiOutput (midiOutputSelector->getText());
    }
    else if (audioDevice != 0)
    {
        if (bufferSizeDropDown != 0 && comboBoxThatHasChanged == bufferSizeDropDown)
        {
            if (bufferSizeDropDown->getSelectedId() > 0)
                deviceManager.setAudioDevice (audioDevice->getName(),
                                              bufferSizeDropDown->getSelectedId(),
                                              audioDevice->getCurrentSampleRate(),
                                              0, 0, true);
        }
        else if (sampleRateDropDown != 0 && comboBoxThatHasChanged == sampleRateDropDown)
        {
            if (sampleRateDropDown->getSelectedId() > 0)
                deviceManager.setAudioDevice (audioDevice->getName(),
                                              audioDevice->getCurrentBufferSizeSamples(),
                                              sampleRateDropDown->getSelectedId(),
                                              0, 0, true);
        }
    }
}

void AudioDeviceSelectorComponent::changeListenerCallback (void*)
{
    deleteAndZero (sampleRateDropDown);
    deleteAndZero (inputChansBox);
    deleteAndZero (inputsLabel);
    deleteAndZero (outputChansBox);
    deleteAndZero (outputsLabel);
    deleteAndZero (sampleRateLabel);
    deleteAndZero (bufferSizeDropDown);
    deleteAndZero (bufferSizeLabel);
    deleteAndZero (launchUIButton);

    AudioIODevice* const currentDevice = deviceManager.getCurrentAudioDevice();

    if (currentDevice != 0)
    {
        audioDeviceDropDown->setText (currentDevice->getName(), true);

        // sample rate
        addAndMakeVisible (sampleRateDropDown = new ComboBox ("samplerate"));
        sampleRateLabel = new Label ("l2", TRANS ("sample rate:"));
        sampleRateLabel->attachToComponent (sampleRateDropDown, true);

        const int numRates = currentDevice->getNumSampleRates();

        int i;
        for (i = 0; i < numRates; ++i)
        {
            const int rate = roundDoubleToInt (currentDevice->getSampleRate (i));
            sampleRateDropDown->addItem (String (rate) + T(" Hz"), rate);
        }

        const double currentRate = currentDevice->getCurrentSampleRate();
        sampleRateDropDown->setSelectedId (roundDoubleToInt (currentRate), true);
        sampleRateDropDown->addListener (this);

        // buffer size
        addAndMakeVisible (bufferSizeDropDown = new ComboBox ("buffersize"));
        bufferSizeLabel = new Label ("l2", TRANS ("audio buffer size:"));
        bufferSizeLabel->attachToComponent (bufferSizeDropDown, true);

        const int numBufferSizes = currentDevice->getNumBufferSizesAvailable();

        for (i = 0; i < numBufferSizes; ++i)
        {
            const int bs = currentDevice->getBufferSizeSamples (i);
            bufferSizeDropDown->addItem (String (bs)
                                          + T(" samples (")
                                          + String (bs * 1000.0 / currentRate, 1)
                                          + T(" ms)"),
                                         bs);
        }

        bufferSizeDropDown->setSelectedId (currentDevice->getCurrentBufferSizeSamples(), true);
        bufferSizeDropDown->addListener (this);

        if (currentDevice->hasControlPanel())
        {
            addAndMakeVisible (launchUIButton = new TextButton (TRANS ("show this device's control panel"),
                                                                TRANS ("opens the device's own control panel")));

            launchUIButton->addButtonListener (this);
        }

        // output chans
        if (maxOutputChannels > 0 && minOutputChannels < currentDevice->getOutputChannelNames().size())
        {
            addAndMakeVisible (outputChansBox
                                = new AudioDeviceSelectorComponentListBox (deviceManager,
                                                           AudioDeviceSelectorComponentListBox::audioOutputType,
                                                           TRANS ("(no audio output channels found)"),
                                                           minOutputChannels, maxOutputChannels));

            outputsLabel = new Label ("l3", TRANS ("active output channels:"));
            outputsLabel->attachToComponent (outputChansBox, true);
        }

        // input chans
        if (maxInputChannels > 0 && minInputChannels < currentDevice->getInputChannelNames().size())
        {
            addAndMakeVisible (inputChansBox
                                = new AudioDeviceSelectorComponentListBox (deviceManager,
                                                           AudioDeviceSelectorComponentListBox::audioInputType,
                                                           TRANS ("(no audio input channels found)"),
                                                           minInputChannels, maxInputChannels));

            inputsLabel = new Label ("l4", TRANS ("active input channels:"));
            inputsLabel->attachToComponent (inputChansBox, true);
        }
    }
    else
    {
        audioDeviceDropDown->setSelectedId (-1, true);
    }

    if (midiInputsList != 0)
    {
        midiInputsList->updateContent();
        midiInputsList->repaint();
    }

    if (midiOutputSelector != 0)
    {
        midiOutputSelector->clear();

        const StringArray midiOuts (MidiOutput::getDevices());

        midiOutputSelector->addItem (TRANS("<< no audio device >>"), -1);
        midiOutputSelector->addSeparator();

        for (int i = 0; i < midiOuts.size(); ++i)
            midiOutputSelector->addItem (midiOuts[i], i + 1);

        int current = -1;

        if (deviceManager.getDefaultMidiOutput() != 0)
            current = 1 + midiOuts.indexOf (deviceManager.getDefaultMidiOutputName());

        midiOutputSelector->setSelectedId (current, true);
    }

    resized();
}

END_JUCE_NAMESPACE
