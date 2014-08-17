#pragma once

#include "RegisterTypes.h"
#include "include/RAEdit.h"

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

struct tagTIMER {
	DWORD hwnd;
	DWORD umsg;
	DWORD lparam;
	DWORD wparam;
};
typedef struct tagTIMER TIMER;

struct tagRABRUSH {
	DWORD hBrBck;		// Back color brush
	DWORD hBrSelBck;		// Sel back color brush
	DWORD hBrHilite1;		// Line hilite 1
	DWORD hBrHilite2;		// Line hilite 2
	DWORD hBrHilite3;		// Line hilite 3
	DWORD hBrSelBar;		// Selection bar
	DWORD hPenSelbar;		// Selection bar pen
};
typedef struct tagRABRUSH RABRUSH;

struct tagWORDINFO {
	DWORD len;		// Length
	DWORD rpprev;		// Relative pointer to previous
	DWORD color;		// Color (high byte is font 0-3)
	DWORD fend;		// End flag
};
typedef struct tagWORDINFO WORDINFO;

struct tagRAFONTINFO {
	DWORD charset;		// Character set
	DWORD fDBCS;		// Use double byte characters
	DWORD fntwt;		// Font width
	DWORD fntht;		// Font height
	DWORD spcwt;		// Space width
	DWORD tabwt;		// Tab width
	DWORD italic;		// Height shift
	DWORD monospace;		// Font is monospaced
	DWORD linespace;		// Extra line spacing
};
typedef struct tagRAFONTINFO RAFONTINFO;

struct tagLINE {
	DWORD rpChars;		// Relative pointer to CHARS
};
typedef struct tagLINE LINE;

struct tagCHARS {
	DWORD len;		// Actual String len
	DWORD max;		// Max String len
	DWORD state;		// Line state
	DWORD bmid;		// Bookmark ID
	DWORD errid;		// Error ID
};
typedef struct tagCHARS CHARS;

struct tagRAUNDO {
	DWORD rpPrev;		// Relative pointer to previous
	DWORD undoid;		// Undo ID
	DWORD cp;		// Character position
	DWORD cb;		// Size in bytes
	BYTE fun;		// Function
};
typedef struct tagRAUNDO RAUNDO;

struct tagRAEDT {
	DWORD hwnd;		// Handle of edit a or b
	DWORD hvscroll;		// Handle of scroll bar
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
	DWORD hwnd;		// Handle of main window
	DWORD fstyle;		// Window style
	DWORD ID;		// Window ID
	DWORD hpar;		// Handle of parent window
	RAEDT edta;
	RAEDT edtb;
	DWORD hhscroll;		// Handle of horizontal scrollbar
	DWORD hgrip;		// Handle of sizegrip
	DWORD hnogrip;		// Handle of nosizegrip
	DWORD hsbtn;		// Handle of splitt button
	DWORD hlin;		// Handle of linenumber button
	DWORD hexp;		// Handle of expand button
	DWORD hcol;		// Handle of collapse button
	DWORD hlock;		// Handle of lock button
	DWORD hsta;		// Handle of state window
	DWORD htt;		// Handle of tooltip
	DWORD fresize;		// Resize in action flag
	DWORD fsplitt;		// Splitt factor
	DWORD nsplitt;		// Splitt height

	DWORD hHeap;	// Handle of heap
	DWORD hLine;		// Handle of line pointer mem
	DWORD cbLine;		// Size of line pointer mem
	DWORD rpLine;		// Relative pointer into line pointer mem
	DWORD rpLineFree;		// Pointer to free line pointer
	DWORD hChars;		// Handle of character mem
	DWORD cbChars;		// Size of character mem
	DWORD rpChars;		// Relative pointer into character mem
	DWORD rpCharsFree;		// Relative pointer to free character
	DWORD hUndo;		// Handle of undo memory
	DWORD cbUndo;		// Size of undo memory
	DWORD rpUndo;		// Relative pointer to free (last)
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
	DWORD fOvr;		// Insert / Overwrite
	DWORD nHidden;		// Number of hidden lines
	DWORD cpx;		// Scroll position
	DWORD focus;		// Handle of edit having focus
	DWORD fCaretHide;		// Caret is hidden
	DWORD fChanged;		// Content changed
	DWORD fHideSel;		// Hide selection
	DWORD fIndent;		// Auto indent
	RACOLOR clr;
	RABRUSH br;
	DWORD nTab;		// Tab size
	RAFONT fnt;
	RAFONTINFO fntinfo;
	DWORD lpBmCB;		// Bookmark paint callback
	DWORD nchange;		// Used by EN_SELCHANGE
	DWORD nlastchange;		// Used by EN_SELCHANGE
	DWORD nWordGroup;		// Hilite word group
	DWORD fExpandTab;		// TRUE/FALSE Epand tabs to spaces
	CHARRANGE savesel;
	DWORD htlt;		// Scroll tooltip
	DWORD nMode;		// Block selection
	BLOCKRANGE blrg;
	DWORD lockundoid;
	DWORD ccmntblocks;
	DWORD cpbrst;
	DWORD cpbren;
	DWORD cpselbar;
	DWORD fLock;
	DWORD nCursorWordType;
	DWORD fstyleex;
	DWORD funicode;
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

extern char *szScrollBar		;
extern char *szStatic			;
extern char *szButton			;
extern char *szToolTips		;

extern char *szToolTip			;
extern char *szSplitterBar		;
extern char *szLineNumber		;
extern char *szExpand			;
extern char *szCollapse		;
extern char *szLock			;
extern char *szChanged			;

extern char *szX				;
extern char *szW				;
extern char *szI				;
extern char *szSpace			;
extern char *szTab				;
extern char *szComment			;
extern char *szInclude			;
extern char *szIncludelib		;
extern char *szMemFailLine		;
extern char *szMemFailChar		;
extern char *szMemFailUndo		;
extern char *szMemFailSyntax	;
extern char *szGlobalFail		;
extern char *szHeapFail		;

// .data

extern BYTE CharTabInit[];
extern BYTE CharTab[];
extern BYTE CaseTab[];
extern char szLine[32];

// data?

extern DWORD hInstance;
extern DWORD hBmpLnr;
extern DWORD hBmpExp;
extern DWORD hBmpCol;
extern DWORD hBmpLck;
extern DWORD hHSCur;
extern DWORD hSelCur;
extern DWORD hIml;
extern DWORD hBrTlt;
extern DWORD SBWT;
extern DWORD OldStateProc;
extern DWORD OldSplittBtnProc;
extern DWORD OldFakeToolTipProc;
extern DWORD hWrdMem;
extern DWORD cbWrdMem;
extern DWORD rpWrdFree;
extern DWORD fSelState;
extern DWORD iYp;
extern DWORD fOnBM;
extern DWORD fOnSel;
extern DWORD nBmid;
extern DWORD nUndoid;
extern DWORD fSize;
extern DWORD TimerID;
extern TIMER tmr1;
extern TIMER tmr2;
extern POINT ptDrag;
extern DWORD hDragWin;
extern DWORD hDragSourceMem;
extern CHARRANGE cpDragSource;
extern DWORD peff;
extern DWORD MpX;
extern DWORD MpY;
extern DWORD fTlln;
extern BYTE blockdefs[4096];
extern BYTE bracketstart[16];
extern BYTE bracketend[16];
extern BYTE bracketcont[16];
extern DWORD fNoSaveUndo;
