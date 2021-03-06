#include "Block.h"

#include "Function.h"
#include "Position.h"

REG_T GetBlock(EDIT *pMem, DWORD nLine, REG_T lpBlockDef)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD nLines;
	BYTE buffer[256];
	DWORD nNest;
	DWORD flag;

	auto void TestBlock(void);
	auto void SkipWhSp(void);
	auto void SkipWrd(void);
	auto void CopyWrd(void);

	nNest = 1;
	esi = lpBlockDef;
	eax = ((RABLOCKDEF *)esi)->flag;
	flag = eax;
	esi = ((RABLOCKDEF *)esi)->lpszEnd;
	if(esi)
	{
		edi = nLine;
		edi *= sizeof(LINE);
		edi += pMem->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += pMem->hChars;
		ecx = 0;
		SkipWhSp();
		RBYTE_LOW(eax) = *(BYTE *)esi;
		if(RBYTE_LOW(eax)=='$')
		{
			// $ endp
			nNest = 0;
			edx = &buffer;
			CopyWrd();
			*(BYTE *)edx = ' ';
			edx++;
anon_1:
			esi++;
			RBYTE_LOW(eax) = *(BYTE *)esi;
			if(RBYTE_LOW(eax)==' ')
			{
				goto anon_1;
			} // endif
			eax = lstrcpy(edx, esi);
			esi = &buffer;
			TestBlock();
		}
		else if(RBYTE_LOW(eax)=='?')
		{
			// ? endp
			nNest = 0;
			edx = &buffer;
			CopyWrd();
			*(BYTE *)edx = ' ';
			edx++;
anon_2:
			esi++;
			RBYTE_LOW(eax) = *(BYTE *)esi;
			if(RBYTE_LOW(eax)==' ')
			{
				goto anon_2;
			} // endif
			eax = lstrcpy(edx, esi);
			temp1 = esi;
			esi = &buffer;
			TestBlock();
			esi = temp1;
			if(eax==-1)
			{
				TestBlock();
			} // endif
		}
		else
		{
			temp1 = ecx;
			eax = strlen(esi);
			ecx = temp1;
			if(eax)
			{
				RBYTE_LOW(eax) = *(BYTE *)(esi+eax-1);
			} // endif
			if(RBYTE_LOW(eax)=='$')
			{
				// endp $
				nNest = 0;
				edx = &buffer;
anon_3:
				RBYTE_LOW(eax) = *(BYTE *)esi;
				if(RBYTE_LOW(eax)==' ')
				{
					goto anon_4;
				} // endif
				if(RBYTE_LOW(eax)=='$')
				{
					goto anon_4;
				} // endif
				*(BYTE *)edx = RBYTE_LOW(eax);
				esi++;
				edx++;
				goto anon_3;
anon_4:
				*(BYTE *)edx = ' ';
				edx++;
				SkipWrd();
				SkipWhSp();
				CopyWrd();
				esi = &buffer;
				TestBlock();
			}
			else
			{
				// endp
				TestBlock();
			} // endif
		} // endif
	}
	else
	{
		nLines = 0;
		if(flag & BD_SEGMENTBLOCK)
		{
			esi = pMem->rpLineFree;
			esi -= sizeof(LINE);
			nLine++;
			while(TRUE)
			{
				edi = nLine;
				edi *= sizeof(LINE);
				if(edi>=esi)
				{
					break;
				} // endif
				edi += pMem->hLine;
				edi = ((LINE *)edi)->rpChars;
				edi += pMem->hChars;
				if(((CHARS *)edi)->state&STATE_SEGMENTBLOCK)
				{
					break;
				} // endif
				nLine++;
				nLines++;
			} // endw
		}
		else if(flag & BD_COMMENTBLOCK)
		{
			esi = pMem->rpLineFree;
			esi -= sizeof(LINE);
			nLine++;
			while(TRUE)
			{
				edi = nLine;
				edi *= sizeof(LINE);
				if(edi>=esi)
				{
					break;
				} // endif
				edi += pMem->hLine;
				edi = ((LINE *)edi)->rpChars;
				edi += pMem->hChars;
				if(!(((CHARS *)edi)->state&STATE_COMMENT))
				{
					break;
				} // endif
				nLine++;
				nLines++;
			} // endw
		} // endif
		eax = nLines;
	} // endif
	return eax;

	void TestBlock(void)
	{
		REG_T temp1, temp2;
		nLines = 0;
		edi = nLine;
		while(TRUE)
		{
			eax = 0;
			eax--;
			ecx = edi;
			ecx *= sizeof(LINE);
			if(ecx>pMem->rpLineFree)
			{
				break;
			} // endif
			if(nNest)
			{
				ecx = lpBlockDef;
				eax = IsLine(pMem, edi, ((RABLOCKDEF *)ecx)->lpszStart);
				if(eax!=-1)
				{
					nNest++;
				} // endif
			} // endif
			eax = IsLine(pMem, edi, esi);
			if(eax!=-1)
			{
				if(nNest==0)
				{
					break;
				} // endif
				nNest--;
				if(nNest==0)
				{
					break;
				} // endif
				if(nNest==1)
				{
					break;
				} // endif
			} // endif
			edi++;
			nLines++;
		} // endw
		if(nNest)
		{
			nNest--;
		} // endif
		if(nLines)
		{
			nLines--;
		} // endif
		if(eax!=-1 && !nNest)
		{
			if(!(flag&BD_INCLUDELAST))
			{
				goto anon_5;
			} // endif
			ecx = edi;
			ecx++;
			ecx *= sizeof(LINE);
			if(ecx==pMem->rpLineFree)
			{
				goto anon_5;
			} // endif
			nLines++;
anon_5:
			eax = nLines;
			if(flag&BD_LOOKAHEAD)
			{
				temp1 = eax;
				ecx = edi;
				ecx += 500;
				while(edi<ecx)
				{
					edi++;
					eax = edi;
					eax *= sizeof(LINE);
					if(eax>pMem->rpLineFree)
					{
						break;
					} // endif
					nLines++;
					temp2 = ecx;
					eax = IsLine(pMem, edi, esi);
					if(eax!=-1)
					{
						ecx = temp2;
						eax = temp1;
						temp1 = nLines;
						ecx = edi;
						ecx += 500;
						temp2 = ecx;
					} // endif
					eax = lpBlockDef;
					eax = ((RABLOCKDEF *)eax)->lpszStart;
					eax = IsLine(pMem, edi, eax);
					ecx = temp2;
					if(eax!=-1)
					{
						break;
					} // endif
				} // endw
				eax = temp1;
			} // endif
		}
		else
		{
			eax = 0;
			eax--;
		} // endif
		return;

	}

	void SkipWhSp(void)
	{
		ecx--;
anon_6:
		ecx++;
		if(ecx>=((CHARS *)edi)->len)
		{
			goto anon_7;
		} // endif
		RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)==VK_TAB)
		{
			goto anon_6;
		} // endif
		if(RBYTE_LOW(eax)==' ')
		{
			goto anon_6;
		} // endif
