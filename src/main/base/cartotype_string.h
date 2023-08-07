/*
cartotype_string.h
Copyright (C) 2004-2022 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_errors.h>
#include <cartotype_iter.h>
#include <cartotype_char.h>
#include <cartotype_base.h>
#include <cartotype_bidi.h>
#include <cartotype_arithmetic.h>

#include <locale.h>
#include <string>
#include <map>
#include <cstring>

namespace CartoTypeCore
{

class String;
class Abbreviation;
class Text;
class MString;

/**
A constant used to mean 'to the end of the string', or 'unknown string length: must be measured'.
It has the same value and definition as std::string::npos.
*/
static const size_t npos = size_t(-1);


/** Types determining where in a phrase an abbreviation can be used. */
enum class AbbreviationType
    {
    /** The word can be abbreviated at any position in a phrase. */
    Any,
    /** The word can be abbreviated at the start of a phrase only. */
    Start,
    /** The word can be abbreviated at the end of a phrase only. */
    End,
    /** The word is a suffix. */
    Suffix
    };

/** A type for a class mapping String objects to other String objects. */
using StringDictionary = std::map<String, String>;
/** A type for a class mapping String objects to integers. */
using StringTypeDictionary = std::map<String,int>;

/** A class providing abbreviations for words. */
class AbbreviationDictionary
    {
    public:
    void Add(const MString& aLongForm,const MString& aShortForm,AbbreviationType aType);
    /** Returns true if the dictionary is empty. */
    bool IsEmpty() const { return m_abbreviations.empty() && m_suffixes.empty(); }
    const Abbreviation* Abbreviate(const MString& aWord) const;

    private:
    /** A map from lower-case words to their abbreviations, which may be empty for words like 'the'. */
    std::map<String,Abbreviation> m_abbreviations;
    /** Suffixes and their abbreviations; for example, 'strasse' becomes 'str.' */
    std::vector<std::pair<String,Abbreviation>> m_suffixes;
    };

/** Flags and constants to tell text searching how to match search terms with found strings. */
enum class StringMatchMethodFlag
    {
    /** The value zero causes exact matching. */
    Exact = 0,
    /** A flag to match all strings for which the search term is a perfect match or a prefix. */
    Prefix = 1,
    /** A flag to ignore all characters that are not letters, digits or whitespace when matching. */
    IgnoreSymbols = 2,
    /** A flag to match accented and unaccented letters. */
    FoldAccents = 4,
    /** A flag to allow imperfect matches with a small number of omitted, incorrect or extra characters. */
    Fuzzy = 8,
    /** A flag to fold letter case. */
    FoldCase = 16,
    /** A flag to ignore whitespace when matching. */
    IgnoreWhitespace = 32
    };

/** The method used in text searching to match search terms with found strings. */
class StringMatchMethod
    {
    public:
    StringMatchMethod() = default;
    StringMatchMethod(StringMatchMethodFlag aFlag1,
                      StringMatchMethodFlag aFlag2 = StringMatchMethodFlag::Exact,
                      StringMatchMethodFlag aFlag3 = StringMatchMethodFlag::Exact,
                      StringMatchMethodFlag aFlag4 = StringMatchMethodFlag::Exact,
                      StringMatchMethodFlag aFlag5 = StringMatchMethodFlag::Exact,
                      StringMatchMethodFlag aFlag6 = StringMatchMethodFlag::Exact);
    /** Adds a new bit flag to a StringMatchMethod. Has no effect if the flag is already set. */
    StringMatchMethod operator+=(StringMatchMethodFlag aFlag) { m_flags |= uint16_t(aFlag); return *this; }
    /** Removes a bit flag from a StringMatchMethod. Has no effect if the flag is not set. */
    StringMatchMethod operator-=(StringMatchMethodFlag aFlag) { m_flags &= ~uint16_t(aFlag); return *this; }
    /** Returns the integer value of the flags. */
    unsigned int Flags() const { return m_flags; }
    /** Creates a StringMatchMethod object from an integer containing StringMatchMethodFlag values. */
    static StringMatchMethod FromFlags(unsigned int aFlags) {  return StringMatchMethod(aFlags);  }
    /** Returns true if aFlag is set. */
    bool operator&(StringMatchMethodFlag aFlag) const { return (m_flags & uint16_t(aFlag)) != 0; }
    bool AnyAreSet(StringMatchMethodFlag aFlag1,
                   StringMatchMethodFlag aFlag2 = StringMatchMethodFlag::Exact,
                   StringMatchMethodFlag aFlag3 = StringMatchMethodFlag::Exact,
                   StringMatchMethodFlag aFlag4 = StringMatchMethodFlag::Exact,
                   StringMatchMethodFlag aFlag5 = StringMatchMethodFlag::Exact,
                   StringMatchMethodFlag aFlag6 = StringMatchMethodFlag::Exact) const;
    bool Ignore(int32_t aCharCode) const;
    /** Returns true if any characters are ignored. */
    bool MayIgnoreCharacters() const
        {
        return (m_flags & (uint16_t(StringMatchMethodFlag::IgnoreSymbols) | uint16_t(StringMatchMethodFlag::IgnoreWhitespace))) != 0;
        }
    /** The equality operator. */
    bool operator==(StringMatchMethod aOther) const { return m_flags == aOther.m_flags; }
    /** The inequality operator. */
    bool operator!=(StringMatchMethod aOther) const { return m_flags != aOther.m_flags; }

    /** Strings must match exactly. */
    static const StringMatchMethod Exact;

    /** Fold (ignore) letter case when matching strings. */
    static const StringMatchMethod FoldCase;

    /** Fold (ignore) accents when matching strings. */
    static const StringMatchMethod FoldAccents;

    /** Fold (ignore) case and accents when matching strings. */
    static const StringMatchMethod Fold;

    /** The search term must be an exact match or a prefix of the found string. */
    static const StringMatchMethod Prefix;

    /** Loose matching: ignore non-alphanumerics and fold accents and case. */
    static const StringMatchMethod Loose;

    /** Allow fuzzy matches: ignore non-alphanumerics, fold accents and allow imperfect matches. */
    static const StringMatchMethod Fuzzy;

    private:
    StringMatchMethod(unsigned int aFlags): m_flags(uint16_t(aFlags & 63)) {  }

    uint16_t m_flags = 0;
    };

