/*
cartotype_errors.h
Copyright (C) 2004-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

namespace CartoTypeCore
{

/**
The result and error code type. It is a class, to enforce initialization to zero, and
to allow a breakpoint to be placed in the constructor or assignment operator.
In release builds on modern compilers the use of a class rather than a plain integer has no cost.
*/
class Result
    {
    public:
    constexpr Result() noexcept { }
    /** Creates a Result object containing a specified code. */
    constexpr Result(uint32_t aCode) noexcept
        {
        *this = aCode;
        }
    /** Returns the integer code of this Result object. */
    constexpr operator uint32_t() const noexcept { return Code; }
    /** Assigns an integer code to this Result object. */
    constexpr void operator=(uint32_t aCode) noexcept
        {
        Code = aCode;
        }

    private:
    uint32_t Code = 0;
    };

/** No error; a successful result. */
constexpr Result KErrorNone = 0;

/**
Use KErrorGeneral where an error has occurred but no other CartoTypeCore
error is applicable. For example, this error code can be used
when FreeType returns an error code for illegal TrueType hinting instructions.
*/
constexpr Result KErrorGeneral = 1;

/** Use KErrorNoMemory when an attempted memory allocation fails. */
constexpr Result KErrorNoMemory = 2;

/**
KErrorEndOfData is returned by iterators and streams when no more data
is available. It may be treated as an error or not depending on the
circumstances.
*/
constexpr Result KErrorEndOfData = 3;

/**
KErrorTextUnmodifiable is returned when an attempt is made to
modify an unmodifiable string represented by a class derived
from MString.
*/
constexpr Result KErrorTextUnmodifiable = 4;

/**
KErrorTextTruncated is returned when text inserted into
a string derived from MString is too long for a fixed-size
buffer.
*/
constexpr Result KErrorTextTruncated = 5;

/**
KErrorNotFound is returned whenever a resource like a file
is not found, or a search fails.
It may be treated as an error or not depending on the
circumstances.
*/
constexpr Result KErrorNotFound = 6;

/**
The code KErrorUnimplemented is used for functions that have
not yet been implemented, or for circumstances or options
within functions that remain unimplemented because they
are deemed to be either rarely or never useful.
*/
constexpr Result KErrorUnimplemented = 7;

/**
If the graphics system detects an invalid outline, such as one
that starts with a cubic control point, it returns
KErrorInvalidOutline.
*/
constexpr Result KErrorInvalidOutline = 8;

/**
The code KErrorIo should be used for unexpected read or write
errors on files or other data streams. This is for where you
attempt to read data that should be there, but the OS says
it's unavailable (e.g., someone has taken the CD out of the drive
or the network is down).
*/
constexpr Result KErrorIo = 9;

/**
Use KErrorCorrupt when illegal values are found when reading data
from a file or other serialized form.
*/
constexpr Result KErrorCorrupt = 10;

/**
This error is returned by database accessors, typeface accessors, etc.,
to indicate that this data format is unknown to a particular accessor.
It is not really an error but tells the caller to try the next
accessor in the list.
*/
constexpr Result KErrorUnknownDataFormat = 11;

/**
This error is returned by transformation inversion if the transformation
has no inverse.
*/
constexpr Result KErrorNoInverse = 12;

/**
This error is returned by the projection system if the projection
causes overflow.
*/
constexpr Result KErrorProjectionOverflow = 13;

/**
The code KErrorCancel is returned to cancel an asynchronous
operation that uses callbacks. The callback or virtual function
returns this code. the caller must terminate further processing
on receipt of any error, but this one indicates that there has been
no actual error, but the handler has canceled the operation.
*/
constexpr Result KErrorCancel = 14;

/**
This error is returned when an invalid argument has been given to a function.
*/
constexpr Result KErrorInvalidArgument = 15;

/**
This error is returned by a data reader when it cannot deal with the data version.
*/
constexpr Result KErrorUnknownVersion = 16;

/**
This error is returned by the base library when reading data or calculations result
in overflow.
*/
constexpr Result KErrorOverflow = 17;

// ABOLISHED: constexpr Result KErrorOverlap = 18;

/**
The error code returned by line intersection algorithms when the lines are parallel.
*/
constexpr Result KErrorParallelLines = 19;

//  ABOLISHED: constexpr Result KErrorPathLengthExceeded = 20;

//  ABOLISHED: constexpr Result KErrorMaxTurnExceeded = 21;

/**
An attempt was made to draw a bitmap needing a color palette
but no palette was provided.
*/
constexpr Result KErrorNoPalette = 22;

/**
An attempt was made to insert a duplicate object into a collection
class that does not allow duplicates.
*/
constexpr Result KErrorDuplicate = 23;

/**
The projection for converting latitude and longitude to map coordinates
is unknown or unavailable.
*/
constexpr Result KErrorNoProjection = 24;

/**
A palette is full and no new entries can be added.
*/
constexpr Result KErrorPaletteFull = 25;

/**
The dash array for drawing strokes is invalid.
*/
constexpr Result KErrorInvalidDashArray = 26;

// ABOLISHED: constexpr Result KErrorCentralPath = 27;

/**
A route was needed by the navigation system
but no route was available.
*/
constexpr Result KErrorNoRoute = 28;

/**
There was an attempt to zoom beyond the legal zoom limits for a map.
*/
constexpr Result KErrorZoomLimitReached = 29;

/**
There was an attempt to project a map object that had already been projected.
*/
constexpr Result KErrorAlreadyProjected = 30;