anon_7:
		return;

	}

	void SkipWrd(void)
	{
		ecx--;
anon_8:
		ecx++;
		if(ecx>=((CHARS *)edi)->len)
		{
			goto anon_9;
		} // endif
		RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)==VK_TAB)
		{
			goto anon_9;
		} // endif
		if(RBYTE_LOW(eax)==' ')
		{
			goto anon_9;
		} // endif
		if(RBYTE_LOW(eax)!=0x0D)
		{
			goto anon_8;
		} // endif
anon_9:
		return;

	}

	void CopyWrd(void)
	{
anon_10:
		if(ecx>=((CHARS *)edi)->len)
		{
			goto anon_11;
		} // endif
		RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)==VK_TAB)
		{
			goto anon_11;
		} // endif
		if(RBYTE_LOW(eax)==' ')
		{
			goto anon_11;
		} // endif
		if(RBYTE_LOW(eax)==0x0D)
		{
			goto anon_11;
		} // endif
		*(BYTE *)edx = RBYTE_LOW(eax);
		ecx++;
		edx++;
		goto anon_10;
anon_11:
		*(BYTE *)edx = 0;
		return;

	}

} // GetBlock

REG_T SetBlocks(EDIT *pMem, REG_T lpLnrg, REG_T lpBlockDef)
{
	REG_T eax = 0, edx, esi, edi;
	DWORD nLine;

	nLine = 0;
	eax = lpLnrg;
	if(eax)
	{
		nLine = ((LINERANGE *)eax)->lnMin;
		eax = ((LINERANGE *)eax)->lnMax;
		eax++;
		eax++;
	} // endif
	eax--;
	eax *= sizeof(LINE);
	esi = eax;
	if(esi>pMem->rpLineFree)
	{
		esi = pMem->rpLineFree;
	} // endif
	nLine--;
anon_12:
	nLine++;
	edi = nLine;
	edi *= sizeof(LINE);
	if(edi<esi)
	{
		eax = IsLine(pMem, nLine, szInclude);
		eax++;
		if(eax!=0)
		{
			goto anon_12;
		} // endif
		eax = IsLine(pMem, nLine, szIncludelib);
		eax++;
		if(eax!=0)
		{
			goto anon_12;
		} // endif
		eax = lpBlockDef;
		eax = ((RABLOCKDEF *)eax)->lpszStart;
		eax = IsLine(pMem, nLine, eax);
		eax++;
		if(eax==0)
		{
			goto anon_12;
		} // endif
		edi += pMem->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += pMem->hChars;
		if(((CHARS *)edi)->state&STATE_NOBLOCK)
		{
			goto anon_12;
		} // endif
		nBmid++;
		eax = nBmid;
		((CHARS *)edi)->bmid = eax;
		((CHARS *)edi)->state &= -1 ^ STATE_BMMASK;
		((CHARS *)edi)->state |= STATE_BM1;
		eax = lpBlockDef;
		if(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK)
		{
			((CHARS *)edi)->state |= STATE_SEGMENTBLOCK;
		}
		else
		{
			((CHARS *)edi)->state &= -1 ^ STATE_SEGMENTBLOCK;
		} // endif
		if(((RABLOCKDEF *)eax)->flag&BD_DIVIDERLINE)
		{
			((CHARS *)edi)->state |= STATE_DIVIDERLINE;
		}
		else
		{
			((CHARS *)edi)->state &= -1 ^ STATE_DIVIDERLINE;
		} // endif
		if(((RABLOCKDEF *)eax)->flag&BD_NONESTING)
		{
			eax = GetBlock(pMem, nLine, lpBlockDef);
			if(eax!=-1)
			{
				nLine += eax;
				goto anon_12;
			} // endif
		} // endif
		eax = lpBlockDef;
		if(((RABLOCKDEF *)eax)->flag&BD_ALTHILITE)
		{
			((CHARS *)edi)->state |= STATE_ALTHILITE;
		}
		else
		{
			((CHARS *)edi)->state &= -1 ^ STATE_ALTHILITE;
		} // endif
		if(((RABLOCKDEF *)eax)->flag&BD_NOBLOCK)
		{
			eax = GetBlock(pMem, nLine, lpBlockDef);
			if(eax!=-1)
			{
				edx = nLine;
				nLine += eax;
				eax = lpBlockDef;
				eax = ((RABLOCKDEF *)eax)->flag;
				eax &= BD_ALTHILITE;
				while(edx<=nLine)
				{
					edx++;
					edi = edx;
					edi *= sizeof(LINE);
					if(edi<esi)
					{
						edi += pMem->hLine;
						edi = ((LINE *)edi)->rpChars;
						edi += pMem->hChars;
						((CHARS *)edi)->state &= -1 ^ (STATE_BMMASK | STATE_SEGMENTBLOCK | STATE_DIVIDERLINE);
						((CHARS *)edi)->state |= STATE_NOBLOCK;
						if(eax)
						{
							((CHARS *)edi)->state |= STATE_ALTHILITE;
						} // endif
					} // endif
				} // endw
			} // endif
		}
		else
		{
			eax = lpBlockDef;
			if(((RABLOCKDEF *)eax)->flag&BD_ALTHILITE)
			{
				eax = GetBlock(pMem, nLine, lpBlockDef);
				if(eax!=-1)
				{
					edx = nLine;
					nLine += eax;
					while(edx<=nLine)
					{
						edx++;
						edi = edx;
						edi *= sizeof(LINE);
						if(edi<esi)
						{
							edi += pMem->hLine;
							edi = ((LINE *)edi)->rpChars;
							edi += pMem->hChars;
							((CHARS *)edi)->state |= STATE_ALTHILITE;
						} // endif
					} // endw
				} // endif
			} // endif
		} // endif
		goto anon_12;
	} // endif
	return eax;

} // SetBlocks

