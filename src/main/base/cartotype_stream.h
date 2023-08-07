/*
cartotype_stream.h
Copyright (C) 2004-2023 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>
#include <cartotype_arithmetic.h>
#include <cartotype_errors.h>
#include <cartotype_string.h>
#include <string>
#include <list>
#include <stdio.h>

#ifdef __unix__
    #include <unistd.h> // to define _POSIX_VERSION
#endif

// Use low-level file i/o on Windows, but not Windows CE, for a small speed improvement (about 5%).
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
    #define CARTOTYPE_LOW_LEVEL_FILE_IO
#endif

#ifdef ANDROID
    #define CARTOTYPE_LOW_LEVEL_FILE_IO
#endif

#ifdef CARTOTYPE_LOW_LEVEL_FILE_IO
    #if defined(ANDROID)
        #include <unistd.h>
        #include <fcntl.h>
        #include <errno.h>
    #else
        #include <io.h>
    #endif
#endif

#undef COLLECT_STATISTICS

namespace CartoTypeCore
{

// Forward declarations.
class MString;
class String;
class MapClient;

/**
The input stream base class.
Streams that do not support random access always throw exceptions for StreamSeek
and may throw exceptions for StreamPosition and StreamLength.

Exceptions that are not caused by lack or memory are of the type Result.
Common values are KErrorEndOfData and KErrorIo.
*/
class InputStream
    {
    private:
    /**
    Reads some data into a buffer owned by the InputStream object and returns
    a pointer to it in aPointer. Returns the number of bytes of data in aLength.
    This function will return at least one byte if there are bytes remaining in the
    stream. The pointer is valid until the next call to Read.
    */
    virtual void StreamRead(const uint8_t*& aPointer,size_t& aLength) = 0;
    /** Returns whether the end of the stream has been reached. */
    virtual bool StreamEnd() const = 0;
    /** Seeks to the specified position. */
    virtual void StreamSeek(int64_t aPosition) = 0;
    /** Returns the current position. */
    virtual int64_t StreamPosition() = 0;

    public:
    /** Virtual destructor. */
    virtual ~InputStream() { }
    /** Creates a copy of the stream. */
    virtual std::unique_ptr<InputStream> Copy() = 0;
    /** Returns the number of bytes in the stream. */
    virtual int64_t StreamLength() = 0;
    /** Returns the file name or URI associated with the stream if any. Return the empty string is there is no file name or URI. */
    virtual std::string StreamName() { return std::string(); }

    void Seek(int64_t aPosition);
    /** Returns the current position as a byte offset from the start of the stream. */
    int64_t Position() const { return m_data_position + (int64_t)(m_data - m_data_start); }
    /** Returns true if this stream is at the end of the data. */
    bool EndOfData() const { return !m_data_bytes && StreamEnd(); }
    /** Reads an 8-bit unsigned integer. */
    uint8_t ReadUint8()
        {
        if (m_data_bytes >= 1)
            {
            m_data_bytes--;
            return *m_data++;
            }
        return ReadUint8Helper();
        }
    uint16_t ReadUint16();
    uint16_t ReadUint16LE();
    uint32_t ReadUint32();
    uint32_t ReadUint32LE();
    /** Reads a 40-bit unsigned integer in big-endian form. */
    uint64_t ReadUint40()
        {
        if (m_data_bytes >= 5)
            {
            m_data_bytes -= 5;
            m_data += 5;
            return uint64_t((uint64_t)m_data[-5] << 32 | (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint40Helper();
        }
    /** Reads a file position: that is, an unsigned integer stored in the number of bytes returned by FilePosBytes. */
    int64_t ReadFilePos() { return m_file_pos_bytes == 4 ? ReadUint32() : ReadUint40(); }
    /** Reads a file position combined with a degree square code: that is, an unsigned integer stored in two more bytes than FilePosBytes. */
    int64_t ReadFilePosWithDegreeSquare() { return  m_file_pos_bytes == 4 ? ReadUint48() : ReadUint56(); }
    /** Returns the number of bytes storing a file position; by default 4. The number is always 4 or 5. */
    int32_t FilePosBytes() const { return m_file_pos_bytes; }
    /** Sets the number of bytes used to store a file position: must be 4 or 5. */
    void SetFilePosBytes(int32_t aBytes)
        {
        if (aBytes != 4 && aBytes != 5)
            throw KErrorInvalidArgument;
        m_file_pos_bytes = aBytes;
        }

    uint32_t ReadUintOfSize(int32_t aSize);
    uint32_t ReadUintOfSizeLE(int32_t aSize);
    uint64_t ReadUint();
    int64_t ReadInt();
    uint32_t ReadUintMax32();
    int32_t ReadIntMax32();
    float ReadFloat();
    float ReadFloatLE();
    double ReadDouble();
    double ReadDoubleLE();
    void ReadLine(uint8_t* aBuffer,size_t aMaxBytes,size_t& aActualBytes);
    void ReadBytes(uint8_t* aBuffer,size_t aMaxBytes,size_t& aActualBytes);
    void Read(const uint8_t*& aPointer,size_t& aLength);
    String ReadNullTerminatedString();
    String ReadUtf8StringWithLength();
    std::string ReadUtf8StringWithLengthToStdString();
    std::string ReadUtf8StringToStdString();
    String ReadString(size_t* aBytesRead = nullptr);
    void SkipUtf8String(size_t* aBytesRead = nullptr);
    void Skip(int64_t aBytes);

    /**
    Reads the next aBytes bytes, returning a pointer to them, or return nullptr if
    fewer than that number of bytes is cached.
    */
    const uint8_t* Read(size_t aBytes)
        {
        if (m_data_bytes >= aBytes)
            {
            m_data += aBytes;
            m_data_bytes -= aBytes;
            return m_data - aBytes;
            }
        return nullptr;
        }

    /** Reads an unsigned big-endian 48-bit number. */
    uint64_t ReadUint48()
        {
        if (m_data_bytes >= 6)
            {
            m_data_bytes -= 6;
            m_data += 6;
            return uint64_t((uint64_t)m_data[-6] << 40 | (uint64_t)m_data[-5] << 32 | (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint48Helper();
        }
    /** Reads an unsigned big-endian 56-bit number. */
    uint64_t ReadUint56()
        {
        if (m_data_bytes >= 7)
            {
            m_data_bytes -= 7;
            m_data += 7;
            return uint64_t((uint64_t)m_data[-7] << 48 | (uint64_t)m_data[-6] << 40 | (uint64_t)m_data[-5] << 32 |
                            (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint56Helper();
        }

    private:
    uint8_t ReadUint8Helper();
    uint64_t ReadUint40Helper();
    uint64_t ReadUint48Helper();
    uint64_t ReadUint56Helper();
    void ReadAdditionalBytes(size_t aBytesRequired);
    void ReadData()
        {
        m_data_position = StreamPosition();
        StreamRead(m_data_start,m_data_bytes);
        m_data = m_data_start;
        }

    /**
    Internal buffer for reading ints and floats, and for holding incomplete UTF
    sequences
    */
    uint8_t m_buffer[8] = { };
    /** The current data pointer. */
    const uint8_t* m_data = nullptr;
    /** The number of data bytes remaining. */
    size_t m_data_bytes = 0;
    /** The position of m_data_start within the whole of the data. */
    int64_t m_data_position = 0;
    /** Start of data returned by the last call to StreamRead. */
    const uint8_t* m_data_start = nullptr;
    /** Number of bytes in which a file position is stored. */
    int32_t m_file_pos_bytes = 4;
    };

/**
The output stream base class. The byte order for integers, floats and doubles is big-endian unless functions suffixed LE are used.
Implementations must define a single function, Write.
*/
class OutputStream
    {
    public:
    /** Virtual destructor. */
    virtual ~OutputStream() { }
    /** Writes aBytes bytes from aBuffer to the stream. */
    virtual void Write(const uint8_t* aBuffer,size_t aBytes) = 0;

    /** Writes a null-terminated string to the stream. Does not write the final null. */
    void WriteString(const char* aString) { Write((const uint8_t*)aString,strlen(aString)); }
    /** Writes a std::string to the stream. */
    void WriteString(const std::string& aString) { Write((const uint8_t*)aString.c_str(),aString.size()); }
    void WriteString(const MString& aString);
    void WriteXmlText(const MString& aString);
    void WriteUint8(uint8_t aValue);
    void WriteUint16(uint16_t aValue);
    void WriteUint16LE(uint16_t aValue);
    void WriteUint32(uint32_t aValue);
    void WriteUint32LE(uint32_t aValue);
    void WriteUint(uint32_t aValue,int32_t aSize);
    void WriteUintLE(uint32_t aValue,int32_t aSize);
    void WriteUint(uint64_t aValue);
    void WriteInt(int64_t aValue);
    void WriteFloat(float aValue);
    void WriteFloatLE(float aValue);
    void WriteDouble(double aValue);
    void WriteDoubleLE(double aValue);
    void WriteUtf8StringWithLength(const MString& aString);
    void WriteUtf8StringWithLength(const std::string& aString);
    void WriteNullTerminatedString(const MString& aString);
    };

/** An input stream for a contiguous piece of memory. */
class MemoryInputStream: public InputStream
    {
    public:
    /** Creates a memory input stream to read from data of aLength bytes starting at aData. */
    MemoryInputStream(const uint8_t* aData,size_t aLength):
        iData(aData),
        iLength(aLength)
        {
        }

    /** Creates a memory input stream to read from a std::string. */
    explicit MemoryInputStream(const std::string& aString):
        iData((const uint8_t*)aString.data()),
        iLength(aString.length())
        {
        }

    // from InputStream
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override { return iPosition >= iLength; }
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override
        {
        return iPosition;
        }
    int64_t StreamLength() override
        {
        return iLength;
        }
    std::unique_ptr<InputStream> Copy() override;

    private:
    const uint8_t* iData = nullptr;
    int64_t iLength = 0;
    int64_t iPosition = 0;
    };

/** A file input class for reading binary data from file which may be greater than 4Gb in size. */
#ifdef CARTOTYPE_LOW_LEVEL_FILE_IO
class BinaryInputFile
    {
    public:
    BinaryInputFile():
        iFile(-1)
        {
        }

    /** Opens a file. */
    Result Open(const char* aFileName);

    /** Opens standard input. */
    void OpenStandardInput()
        {
        iFile = 0;
        }

    ~BinaryInputFile()
        {
        if (iFile != -1)
#if (defined(_MSC_VER))
            _close(iFile);
#else
            close(iFile);
#endif
        }

    /** Seeks to a byte position aOffset in the file; aOrigin is the same as for fseek(). */
    Result Seek(int64_t aOffset,int aOrigin)
        {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        int64_t pos = _lseeki64(iFile,aOffset,aOrigin);
#elif defined(__APPLE__)
        int64_t pos = lseek(iFile,aOffset,aOrigin);
#elif (defined(_POSIX_VERSION) || defined(__MINGW32__))
        int64_t pos = lseek64(iFile,aOffset,aOrigin);
#else
        int64_t pos = -1;
        if (aOffset >= INT32_MIN && aOffset <= INT32_MAX)
            pos = lseek(iFile,long(aOffset),aOrigin);
#endif
        return pos > -1 ? KErrorNone : KErrorIo;
        }

    /** Returns the current byte position in the file. */
    int64_t Tell() const
        {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        return _telli64(iFile);
#elif defined (__APPLE__)
        return lseek(iFile,0,SEEK_CUR);
#elif (defined(_POSIX_VERSION) || defined(__MINGW32__))
        return lseek64(iFile,0,SEEK_CUR);
#else
        return lseek(iFile,0,SEEK_CUR);
#endif
        }

    /** Reads up to aBufferSize bytes into aBuffer and returns the number of bytes actually read. */
    size_t Read(uint8_t* aBuffer,size_t aBufferSize)
        {
#if (defined(_MSC_VER))
        return _read(iFile,aBuffer,(unsigned int)aBufferSize);
#else
        return read(iFile,aBuffer,aBufferSize);
#endif
        }

    BinaryInputFile(const BinaryInputFile&) = delete;
    BinaryInputFile(BinaryInputFile&&) = delete;
    void operator=(const BinaryInputFile&) = delete;
    void operator=(BinaryInputFile&&) = delete;

    private:
    int iFile;
    };
#else
class BinaryInputFile
    {
    public:
    BinaryInputFile() = default;

    /** Opens a file. */
    Result Open(const char* aFileName);

    /** Opens standard input. */
    void OpenStandardInput()
        {
        iFile = stdin;
        }

    ~BinaryInputFile()
        {
        if (iFile)
            fclose(iFile);
        }

    /** Seeks to a byte position aOffset in the file; aOrigin is the same as for fseek(). */
    Result Seek(int64_t aOffset,int aOrigin)
        {
        int e;
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        e = _fseeki64(iFile,aOffset,aOrigin);
#elif defined(__APPLE__)
        e = fseeko(iFile,aOffset,aOrigin);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
        e = fseeko64(iFile,aOffset,aOrigin);
#else
        if (aOffset < INT32_MIN)
            return KErrorIo;
        else if (aOffset > INT32_MAX)
            return KErrorIo;
        e = fseek(iFile,long(aOffset),aOrigin);
#endif
        return e ? KErrorIo : KErrorNone;
        }

    /** Returns the current byte position in the file. */
    int64_t Tell() const
        {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        return _ftelli64(iFile);
#elif defined (__APPLE__)
        return ftello(iFile);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
        return ftello64(iFile);
#else
        return ftell(iFile);
#endif
        }

    /** Reads up to aBufferSize bytes into aBuffer and returns the number of bytes actually read. */
    size_t Read(uint8_t* aBuffer,size_t aBufferSize)
        {
        return fread(aBuffer,1,aBufferSize,iFile);
        }

    BinaryInputFile(const BinaryInputFile&) = delete;
    BinaryInputFile(BinaryInputFile&&) = delete;
    void operator=(const BinaryInputFile&) = delete;
    void operator=(BinaryInputFile&&) = delete;

    private:
    FILE* iFile = nullptr;
    };
#endif

/** The base class for file input stream classes. */
class FileInputStreamBase: public InputStream
    {
    public:
    // from InputStream
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override;
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override
        {
        return iLogicalPosition;
        }
    int64_t StreamLength() override
        {
        return iLength;
        }
    std::string StreamName() override { return iName; }

    /** The default size of each buffer in bytes. */
    static constexpr size_t KDefaultBufferSize = 64 * 1024;

    /** The default maximum number of buffers. */
    static constexpr size_t KDefaultMaxBuffers = 32;

#ifdef COLLECT_STATISTICS
    void ResetStatistics()
        {
        iSeekCount = 0;
        iReadCount = 0;
        }
    int32_t SeekCount() const
        { return iSeekCount; }
    int32_t ReadCount() const
        { return iReadCount; }
#endif

    FileInputStreamBase(const FileInputStreamBase&) = delete;
    FileInputStreamBase(FileInputStreamBase&&) = delete;
    void operator=(const FileInputStreamBase&) = delete;
    void operator=(FileInputStreamBase&&) = delete;

    protected:
    FileInputStreamBase() = default;
    void Construct(size_t aBufferSize,size_t aMaxBuffers);

    /** A buffer storing some data from the file. */
    class CBuffer
        {
        public:
        /** The byte offset in the file of the data in this buffer. */
        int64_t iPosition = -1;
        /** The number of bytes stored in this buffer. */
        size_t iSize = 0;
        /** The data stored in this buffer. */
        std::vector<uint8_t> iData;
        };

    /** Override this function to read a buffer at a certain position in the file. */
    virtual void ReadBuffer(CBuffer& aBuffer,int64_t aPos) = 0;

    /** A type for the data cache. */
    using CBufferList = std::list<CBuffer>;
    /** Cached data from the file. */
    CBufferList iBuffers;
    /** The size of a buffer in bytes. */
    size_t iBufferSize = KDefaultBufferSize;
    /** The position in the file from the user's point of view. */
    int64_t iLogicalPosition = 0;
    /** the length of the file in bytes. */
    int64_t iLength = 0;
    /** The name of the file. */
    std::string iName;
#ifdef COLLECT_STATISTICS
    int32_t iSeekCount = 0;
    int32_t iReadCount = 0;
#endif
    };


/** An input stream for reading from files. */
class FileInputStream: public FileInputStreamBase
    {
    public:
    /** Creates a InputStream to read from the file aFileName. Returns the result in aError. */
    static std::unique_ptr<FileInputStream> New(Result& aError,const std::string& aFileName,size_t aBufferSize = KDefaultBufferSize,size_t aMaxBuffers = KDefaultMaxBuffers);
    /** Creates a InputStream to read from the file aFileName. Throws an exception if the file cannot be opened. */
    FileInputStream(const std::string& aFileName,size_t aBufferSize = KDefaultBufferSize,size_t aMaxBuffers = KDefaultMaxBuffers);
    /** Returns a copy of this InputStream. */
    std::unique_ptr<InputStream> Copy() override;

    /** The default size of each buffer in bytes. */
    static constexpr size_t KDefaultBufferSize = 64 * 1024;

    /** The default maximum number of buffers. */
    static constexpr size_t KDefaultMaxBuffers = 32;

    protected:
    void ReadBuffer(CBuffer& aBuffer,int64_t aPos) override;

    /** The file. */
    BinaryInputFile iFile;
    /** The physical position in the file. */
    int64_t iPositionInFile = 0;
    };

/**
A simple file input stream that does not use seek when reading sequentially.
If the first part of the filename, before any extensions, is '-', it reads from standard input.
*/
class SimpleFileInputStream: public InputStream
    {
    public:
    /** Creates a SimpleFileInputStream to read from the file aFileName. Returns the result in aError. */
    static std::unique_ptr<SimpleFileInputStream> New(Result& aError,const std::string& aFileName,size_t aBufferSize = 64 * 1024);
    /** Creates a SimpleFileInputStream to read from the file aFileName. Throws an exception if the file cannot be opened. */
    SimpleFileInputStream(const std::string& aFileName,size_t aBufferSize = 64 * 1024);

    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override;
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override;
    int64_t StreamLength() override;
    std::string StreamName() override { return iName; }
    std::unique_ptr<InputStream> Copy() override;

    private:
    BinaryInputFile iFile;
    std::vector<uint8_t> iBuffer;
    std::string iName;
    int64_t iLength = -1;
    bool iStandardInput = false;
    bool iEndOfStream = false;
    };

/**
An output stream to write to a file that is already open for writing.
The destructor does not close the file.
*/
class OpenFileOutputStream: public OutputStream
    {
    public:
    /**
    Creates a file output stream from a file descriptor (the value returned by fopen).
    The file must already have been opened for writing.
    */
    OpenFileOutputStream(void* aFile): iFD(aFile) { }
    void Write(const uint8_t* aBuffer,size_t aBytes) override;
    /** Returns the current position in the file as a byte offset relative to the start of the file. */
    int64_t Position();

    OpenFileOutputStream(const OpenFileOutputStream&) = delete;
    OpenFileOutputStream(OpenFileOutputStream&&) = delete;
    void operator=(const OpenFileOutputStream&) = delete;
    void operator=(OpenFileOutputStream&&) = delete;

    protected:
    OpenFileOutputStream(): iFD(nullptr) { }
    /** The file pointer. The actual type is FILE*. */
    void* iFD;
    };

/**
An output stream to write to a file. The New function opens the file and
the destructor closes it.
*/
class FileOutputStream: public OpenFileOutputStream
    {
    public:
    /** Creates a FileOutputStream to write to the file aFileName. Returns the result in aError. */
    static std::unique_ptr<FileOutputStream> New(Result& aError,const std::string& aFileName);
    /** Creates a FileOutputStream to write to the file aFileName. Throws an exception if the file cannot be opened. */
    FileOutputStream(const std::string& aFileName);
    ~FileOutputStream();
    };

/**
Output stream for a buffer in memory. The caller specifies the initial size of the buffer,
which is automatically enlarged when necessary.
*/
class MemoryOutputStream: public OutputStream
    {
    public:
    /** Creates a MemoryOutputStream object to write to a buffer owned by it, optionally specifying an initial buffer size in bytes. */
    MemoryOutputStream(size_t aInitialBufferSize = 0) { iBuffer.reserve(aInitialBufferSize); }
    void Write(const uint8_t* aBuffer,size_t aBytes) override;

    /** Return a pointer to the memory buffer. */
    const uint8_t* Data() const { return iBuffer.data(); }
    /** Take ownership of the data. */
    std::vector<uint8_t> RemoveData() { std::vector<uint8_t> a; std::swap(a,iBuffer); return a; }
    /** Return the number of bytes written. */
    size_t Length() const { return iBuffer.size(); }

    private:
    std::vector<uint8_t> iBuffer;
    };

/**
An fseek-compatible function for moving to a position in a file, specifying
it using a 64-bit signed integer.
*/
inline int FileSeek(FILE* aFile,int64_t aOffset,int aOrigin)
    {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
    return _fseeki64(aFile,aOffset,aOrigin);
#elif defined(__APPLE__)
    return fseeko(aFile,aOffset,aOrigin);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
    return fseeko64(aFile,aOffset,aOrigin);
#else
    if (aOffset < INT32_MIN)
        return -1;
    else if (aOffset > INT32_MAX)
        return -1;
    return fseek(aFile,long(aOffset),aOrigin);
#endif
    }

/**
An ftell-compatible function for getting the current position in a file,
returning a 64-bit signed integer.
*/
inline int64_t FileTell(FILE* aFile)
    {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
    return _ftelli64(aFile);
#elif defined (__APPLE__)
    return ftello(aFile);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
    return ftello64(aFile);
#else
    return ftell(aFile);
#endif
    }

} // namespace CartoTypeCore