/**
Conditions are too deeply nested in the style sheet.
*/
constexpr Result KErrorConditionsTooDeeplyNested = 31;

/**
There was an attempt to use a null font for drawing text.
*/
constexpr Result KErrorNullFont = 32;

/**
An attempt to read data from the internet failed.
*/
constexpr Result KErrorInternetIo = 33;

/**
Division by zero in an interpreted expression.
*/
constexpr Result KErrorDivideByZero = 34;

/**
A transform failed because an argument or result was out of range.
*/
constexpr Result KErrorTransformFailed = 35;

/**
Reading a bitmap from PNG format failed.
*/
constexpr Result KErrorPngRead = 36;

/**
Reading a bitmap from JPG format failed.
*/
constexpr Result KErrorJpgRead = 37;

/**
An object did not intersect a specified region.
*/
constexpr Result KErrorNoIntersection = 38;

/**
An operation was interrupted, for example by another thread writing to a shared flag.
*/
constexpr Result KErrorInterrupt = 39;

/**
There was an attempt to use map databases of incompatible formats
(TMapGrid values containing point format, datum and axis orientations)
to draw a map or find objects in a map.
*/
constexpr Result KErrorMapDataBaseFormatMismatch = 40;

/** A key supplied for encryption was too short. */
constexpr Result KErrorEncryptionKeyTooShort = 41;

/** No encryption key has been set. */
constexpr Result KErrorNoEncryptionKey = 42;

/** A code for standard emergency messages. */
constexpr Result KErrorEmergencyMessage = 43;

/** A code for standard alert messages. */
constexpr Result KErrorAlertMessage = 44;

/** A code for standard critical messages. */
constexpr Result KErrorCriticalMessage = 45;

/** A code for standard error messages. */
constexpr Result KErrorErrorMessage = 46;

/** A code for standard warning messages. */
constexpr Result KErrorWarningMessage = 47;

/** A code for standard notice messages. */
constexpr Result KErrorNoticeMessage = 48;

/** A code for standard information messages. */
constexpr Result KErrorInfoMessage = 49;

/** A code for standard debug messages. */
constexpr Result KErrorDebugMessage = 50;

/** A function has been called which is available only when navigating. */
constexpr Result KErrorNotNavigating = 51;

/** The global framework object does not exist. */
constexpr Result KErrorNoFramework = 52;

/** The global framework object already exists. */
constexpr Result KErrorFrameworkAlreadyExists = 53;

/** A string was not transliterable. */
constexpr Result KErrorUntransliterable = 54;

/** Writing a bitmap to PNG format failed. */
constexpr Result KErrorPngWrite = 55;

/** There was an attempt to write to a read-only map database. */
constexpr Result KErrorReadOnlyMapDataBase = 56;

/** There was an error in the PROJ.4 projection library error other than a projection overflow. */
constexpr Result KErrorProj4 = 57;

/** A function was called from the unlicensed version of CartoTypeCore that is available only in the licensed version. */
constexpr Result KErrorUnlicensed = 58;

/** No route could be created because there were no roads near the start point of a route section. */
constexpr Result KErrorNoRoadsNearStartOfRoute = 59;

/** No route could be created because there were no roads near the end point of a route section. */
constexpr Result KErrorNoRoadsNearEndOfRoute = 60;

/** No route could be created because the start and end point were not connected. */
constexpr Result KErrorNoRouteConnectivity = 61;

/** An unsupported feature was requested from the XML parser. */
constexpr Result KErrorXmlFeatureNotSupported = 62;

/** A map file was not found. */
constexpr Result KErrorMapNotFound = 63;

/** A font file was not found. */
constexpr Result KErrorFontNotFound = 64;

/** A style sheet was not found.  */
constexpr Result KErrorStyleSheetNotFound = 65;

/** An attempt was made to set a route attribute on a feature info that is not a route. */
constexpr Result KErrorFeatureInfoIsNotRoute = 66;

/** An attempt was made to set a non-route attribute on a feature info that is a route. */
constexpr Result KErrorFeatureInfoIsRoute = 67;

/** The number of standard error codes. */
constexpr int32_t KStandardErrorCodeCount = 68;

/** Returns a short description of an error, given its code. */
std::string ErrorString(uint32_t aErrorCode);


/**
The start of the range of errors in XML parsing or in the style sheet format, such as a syntactically incorrect dimension or color.
The top byte is 0x10 for style sheet errors, or in the range 0x11 to 0x2A for Expat XML parser error codes.

The low three bytes give the error location: one byte for the column number (clamped to 0...255) two bytes for the line number (clamped to 0...65535).
*/
constexpr Result KErrorXmlRangeStart = 0x10000000;

/** The end of the range of errors in XML parsing or in the style sheet format. */
constexpr Result KErrorXmlRangeEnd = 0x2AFFFFFF;

/**
The base of error codes for returning SQLite errors.
The SQLite error code is placed in the lower two bytes.
*/
constexpr uint32_t KErrorSQLite = 0x30000000;

/** Result codes for drawing operations. */
enum class DrawResult
    {
    Success,
    OverlapText,
    OverlapEdge,
    MaxTurnExceeded,
    PathLengthExceeded,
    GlyphNotFound,
    TransformFailed
    };

// For unit tests
#ifdef CARTOTYPE_TEST
inline void Panic() { abort(); }
inline void Check(bool aExp) { if (!aExp) Panic(); }
inline void Check(int aExp) { if (!aExp) Panic(); }
inline void Check(const void* aPtr) { if (!aPtr) Panic(); }
#endif

} // namespace CartoTypeCore