REG_T IsBlockDefEqual(REG_T lpRABLOCKDEF1, REG_T lpRABLOCKDEF2)
{
	REG_T eax = 0, edx, esi, edi;

	esi = lpRABLOCKDEF1;
	edi = lpRABLOCKDEF2;
	eax = ((RABLOCKDEF *)esi)->flag;
	if(eax==((RABLOCKDEF *)edi)->flag)
	{
		eax = ((RABLOCKDEF *)esi)->lpszStart;
		edx = ((RABLOCKDEF *)edi)->lpszStart;
		if(eax && edx)
		{
			eax = lstrcmp(eax, edx);
			if(eax!=0)
			{
				goto NotEq;
			} // endif
		}
		else if((eax && !edx) || (!eax && edx))
		{
			goto NotEq;
		} // endif
		eax = ((RABLOCKDEF *)esi)->lpszEnd;
		edx = ((RABLOCKDEF *)edi)->lpszEnd;
		if(eax && edx)
		{
			eax = lstrcmp(eax, edx);
			if(eax!=0)
			{
				goto NotEq;
			} // endif
		}
		else if((eax && !edx) || (!eax && edx))
		{
			goto NotEq;
		} // endif
		eax = ((RABLOCKDEF *)esi)->lpszNot1;
		edx = ((RABLOCKDEF *)edi)->lpszNot1;
		if(eax && edx)
		{
			eax = lstrcmp(eax, edx);
			if(eax!=0)
			{
				goto NotEq;
			} // endif
		}
		else if((eax && !edx) || (!eax && edx))
		{
			goto NotEq;
		} // endif
		eax = ((RABLOCKDEF *)esi)->lpszNot2;
		edx = ((RABLOCKDEF *)edi)->lpszNot2;
		if(eax && edx)
		{
			eax = lstrcmp(eax, edx);
			if(eax!=0)
			{
				goto NotEq;
			} // endif
		}
		else if((eax && !edx) || (!eax && edx))
		{
			goto NotEq;
		} // endif
	}
	else
	{
		goto NotEq;
	} // endif
	eax = 0;
	eax++;
	return eax;
NotEq:
	eax = 0;
	return eax;

} // IsBlockDefEqual

REG_T IsInBlock(EDIT *pMem, DWORD nLine, REG_T lpBlockDef)
{
	REG_T eax = 0, esi, edi;

	edi = nLine;
	esi = lpBlockDef;
	esi = ((RABLOCKDEF *)esi)->lpszStart;
anon_13:
	eax = PreviousBookMark(pMem, edi, 1);
	edi = eax;
	eax++;
	if(eax)
	{
		eax = IsLine(pMem, edi, esi);
		eax++;
		if(eax==0)
		{
			goto anon_13;
		} // endif
		eax = GetBlock(pMem, edi, lpBlockDef);
		edi += eax;
		eax = lpBlockDef;
		if(!(((RABLOCKDEF *)eax)->flag&BD_INCLUDELAST))
		{
			edi++;
		} // endif
		eax = 0;
		if(edi>=nLine)
		{
			eax++;
		} // endif
	} // endif
	return eax;

} // IsInBlock

