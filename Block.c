#include <windows.h>
#include "Data.h"

REG_T GetBlock(DWORD hMem, DWORD nLine, DWORD lpBlockDef)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLines;
	BYTE buffer[256];
	DWORD nNest;
	DWORD flag;

	auto void TestBlock(void);
	auto void SkipWhSp(void);
	auto void SkipWrd(void);
	auto void CopyWrd(void);

	ebx = hMem;
	nNest = 1;
	esi = lpBlockDef;
	eax = ((RABLOCKDEF *)esi)->flag;
	flag = eax;
	esi = ((RABLOCKDEF *)esi)->lpszEnd;
	if(esi)
	{
		edi = nLine;
		edi *= 4;
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
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
			esi = ((EDIT *)ebx)->rpLineFree;
			esi -= 4;
			nLine++;
			while(TRUE)
			{
				edi = nLine;
				edi *= 4;
				if(edi>=esi)
				{
					break;
				} // endif
				edi += ((EDIT *)ebx)->hLine;
				edi = ((LINE *)edi)->rpChars;
				edi += ((EDIT *)ebx)->hChars;
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
			esi = ((EDIT *)ebx)->rpLineFree;
			esi -= 4;
			nLine++;
			while(TRUE)
			{
				edi = nLine;
				edi *= 4;
				if(edi>=esi)
				{
					break;
				} // endif
				edi += ((EDIT *)ebx)->hLine;
				edi = ((LINE *)edi)->rpChars;
				edi += ((EDIT *)ebx)->hChars;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		nLines = 0;
		edi = nLine;
		while(TRUE)
		{
			eax = 0;
			eax--;
			ecx = edi;
			ecx *= 4;
			if(ecx>((EDIT *)ebx)->rpLineFree)
			{
				break;
			} // endif
			if(nNest)
			{
				ecx = lpBlockDef;
				eax = IsLine(ebx, edi, ((RABLOCKDEF *)ecx)->lpszStart);
				if(eax!=-1)
				{
					nNest++;
				} // endif
			} // endif
			eax = IsLine(ebx, edi, esi);
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
			ecx *= 4;
			if(ecx==((EDIT *)ebx)->rpLineFree)
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
					eax *= 4;
					if(eax>((EDIT *)ebx)->rpLineFree)
					{
						break;
					} // endif
					nLines++;
					temp2 = ecx;
					eax = IsLine(ebx, edi, esi);
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
					eax = IsLine(ebx, edi, eax);
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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

REG_T SetBlocks(DWORD hMem, DWORD lpLnrg, DWORD lpBlockDef)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;

	ebx = hMem;
	nLine = 0;
	eax = lpLnrg;
	if(eax)
	{
		eax = ((LINERANGE *)eax)->lnMin;
		nLine = eax;
		eax = ((LINERANGE *)eax)->lnMax;
		eax++;
		eax++;
	} // endif
	eax--;
	eax *= 4;
	esi = eax;
	if(esi>((EDIT *)ebx)->rpLineFree)
	{
		esi = ((EDIT *)ebx)->rpLineFree;
	} // endif
	nLine--;
anon_12:
	nLine++;
	edi = nLine;
	edi *= 4;
	if(edi<esi)
	{
		eax = IsLine(ebx, nLine, &szInclude);
		eax++;
		if(eax!=0)
		{
			goto anon_12;
		} // endif
		eax = IsLine(ebx, nLine, &szIncludelib);
		eax++;
		if(eax!=0)
		{
			goto anon_12;
		} // endif
		eax = lpBlockDef;
		eax = ((RABLOCKDEF *)eax)->lpszStart;
		eax = IsLine(ebx, nLine, eax);
		eax++;
		if(eax==0)
		{
			goto anon_12;
		} // endif
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
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
			eax = GetBlock(ebx, nLine, lpBlockDef);
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
			eax = GetBlock(ebx, nLine, lpBlockDef);
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
					edi *= 4;
					if(edi<esi)
					{
						edi += ((EDIT *)ebx)->hLine;
						edi = ((LINE *)edi)->rpChars;
						edi += ((EDIT *)ebx)->hChars;
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
				eax = GetBlock(ebx, nLine, lpBlockDef);
				if(eax!=-1)
				{
					edx = nLine;
					nLine += eax;
					while(edx<=nLine)
					{
						edx++;
						edi = edx;
						edi *= 4;
						if(edi<esi)
						{
							edi += ((EDIT *)ebx)->hLine;
							edi = ((LINE *)edi)->rpChars;
							edi += ((EDIT *)ebx)->hChars;
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

REG_T IsBlockDefEqual(DWORD lpRABLOCKDEF1, DWORD lpRABLOCKDEF2)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

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

REG_T IsInBlock(DWORD hMem, DWORD nLine, DWORD lpBlockDef)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edi = nLine;
	esi = lpBlockDef;
	esi = ((RABLOCKDEF *)esi)->lpszStart;
anon_13:
	eax = PreviousBookMark(ebx, edi, 1);
	edi = eax;
	eax++;
	if(eax)
	{
		eax = IsLine(ebx, edi, esi);
		eax++;
		if(eax==0)
		{
			goto anon_13;
		} // endif
		eax = GetBlock(ebx, edi, lpBlockDef);
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

REG_T TestBlockStart(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = nLine;
	esi *= 4;
	if(esi<((EDIT *)ebx)->rpLineFree)
	{
		esi += ((EDIT *)ebx)->hLine;
		esi = *(DWORD *)esi;
		esi += ((EDIT *)ebx)->hChars;
		if(!(((CHARS *)esi)->state&STATE_NOBLOCK))
		{
			esi = &blockdefs;
			edi = esi+32*4;
			while(*(DWORD *)esi)
			{
				eax = ((RABLOCKDEF *)edi)->flag;
				eax >>= 16;
				if(eax==((EDIT *)ebx)->nWordGroup)
				{
					eax = IsLine(ebx, nLine, ((RABLOCKDEF *)edi)->lpszStart);
					if(eax!=-1)
					{
						eax = edi;
						goto Ex;
					} // endif
				} // endif
				edi = *(DWORD *)esi;
				esi += 4;
			} // endw
		} // endif
	} // endif
	eax = 0;
	eax--;
Ex:
	return eax;

} // TestBlockStart

REG_T TestBlockEnd(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD lpSecond;

	ebx = hMem;
	esi = &blockdefs;
	edi = esi+32*4;
	while(*(DWORD *)esi)
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
		if(((RABLOCKDEF *)edi)->lpszEnd && eax==((EDIT *)ebx)->nWordGroup)
		{
			eax = nLine;
			eax *= 4;
			eax += ((EDIT *)ebx)->hLine;
			eax = ((LINE *)eax)->rpChars;
			eax += ((EDIT *)ebx)->hChars;
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
				eax = IsLine(ebx, nLine, ((RABLOCKDEF *)edi)->lpszEnd);
				if(eax!=-1)
				{
					eax = edi;
					goto Ex;
				}
				else if(lpSecond)
				{
					eax = IsLine(ebx, nLine, lpSecond);
					if(eax!=-1)
					{
						eax = edi;
						goto Ex;
					} // endif
				} // endif
			} // endif
		} // endif
		edi = *(DWORD *)esi;
		esi += 4;
	} // endw
	eax = 0;
	eax--;
Ex:
	return eax;

} // TestBlockEnd

REG_T CollapseGetEnd(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLines;
	DWORD nNest;
	DWORD nMax;
	DWORD Nest[256];

	ebx = hMem;
	nLines = 0;
	nNest = 0;
	eax = ((EDIT *)ebx)->rpLineFree;
	eax /= 4;
	nMax = eax;
	edi = nLine;
	eax = TestBlockStart(ebx, edi);
	if(eax!=-1)
	{
		edx = nNest;
		Nest[edx*4] = eax;
		if(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK)
		{
			edi++;
			while(edi<nMax)
			{
				esi = edi;
				esi *= 4;
				esi += ((EDIT *)ebx)->hLine;
				esi = *(DWORD *)esi;
				esi += ((EDIT *)ebx)->hChars;
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
					eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszStart);
					if(eax!=-1)
					{
						break;
					} // endif
					eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszEnd);
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
					eax = TestBlockStart(ebx, edi);
					if(eax!=-1)
					{
						if(!(((RABLOCKDEF *)eax)->flag&BD_SEGMENTBLOCK))
						{
							edx = nNest;
							Nest[edx*4] = eax;
							nNest++;
						} // endif
					}
					else
					{
						eax = TestBlockEnd(ebx, edi);
						if(eax!=-1)
						{
							edx = nNest;
							edx--;
							if(eax!=Nest[edx*4])
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

REG_T Collapse(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLines;
	DWORD nNest;
	DWORD nMax;
	DWORD fmasmcomment;

	ebx = hMem;
	eax = 0;
	nLines = eax;
	nNest = eax;
	fmasmcomment = eax;
	edi = nLine;
	eax = TestBlockStart(ebx, edi);
	if(eax!=-1)
	{
		esi = eax;
		eax = ((EDIT *)ebx)->rpLineFree;
		eax /= 4;
		nMax = eax;
		if(((RABLOCKDEF *)esi)->flag&BD_SEGMENTBLOCK)
		{
			eax = SetBookMark(ebx, edi, 2);
			edx = eax;
			edi++;
			while(edi<nMax)
			{
				esi = edi;
				esi *= 4;
				esi += ((EDIT *)ebx)->hLine;
				esi = *(DWORD *)esi;
				esi += ((EDIT *)ebx)->hChars;
				if(((CHARS *)esi)->state&STATE_SEGMENTBLOCK)
				{
					break;
				} // endif
				if(!(((CHARS *)esi)->state&STATE_HIDDEN))
				{
					((CHARS *)esi)->state |= STATE_HIDDEN;
					((CHARS *)esi)->bmid = edx;
					((EDIT *)ebx)->nHidden++;
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
					eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszStart);
					if(eax!=-1)
					{
						break;
					} // endif
					eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszEnd);
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
				eax = SetBookMark(ebx, edi, 2);
				edx = eax;
				edi++;
				while(edi<=nLines)
				{
					eax = 0;
					eax--;
					temp1 = edx;
					if(((RABLOCKDEF *)esi)->lpszNot1)
					{
						eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszNot1);
						if(eax==-1)
						{
							if(((RABLOCKDEF *)esi)->lpszNot2)
							{
								eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszNot2);
							} // endif
						} // endif
					} // endif
					edx = temp1;
					if(eax==-1)
					{
						temp1 = edi;
						edi *= 4;
						edi += ((EDIT *)ebx)->hLine;
						edi = *(DWORD *)edi;
						edi += ((EDIT *)ebx)->hChars;
						if(!(((CHARS *)edi)->state&STATE_HIDDEN))
						{
							((CHARS *)edi)->state |= STATE_HIDDEN;
							((CHARS *)edi)->bmid = edx;
							((EDIT *)ebx)->nHidden++;
						} // endif
						edi = temp1;
					} // endif
					edi++;
				} // endw
			}
			else
			{
				if(((RABLOCKDEF *)esi)->flag&BD_COMMENTBLOCK && ((EDIT *)ebx)->ccmntblocks==4)
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
							eax = TestBlockStart(ebx, edi);
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
							edx *= 4;
							edx += ((EDIT *)ebx)->hLine;
							edx = ((LINE *)edx)->rpChars;
							edx += ((EDIT *)ebx)->hChars;
							if(!(((CHARS *)edx)->state&STATE_COMMENT))
							{
								eax = 0;
							} // endif
						}
						else
						{
							eax = TestBlockEnd(ebx, edi);
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
								eax = SetBookMark(ebx, edi, 2);
								edx = eax;
								edi++;
								while(edi<=nLines)
								{
									eax = -1;
									if(!fmasmcomment)
									{
										temp1 = edx;
										eax = TestBlockStart(ebx, edi);
										if(eax!=-1)
										{
											nNest++;
										}
										else
										{
											eax = TestBlockEnd(ebx, edi);
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
												eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszNot1);
												if(eax==-1)
												{
													if(((RABLOCKDEF *)esi)->lpszNot2)
													{
														eax = IsLine(ebx, edi, ((RABLOCKDEF *)esi)->lpszNot2);
													} // endif
												} // endif
											} // endif
											edx = temp1;
										} // endif
									} // endif
									if(eax==-1)
									{
										temp1 = edi;
										edi *= 4;
										edi += ((EDIT *)ebx)->hLine;
										edi = *(DWORD *)edi;
										edi += ((EDIT *)ebx)->hChars;
										if(!(((CHARS *)edi)->state&STATE_HIDDEN))
										{
											((CHARS *)edi)->state |= STATE_HIDDEN;
											((CHARS *)edi)->bmid = edx;
											((EDIT *)ebx)->nHidden++;
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
		if(edx<((EDIT *)ebx)->edta.topln)
		{
			((EDIT *)ebx)->edta.topyp = eax;
			((EDIT *)ebx)->edta.topln = eax;
			((EDIT *)ebx)->edta.topcp = eax;
		} // endif
		if(edx<((EDIT *)ebx)->edtb.topln)
		{
			((EDIT *)ebx)->edtb.topyp = eax;
			((EDIT *)ebx)->edtb.topln = eax;
			((EDIT *)ebx)->edtb.topcp = eax;
		} // endif
		eax = ((EDIT *)ebx)->rpLineFree;
		eax /= 4;
		eax -= ((EDIT *)ebx)->nHidden;
		ecx = ((EDIT *)ebx)->fntinfo.fntht;
		eax *= ecx;
		ecx = 0;
		if(eax<((EDIT *)ebx)->edta.cpy)
		{
			((EDIT *)ebx)->edta.cpy = eax;
			((EDIT *)ebx)->edta.topyp = ecx;
			((EDIT *)ebx)->edta.topln = ecx;
			((EDIT *)ebx)->edta.topcp = ecx;
		} // endif
		if(eax<((EDIT *)ebx)->edtb.cpy)
		{
			((EDIT *)ebx)->edtb.cpy = eax;
			((EDIT *)ebx)->edtb.topyp = ecx;
			((EDIT *)ebx)->edtb.topln = ecx;
			((EDIT *)ebx)->edtb.topcp = ecx;
		} // endif
	} // endif
	return eax;

} // Collapse

REG_T CollapseAll(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, ((EDIT *)ebx)->cpMin);
	esi = 0;
	edi = ((EDIT *)ebx)->rpLineFree;
	edi /= 4;
anon_14:
	eax = PreviousBookMark(ebx, edi, 1);
	if(eax!=-1)
	{
		edi = eax;
		eax = Collapse(ebx, edi);
		if(eax!=-1)
		{
			esi++;
		} // endif
		goto anon_14;
	} // endif
	eax = esi;
	return eax;

} // CollapseAll

REG_T Expand(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	temp1 = ((EDIT *)ebx)->nHidden;
	esi = nLine;
	eax = 0;
	if(esi<((EDIT *)ebx)->edta.topln)
	{
		((EDIT *)ebx)->edta.topyp = eax;
		((EDIT *)ebx)->edta.topln = eax;
		((EDIT *)ebx)->edta.topcp = eax;
	} // endif
	if(esi<((EDIT *)ebx)->edtb.topln)
	{
		((EDIT *)ebx)->edtb.topyp = eax;
		((EDIT *)ebx)->edtb.topln = eax;
		((EDIT *)ebx)->edtb.topcp = eax;
	} // endif
	esi *= 4;
	if(esi>=((EDIT *)ebx)->rpLineFree)
	{
		goto Ex;
	} // endif
	esi += ((EDIT *)ebx)->hLine;
	ecx = ((EDIT *)ebx)->rpLineFree;
	ecx += ((EDIT *)ebx)->hLine;
	eax = ((LINE *)esi)->rpChars;
	eax += ((EDIT *)ebx)->hChars;
	if(((CHARS *)eax)->state&STATE_HIDDEN)
	{
		goto Ex;
	} // endif
	edi = ((LINE *)esi)->rpChars;
	edi += ((EDIT *)ebx)->hChars;
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
			edi += ((EDIT *)ebx)->hChars;
			if(((CHARS *)edi)->state&STATE_HIDDEN)
			{
				break;
			} // endif
			esi += sizeof(LINE);
		} // endw
		ecx = temp2;
		edi = ((LINE *)esi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		if(!(((CHARS *)edi)->state&STATE_HIDDEN))
		{
			goto Ex;
		} // endif
		edx = ((CHARS *)edi)->bmid;
		while(esi<ecx)
		{
			edi = ((LINE *)esi)->rpChars;
			edi += ((EDIT *)ebx)->hChars;
			if(edx==((CHARS *)edi)->bmid)
			{
				if(((CHARS *)edi)->state&STATE_HIDDEN)
				{
					((CHARS *)edi)->state &= -1 ^ STATE_HIDDEN;
					((EDIT *)ebx)->nHidden--;
				} // endif
			} // endif
			esi += sizeof(LINE);
		} // endw
	} // endif
Ex:
	eax = temp1;
	eax -= ((EDIT *)ebx)->nHidden;
	return eax;

} // Expand

REG_T ExpandAll(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	esi = 0;
	edi = 0;
	eax = GetBookMark(ebx, edi);
anon_15:
	if(eax==2)
	{
		eax = Expand(ebx, edi);
		esi++;
	} // endif
	eax = NextBookMark(ebx, edi, 2);
	if(eax!=-1)
	{
		edi = eax;
		eax = 2;
		goto anon_15;
	} // endif
	eax = esi;
	return eax;

} // ExpandAll

REG_T TestExpand(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	temp1 = ((EDIT *)ebx)->nHidden;
anon_16:
	eax = IsLineHidden(ebx, nLine);
	if(eax)
	{
		temp2 = nLine;
		while(eax && nLine)
		{
			nLine--;
			eax = IsLineHidden(ebx, nLine);
		} // endw
		eax = Expand(ebx, nLine);
		nLine = temp2;
		goto anon_16;
	} // endif
	eax = temp1;
	if(eax!=((EDIT *)ebx)->nHidden)
	{
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	} // endif
	return eax;

} // TestExpand

REG_T SetCommentBlocks(DWORD hMem, DWORD lpStart, DWORD lpEnd)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;
	DWORD nCmnt;
	DWORD fCmnt;
	DWORD cmntchar;
	DWORD fChanged;

	auto void TestWrd(void);
	auto void IsLineStart(void);
	auto void IsLineEnd(void);

	ebx = hMem;
	cmntchar = 0;
	((EDIT *)ebx)->ccmntblocks = 0;
	eax = lpStart;
	edx = lpEnd;
	if(*(WORD *)eax=='*/' && *(WORD *)edx=='/*')
	{
		((EDIT *)ebx)->ccmntblocks = 1;
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		edi = ((EDIT *)ebx)->rpLineFree;
		edi /= 4;
		while(nLine<edi)
		{
			esi = nLine;
			esi *= 4;
			esi += ((EDIT *)ebx)->hLine;
			esi = *(DWORD *)esi;
			esi += ((EDIT *)ebx)->hChars;
			temp1 = ((CHARS *)esi)->state;
			ecx = 0;
			ecx++;
			eax = nCmnt;
			fCmnt = eax;
			while(ecx<((CHARS *)esi)->len)
			{
				if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=="*/")
				{
					ecx++;
					nCmnt++;
				}
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=="/*")
				{
					ecx++;
					if(nCmnt)
					{
						nCmnt--;
					} // endif
				} // endif
				ecx++;
			} // endw
			((CHARS *)esi)->state &= -1 ^ STATE_COMMENT | STATE_COMMENTNEST;
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
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		} // endif
		return eax;
	}
	else if(*(WORD *)eax=="'/" && *(WORD *)edx=="/'")
	{
		((EDIT *)ebx)->ccmntblocks = 2;
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		edi = ((EDIT *)ebx)->rpLineFree;
		edi /= 4;
		while(nLine<edi)
		{
			esi = nLine;
			esi *= 4;
			esi += ((EDIT *)ebx)->hLine;
			esi = *(DWORD *)esi;
			esi += ((EDIT *)ebx)->hChars;
			temp1 = ((CHARS *)esi)->state;
			ecx = 0;
			ecx++;
			eax = nCmnt;
			fCmnt = eax;
			while(ecx<((CHARS *)esi)->len)
			{
				if(*(BYTE *)(esi+ecx+sizeof(CHARS)-1)=="'" &&  *(WORD *)(esi+ecx+sizeof(CHARS)-1)!="/'")
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
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=="'/")
				{
					ecx++;
					nCmnt++;
				}
				else if(*(WORD *)(esi+ecx+sizeof(CHARS)-1)=="/'")
				{
					ecx++;
					if(nCmnt)
					{
						nCmnt--;
					} // endif
				} // endif
				ecx++;
			} // endw
			((CHARS *)esi)->state &= -1 ^ STATE_COMMENT | STATE_COMMENTNEST;
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
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		} // endif
		return eax;
	}
	else if(*(WORD *)eax=='{' && *(WORD *)edx=='}')
	{
		((EDIT *)ebx)->ccmntblocks = 3;
	}
	else
	{
		eax = lstrcmpi(lpStart, &szComment);
		edx = lpEnd;
		if(!eax && *(WORD *)edx=='-')
		{
			((EDIT *)ebx)->ccmntblocks = 4;
		} // endif
		eax = lpStart;
	} // endif
	RBYTE_LOW(eax) = *(BYTE *)eax;
	if(RBYTE_LOW(eax))
	{
		ebx = hMem;
		ecx = 0;
		nLine = ecx;
		nCmnt = ecx;
		fChanged = ecx;
		fCmnt = ecx;
		edi = ((EDIT *)ebx)->rpLineFree;
		edi /= 4;
		while(nLine<edi)
		{
			esi = nLine;
			esi *= 4;
			esi += ((EDIT *)ebx)->hLine;
			esi = *(DWORD *)esi;
			esi += ((EDIT *)ebx)->hChars;
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
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		} // endif
	} // endif
	return eax;

	void TestWrd(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
			*(BYTE *)(&cmntchar) = RBYTE_HIGH(eax);
			goto anon_18;
		}
		else if(RBYTE_LOW(eax)=='-')
		{
			RBYTE_LOW(eax) = (BYTE)cmntchar;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		if(((EDIT *)ebx)->ccmntblocks && ((EDIT *)ebx)->ccmntblocks!=4)
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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

REG_T SetChangedState(DWORD hMem, DWORD fUpdate)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;
	DWORD fChanged;

	ebx = hMem;
	edx = 0;
	nLine = edx;
	fChanged = edx;
	edx = fUpdate;
	edi = ((EDIT *)ebx)->rpLineFree;
	edi /= 4;
	while(nLine<edi)
	{
		esi = nLine;
		esi *= 4;
		esi += ((EDIT *)ebx)->hLine;
		esi = *(DWORD *)esi;
		esi += ((EDIT *)ebx)->hChars;
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
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	} // endif
	eax = fChanged;
	return eax;

} // SetChangedState

