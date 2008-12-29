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

#ifdef _MSC_VER
  #pragma warning (disable: 4514)
  #pragma warning (push)
#endif

#include "win32_headers.h"
#include <float.h>
#include "../../../src/juce_core/basics/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "../../../src/juce_core/misc/juce_PlatformUtilities.h"
#include "../../../src/juce_core/basics/juce_SystemStats.h"

#ifdef _MSC_VER
  #pragma warning (pop)
#endif


//==============================================================================
static HKEY findKeyForPath (String name,
                            const bool createForWriting,
                            String& valueName) throw()
{
    HKEY rootKey = 0;

    if (name.startsWithIgnoreCase (T("HKEY_CURRENT_USER\\")))
        rootKey = HKEY_CURRENT_USER;
    else if (name.startsWithIgnoreCase (T("HKEY_LOCAL_MACHINE\\")))
        rootKey = HKEY_LOCAL_MACHINE;
    else if (name.startsWithIgnoreCase (T("HKEY_CLASSES_ROOT\\")))
        rootKey = HKEY_CLASSES_ROOT;

    if (rootKey != 0)
    {
        name = name.substring (name.indexOfChar (T('\\')) + 1);

        const int lastSlash = name.lastIndexOfChar (T('\\'));
        valueName = name.substring (lastSlash + 1);
        name = name.substring (0, lastSlash);

        HKEY key;
        DWORD result;

        if (createForWriting)
        {
            if (RegCreateKeyEx (rootKey, name, 0, L"", REG_OPTION_NON_VOLATILE,
                                (KEY_WRITE | KEY_QUERY_VALUE), 0, &key, &result) == ERROR_SUCCESS)
                return key;
        }
        else
        {
            if (RegOpenKeyEx (rootKey, name, 0, KEY_READ, &key) == ERROR_SUCCESS)
                return key;
        }
    }

    return 0;
}

const String PlatformUtilities::getRegistryValue (const String& regValuePath,
                                                  const String& defaultValue)
{
    String valueName, s;
    HKEY k = findKeyForPath (regValuePath, false, valueName);

    if (k != 0)
    {
        WCHAR buffer [2048];
        unsigned long bufferSize = sizeof (buffer);
        DWORD type = REG_SZ;

        if (RegQueryValueEx (k, valueName, 0, &type, (LPBYTE) buffer, &bufferSize) == ERROR_SUCCESS)
            s = buffer;
        else
            s = defaultValue;

        RegCloseKey (k);
    }

    return s;
}

void PlatformUtilities::setRegistryValue (const String& regValuePath,
                                          const String& value)
{
    String valueName;
    HKEY k = findKeyForPath (regValuePath, true, valueName);

    if (k != 0)
    {
        RegSetValueEx (k, valueName, 0, REG_SZ,
                       (const BYTE*) (const WCHAR*) value,
                       sizeof (WCHAR) * (value.length() + 1));

        RegCloseKey (k);
    }
}

bool PlatformUtilities::registryValueExists (const String& regValuePath)
{
    bool exists = false;
    String valueName;
    HKEY k = findKeyForPath (regValuePath, false, valueName);

    if (k != 0)
    {
        unsigned char buffer [2048];
        unsigned long bufferSize = sizeof (buffer);
        DWORD type = 0;

        if (RegQueryValueEx (k, valueName, 0, &type, buffer, &bufferSize) == ERROR_SUCCESS)
            exists = true;

        RegCloseKey (k);
    }

    return exists;
}

void PlatformUtilities::deleteRegistryValue (const String& regValuePath)
{
    String valueName;
    HKEY k = findKeyForPath (regValuePath, true, valueName);

    if (k != 0)
    {
        RegDeleteValue (k, valueName);
        RegCloseKey (k);
    }
}

void PlatformUtilities::deleteRegistryKey (const String& regKeyPath)
{
    String valueName;
    HKEY k = findKeyForPath (regKeyPath, true, valueName);

    if (k != 0)
    {
        RegDeleteKey (k, valueName);
        RegCloseKey (k);
    }
}

bool juce_IsRunningInWine() throw()
{
    HKEY key;
    if (RegOpenKeyEx (HKEY_CURRENT_USER, _T("Software\\Wine"), 0, KEY_READ, &key) == ERROR_SUCCESS)
    {
        RegCloseKey (key);
        return true;
    }

    return false;
}

//==============================================================================
static void* currentModuleHandle = 0;

void* PlatformUtilities::getCurrentModuleInstanceHandle() throw()
{
    if (currentModuleHandle == 0)
        currentModuleHandle = GetModuleHandle (0);

    return currentModuleHandle;
}

void PlatformUtilities::setCurrentModuleInstanceHandle (void* const newHandle) throw()
{
    currentModuleHandle = newHandle;
}

void PlatformUtilities::fpuReset()
{
#if JUCE_MSVC
    _clearfp();
#endif
}

END_JUCE_NAMESPACE