REG_T TestBlockStart(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, esi, edi;

	esi = nLine;
	esi *= sizeof(LINE);
	if(esi<pMem->rpLineFree)
	{
		esi += pMem->hLine;
		esi = ((LINE *)esi)->rpChars;
		esi += pMem->hChars;
		if(!(((CHARS *)esi)->state&STATE_NOBLOCK))
		{
			esi = blockdefs;
			edi = esi+32*sizeof(REG_T);
			while(*(REG_T *)esi)
			{
				eax = ((RABLOCKDEF *)edi)->flag;
				eax >>= 16;
				if(eax==pMem->nWordGroup)
				{
					eax = IsLine(pMem, nLine, ((RABLOCKDEF *)edi)->lpszStart);
					if(eax!=-1)
					{
						eax = edi;
						goto Ex;
					} // endif
				} // endif
				edi = *(REG_T *)esi;
				esi += sizeof(REG_T);
			} // endw
		} // endif
	} // endif
	eax = 0;
	eax--;
Ex:
	return eax;

} // TestBlockStart

REG_T TestBlockEnd(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx, esi, edi;
	REG_T lpSecond;

	esi = blockdefs;
	edi = esi+32*sizeof(REG_T);
	while(*(REG_T *)esi)
	{
		lpSecond = 0;
		if(((RABLOCKDEF *)edi)->lpszEnd)
		{
			eax = strlen(((RABLOCKDEF *)edi)->lpszEnd);
			edx = ((RABLOCKDEF *)edi)->lpszEnd;
			eax = edx+eax+1;
			if(*(BYTE *)eax)
			{
				lpSecond = eax;
			} // endif
		} // endif
		eax = ((RABLOCKDEF *)edi)->flag;
		eax >>= 16;
		if(((RABLOCKDEF *)edi)->lpszEnd && eax==pMem->nWordGroup)
		{
			eax = nLine;
			eax *= sizeof(LINE);
			eax += pMem->hLine;
			eax = ((LINE *)eax)->rpChars;
			eax += pMem->hChars;
			if(((CHARS *)eax)->state&STATE_ALTHILITE)
			{
				if(!(((RABLOCKDEF *)edi)->flag&BD_ALTHILITE))
				{
					eax = 0;
				}
				else
				{
					eax |= 1;
				} // endif
			}
			else
			{
				eax |= 1;
			} // endif
			if(eax)
			{
				eax = IsLine(pMem, nLine, ((RABLOCKDEF *)edi)->lpszEnd);
				if(eax!=-1)
				{
					eax = edi;
					goto Ex;
				}
				else if(lpSecond)
				{
					eax = IsLine(pMem, nLine, lpSecond);
					if(eax!=-1)
					{
						eax = edi;
						goto Ex;
					} // endif
				} // endif
			} // endif
		} // endif
		edi = *(REG_T *)esi;
		esi += sizeof(REG_T);
	} // endw
	eax = 0;
	eax--;
Ex:
	return eax;

} // TestBlockEnd

REG_T CollapseGetEnd(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx, esi, edi;
	DWORD nLines;
	DWORD nNest;
	DWORD nMax;
	REG_T Nest[256];

	nLines = 0;
	nNest = 0;
	eax = pMem->rpLineFree;
	eax /= sizeof(LINE);
	nMax = eax;
	edi = nLine;
	eax = TestBlockStart(pMem, edi);
	if(eax!=-1)
	{
		edx = nNest;
		Nest[edx] = eax;
		if(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK)
		{
			edi++;
			while(edi<nMax)
			{
				esi = edi;
				esi *= sizeof(LINE);
				esi += pMem->hLine;
				esi = ((LINE *)esi)->rpChars;
				esi += pMem->hChars;
				if(((CHARS *)esi)->state&STATE_SEGMENTBLOCK)
				{
					break;
				} // endif
				edi++;
			} // endw
			eax = edi;
			goto Ex;
		}
		else
		{
			nNest++;
			edi++;
			if(((RABLOCKDEF *)eax)->flag&BD_LOOKAHEAD)
			{
				esi = eax;
				eax = edi;
				eax += 500;
				if(eax<nMax)
				{
					nMax = eax;
				} // endif
				while(edi<nMax)
				{
					eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszStart);
					if(eax!=-1)
					{
						break;
					} // endif
					eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszEnd);
					if(eax!=-1)
					{
						nLines = edi;
					} // endif
					edi++;
				} // endw
				edi = nLines;
				eax = edi;
				goto Ex;
			}
			else
			{
				while(edi<nMax)
				{
					eax = TestBlockStart(pMem, edi);
					if(eax!=-1)
					{
						if(!(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK))
						{
							edx = nNest;
							Nest[edx] = eax;
							nNest++;
						} // endif
					}
					else
					{
						eax = TestBlockEnd(pMem, edi);
						if(eax!=-1)
						{
							edx = nNest;
							edx--;
							if(eax!=Nest[edx])
							{
								eax = 0;
								eax--;
								goto Ex;
							} // endif
							nNest--;
							if(nNest==0)
							{
								eax = edi;
								goto Ex;
							} // endif
						} // endif
					} // endif
					edi++;
				} // endw
			} // endif
		} // endif
	} // endif
	eax = 0;
	eax--;
Ex:
	return eax;

} // CollapseGetEnd

