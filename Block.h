#pragma once

#include <windows.h>
#include "Data.h"

REG_T GetBlock(DWORD hMem, DWORD nLine, DWORD lpBlockDef);
REG_T SetBlocks(DWORD hMem, DWORD lpLnrg, DWORD lpBlockDef);
REG_T IsBlockDefEqual(DWORD lpRABLOCKDEF1, DWORD lpRABLOCKDEF2);
REG_T IsInBlock(DWORD hMem, DWORD nLine, DWORD lpBlockDef);
REG_T TestBlockStart(DWORD hMem, DWORD nLine);
REG_T TestBlockEnd(DWORD hMem, DWORD nLine);
REG_T CollapseGetEnd(DWORD hMem, DWORD nLine);
REG_T Collapse(DWORD hMem, DWORD nLine);
REG_T CollapseAll(DWORD hMem);
REG_T Expand(DWORD hMem, DWORD nLine);
REG_T ExpandAll(DWORD hMem);
REG_T TestExpand(DWORD hMem, DWORD nLine);
REG_T SetCommentBlocks(DWORD hMem, DWORD lpStart, DWORD lpEnd);
REG_T SetChangedState(DWORD hMem, DWORD fUpdate);

