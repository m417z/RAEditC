#pragma once

#include <windows.h>
#include <commctrl.h>
#include "RegisterTypes.h"
#include "include/RAEdit.h"

#pragma pack(push, 1)

#define MAXLINEMEM				(8*1024)
#define MAXCHARMEM				(24*1024)
//#define MAXCHARMEM				(32*1024)
#define MAXUNDOMEM				(24*1024)
#define MAXWORDMEM				(8*1024)
//#define MAXFREE					32
#define MAXFREE					256
#define MAXSTREAM				(32*1024)

#define BTNWT					14
#define BTNHT					6
#define SELWT					26
#define LNRWT					28

#define UNDO_INSERT				1
#define UNDO_OVERWRITE			2
#define UNDO_DELETE				3
#define UNDO_BACKDELETE			4
#define UNDO_INSERTBLOCK		5
#define UNDO_DELETEBLOCK		6

typedef int CALLBACK (* BOOKMARK_PAINT_CALLBACK)(HWND hWnd, DWORD line);

struct tagTIMER {
	HWND hwnd;
	UINT umsg;
	LPARAM lparam;
	WPARAM wparam;
};
typedef struct tagTIMER TIMER;

struct tagRABRUSH {
	HBRUSH hBrBck;		// Back color brush
	HBRUSH hBrSelBck;		// Sel back color brush
	HBRUSH hBrHilite1;		// Line hilite 1
	HBRUSH hBrHilite2;		// Line hilite 2
	HBRUSH hBrHilite3;		// Line hilite 3
	HBRUSH hBrSelBar;		// Selection bar
	HBRUSH hPenSelbar;		// Selection bar pen
};
typedef struct tagRABRUSH RABRUSH;

struct tagWORDINFO {
	ULONG_PTR len;		// Length
	ULONG_PTR rpprev;		// Relative pointer to previous
	DWORD color;		// Color (high byte is font 0-3)
	DWORD fend;		// End flag
};
typedef struct tagWORDINFO WORDINFO;

struct tagRAFONTINFO {
	BYTE charset;		// Character set
	BYTE fDBCS;		// Use double byte characters
	DWORD fntwt;		// Font width
	DWORD fntht;		// Font height
	DWORD spcwt;		// Space width
	DWORD tabwt;		// Tab width
	DWORD italic;		// Height shift
	BOOL monospace;		// Font is monospaced
	DWORD linespace;		// Extra line spacing
};
typedef struct tagRAFONTINFO RAFONTINFO;

struct tagLINE {
	ULONG_PTR rpChars;		// Relative pointer to CHARS
};
typedef struct tagLINE LINE;

struct tagCHARS {
	ULONG_PTR len;		// Actual String len
	ULONG_PTR max;		// Max String len
	DWORD state;		// Line state
	DWORD bmid;		// Bookmark ID
	DWORD errid;		// Error ID
};
typedef struct tagCHARS CHARS;

struct tagRAUNDO {
	ULONG_PTR rpPrev;		// Relative pointer to previous
	DWORD undoid;		// Undo ID
	ULONG_PTR cp;		// Character position
	ULONG_PTR cb;		// Size in bytes
	BYTE fun;		// Function
};
typedef struct tagRAUNDO RAUNDO;

struct tagRAEDT {
	HWND hwnd;		// Handle of edit a or b
	HWND hvscroll;		// Handle of scroll bar
	DWORD cpxmax;		// Last cursor pos x
	DWORD cpy;		// Scroll position
	DWORD cp;		// Character position
	RECT rc;				// Edit a or b rect
	DWORD topyp;		// First visible lines Y position
	DWORD topln;		// First visible lines linenumber
	DWORD topcp;		// First visible lines character position
};
typedef struct tagRAEDT RAEDT;

struct tagEDIT {
	HWND hwnd;		// Handle of main window
	DWORD fstyle;		// Window style
	LONG_PTR ID;		// Window ID
	HWND hpar;		// Handle of parent window
	RAEDT edta;
	RAEDT edtb;
	HWND hhscroll;		// Handle of horizontal scrollbar
	HWND hgrip;		// Handle of sizegrip
	HWND hnogrip;		// Handle of nosizegrip
	HWND hsbtn;		// Handle of splitt button
	HWND hlin;		// Handle of linenumber button
	HWND hexp;		// Handle of expand button
	HWND hcol;		// Handle of collapse button
	HWND hlock;		// Handle of lock button
	HWND hsta;		// Handle of state window
	HWND htt;		// Handle of tooltip
	DWORD fresize;		// Resize in action flag
	DWORD fsplitt;		// Splitt factor
	DWORD nsplitt;		// Splitt height

	HANDLE hHeap;	// Handle of heap
	HANDLE hLine;		// Handle of line pointer mem
	ULONG_PTR cbLine;		// Size of line pointer mem
	ULONG_PTR rpLine;		// Relative pointer into line pointer mem
	ULONG_PTR rpLineFree;		// Pointer to free line pointer
	HANDLE hChars;		// Handle of character mem
	ULONG_PTR cbChars;		// Size of character mem
	ULONG_PTR rpChars;		// Relative pointer into character mem
	ULONG_PTR rpCharsFree;		// Relative pointer to free character
	HANDLE hUndo;		// Handle of undo memory
	ULONG_PTR cbUndo;		// Size of undo memory
	ULONG_PTR rpUndo;		// Relative pointer to free (last)
	DWORD line;		// Linenumber
	DWORD cpLine;		// Character position for start of line