REG_T Collapse(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD nLines;
	DWORD nNest;
	DWORD nMax;
	DWORD fmasmcomment;

	eax = 0;
	nLines = eax;
	nNest = eax;
	fmasmcomment = eax;
	edi = nLine;
	eax = TestBlockStart(pMem, edi);
	if(eax!=-1)
	{
		esi = eax;
		eax = pMem->rpLineFree;
		eax /= sizeof(LINE);
		nMax = eax;
		if(((RABLOCKDEF *)esi)->flag&BD_SEGMENTBLOCK)
		{
			eax = SetBookMark(pMem, edi, 2);
			edx = eax;
			edi++;
			while(edi<nMax)
			{
				esi = edi;
				esi *= sizeof(LINE);
				esi += pMem->hLine;
				esi = ((LINE *)esi)->rpChars;
				esi += pMem->hChars;
				if(((CHARS *)esi)->state&STATE_SEGMENTBLOCK)
				{
					break;
				} // endif
				if(!(((CHARS *)esi)->state&STATE_HIDDEN))
				{
					((CHARS *)esi)->state |= STATE_HIDDEN;
					((CHARS *)esi)->bmid = edx;
					pMem->nHidden++;
				} // endif
				edi++;
			} // endw
		}
		else
		{
			nNest++;
			edi++;
			if(((RABLOCKDEF *)esi)->flag&BD_LOOKAHEAD)
			{
				eax = edi;
				eax += 500;
				if(eax<nMax)
				{
					nMax = eax;
				} // endif
				while(edi<nMax)
				{
					eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszStart);
					if(eax!=-1)
					{
						break;
					} // endif
					eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszEnd);
					if(eax!=-1)
					{
						nLines = edi;
					} // endif
					edi++;
				} // endw
				if(!(((RABLOCKDEF *)esi)->flag&BD_INCLUDELAST))
				{
					nLines++;
				} // endif
				edi = nLine;
				eax = SetBookMark(pMem, edi, 2);
				edx = eax;
				edi++;
				while(edi<=nLines)
				{
					eax = 0;
					eax--;
					temp1 = edx;
					if(((RABLOCKDEF *)esi)->lpszNot1)
					{
						eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszNot1);
						if(eax==-1)
						{
							if(((RABLOCKDEF *)esi)->lpszNot2)
							{
								eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszNot2);
							} // endif
						} // endif
					} // endif
					edx = temp1;
					if(eax==-1)
					{
						temp1 = edi;
						edi *= sizeof(LINE);
						edi += pMem->hLine;
						edi = ((LINE *)edi)->rpChars;
						edi += pMem->hChars;
						if(!(((CHARS *)edi)->state&STATE_HIDDEN))
						{
							((CHARS *)edi)->state |= STATE_HIDDEN;
							((CHARS *)edi)->bmid = edx;
							pMem->nHidden++;
						} // endif
						edi = temp1;
					} // endif
					edi++;
				} // endw
			}
			else
			{
				if(((RABLOCKDEF *)esi)->flag&BD_COMMENTBLOCK && pMem->ccmntblocks==4)
				{
					fmasmcomment++;
				} // endif
				while(edi<nMax)
				{
					eax = -1;
					if(!fmasmcomment)
					{
						if(!(((RABLOCKDEF *)esi)->flag&BD_NOBLOCK))
						{
							eax = TestBlockStart(pMem, edi);
						} // endif
					} // endif
					if(eax!=-1)
					{
						if(!(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK))
						{
							nNest++;
						} // endif
					}
					else
					{
						if(fmasmcomment)
						{
							edx = edi;
							edx++;
							edx *= sizeof(LINE);
							edx += pMem->hLine;
							edx = ((LINE *)edx)->rpChars;
							edx += pMem->hChars;
							if(!(((CHARS *)edx)->state&STATE_COMMENT))
							{
								eax = 0;
							} // endif
						}
						else
						{
							eax = TestBlockEnd(pMem, edi);
						} // endif
						if(eax!=-1)
						{
							nNest--;
							if(nNest==0)
							{
								if(!(((RABLOCKDEF *)esi)->flag&BD_INCLUDELAST))
								{
									edi--;
								} // endif
								nLines = edi;
								edi = nLine;
								eax = SetBookMark(pMem, edi, 2);
								edx = eax;
								edi++;
								while(edi<=nLines)
								{
									eax = -1;
									if(!fmasmcomment)
									{
										temp1 = edx;
										eax = TestBlockStart(pMem, edi);
										if(eax!=-1)
										{
											nNest++;
										}
										else
										{
											eax = TestBlockEnd(pMem, edi);
											if(eax!=-1)
											{
												nNest--;
											} // endif
										} // endif
										edx = temp1;
										eax = 0;
										eax--;
										if(!nNest)
										{
											temp1 = edx;
											if(((RABLOCKDEF *)esi)->lpszNot1)
											{
												eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszNot1);
												if(eax==-1)
												{
													if(((RABLOCKDEF *)esi)->lpszNot2)
													{
														eax = IsLine(pMem, edi, ((RABLOCKDEF *)esi)->lpszNot2);
													} // endif
												} // endif
											} // endif
											edx = temp1;
										} // endif
									} // endif
									if(eax==-1)
									{
										temp1 = edi;
										edi *= sizeof(LINE);
										edi += pMem->hLine;
										edi = ((LINE *)edi)->rpChars;
										edi += pMem->hChars;
										if(!(((CHARS *)edi)->state&STATE_HIDDEN))
										{
											((CHARS *)edi)->state |= STATE_HIDDEN;
											((CHARS *)edi)->bmid = edx;
											pMem->nHidden++;
										} // endif
										edi = temp1;
									} // endif
									edi++;
								} // endw
								goto Ex;
							} // endif
						} // endif
					} // endif
					edi++;
				} // endw
			} // endif
		} // endif
