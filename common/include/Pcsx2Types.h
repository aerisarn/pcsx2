/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PCSX2TYPES_H__
#define __PCSX2TYPES_H__

// --------------------------------------------------------------------------------------
//  Forward declarations
// --------------------------------------------------------------------------------------
// Forward declarations for wxWidgets-supporting features.
// If you aren't linking against wxWidgets libraries, then functions that
// depend on these types will not be usable (they will yield linker errors).

#ifdef __cplusplus
class wxString;
class FastFormatAscii;
class FastFormatUnicode;
#endif


// --------------------------------------------------------------------------------------
//  Basic Atomic Types
// --------------------------------------------------------------------------------------

#include <stdint.h>

// Note: char does not have a default sign, unlike other types. As we actually want
// char and not signed char in pcsx2, we define s8 to char

typedef char s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uintptr_t uptr;
typedef intptr_t sptr;

typedef unsigned int uint;

// --------------------------------------------------------------------------------------
//  u128 / s128 - A rough-and-ready cross platform 128-bit datatype, Non-SSE style.
// --------------------------------------------------------------------------------------
// Note: These structs don't provide any additional constructors because C++ doesn't allow
// the use of datatypes with constructors in unions (and since unions aren't the primary
// uses of these types, that means we can't have constructors). Embedded functions for
// performing explicit conversion from 64 and 32 bit values are provided instead.
//
#ifdef __cplusplus
union u128
{
    struct
    {
        u64 lo;
        u64 hi;
    };

    u64 _u64[2];
    u32 _u32[4];
    u16 _u16[8];
    u8 _u8[16];

    // Explicit conversion from u64. Zero-extends the source through 128 bits.
    static u128 From64(u64 src)
    {
        u128 retval;
        retval.lo = src;
        retval.hi = 0;
        return retval;
    }

    // Explicit conversion from u32. Zero-extends the source through 128 bits.
    static u128 From32(u32 src)
    {
        u128 retval;
        retval._u32[0] = src;
        retval._u32[1] = 0;
        retval.hi = 0;
        return retval;
    }

    operator u32() const { return _u32[0]; }
    operator u16() const { return _u16[0]; }
    operator u8() const { return _u8[0]; }

    bool operator==(const u128 &right) const
    {
        return (lo == right.lo) && (hi == right.hi);
    }

    bool operator!=(const u128 &right) const
    {
        return (lo != right.lo) || (hi != right.hi);
    }

    // In order for the following ToString() and WriteTo methods to be available, you must
    // be linking to both wxWidgets and the pxWidgets extension library.  If you are not
    // using them, then you will need to provide your own implementations of these methods.
    wxString ToString() const;
    wxString ToString64() const;
    wxString ToString8() const;

    void WriteTo(FastFormatAscii &dest) const;
    void WriteTo8(FastFormatAscii &dest) const;
    void WriteTo64(FastFormatAscii &dest) const;
};

struct s128
{
    s64 lo;
    s64 hi;

    // explicit conversion from s64, with sign extension.
    static s128 From64(s64 src)
    {
        s128 retval = {src, (src < 0) ? -1 : 0};
        return retval;
    }

    // explicit conversion from s32, with sign extension.
    static s128 From64(s32 src)
    {
        s128 retval = {src, (src < 0) ? -1 : 0};
        return retval;
    }

    operator u32() const { return (s32)lo; }
    operator u16() const { return (s16)lo; }
    operator u8() const { return (s8)lo; }

    bool operator==(const s128 &right) const
    {
        return (lo == right.lo) && (hi == right.hi);
    }

    bool operator!=(const s128 &right) const
    {
        return (lo != right.lo) || (hi != right.hi);
    }
};

#else

typedef union _u128_t
{
    struct
    {
        u64 lo;
        u64 hi;
    };

    u64 _u64[2];
    u32 _u32[4];
    u16 _u16[8];
    u8 _u8[16];
} u128;

typedef union _s128_t
{
    u64 lo;
    s64 hi;
} s128;

#endif

// On linux sizes of long depends on the architecture (4B/x86 vs 86/amd64)
// Windows compiler requires int/long type for _InterlockedExchange* function.
// The best would be to port all _InterlockedExchange function to use
// Theading::Atomic* function. Unfortunately Win version is not happy, until
// code is properly fixed let's use a basic type alias.
#ifdef _WIN32
typedef long vol_t;
#else
typedef s32 vol_t;
#endif

//funny thing use wxChar that should wrap UNICODE, and then define
//operator overloads for both char and wchar_t
typedef wchar_t wxChar;

