#pragma once

#include <windows.h>
#include "Data.h"

REG_T GetBlock(EDIT *pMem, DWORD nLine, DWORD lpBlockDef);
REG_T SetBlocks(EDIT *pMem, DWORD lpLnrg, DWORD lpBlockDef);
REG_T IsBlockDefEqual(DWORD lpRABLOCKDEF1, DWORD lpRABLOCKDEF2);
REG_T IsInBlock(EDIT *pMem, DWORD nLine, DWORD lpBlockDef);
REG_T TestBlockStart(EDIT *pMem, DWORD nLine);
REG_T TestBlockEnd(EDIT *pMem, DWORD nLine);
REG_T CollapseGetEnd(EDIT *pMem, DWORD nLine);
REG_T Collapse(EDIT *pMem, DWORD nLine);
REG_T CollapseAll(EDIT *pMem);
REG_T Expand(EDIT *pMem, DWORD nLine);
REG_T ExpandAll(EDIT *pMem);
REG_T TestExpand(EDIT *pMem, DWORD nLine);
REG_T SetCommentBlocks(EDIT *pMem, DWORD lpStart, DWORD lpEnd);
REG_T SetChangedState(EDIT *pMem, DWORD fUpdate);