Ex:
		eax = 0;
		edx = nLine;
		if(edx<pMem->edta.topln)
		{
			pMem->edta.topyp = eax;
			pMem->edta.topln = eax;
			pMem->edta.topcp = eax;
		} // endif
		if(edx<pMem->edtb.topln)
		{
			pMem->edtb.topyp = eax;
			pMem->edtb.topln = eax;
			pMem->edtb.topcp = eax;
		} // endif
		eax = pMem->rpLineFree;
		eax /= sizeof(LINE);
		eax -= pMem->nHidden;
		ecx = pMem->fntinfo.fntht;
		eax *= ecx;
		ecx = 0;
		if(eax<pMem->edta.cpy)
		{
			pMem->edta.cpy = eax;
			pMem->edta.topyp = ecx;
			pMem->edta.topln = ecx;
			pMem->edta.topcp = ecx;
		} // endif
		if(eax<pMem->edtb.cpy)
		{
			pMem->edtb.cpy = eax;
			pMem->edtb.topyp = ecx;
			pMem->edtb.topln = ecx;
			pMem->edtb.topcp = ecx;
		} // endif
	} // endif
	return eax;

} // Collapse

REG_T CollapseAll(EDIT *pMem)
{
	REG_T eax = 0, ecx, edx, esi, edi;

	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	esi = 0;
	edi = pMem->rpLineFree;
	edi /= sizeof(LINE);
anon_14:
	eax = PreviousBookMark(pMem, edi, 1);
	if(eax!=-1)
	{
		edi = eax;
		eax = Collapse(pMem, edi);
		if(eax!=-1)
		{
			esi++;
		} // endif
		goto anon_14;
	} // endif
	eax = esi;
	return eax;

} // CollapseAll

REG_T Expand(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2;

	temp1 = pMem->nHidden;
	esi = nLine;
	eax = 0;
	if(esi<pMem->edta.topln)
	{
		pMem->edta.topyp = eax;
		pMem->edta.topln = eax;
		pMem->edta.topcp = eax;
	} // endif
	if(esi<pMem->edtb.topln)
	{
		pMem->edtb.topyp = eax;
		pMem->edtb.topln = eax;
		pMem->edtb.topcp = eax;
	} // endif
	esi *= sizeof(LINE);
	if(esi>=pMem->rpLineFree)
	{
		goto Ex;
	} // endif
	esi += pMem->hLine;
	ecx = pMem->rpLineFree;
	ecx += pMem->hLine;
	eax = ((LINE *)esi)->rpChars;
	eax += pMem->hChars;
	if(((CHARS *)eax)->state&STATE_HIDDEN)
	{
		goto Ex;
	} // endif
	edi = ((LINE *)esi)->rpChars;
	edi += pMem->hChars;
	eax = ((CHARS *)edi)->state;
	eax &= STATE_BMMASK;
	if(eax==STATE_BM2)
	{
		eax = ((CHARS *)edi)->state;
		eax &= -1 ^ STATE_BMMASK;
		eax |= STATE_BM1;
		((CHARS *)edi)->state = eax;
	}
	else if(eax==STATE_BM8)
	{
		eax = ((CHARS *)edi)->state;
		eax &= -1 ^ STATE_BMMASK;
		((CHARS *)edi)->state = eax;
	} // endif
	esi += sizeof(LINE);
	if(esi<ecx)
	{
		temp2 = ecx;
		eax = esi;
		eax += (sizeof(LINE))*64;
		if(eax<ecx)
		{
			// Check max 64 lines ahead
			ecx = eax;
		} // endif
		while(esi<ecx)
		{
			edi = ((LINE *)esi)->rpChars;
			edi += pMem->hChars;
			if(((CHARS *)edi)->state&STATE_HIDDEN)
			{
				break;
			} // endif
			esi += sizeof(LINE);
		} // endw
		ecx = temp2;
		edi = ((LINE *)esi)->rpChars;
		edi += pMem->hChars;
		if(!(((CHARS *)edi)->state&STATE_HIDDEN))
		{
			goto Ex;
		} // endif
		edx = ((CHARS *)edi)->bmid;
		while(esi<ecx)
		{
			edi = ((LINE *)esi)->rpChars;
			edi += pMem->hChars;
			if(edx==((CHARS *)edi)->bmid)
			{
				if(((CHARS *)edi)->state&STATE_HIDDEN)
				{
					((CHARS *)edi)->state &= -1 ^ STATE_HIDDEN;
					pMem->nHidden--;
				} // endif
			} // endif
			esi += sizeof(LINE);
		} // endw
	} // endif
Ex:
	eax = temp1;
	eax -= pMem->nHidden;
	return eax;

} // Expand

REG_T ExpandAll(EDIT *pMem)
{
	REG_T eax = 0, esi, edi;

	esi = 0;
	edi = 0;
	eax = GetBookMark(pMem, edi);
anon_15:
	if(eax==2)
	{
		eax = Expand(pMem, edi);
		esi++;
	} // endif
	eax = NextBookMark(pMem, edi, 2);
	if(eax!=-1)
	{
		edi = eax;
		eax = 2;
		goto anon_15;
	} // endif
	eax = esi;
	return eax;

} // ExpandAll