class wxCharBuffer {
public:
	wxCharBuffer(const wxChar*);
	//utilities sometimes use char directly eve if wchar
	//const wxChar* data() const;
	const char* data() const;
	operator wxChar*() const;
	operator char*() const;
};

//OO wrapper for c89 dynamic strings, probably to be moved
//#include <dstr.h>
class wxString {
public:
	wxString();
	wxString(const wxChar, size_t size);
	wxString(const wxString&);
	wxString(const wxChar*);
	//mah, this should just be unwrapped
	wxString(const char*);

	const wxChar* data();

	const wxChar* ToUTF8() const;
	wxChar* ToUTF8();
	const wchar_t* wc_str() const;
	const wxChar *wx_str() const;

	//wxString& operator+(const wxString& b);
	//wxString& operator+(const wxChar* b);
	//wxString operator+(const wxChar* b) const;

	inline const wchar_t* AsWChar() const;
	operator const wchar_t*() const { return AsWChar(); }

	inline const char* AsChar() const;
	const unsigned char* AsUnsignedChar() const
	{
		return (const unsigned char *)AsChar();
	}
	operator const char*() const { return AsChar(); }
	operator const unsigned char*() const { return AsUnsignedChar(); }

	operator const void*() const { return AsChar(); }

	wxString& operator=(const wxString& b);
	wxString& operator=(const wxChar* b);

	operator wxChar*() const;

	static wxString Format(...);

	static wxString FromAscii(const char *ascii, size_t len);
	static wxString FromAscii(const char *ascii);
	static wxString FromAscii(char ascii);

	size_t Replace(const wxString& strOld,
		const wxString& strNew,
		bool bReplaceAll = true);

	const wxCharBuffer ToAscii() const;

	wxString& MakeUpper();
};

wxString &operator+=(wxString &str1, const FastFormatUnicode &str2);
wxString operator+(const wxString &str1, const FastFormatUnicode &str2);
wxString operator+(const wxChar *str1, const FastFormatUnicode &str2);
wxString operator+(const FastFormatUnicode &str1, const wxString &str2);
wxString operator+(const FastFormatUnicode &str1, const wxChar *str2);
wxString operator+(const wxString &str1, const wxChar *str2);

inline bool operator==(const wxString& s1, const wxString& s2);

#define wxEmptyString L""

#define wxT



class wxArrayString {

};

enum wxStringTokenizerMode {
	wxTOKEN_RET_EMPTY_ALL
};

class wxStringTokenizer {

};

// The macros and functions in this file are designed to work whether or not
// _UNICODE is defined.  The following are helper macros that give certain
// strings the right character width.
#define WINUNIT_WIDEN2(x) L ## x
#define WINUNIT_WIDEN(x) WINUNIT_WIDEN2(x)

#ifndef __WFILE__
#define __WFILE__ WINUNIT_WIDEN(__FILE__)
#else
#pragma message("WinUnit.h: __WFILE__ already defined")
#endif

#ifdef _UNICODE
#define TSTRING(x) WINUNIT_WIDEN(#x)
#define __TFILE__ __WFILE__
#define __TFUNCTION__ __WFUNCTION__
#else
#define TSTRING(x) #x
#define __TFILE__ __FILE__
#define __TFUNCTION__ __FUNCTION__
#endif

#ifdef _UNICODE

#define _T(str) L##str

#else

#define _T(str) str

#endif

// ----------------------------------------------------------------------------
// wxDECLARE class macros
// ----------------------------------------------------------------------------

#define wxDECLARE_NO_COPY_CLASS(classname)      \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&)

#define wxDECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg)  \
    private:                                              \
        classname(const classname<arg>&);                 \
        classname& operator=(const classname<arg>&)

#define wxDECLARE_NO_COPY_TEMPLATE_CLASS_2(classname, arg1, arg2) \
    private:                                                      \
        classname(const classname<arg1, arg2>&);                  \
        classname& operator=(const classname<arg1, arg2>&)

#define wxDECLARE_NO_ASSIGN_CLASS(classname)    \
    private:                                    \
        classname& operator=(const classname&)

/* deprecated variants _not_ requiring a semicolon after them */
#define DECLARE_NO_COPY_CLASS(classname) \
    wxDECLARE_NO_COPY_CLASS(classname);
#define DECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg) \
    wxDECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg);
#define DECLARE_NO_ASSIGN_CLASS(classname) \
    wxDECLARE_NO_ASSIGN_CLASS(classname);


#define wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(name)             \
    wxDECLARE_NO_ASSIGN_CLASS(name);                        \

#define wxDECLARE_DYNAMIC_CLASS_NO_COPY(name)               \
    wxDECLARE_NO_COPY_CLASS(name);                          \


