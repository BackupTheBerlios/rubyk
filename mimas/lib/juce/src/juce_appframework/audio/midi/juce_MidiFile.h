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

#ifndef __JUCE_MIDIFILE_JUCEHEADER__
#define __JUCE_MIDIFILE_JUCEHEADER__

#include "juce_MidiMessageSequence.h"
#include "../../../juce_core/io/juce_InputStream.h"
#include "../../../juce_core/io/juce_OutputStream.h"


//==============================================================================
/**
    Reads/writes standard midi format files.

    To read a midi file, create a MidiFile object and call its readFrom() method. You
    can then get the individual midi tracks from it using the getTrack() method.

    To write a file, create a MidiFile object, add some MidiMessageSequence objects
    to it using the addTrack() method, and then call its writeTo() method to stream
    it out.

    @see MidiMessageSequence
*/
class JUCE_API  MidiFile
{
public:
    //==============================================================================
    /** Creates an empty MidiFile object.
    */
    MidiFile() throw();

    /** Destructor. */
    ~MidiFile() throw();

    //==============================================================================
    /** Returns the number of tracks in the file.

        @see getTrack, addTrack
    */
    int getNumTracks() const throw();

    /** Returns a pointer to one of the tracks in the file.

        @returns a pointer to the track, or 0 if the index is out-of-range
        @see getNumTracks, addTrack
    */
    const MidiMessageSequence* getTrack (const int index) const throw();

    /** Adds a midi track to the file.

        This will make its own internal copy of the sequence that is passed-in.

        @see getNumTracks, getTrack
    */
    void addTrack (const MidiMessageSequence& trackSequence)  throw();

    /** Removes all midi tracks from the file.

        @see getNumTracks
    */
    void clear() throw();

    /** Returns the raw time format code that will be written to a stream.

        After reading a midi file, this method will return the time-format that
        was read from the file's header. It can be changed using the setTicksPerQuarterNote()
        or setSmpteTimeFormat() methods.

        If the value returned is positive, it indicates the number of midi ticks
        per quarter-note - see setTicksPerQuarterNote().

        It it's negative, the upper byte indicates the frames-per-second (but negative), and
        the lower byte is the number of ticks per frame - see setSmpteTimeFormat().
    */
    short getTimeFormat() const throw();

    /** Sets the time format to use when this file is written to a stream.

        If this is called, the file will be written as bars/beats using the
        specified resolution, rather than SMPTE absolute times, as would be
        used if setSmpteTimeFormat() had been called instead.

        @param ticksPerQuarterNote  e.g. 96, 960
        @see setSmpteTimeFormat
    */
    void setTicksPerQuarterNote (const int ticksPerQuarterNote) throw();

    /** Sets the time format to use when this file is written to a stream.

        If this is called, the file will be written using absolute times, rather
        than bars/beats as would be the case if setTicksPerBeat() had been called
        instead.

        @param framesPerSecond      must be 24, 25, 29 or 30
        @param subframeResolution   the sub-second resolution, e.g. 4 (midi time code),
                                    8, 10, 80 (SMPTE bit resolution), or 100. For millisecond
                                    timing, setSmpteTimeFormat (25, 40)
        @see setTicksPerBeat
    */
    void setSmpteTimeFormat (const int framesPerSecond,
                             const int subframeResolution) throw();

    //==============================================================================
    /** Makes a list of all the tempo-change meta-events from all tracks in the midi file.

        Useful for finding the positions of all the tempo changes in a file.

        @param tempoChangeEvents    a list to which all the events will be added
    */
    void findAllTempoEvents (MidiMessageSequence& tempoChangeEvents) const;

    /** Makes a list of all the time-signature meta-events from all tracks in the midi file.

        Useful for finding the positions of all the tempo changes in a file.

        @param timeSigEvents        a list to which all the events will be added
    */
    void findAllTimeSigEvents (MidiMessageSequence& timeSigEvents) const;

    /** Returns the latest timestamp in any of the tracks.

        (Useful for finding the length of the file).
    */
    double getLastTimestamp() const;

    //==============================================================================
    /** Reads a midi file format stream.

        After calling this, you can get the tracks that were read from the file by using the
        getNumTracks() and getTrack() methods.

        The timestamps of the midi events in the tracks will represent their positions in
        terms of midi ticks. To convert them to seconds, use the convertTimestampTicksToSeconds()
        method.

        @returns true if the stream was read successfully
    */
    bool readFrom (InputStream& sourceStream);

    /** Writes the midi tracks as a standard midi file.

        @returns true if the operation succeeded.
    */
    bool writeTo (OutputStream& destStream);

    /** Converts the timestamp of all the midi events from midi ticks to seconds.

        This will use the midi time format and tempo/time signature info in the
        tracks to convert all the timestamps to absolute values in seconds.
    */
    void convertTimestampTicksToSeconds();


    //==============================================================================
    juce_UseDebuggingNewOperator

    /** @internal */
    static int compareElements (const MidiMessageSequence::MidiEventHolder* const first,
                                const MidiMessageSequence::MidiEventHolder* const second) throw();

private:
    MidiMessageSequence* tracks [128];
    short numTracks, timeFormat;

    MidiFile (const MidiFile&);
    const MidiFile& operator= (const MidiFile&);

    void readNextTrack (const char* data, int size);
    void writeTrack (OutputStream& mainOut, const int trackNum);
};


#endif   // __JUCE_MIDIFILE_JUCEHEADER__