REG_T TestExpand(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0;
	REG_T temp1, temp2;

	temp1 = pMem->nHidden;
anon_16:
	eax = IsLineHidden(pMem, nLine);
	if(eax)
	{
		temp2 = nLine;
		while(eax && nLine)
		{
			nLine--;
			eax = IsLineHidden(pMem, nLine);
		} // endw
		eax = Expand(pMem, nLine);
		nLine = temp2;
		goto anon_16;
	} // endif
	eax = temp1;
	if(eax!=pMem->nHidden)
	{
		eax = InvalidateEdit(pMem, pMem->edta.hwnd);
		eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	} // endif
	return eax;

} // TestExpand

REG_T SetCommentBlocks(EDIT *pMem, REG_T lpStart, REG_T lpEnd)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD nLine;
	DWORD nCmnt;
	DWORD fCmnt;
	BYTE cmntchar;
	DWORD fChanged;

	auto void TestWrd(void);
	auto void IsLineStart(void);
	auto void IsLineEnd(void);

	cmntchar = 0;
	pMem->ccmntblocks = 0;
	eax = lpStart;
	edx = lpEnd;
	if(*(WORD *)eax=='*/' && *(WORD *)edx=='/*')
	{
		pMem->ccmntblocks = 1;
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		edi = pMem->rpLineFree;
		edi /= sizeof(LINE);
		while(nLine<edi)
		{
			esi = nLine;
			esi *= sizeof(LINE);
			esi += pMem->hLine;
			esi = ((LINE *)esi)->rpChars;
			esi += pMem->hChars;
			temp1 = ((CHARS *)esi)->state;
			ecx = 0;
			ecx++;
			eax = nCmnt;
			fCmnt = eax;
			while(ecx<((CHARS *)esi)->len)
			{
				if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=='*/')
				{
					ecx++;
					nCmnt++;
				}
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=='/*')
				{
					ecx++;
					if(nCmnt)
					{
						nCmnt--;
					} // endif
				} // endif
				ecx++;
			} // endw
			((CHARS *)esi)->state &= -1 ^ (STATE_COMMENT | STATE_COMMENTNEST);
			if(nCmnt>1 || fCmnt)
			{
				((CHARS *)esi)->state |= STATE_COMMENT;
				if(nCmnt && fCmnt)
				{
					((CHARS *)esi)->state |= STATE_COMMENT | STATE_COMMENTNEST;
				} // endif
			} // endif
			eax = temp1;
			if(eax!=((CHARS *)esi)->state)
			{
				fChanged++;
			} // endif
			nLine++;
		} // endw
		if(fChanged)
		{
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		} // endif
		return eax;
	}
	else if(*(WORD *)eax=='\'/' && *(WORD *)edx=='/\'')
	{
		pMem->ccmntblocks = 2;
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		edi = pMem->rpLineFree;
		edi /= sizeof(LINE);
		while(nLine<edi)
		{
			esi = nLine;
			esi *= sizeof(LINE);
			esi += pMem->hLine;
			esi = ((LINE *)esi)->rpChars;
			esi += pMem->hChars;
			temp1 = ((CHARS *)esi)->state;
			ecx = 0;
			ecx++;
			eax = nCmnt;
			fCmnt = eax;
			while(ecx<((CHARS *)esi)->len)
			{
				if(*(BYTE *)(esi+ecx+sizeof(CHARS)-1)=='\'' &&  *(WORD *)(esi+ecx+sizeof(CHARS)-1)!='/\'')
				{
					ecx = ((CHARS *)esi)->len;
				}
				else if(*(BYTE *)(esi+ecx+sizeof(CHARS)-1)=='"')
				{
					ecx++;
					while(ecx<((CHARS *)esi)->len && *(BYTE *)(esi+ecx+sizeof(CHARS)-1)!='"')
					{
						ecx++;
					} // endw
				}
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=='\'/')
				{
					ecx++;
					nCmnt++;
				}
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=='/\'')
				{
					ecx++;
					if(nCmnt)
					{
						nCmnt--;
					} // endif
				} // endif
				ecx++;
			} // endw
			((CHARS *)esi)->state &= -1 ^ (STATE_COMMENT | STATE_COMMENTNEST);
			if(nCmnt>1 || fCmnt)
			{
				((CHARS *)esi)->state |= STATE_COMMENT;
				if(nCmnt && fCmnt)
				{
					((CHARS *)esi)->state |= STATE_COMMENT | STATE_COMMENTNEST;
				} // endif
			} // endif
			eax = temp1;
			if(eax!=((CHARS *)esi)->state)
			{
				fChanged++;
			} // endif
			nLine++;
		} // endw
		if(fChanged)
		{
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		} // endif
		return eax;
	}
	else if(*(WORD *)eax=='{' && *(WORD *)edx=='}')
	{
		pMem->ccmntblocks = 3;
	}
	else
	{
		eax = lstrcmpi(lpStart, szComment);
		edx = lpEnd;
		if(!eax && *(WORD *)edx=='-')
		{
			pMem->ccmntblocks = 4;
		} // endif
		eax = lpStart;
	} // endif
	RBYTE_LOW(eax) = *(BYTE *)eax;
	if(RBYTE_LOW(eax))
	{
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		fCmnt = ecx;
		edi = pMem->rpLineFree;
		edi /= sizeof(LINE);
		while(nLine<edi)
		{
			esi = nLine;
			esi *= sizeof(LINE);
			esi += pMem->hLine;
			esi = ((LINE *)esi)->rpChars;
			esi += pMem->hChars;
			temp1 = ((CHARS *)esi)->state;
			edx = lpStart;
			RWORD(eax) = *(WORD *)edx;
			IsLineStart();
			if(!eax)
			{
				nCmnt++;
				fCmnt++;
			}
			else
			{
				ecx = 0;
			} // endif
			if(nCmnt>1 || (nCmnt && !fCmnt))
			{
				((CHARS *)esi)->state |= STATE_COMMENT;
			}
			else
			{
				((CHARS *)esi)->state &= -1 ^ STATE_COMMENT;
			} // endif
			fCmnt = 0;
			if(nCmnt)
			{
				edx = lpEnd;
				IsLineEnd();
				if(!eax)
				{
					nCmnt--;
				} // endif
			} // endif
			eax = temp1;
			if(eax!=((CHARS *)esi)->state)
			{
				fChanged++;
			} // endif
			nLine++;
		} // endw
		if(fChanged)
		{
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		} // endif
	} // endif
	return eax;

	void TestWrd(void)
	{
		REG_T temp1, temp2;
		temp1 = ecx;
		temp2 = edx;
		ecx--;
		edx--;
anon_17:
		edx++;
		RBYTE_LOW(eax) = *(BYTE *)edx;
		if(RBYTE_LOW(eax)==0)
		{
			goto anon_18;
		} // endif
TestWrd1:
		ecx++;
		RBYTE_HIGH(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)=='+')
		{
			if(RBYTE_HIGH(eax)==' ')
			{
				goto TestWrd1;
			} // endif
			if(RBYTE_HIGH(eax)==VK_TAB)
			{
				goto TestWrd1;
			} // endif
			cmntchar = RBYTE_HIGH(eax);
			goto anon_18;
		}
		else if(RBYTE_LOW(eax)=='-')
		{
			RBYTE_LOW(eax) = cmntchar;
		} // endif
		if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax)<='z')
		{
			RBYTE_LOW(eax) &= 0x5F;
		} // endif
		if(RBYTE_HIGH(eax)>='a' && RBYTE_HIGH(eax)<='z')
		{
			RBYTE_HIGH(eax) &= 0x5F;
		} // endif
		if(RBYTE_LOW(eax)==RBYTE_HIGH(eax))
		{
			goto anon_17;
		} // endif
		edx = temp2;
		ecx = temp1;
		return;