#define DECLARE_DYNAMIC_CLASS_NO_COPY DECLARE_NO_COPY_CLASS

//THREADING
class wxThread {
public:
	static bool IsMain();
};

//EVENTS

typedef int wxEventType;

class wxEvent {};
class wxDC {};

template <typename T>
class wxEventTypeTag
{
public:
	// The class of wxEvent-derived class carried by the events of this type.
	typedef T EventClass;

	wxEventTypeTag(wxEventType type) { m_type = type; }

	// Return a wxEventType reference for the initialization of the static
	// event tables. See wxEventTableEntry::m_eventType for a more thorough
	// explanation.
	operator const wxEventType&() const { return m_type; }

private:
	wxEventType m_type;
};

wxEventType wxNewEventType();

#define wxDECLARE_EVENT( name, type ) \
        extern const wxEventTypeTag< type > name

#define wxDEFINE_EVENT_ALIAS( name, type ) \
        const wxEventTypeTag< type > name( wxNewEventType() )

#define wxDEFINE_EVENT( name, type ) \
        const wxEventType name( wxNewEventType() )

class wxCommandEvent : public wxEvent {}; //public wxEventBasicPayloadMixin
	

//GUI

#define wxRESIZE_BORDER         0x0040  // == wxCLOSE
#define wxCAPTION               0x20000000
#define wxCLOSE_BOX             0x1000  // == wxHELP so can't be used with it

#define wxSYSTEM_MENU           0x0800
#define wxMINIMIZE_BOX          0x0400
#define wxMAXIMIZE_BOX          0x0200

enum { wxDefaultCoord = -1 };

enum wxDirection
{
	wxALL
};

class wxBoxSizer {};
class wxPoint {
public:
	wxPoint(int x, int y);
};

class wxSize { 
public:
	int x; int y; 

	wxSize();
	wxSize(int x, int y);
	void SetWidth(int width);
	void SetHeight(int height);
};

struct wxCursor {};

class wxRect {};

class wxControl {};

enum wxOrientation
{
	/* don't change the values of these elements, they are used elsewhere */
	wxHORIZONTAL = 0x0004,
	wxVERTICAL = 0x0008,

	wxBOTH = wxVERTICAL | wxHORIZONTAL,

	/*  a mask to extract orientation from the combination of flags */
	wxORIENTATION_MASK = wxBOTH
};

enum wxAlignment {
	wxALIGN_CENTRE_HORIZONTAL
};

class wxFont {};

class wxWindow {
public:
//	wxSize GetTextExtent(const wxString& string) const;
	virtual void GetTextExtent(const wxString& string,
		int *x, int *y,
		int *descent = NULL,
		int *externalLeading = NULL,
		const wxFont *font = NULL) const;

	virtual void SetMinSize(const wxSize& minSize);
};

wxWindow* wxFindWindowByName(const wxString& name, wxWindow *parent = NULL);

class wxSpinCtrl : public wxWindow {};

class wxDialog {};

class wxPanel {};

enum wxSIZE {
	wxSIZE_AUTO
};

/* don't use any elements of this enum in the new code */
enum wxDeprecatedGUIConstants
{
	/*  Text font families */
	wxDEFAULT = 70,
	wxDECORATIVE,
	wxROMAN,
	wxSCRIPT,
	wxSWISS,
	wxMODERN,
	wxTELETYPE,  /* @@@@ */

	/*  Proportional or Fixed width fonts (not yet used) */
	wxVARIABLE = 80,
	wxFIXED,

	wxNORMAL = 90,
	wxLIGHT,
	wxBOLD,
	/*  Also wxNORMAL for normal (non-italic text) */
	wxITALIC,
	wxSLANT,

	/*  Pen styles */
	wxSOLID = 100,
	wxDOT,
	wxLONG_DASH,
	wxSHORT_DASH,
	wxDOT_DASH,
	wxUSER_DASH,

	wxTRANSPARENT,

	/*  Brush & Pen Stippling. Note that a stippled pen cannot be dashed!! */
	/*  Note also that stippling a Pen IS meaningful, because a Line is */
	wxSTIPPLE_MASK_OPAQUE, /* mask is used for blitting monochrome using text fore and back ground colors */
	wxSTIPPLE_MASK,        /* mask is used for masking areas in the stipple bitmap (TO DO) */
						/*  drawn with a Pen, and without any Brush -- and it can be stippled. */
	wxSTIPPLE = 110
};

// font styles
enum wxFontStyle
{
	wxFONTSTYLE_NORMAL = wxNORMAL,
	wxFONTSTYLE_ITALIC = wxITALIC,
	wxFONTSTYLE_SLANT = wxSLANT,
	wxFONTSTYLE_MAX
};

