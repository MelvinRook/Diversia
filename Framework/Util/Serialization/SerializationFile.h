/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_SERIALIZATIONFILE_H
#define DIVERSIA_UTIL_SERIALIZATIONFILE_H

#include "Util/Serialization/SerializationStream.h"

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

/**
Interface for a serialization file.
**/
class DIVERSIA_UTIL_API SerializationFile : public SerializationStream
{
public:
    /**
    Constructor with single tag.

    @param  rTag    The tag to include or exclude properties with.
    @param  include True to include properties, false to exclude properties.
    **/
    SerializationFile( const camp::Value& rTag, bool include ): SerializationStream( rTag, include ) {}
    /**
    Constructor with multiple tags.

    @param  rTags   The tags to include or exclude properties with.
    @param  include True to include properties, false to exclude properties.
    **/
    SerializationFile( const camp::Args& rTags, bool include ):  SerializationStream( rTags, include ) {}
    /**
    Destructor.
    **/
    virtual ~SerializationFile() {}

    /**
    Loads from file.

    @return True if it succeeds, false if it fails.
    **/
    virtual bool load() = 0;
    /**
    Saves to file.

    @param  backup  True to backup the existing file before saving. Defaults to false.

    @return True if it succeeds, false if it fails.
    **/
    virtual bool save( bool backup = false ) = 0;

};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_SERIALIZATIONFILE_H