/** Different qualities of string match used when comparing map object attributes. */
enum class StringMatchType
    {
    /** No match. */
    None,
    /** A substring of the text matches, but the substring is not aligned to token boundaries. */
    Substring,
    /** A fuzzy match: a small number of extra characters, missing characters, or transpositions. */
    Fuzzy,
    /** A phrase match: a substring match aligned to token boundaries. */
    Phrase,
    /** A full match, ignoring letter case, accents and non-alphanumeric characters. */
    Full
    };

/** A constant for the title case dictionary passed to MString::SetCase. No special title case treatment. This constant must be zero. */
constexpr int KStandardTitleCase = 0;
/**
A constant for the title case dictionary passed to MString::SetCase.
When setting text to title case, do not capitalize the first letter unless the word is initial.
Used for particles like 'on', 'of', etc. and their upper-case variants.
*/
constexpr int KLowerTitleCase = 1;
/**
A constant for the title case dictionary passed to MString::SetCase.
When setting text to title case, leave these strings as they are (they are already upper-case).
Used for acronyms like US, roman numerals like II, etc.
*/
constexpr int KUpperTitleCase = 2;

/**
The string interface class. All strings implement this interface.
The private part of the interface allows completely general compare, append,
insert, delete and replace functions to be implemented in MString.

Although the M prefix of MString implies that this class is a pure mix-in with
no data, it has one data item, the length, which was moved to the base
class for efficiency reasons after profiling indicated a bottleneck accessing
the various virtual Length functions of derived classes.
*/
class MString
    {
    public:
    /**
    A virtual destructor. Provided so that templated collection classes can be created using
    MString as the template class, while still being able to own the strings.
    */
    virtual ~MString() { }

    /** The assignment operator. */
    template<typename T> MString& operator=(const T& aText)
        {
        Set(aText);
        return *this;
        }
    /** Appends aText to this string. */
    template<typename T> MString& operator+=(const T& aText)
        {
        Append(aText);
        return *this;
        }

    /** Returns the length of the text. */
    size_t Length() const { return iLength; }

    /** Returns true if the text is empty. */
    bool IsEmpty() const { return iLength == 0; }

    /** Returns a constant pointer to Unicode text stored in UTF16 format. */
    virtual const uint16_t* Data() const = 0;

    [[nodiscard]] static int32_t Compare(MIter<int32_t>& aIter1,MIter<int32_t>& aIter2,StringMatchMethod aStringMatchMethod) noexcept;
    /** Deletes the text in the range aStart...aEnd. */
    void Delete(size_t aStart,size_t aEnd) { Replace(aStart,aEnd,(const uint16_t*)0,0); }
    /** Deletes all the text. */
    void Clear()
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        ResizeBuffer(0);
        }
    /** Sets the length of the string to zero without discarding reserved memory if any. */
    void SetLengthToZero() { iLength = 0; }

    std::basic_string<uint16_t> CreateUtf16String() const;
    std::string CreateUtf8String() const;

    /** A conversion operator to convert a string to a UTF-8 string. */
    operator std::string() const { return CreateUtf8String(); }

    /** Gets the character at the position aIndex. */
    uint16_t operator[](size_t aIndex) const { assert(aIndex < iLength); return Data()[aIndex]; }

    Text First(size_t aLength) const;
    Text Last(size_t aLength) const;
    Text Substring(size_t aStart,size_t aLength) const;

    void DeletePrefix(const MString& aPrefix);
    void DeleteSuffix(const MString& aSuffix);

    void SetCase(LetterCase aCase,const StringTypeDictionary* aTitleCaseDictionary = 0);
    void SetSentenceCase();

    // functions taking an iterator
    int32_t Compare(MIter<int32_t>& aIter,StringMatchMethod aStringMatchMethod) const noexcept;
    Result Replace(size_t aStart,size_t aEnd,MIter<int32_t>& aText,size_t aMaxLength);

    // functions taking an MString

    /**
    Compares two strings lexicographically.

    Returns 0 if the strings are equal, otherwise:

    returns -2 if this string is less than aString and not a prefix of aString;
    returns -1 if this string is a prefix of aString;
    returns 1 if aString is a prefix of this string;
    returns 2 if aString is less than this string and not a prefix of it.
    */
    int32_t Compare(const MString& aString) const
        {
        auto n = iLength;
        auto m = aString.iLength;
        const uint16_t* p = Data();
        const uint16_t* q = aString.Data();
        while (n != 0 && m != 0)
            {
            if (*p < *q)
                return -2;
            if (*p > *q)
                return 2;
            p++;
            q++;
            n--;
            m--;
            }
        if (m)
            return -1;
        if (n)
            return 1;
        return 0;
        }
    int32_t Compare(const MString& aString,StringMatchMethod aStringMatchMethod) const noexcept;
    size_t Find(const MString& aString,StringMatchMethod aStringMatchMethod = StringMatchMethod::Exact,size_t* aEndPos = nullptr) const noexcept;
    /** Replaces the text aStart...aEnd with aString. */
    void Replace(size_t aStart,size_t aEnd,const MString& aString) { Replace(aStart,aEnd,aString.Data(),aString.Length()); }
    /** Inserts aString at aIndex. */
    void Insert(size_t aIndex,const MString& aString) { Replace(aIndex,aIndex,aString); }

    /** Appends aString. */
    void Append(const MString& aString)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        size_t old_length = iLength;
        ResizeBuffer(iLength + aString.iLength);
        memcpy(WritableText() + old_length,aString.Data(),(iLength - old_length) * 2);
        }

    void AppendCommaSeparated(const String& aLabel,const String& aText,size_t& aItems,size_t aMaxItems = SIZE_MAX);

    /** Sets the string to aString. */
    void Set(const MString& aString)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        ResizeBuffer(aString.iLength);
        std::memcpy(WritableText(),aString.Data(),iLength * 2);
        }

    // functions taking sixteen-bit text

    /**
    Compares null-terminated UTF16 text with this string lexicographically.

    Returns 0 if the strings are equal, otherwise:

    returns -2 if this string is less than aText and not a prefix of aText;
    returns -1 if this string is a prefix of aText;
    returns 1 if aText is a prefix of this string;
    returns 2 if aText is less than this string and not a prefix of it.
    */
    int32_t Compare(const uint16_t* aText) const
        {
        auto n = iLength;
        const uint16_t* p = Data();
        const uint16_t* q = aText;
        while (n-- != 0)
            {
            if (*q == 0)
                return 1;
            if (*p < *q)
                return -2;
            if (*p > *q)
                return 2;
            p++;
            q++;
            }
        if (*q == 0)
            return 0;
        return -1;
        }

    /**
    Compares UTF16 text with a specified length with this string lexicographically.
    If aLength is npos the text is assumed to be null-terminated.

    Returns 0 if the strings are equal, otherwise:

    returns -2 if this string is less than aText and not a prefix of aText;
    returns -1 if this string is a prefix of aText;
    returns 1 if aText is a prefix of this string;
    returns 2 if aText is less than this string and not a prefix of it.
    */
    int32_t Compare(const uint16_t* aText,size_t aLength) const
        {
        if (aLength == npos)
            return Compare(aText);

        auto n = iLength;
        auto m = aLength;
        const uint16_t* p = Data();
        const uint16_t* q = aText;
        while (n != 0 && m != 0)
            {
            if (*p < *q)
                return -2;
            if (*p > *q)
                return 2;
            p++;
            q++;
            n--;
            m--;
            }
        if (m)
            return -1;
        if (n)
            return 1;
        return 0;
        }
    /**
    Compares UTF16 text with a specified length with this string lexicographically.
    If aLength is npos (the default) the text is assumed to be null-terminated.

    Returns 0 if the strings are equal, otherwise:

    returns -2 if this string is less than aText and not a prefix of aText;
    returns -1 if this string is a prefix of aText;
    returns 1 if aText is a prefix of this string;
    returns 2 if aText is less than this string and not a prefix of it.
    */
    int32_t Compare(const char16_t* aText,size_t aLength = npos) const { return Compare((const uint16_t*)aText,aLength); }
    int32_t Compare(const uint16_t* aText,size_t aLength,StringMatchMethod aStringMatchMethod) const noexcept;
    void Replace(size_t aStart,size_t aEnd,const uint16_t* aText,size_t aLength = npos);

    /**
    Replaces the text from aStart to aEnd with the UTF16 text starting at aText. If aLength is npos
    aText must point to null-terminated text.
    */
    void Replace(size_t aStart,size_t aEnd,const char16_t* aText,size_t aLength = npos) { Replace(aStart,aEnd,(const uint16_t*)aText,aLength); }

    /** Inserts aText at aIndex. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Insert(size_t aIndex,const uint16_t* aText,size_t aLength = npos)
        { Replace(aIndex,aIndex,aText,aLength); }

    /** Appends aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Append(const uint16_t* aText,size_t aLength = npos)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        if (aLength == npos)
            {
            const uint16_t* p = aText;
            aLength = 0;
            while (*p++)
                aLength++;
            }
        size_t old_length = iLength;
        ResizeBuffer(iLength + aLength);
        memcpy(WritableText() + old_length,aText,(iLength - old_length) * 2);
        }

    /** Sets the string to aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength. */
    void Set(const uint16_t* aText,size_t aLength = npos)
        {
        if (!Writable())
            {
            assert(false);
            return;
            }
        if (aLength == npos)
            {
            const uint16_t* p = aText;
            aLength = 0;
            while (*p++)
                aLength++;
            }

        ResizeBuffer(aLength);
        memcpy(WritableText(),aText,iLength * 2);
        }

    // functions taking eight-bit text
    int32_t Compare(const char* aText,size_t aLength = npos,StringMatchMethod aStringMatchMethod = StringMatchMethod::Exact) const noexcept;
    int32_t CompareExact(const char* aText,size_t aLength = npos) const noexcept;
    int32_t CompareFoldCase(const char* aText,size_t aLength = npos) const noexcept;

    void Replace(size_t aStart,size_t aEnd,const char* aText,size_t aLength = npos,int32_t aCodePage = 0);
    bool operator==(const char* aText) const noexcept;
    bool operator%(const char* aText) const noexcept;
    /** The inequality operator for comparing MString objects with null-terminated UTF8 strings */
    bool operator!=(const char* aText) const { return !(*this == aText); }

    StringMatchType MatchType(const MString& aText) const;

    /**
    Inserts the eight-bit string aText at aIndex. If aLength is -1 the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 (the default) implies UTF-8.
    */
    void Insert(size_t aIndex,const char* aText,size_t aLength = npos,int32_t aCodePage = 0)
        { Replace(aIndex,aIndex,aText,aLength,aCodePage); }
    /**
    Appends the eight-bit string aText. If aLength is npos the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 (the default) implies UTF-8.
    */
    void Append(const char* aText,size_t aLength = npos,int32_t aCodePage = 0)
        { Insert(Length(),aText,aLength,aCodePage); }

    /** Appends a standard string. */
    template<typename T> void Append(const std::basic_string<T> aString) { Append(aString.data(),aString.length()); }

    /** Sets the text to a standard string. */
    template<typename T> void Set(const std::basic_string<T> aString) { Set(aString.data(),aString.length()); }

    void Set(const char* aText,size_t aLength = npos);

    /**
    Sets the string to the eight-bit string aText. If aLength is -1 the text must be null-terminated, otherwise the length is aLength.
    A code page of 0 implies UTF-8.
    */
    void Set(const char* aText,size_t aLength,int32_t aCodePage)
        { Replace(0,Length(),aText,aLength,aCodePage); }

    // functions taking a character
    /** Replaces the range aStart...aEnd with the single character aChar. */
    void Replace(size_t aStart,size_t aEnd,uint16_t aChar) { Replace(aStart,aEnd,&aChar,1); }
    /** Inserts aChar at aIndex. */
    void Insert(size_t aIndex,uint16_t aChar) { Replace(aIndex,aIndex,aChar); }
    /** Appends aChar. */
    void Append(uint16_t aChar) { Append(&aChar,1); }
    /** Sets the entire text to aChar. */
    void Set(uint16_t aChar) { Replace(0,Length(),aChar); }
    void Trim();

    /** The equality operator. */
    bool operator==(const MString& aString) const
        {
        return aString.iLength == iLength && !memcmp(aString.Data(),Data(),iLength * sizeof(uint16_t));
        }
    /** The equality operator for comparing MString objects with null-terminated UTF16 strings. */
    bool operator==(const uint16_t* aText) const
        {
        const uint16_t* p = Data();
        const uint16_t* q = p + iLength;
        while (p < q && *aText && *p == *aText)
            {
            p++;
            aText++;
            }
        return p == q && *aText == 0;
        }
    /** The equality operator for comparing MString objects with null-terminated UTF16 strings. */
    bool operator==(const char16_t* aText) const
        {
        const char16_t* p = (char16_t*)Data();
        const char16_t* q = p + iLength;
        while (p < q && *aText && *p == *aText)
            {
            p++;
            aText++;
            }
        return p == q && *aText == 0;
        }
    /** The inequality operator. */
    bool operator!=(const MString& aString) const { return !(*this == aString); }
    /** The inequality operator for comparing MString objects with null-terminated UTF16 strings */
    bool operator!=(const uint16_t* aText) const { return !(*this == aText); }
    /** The inequality operator for comparing MString objects with null-terminated UTF16 strings */
    bool operator!=(const char16_t* aText) const { return !(*this == aText); }

    /** The equality operator, ignoring case. */
    bool operator%(const MString& aString) const { return Compare(aString,StringMatchMethod::FoldCase) == 0; }
    /** The equality operator, ignoring case, for comparing MString objects with null-terminated UTF16 strings */
    bool operator%(const uint16_t* aText) const { return Compare(aText,npos,StringMatchMethod::FoldCase) == 0; }

    // wild-card comparison
    bool WildMatch(const MString& aWildText) const noexcept;
    bool WildMatch(const char* aWildText) const noexcept;
    bool LayerMatch(const MString& aWildText) const;
    bool LayerMatch(const char* aWildText) const noexcept;

    // fuzzy comparison
    /** Maximum edit distance allowed by fuzzy matching: the maximum value for aMaxDistance in the fuzzy matching functions. */
    static constexpr int KMaxFuzzyDistance = 4;
    [[nodiscard]] static int32_t FuzzyCompare(MIter<int32_t>& aIter1,MIter<int32_t>& aIter2,int32_t aMaxDistance) noexcept;
    [[nodiscard]] static bool FuzzyMatch(MIter<int32_t>& aIter1,MIter<int32_t>& aIter2,int32_t aMaxDistance) noexcept;
    [[nodiscard]] static bool FuzzyMatch(const char* aText1,const char* aText2,int32_t aMaxDistance) noexcept;

    // comparison operators

    /** The less-than operator. */
    template<class T> bool operator<(const T& aOther) const { return Compare(aOther) < 0; }
    /** The less-than-or-equals operator. */
    template<class T> bool operator<=(const T& aOther) const { return Compare(aOther) <= 0; }
    /** The greater-than operator. */
    template<class T> bool operator>(const T& aOther) const { return Compare(aOther) > 0; }
    /** The greater-than-or-equals operator. */
    template<class T> bool operator>=(const T& aOther) const { return Compare(aOther) >= 0; }

    // conversion functions
    static int32_t ToInt32(const uint8_t* aText,size_t aLength,size_t* aLengthUsed = nullptr,int32_t aBase = 10) noexcept;
    int32_t ToInt32(size_t* aLengthUsed = nullptr,int32_t aBase = 10) const noexcept;
    int64_t ToInt64(size_t* aLengthUsed = nullptr,int32_t aBase = 10) const noexcept;
    uint32_t ToUint32(size_t* aLengthUsed = nullptr,int32_t aBase = 10) const noexcept;
    double ToDouble(size_t* aLengthUsed = nullptr) const noexcept;
    double ToDimension(size_t* aLengthUsed = nullptr) const noexcept;

    // conversion to presentation form: bidirectional reordering, mirroring and contextual shaping
    void Shape(BidiParDir aParDir,BidiEngine* aBidiEngine,bool aParStart,bool aReorderFontSelectors);

    void Abbreviate(const AbbreviationDictionary& aDictionary,bool aDeleteWords = false);
    void Transliterate(const StringTypeDictionary* aTitleCaseDictionary = nullptr,const char* aLocale = nullptr);

    // line breaking
    bool IsLineBreak(size_t aPos) const noexcept;
    size_t LineBreakBefore(size_t aPos) const noexcept;
    size_t LineBreakAfter(size_t aPos) const noexcept;

    // treating a string as a set of string attributes used by a map object
    void SetAttribute(const MString& aKey,const MString& aValue);
    void SetAttribute(const String& aKey,const String& aValue);
    Text Attribute(const MString& aKey) const noexcept;
    Text Attribute(const String& aKey) const noexcept;
    bool NextAttribute(size_t& aPos,Text& aKey,Text& aValue) const noexcept;

    protected:
    /** Constructs the MString base class part of a string and gives it zero length. */
    MString(): iLength(0) { }
    /** Constructs the MString base class part of a string and gives it a specified length. */
    MString(size_t aLength): iLength(aLength) { }

    /** The length of the text in 16-bit UTF16 values. */
    size_t iLength;

    private:
    /** Return true if the string is writable. */
    virtual bool Writable() const = 0;

    /**
    Returns the maximum writable length of the text: the length of the currently reserved buffer.
    Returns 0 if no buffer is reserved or if the text is not writable.
    */
    virtual size_t MaxWritableLength() const = 0;

    /**
    Returns a non-constant pointer to the text.
    Returns null if the text is not writable.
    */
    virtual uint16_t* WritableText() = 0;

    /**
    Adjusts the text buffer to hold up to aNewLength characters;
    the actual new size may be less. Sets iLength to the actual new size.
    */
    virtual void ResizeBuffer(size_t aNewLength) = 0;

    void ReplaceAndConvert(size_t aStart,size_t aEnd,const char* aText,size_t aLength,int32_t aCodePage);
    void FindStringAttrib(const MString& aKey,size_t& aKeyStart,size_t& aValueStart,size_t& aValueEnd) const noexcept;
    };