// font weights
enum wxFontWeight
{
	wxFONTWEIGHT_NORMAL = wxNORMAL,
	wxFONTWEIGHT_LIGHT = wxLIGHT,
	wxFONTWEIGHT_BOLD = wxBOLD,
	wxFONTWEIGHT_MAX
};


class wxStaticText {};
class wxClientDC {};
class wxPaintEvent {};
class wxSizerFlags {
public:
	wxSizerFlags(int proportion = 0);

	wxSizerFlags& Expand();
	wxSizerFlags& Align(int alignment);
	wxSizerFlags& Proportion(int proportion);
	wxSizerFlags& Border(int direction, int borderInPixels);

};
class wxSizer {
public:
	template <typename WinType>
	void Add(const WinType* target, const wxSizerFlags &src);
};

#define wxTE_RIGHT          0x0200             // 0x0200
#define wxID_ANY			-1
class wxTextCtrl : public wxWindow {
public:
	wxTextCtrl();
	wxTextCtrl(wxWindow*, int); //    wxTextCtrl *ctrl = new wxTextCtrl(parent, wxID_ANY);
	
	void SetWindowStyleFlag(int);
};

//template <typename WinType>
//struct pxWindowAndFlags
//{
//	WinType *window;
//	wxSizerFlags flags;
//};
//
//template <typename T>
//inline void operator+=(wxSizer &target, const pxWindowAndFlags<T> &src);

enum wxPathNormalize
{
	wxPATH_NORM_ALL
};

//FS

class wxFileName 
{
public:

	wxFileName();
	wxFileName(const wxFileName& filepath);

	// from a full filename: if it terminates with a '/', a directory path
	// is contructed (the name will be empty), otherwise a file name and
	// extension are extracted from it
	wxFileName(const wxString& fullpath);

	// from a directory name and a file name
	wxFileName(const wxString& path,
		const wxString& name);

	// from a volume, directory name, file base name and extension
	wxFileName(const wxString& volume,
		const wxString& path,
		const wxString& name,
		const wxString& ext);

	// from a directory name, file base name and extension
	wxFileName(const wxString& path,
		const wxString& name,
		const wxString& ext);

	wxString GetPath() const;

	void Assign(const wxFileName& filepath);

	void Assign(const wxString& fullpath);

	void Assign(const wxString& volume,
		const wxString& path,
		const wxString& name,
		const wxString& ext,
		bool hasExt);

	void Assign(const wxString& volume,
		const wxString& path,
		const wxString& name,
		const wxString& ext);
	//{
	//	Assign(volume, path, name, ext, !ext.empty());
	//}

	void Assign(const wxString& path,
		const wxString& name);

	void Assign(const wxString& path,
		const wxString& name,
		const wxString& ext);

	// reset all components to default, uninitialized state
	void Clear();

	bool IsDirWritable() const;
	static bool IsDirWritable(const wxString &path);

	bool IsDirReadable() const;
	static bool IsDirReadable(const wxString &path);

	// NOTE: IsDirExecutable() is not present because the meaning of "executable"
	//       directory is very platform-dependent and also not so useful

	bool IsFileWritable() const;
	static bool IsFileWritable(const wxString &path);

	bool IsFileReadable() const;
	static bool IsFileReadable(const wxString &path);

	bool IsFileExecutable() const;
	static bool IsFileExecutable(const wxString &path);

	// does the file with this name exist?
	bool FileExists() const;
	static bool FileExists(const wxString &file);

	// does the directory with this name exist?
	bool DirExists() const;
	static bool DirExists(const wxString &dir);

	// does anything at all with this name (i.e. file, directory or some
	// other file system object such as a device, socket, ...) exist?
	bool Exists() const;
	static bool Exists(const wxString& path);

	bool IsOk() const;
	bool IsRelative() const;
	bool IsAbsolute() const;

	bool SameAs(const wxFileName &filepath) const;

	size_t GetDirCount() const;
	void RemoveLastDir();

	wxString GetFullPath();
	wxString GetVolume() const;

	bool MakeRelativeTo(const wxString& pathBase = wxEmptyString);

	bool HasVolume() const;

	// split a path into volume and pure path part
	static void SplitVolume(const wxString& fullpathWithVolume,
		wxString *volume,
		wxString *path);

	void RemoveDir(size_t pos);

	// change the current working directory
	bool SetCwd() const;
	static bool SetCwd(const wxString &cwd);

	void AssignCwd(const wxString& volume = wxEmptyString);

	wxFileName& operator=(const wxFileName& filename);

	wxFileName& operator=(const wxString& filename);
};

//CONFIG

class wxConfigBase {};


#endif