anon_18:
		edx = temp2;
		eax = temp1;
		eax = 0;
		return;

	}

	void IsLineStart(void)
	{
		ecx = 0;
		ecx--;
		eax = ecx;
anon_19:
		ecx++;
		if(ecx==((CHARS *)esi)->len)
		{
			goto anon_20;
		} // endif
		RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)==' ')
		{
			goto anon_19;
		} // endif
		if(RBYTE_LOW(eax)==VK_TAB)
		{
			goto anon_19;
		} // endif
		eax = *(BYTE *)(esi+ecx+sizeof(CHARS));
		eax = *(BYTE *)(eax+CharTab);
		if(eax==CT_CMNTCHAR)
		{
			goto anon_20;
		} // endif
		if(pMem->ccmntblocks && pMem->ccmntblocks!=4)
		{
			while(ecx<((CHARS *)esi)->len)
			{
				TestWrd();
				ecx++;
				if(!eax)
				{
					break;
				} // endif
			} // endw
		}
		else
		{
			TestWrd();
			ecx++;
		} // endif
anon_20:
		return;

	}

	void IsLineEnd(void)
	{
		while(ecx<((CHARS *)esi)->len)
		{
			TestWrd();
			ecx++;
			if(!eax)
			{
				break;
			} // endif
		} // endw
		return;

	}

} // SetCommentBlocks

REG_T SetChangedState(EDIT *pMem, DWORD fUpdate)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	DWORD nLine;
	DWORD fChanged;

	edx = 0;
	nLine = edx;
	fChanged = edx;
	edx = fUpdate;
	edi = pMem->rpLineFree;
	edi /= sizeof(LINE);
	while(nLine<edi)
	{
		esi = nLine;
		esi *= sizeof(LINE);
		esi += pMem->hLine;
		esi = ((LINE *)esi)->rpChars;
		esi += pMem->hChars;
		ecx = ((CHARS *)esi)->state;
		if(((CHARS *)esi)->state&STATE_CHANGED)
		{
			((CHARS *)esi)->state &= -1 ^ STATE_CHANGED;
			if(edx)
			{
				((CHARS *)esi)->state |= STATE_CHANGESAVED;
			} // endif
		}
		else if(!edx)
		{
			((CHARS *)esi)->state &= -1 ^ STATE_CHANGESAVED;
		} // endif
		if(ecx!=((CHARS *)esi)->state)
		{
			fChanged++;
		} // endif
		nLine++;
	} // endw
	if(fChanged)
	{
		eax = InvalidateEdit(pMem, pMem->edta.hwnd);
		eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	} // endif
	eax = fChanged;
	return eax;

} // SetChangedState

