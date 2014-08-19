#pragma once

#include <windows.h>
#include "Data.h"

REG_T GetBlock(EDIT *pMem, DWORD nLine, REG_T lpBlockDef);
REG_T SetBlocks(EDIT *pMem, REG_T lpLnrg, REG_T lpBlockDef);
REG_T IsBlockDefEqual(REG_T lpRABLOCKDEF1, REG_T lpRABLOCKDEF2);
REG_T IsInBlock(EDIT *pMem, DWORD nLine, REG_T lpBlockDef);
REG_T TestBlockStart(EDIT *pMem, DWORD nLine);
REG_T TestBlockEnd(EDIT *pMem, DWORD nLine);
REG_T CollapseGetEnd(EDIT *pMem, DWORD nLine);
REG_T Collapse(EDIT *pMem, DWORD nLine);
REG_T CollapseAll(EDIT *pMem);
REG_T Expand(EDIT *pMem, DWORD nLine);
REG_T ExpandAll(EDIT *pMem);
REG_T TestExpand(EDIT *pMem, DWORD nLine);
REG_T SetCommentBlocks(EDIT *pMem, REG_T lpStart, REG_T lpEnd);
REG_T SetChangedState(EDIT *pMem, DWORD fUpdate);

