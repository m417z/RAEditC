#include "Data.h"

// .const

char *szScrollBar		=		"ScrollBar";
char *szStatic			=	"Static";
char *szButton			=	"Button";
char *szToolTips		=		"Tooltips_class32";

char *szToolTip			=	"RAEdit control";
char *szSplitterBar		=	"Splitter Bar";
char *szLineNumber		=	"Show/Hide Linenumbers";
char *szExpand			=	"Expand all";
char *szCollapse		=		"Collapse all";
char *szLock			=		"Lock/Unlock Tab";
char *szChanged			=	"Changed state";

char *szX				=		"X";
char *szW				=		"W";
char *szI				=		"I";
char *szSpace			=		" ";
char *szTab				=	"\t";
char *szComment			=	"comment +";
char *szInclude			=	"include";
char *szIncludelib		=	"includelib";
char *szMemFailLine		=	"Memory allocation failed! (Line)";
char *szMemFailChar		=	"Memory allocation failed! (Char)";
char *szMemFailUndo		=	"Memory allocation failed! (Undo)";
char *szMemFailSyntax	=		"Memory allocation failed! (Syntax)";
char *szGlobalFail		=	"GlobalAlloc failed.";
char *szHeapFail		=		"HeapAlloc failed.";

// .data

BYTE CharTabInit[] = {
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 00
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 10
	//   ! " # $ % & ' ( ) * + , - . / 
	   0,2,5,3,2,2,2,5,2,2,2,2,2,2,3,2,		// 20
	// 0 1 2 3 4 5 6 7 8 9 : ; < = > ? 
	   1,1,1,1,1,1,1,1,1,1,2,4,2,2,2,3,		// 30
	// @ A B C D E F G H I J K L M N O 
	   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		// 40
	// P Q R S T U V W X Y Z [ \ ] ^ _ 
	   1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1,		// 50
	// ` a b c d e f g h i j k l m n o 
	   0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		// 60
	// p q r s t u v w x y z { | } ~   
	   1,1,1,1,1,1,1,1,1,1,1,2,2,2,0,0,		// 70
	   
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 80
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 90
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// A0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// B0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// C0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// D0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// E0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// F0
};

BYTE CharTab[] = {
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 00
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 10
	//   ! " # $ % & ' ( ) * + , - . / 
	   0,2,5,3,2,2,2,5,2,2,2,2,2,2,3,2,		// 20
	// 0 1 2 3 4 5 6 7 8 9 : ; < = > ? 
	   1,1,1,1,1,1,1,1,1,1,2,4,2,2,2,3,		// 30
	// @ A B C D E F G H I J K L M N O 
	   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		// 40
	// P Q R S T U V W X Y Z [ \ ] ^ _ 
	   1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1,		// 50
	// ` a b c d e f g h i j k l m n o 
	   0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		// 60
	// p q r s t u v w x y z { | } ~   
	   1,1,1,1,1,1,1,1,1,1,1,2,2,2,0,0,		// 70
	   
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 80
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 90
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// A0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// B0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// C0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// D0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// E0
	   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// F0
};

BYTE CaseTab[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 00
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 10
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 20
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 30
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 40
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 50
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 60
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 70
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 80
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// 90
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// A0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// B0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// C0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// D0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// E0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,		// F0
};

char szLine[32] =					"Ln: ";

// data?

DWORD hInstance;
DWORD hBmpLnr;
DWORD hBmpExp;
DWORD hBmpCol;
DWORD hBmpLck;
DWORD hHSCur;
DWORD hSelCur;
DWORD hIml;
DWORD hBrTlt;
DWORD SBWT;
DWORD OldStateProc;
DWORD OldSplittBtnProc;
DWORD OldFakeToolTipProc;
DWORD hWrdMem;
DWORD cbWrdMem;
DWORD rpWrdFree;
DWORD fSelState;
DWORD iYp;
DWORD fOnBM;
DWORD fOnSel;
DWORD nBmid;
DWORD nUndoid;
DWORD fSize;
DWORD TimerID;
TIMER tmr1;
TIMER tmr2;
POINT ptDrag;
DWORD hDragWin;
DWORD hDragSourceMem;
CHARRANGE cpDragSource;
DWORD peff;
DWORD MpX;
DWORD MpY;
DWORD fTlln;
BYTE blockdefs[4096];
BYTE bracketstart[16];
BYTE bracketend[16];
BYTE bracketcont[16];
DWORD fNoSaveUndo;
