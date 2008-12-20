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

// This file is included at the start of each FLAC .c file, just to do a few housekeeping
// tasks..


#include "../../../../../../juce_Config.h"

#define VERSION "1.2.1"

#define FLAC__NO_DLL 1

#ifdef _MSC_VER
  #pragma warning (disable: 4267 4127 4244 4996 4100 4701 4702 4013 4133 4206 4312)
#endif

#if ! (defined (_WIN32) || defined (_WIN64) || defined (LINUX))
 #define FLAC__SYS_DARWIN 1
#endif