	RECT rc;			 	// Main rect
	DWORD selbarwt;		// Width of selection bar
	DWORD nlinenrwt;		// Initial width of linenumber bar
	DWORD linenrwt;		// Width of linenumber bar
	DWORD nScroll;		// Mouse wheel scroll lines
	DWORD nPageBreak;		// Page break
	DWORD cpMin;		// Selection min
	DWORD cpMax;		// Selection max
	BOOL fOvr;		// Insert / Overwrite
	DWORD nHidden;		// Number of hidden lines
	DWORD cpx;		// Scroll position
	HANDLE focus;		// Handle of edit having focus
	BOOL fCaretHide;		// Caret is hidden
	BOOL fChanged;		// Content changed
	BOOL fHideSel;		// Hide selection
	BOOL fIndent;		// Auto indent
	RACOLOR clr;
	RABRUSH br;
	DWORD nTab;		// Tab size
	RAFONT fnt;
	RAFONTINFO fntinfo;
	BOOKMARK_PAINT_CALLBACK lpBmCB;		// Bookmark paint callback
	DWORD nchange;		// Used by EN_SELCHANGE
	DWORD nlastchange;		// Used by EN_SELCHANGE
	DWORD nWordGroup;		// Hilite word group
	BOOL fExpandTab;		// TRUE/FALSE Epand tabs to spaces
	CHARRANGE savesel;
	HWND htlt;		// Scroll tooltip
	DWORD nMode;		// Block selection
	BLOCKRANGE blrg;
	DWORD lockundoid;
	DWORD ccmntblocks;
	DWORD cpbrst; // bracket start
	DWORD cpbren; // bracket end
	DWORD cpselbar;
	BOOL fLock;
	DWORD nCursorWordType;
	DWORD fstyleex;
	BOOL funicode;
	DWORD fhilite;
	DWORD lastline;
};
typedef struct tagEDIT EDIT;

#define IDB_RAEDITBUTTON		100
#define IDC_HSPLITTCUR			101
#define IDB_BOOKMARK			102
#define IDC_SELECTCUR			103
#define IDB_LINENUMBER			104
#define IDB_EXPAND				105
#define IDB_COLLAPSE			106
#define IDB_LOCK				107

// .const

extern char *szScrollBar;
extern char *szStatic;
extern char *szButton;
extern char *szToolTips;

extern char *szToolTip;
extern char *szSplitterBar;
extern char *szLineNumber;
extern char *szExpand;
extern char *szCollapse;
extern char *szLock;
extern char *szChanged;

extern char *szX;
extern char *szW;
extern char *szI;
extern char *szSpace;
extern char *szTab;
extern char *szComment;
extern char *szInclude;
extern char *szIncludelib;
extern char *szMemFailLine;
extern char *szMemFailChar;
extern char *szMemFailUndo;
extern char *szMemFailSyntax;
extern char *szGlobalFail;
extern char *szHeapFail;

extern char *szRAEditClass;
extern char *szEditClassName;

// .data

extern BYTE CharTabInit[];
extern BYTE CharTab[];
extern BYTE CaseTab[];
extern char szLine[32];

// data?

extern HINSTANCE hInstance;
extern HBITMAP hBmpLnr;
extern HBITMAP hBmpExp;
extern HBITMAP hBmpCol;
extern HBITMAP hBmpLck;
extern HCURSOR hHSCur;
extern HCURSOR hSelCur;
extern HIMAGELIST hIml;
extern HBRUSH hBrTlt;
extern DWORD SBWT; // Scroll bar width, GetSystemMetrics(SM_CXVSCROLL)
extern WNDPROC OldStateProc;
extern WNDPROC OldSplittBtnProc;
extern WNDPROC OldFakeToolTipProc;
extern void *hWrdMem;
extern ULONG_PTR cbWrdMem;
extern ULONG_PTR rpWrdFree;
extern DWORD fSelState;
extern DWORD iYp;
extern BOOL fOnBM;
extern DWORD fOnSel;
extern DWORD nBmid; // Bookmark id
extern DWORD nUndoid;
extern BOOL fSize;
extern UINT_PTR TimerID;
extern TIMER tmr1;
extern TIMER tmr2;
extern POINT ptDrag;
extern HWND hDragWin;
extern EDIT *hDragSourceMem;
extern CHARRANGE cpDragSource;
extern DWORD peff; // Drop effect
extern DWORD MpX;
extern DWORD MpY;
extern DWORD fTlln;
extern BYTE blockdefs[4096];
extern BYTE bracketstart[16];
extern BYTE bracketend[16];
extern BYTE bracketcont[16];
extern BOOL fNoSaveUndo;

#pragma pack(pop)