/** An unmodifiable string that doesn't own its text. */
class Text: public MString
    {
    public:
    /** Constructs an empty TText object. */
    Text() noexcept: iText(nullptr) { }
    /** Constructs a TText object from a null-terminated string. */
    Text(const uint16_t* aText)noexcept : iText(aText)
        {
        iLength = 0;
        if (iText)
            while (*aText++)
                iLength++;
        }
    /** Constructs a TText object from a pointer and a length. */
    Text(const uint16_t* aText,size_t aLength) noexcept : MString(aLength), iText(aText) { }
    /** Constructs a TText object from a pointer and a length. */
    Text(const char16_t* aText,size_t aLength) noexcept : MString(aLength), iText((const uint16_t*)aText) { }
    /** Constructs a TText object from an MString object. */
    Text(const MString& aString) noexcept : MString(aString.Length()), iText(aString.Data()) { }
    /** Constructs a TText object from null-terminated UTF-16 text. */
    Text(const char16_t* aText) noexcept : MString(0), iText((const uint16_t*)aText) { auto p = aText; while (*p++) iLength++; }

    // virtual functions from MString
    const uint16_t* Data() const noexcept override { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const noexcept override { return false; }
    size_t MaxWritableLength() const noexcept override { return 0; }
    uint16_t* WritableText() noexcept override { return nullptr; }
    void ResizeBuffer(size_t /*aNewLength*/) override { assert(false); }

    const uint16_t* iText;
    };

/** A writable string that doesn't own its text. */
class WritableTextView: public MString
    {
    public:
    /** Creates an empty WritableTextView object. */
    WritableTextView(): iText(nullptr), iMaxLength(0) { }
    /** Creates a WritableTextView object to access aLength 16-bit characters starting at aText. */
    WritableTextView(uint16_t* aText,size_t aLength):
        MString(aLength),
        iText(aText),
        iMaxLength(aLength)
        {
        }

    // virtual functions from MString
    const uint16_t* Data() const { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const { return true; }
    size_t MaxWritableLength() const { return iMaxLength; }
    uint16_t* WritableText() { return iText; }
    void ResizeBuffer(size_t aNewLength) { iLength = std::min(aNewLength,iMaxLength);  }

    uint16_t* iText;
    size_t iMaxLength;
    };

/** A string that owns its own text, which has a maximum length fixed at compile time. */
template<size_t aMaxLength> class TextBuffer: public MString
    {
    public:
    /** Construct an empty TextBuffer object. */
    TextBuffer() { }

    /** Construct a TextBuffer object from an 8-bit ASCII string. */
    TextBuffer(const char* aText)
        {
        iLength = 0;
        while (*aText && iLength < aMaxLength)
            iText[iLength++] = *aText++;
        }

    // virtual functions from MString
    const uint16_t* Data() const { return iText; }

    private:
    // virtual functions from MString
    bool Writable() const { return true; }
    size_t MaxWritableLength() const { return aMaxLength; }
    uint16_t* WritableText() { return iText; }
    void ResizeBuffer(size_t aNewLength) { iLength = std::min(aNewLength,aMaxLength);  }

    uint16_t iText[aMaxLength];
    };

/** A macro to define the literal Text object aName from the UTF16 string aText. Example: TextLiteral(mytext,u"hello"). */
#define TextLiteral(aName,aText) Text static const aName(aText,sizeof(aText) / sizeof(char16_t) - 1);

/** A string that owns its own text, which is freely resizable. */
class String: public MString
    {
    public:
    /** Constructs an empty String object. */
    String():
        iText(iOwnText),
        iReserved(KOwnTextLength)
        {
        }
    /** Constructs an empty String object. This constructor allows a null argument in a function expecting a String to be converted to an empty string. */
    String(std::nullptr_t):
        iText(iOwnText),
        iReserved(KOwnTextLength)
        {
        }
    String(const String& aOther);
    String(const MString& aOther);

    String(String&& aOther) noexcept;
    String& operator=(const String& aOther);
    String& operator=(const MString& aOther);
    String& operator=(String&& aOther) noexcept;
    String(const char* aText,size_t aLength = npos);
    String(const uint16_t* aText,size_t aLength = npos);
    String(const char16_t* aText,size_t aLength = npos);
    String(const std::string& aText);

    ~String()
        {
        if (iText != iOwnText)
            delete [] iText;
        }

    using MString::operator[];
    /** Gets the character at the position aIndex. */
    uint16_t& operator[](size_t aIndex) const { assert(aIndex < iLength); return iText[aIndex]; }

    // virtual functions from MString
    const uint16_t* Data() const override { return iText; }

    private:

    // virtual functions from MString
    bool Writable() const override { return true; }
    size_t MaxWritableLength() const override { return iReserved; }
    uint16_t* WritableText() override { return iText; }
    void ResizeBuffer(size_t aNewLength) override;

    static constexpr size_t KOwnTextLength = 32;
    uint16_t iOwnText[KOwnTextLength] = {  };
    uint16_t* iText = nullptr;
    size_t iReserved = 0;
    };

/** A type for immutable reference-counted strings, which are used for layer names in map objects. */
class RefCountedString: public std::shared_ptr<const String>
    {
    public:
    /** Creates a ref-counted string containing a shared pointer to an empty string. */
    RefCountedString(): std::shared_ptr<const String>(new String) { } // ensure that the string is not empty on default construction
    /** Creates a ref-counted string containing aText. */
    RefCountedString(const MString& aText): std::shared_ptr<const String>(new String(aText)) { }
    /** Creates a ref-counted string containing aText. */
    RefCountedString(const char* aText) : std::shared_ptr<const String>(new String(aText)) { }
    /** Creates a ref-counted string containing an empty shared pointer. Note that the default constructor does not do this but creates a shared pointer to an empty string. */
    RefCountedString(std::nullptr_t): std::shared_ptr<const String>() { }
    };

/** An iterator to convert UTF8 text to UTF32. */
class Utf8ToUtf32: public MIter<int32_t>
    {
    public:
    Utf8ToUtf32(const uint8_t* aText,size_t aLength = npos);
    /** Creates an iterator over some UTF8 text. If aLength is npos (the default) the text must be null-terminated. */
    Utf8ToUtf32(const char* aText,size_t aLength = npos) :
        Utf8ToUtf32(reinterpret_cast<const uint8_t*>(aText),aLength)
        {
        }
    Result Next(int32_t& aValue);
    void Back();
    /** Returns a pointer to the current position in the text. */
    const uint8_t* Pos() const { return iPos; }

    private:
    const uint8_t* iStart;
    const uint8_t* iPos;
    const uint8_t* iEnd;
    bool iEof;
    };

/** An iterator to convert UTF16 text to UTF32. */
class Utf16ToUtf32: public MIter<int32_t>
    {
    public:
    Utf16ToUtf32(const uint16_t* aText,size_t aLength);
    Result Next(int32_t& aValue);
    void Back();
    /** Returns a pointer to the current position in the text. */
    const uint16_t* Pos() const { return iPos; }

    private:
    const uint16_t* iStart;
    const uint16_t* iPos;
    const uint16_t* iEnd;
    bool iEof;
    };

/** An iterator that does no conversion but simply passes UTF32 text straight through. */
class Utf32Iter: public MIter<int32_t>
    {
    public:
    Utf32Iter(const int32_t* aText,size_t aLength);
    Result Next(int32_t& aValue);
    void Back();
    /** Returns a pointer to the current position in the text. */
    const int32_t* Pos() const { return iPos; }

    private:
    const int32_t* iStart;
    const int32_t* iPos;
    const int32_t* iEnd;
    bool iEof;
    };

/** An iterator to fold characters according to the specified string match method. */
class FoldingIterator: public MIter<int32_t>
    {
    public:
    /** Creates an iterator to fold characters using aStringMatchMethod. */
    FoldingIterator(MIter<int32_t>& aIter,StringMatchMethod aStringMatchMethod):
        iIter(aIter),
        iStringMatchMethod(aStringMatchMethod),
        iFoldAccents(aStringMatchMethod & StringMatchMethodFlag::FoldAccents),
        iFoldCase(aStringMatchMethod & StringMatchMethodFlag::FoldCase)
        {
        }
    Result Next(int32_t& aCode) override
        {
        if (iCaseVariantIndex < iCaseVariantLength)
            {
            aCode = iCaseVariant[iCaseVariantIndex++];
            return KErrorNone;
            }
        Char c;
        for (;;)
            {
            Result error = iIter.Next(c.Code);
            if (error)
                {
                assert(error == KErrorEndOfData);
                aCode = -1;
                return error;
                }
            if (!iStringMatchMethod.Ignore(c.Code))
                break;
            }
        if (iFoldAccents)
            c = c.AccentStripped();
        if (iFoldCase)
            {
            c.GetLowerCase(iCaseVariant,iCaseVariantLength);
            iCaseVariantIndex = 1;
            aCode = iCaseVariant[0];
            return KErrorNone;
            }
        aCode = c.Code;
        return KErrorNone;
        }
    void Back() override
        {
        throw KErrorUnimplemented;
        }

    private:
    MIter<int32_t>& iIter;
    StringMatchMethod iStringMatchMethod = StringMatchMethod::Exact;
    bool iFoldAccents = false;
    bool iFoldCase = false;
    int32_t iCaseVariant[Char::KMaxCaseVariantLength] = { };
    int32_t iCaseVariantLength = 0;
    int32_t iCaseVariantIndex = 0;
    };

/** An abbreviation as stored in an abbreviation dictionary. */
class Abbreviation
    {
    public:
    /** The abbreviated form of the word. */
    String ShortForm;
    /** The type, determining where in a phrase the abbreviation can be used. */
    AbbreviationType Type = AbbreviationType::Any;
    /** If non-zero, the number of characters to replace in the unabbreviated form. If zero, all characters are replaced. */
    uint16_t ReplaceCount = 0;
    };

/** An abbreviation as stored as static data in an abbreviation table. */
class AbbreviationTableEntry
    {
    public:
    /** The unabbreviated form of the word, in lower case. */
    const char* const LongForm = "";
    /** The abbreviated form of the word, in lower case. */
    const char* const ShortForm = "";
    /** The type, determining where in a phrase the abbreviation can be used. */
    AbbreviationType Type = AbbreviationType::Any;
    };

/** A collection of data controlling abbreviations and letter-case conversions. */
class AbbreviationInfo
    {
    public:
    /** The locale (e.g., "en" or "en_GB") for this data: null implies universal data. */
    const char* Locale = nullptr;
    /** An array of abbreviations: pairs of UTF-8 strings in the order long, short. */
    const AbbreviationTableEntry* AbbreviationArray;
    /** The number of strings in iAbbreviation: twice the number of abbreviations. */
    size_t AbbreviationCount = 0;
    /** An array of words to be put into lower case if the entire string is put into title case. */
    const char* const * LowerTitleCaseException = nullptr;
    /** The number of lower-case title-case exceptions. */
    size_t LowerTitleCaseExceptionCount = 0;
    /** An array of words to be put into upper case if the entire string is put into title case. */
    const char* const * UpperTitleCaseException = nullptr;
    /** The number of upper-case title-case exceptions. */
    size_t UpperTitleCaseExceptionCount = 0;
    };

/** A key-value pair; often referred to as a tag. */
class Tag
    {
    public:
    /** Creates a Tag with an empty key and value. */
    Tag() = default;
    /** Creates a Tag with a specified key and value. */
    Tag(const MString& aKey,const MString& aValue): Key(aKey), Value(aValue) { }

    /** The key. */
    String Key;
    /** The value. */
    String Value;
    };

const AbbreviationInfo* AbbreviationInfoForLocale(const char* aLocale);
/**
Returns the two-letter country code (ISO 3166-1 alpha-2) as a lower-case string, given the English-language name of the country.
The letter case of aCountry is ignored.
Returns the empty string if aCountry is not found.
*/
String CountryToCode(const MString& aCountry);
/**
Returns the English-language name of a country as a lower-case string, given its two-letter country code (ISO 3166-1 alpha-2).
The letter case of aCode is ignored.
Returns the code itself, converted to lower case, if aCode is not found.
*/
String CodeToCountry(const MString& aCode);
/** Creates a string representing a 64-bit map object ID, encoded as one to four 16-bit characters in big-endian order, from the ID. */
String IdToString(uint64_t aId);
/** Converts a string representing a 64-bit map object ID, encoded as one to four 16-bit characters in big-endian order, to the ID. */
uint64_t StringToId(const MString& aIdString);

/**
Converts UTF32 text to Utf8, returning the number of bytes needed.
If aDest is null do not write to aDest but still return the number of bytes needed.
*/
size_t Utf32ToUtf8(uint8_t* aDest,MIter<int32_t>& aIter);

} // namespace CartoTypeCore
