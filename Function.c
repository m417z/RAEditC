#include "Function.h"

#include "Edit.h"
#include "Memory.h"
#include "Misc.h"
#include "Position.h"
#include "Undo.h"

REG_T FindTheText(EDIT *pMem, REG_T pFind, DWORD fMC, DWORD fWW, DWORD fWhiteSpace, DWORD cpMin, DWORD cpMax, DWORD fDir, REG_T *pnIgnore)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	DWORD nLine;
	DWORD lnlen;
	REG_T lpFind[16];
	DWORD len[16];
	BYTE findbuff[512];
	DWORD nIgnore;
	DWORD prev;
	DWORD cp;

	auto void TstFind(void);
	auto void TstLnDown(void);
	auto void TstLnUp(void);

	esi = 0;
	lnlen = esi;
	while(esi<16)
	{
		lpFind[esi] = 0;
		len[esi] = 0;
		esi++;
	} // endw
	esi = pFind;
	edi = &findbuff;
	lpFind[0] = edi;
	ecx = 0;
	edx = 0;
	while(*(BYTE *)esi && ecx<255 && edx<16)
	{
		RBYTE_LOW(eax) = *(BYTE *)esi;
		*(BYTE *)edi = RBYTE_LOW(eax);
		if(RBYTE_LOW(eax)==VK_RETURN)
		{
			edi++;
			*(BYTE *)edi = 0;
			edi++;
			ecx++;
			len[edx] = ecx;
			ecx = 0;
			edx++;
			lpFind[edx] = edi;
			edi--;
			ecx--;
		} // endif
		esi++;
		edi++;
		ecx++;
	} // endw
	*(BYTE *)edi = 0;
	len[edx] = ecx;
	if(fDir==1)
	{
		// Down
		eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
		nLine = edx;
		ecx = eax;
		cpMin -= ecx;
		edx = cpMin;
		eax = -1;
		while(edx<=cpMax)
		{
			nIgnore = 0;
			temp1 = nLine;
			esi = 0;
			while(len[esi])
			{
				TstLnDown();
				if(eax==-1)
				{
					break;
				} // endif
				nLine++;
				esi++;
				ecx = 0;
			} // endw
			nLine = temp1;
			if(eax!=-1)
			{
				break;
			} // endif
			edx = lnlen;
			cpMin += edx;
			edx = cpMin;
			nLine++;
			ecx = 0;
			eax = -1;
		} // endw
		if(eax>cpMax)
		{
			eax = -1;
		} // endif
	}
	else
	{
		// Up
		eax = cpMin;
		cp = eax;
		eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
		nLine = edx;
		ecx = eax;
		edx = cpMin;
		eax = -1;
		while(R_SIGNED(edx) >= (SDWORD)cpMax)
		{
			nIgnore = 0;
			temp1 = nLine;
			esi = 0;
			while(len[esi])
			{
				TstLnUp();
				if(eax==-1)
				{
					break;
				} // endif
				nLine++;
				esi++;
				ecx = 0;
			} // endw
			nLine = temp1;
			if(eax!=-1 && eax<=cp)
			{
				break;
			} // endif
			nLine--;
			edi = nLine;
			edi *= sizeof(LINE);
			eax = -1;
			if(edi>=pMem->rpLineFree)
			{
				break;
			} // endif
			eax = GetCpFromLine(pMem, nLine);
			cpMin = eax;
			edi += pMem->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += pMem->hChars;
			ecx = ((CHARS *)edi)->len;
			cpMin += ecx;
			edx = cpMin;
			eax = -1;
		} // endw
	} // endif
	*pnIgnore = nIgnore;
	return eax;

	void TstFind(void)
	{
		REG_T temp1, temp2;
		prev = 1;
		temp1 = ecx;
		temp2 = esi;
		esi = lpFind[esi];
		esi--;
		ecx--;
TstFind1:
		esi++;
		ecx++;
TstFind3:
		RBYTE_LOW(eax) = *(BYTE *)esi;
		if(RBYTE_LOW(eax)==0)
		{
			goto TstFind2;
		} // endif
		RBYTE_HIGH(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		if(fWhiteSpace)
		{
			edx = RBYTE_LOW(eax);
			edx = CharTab[edx];
			if((RBYTE_LOW(eax)==VK_SPACE || RBYTE_LOW(eax)==VK_TAB) && (RBYTE_HIGH(eax)==VK_SPACE || RBYTE_HIGH(eax)==VK_TAB))
			{
				while((*(BYTE *)(edi+ecx+sizeof(CHARS))==VK_SPACE || *(BYTE *)(edi+ecx+sizeof(CHARS))==VK_TAB) && ecx<((CHARS *)edi)->len)
				{
					ecx++;
					nIgnore++;
				} // endw
				while(*(BYTE *)esi==VK_SPACE || *(BYTE *)esi==VK_TAB)
				{
					esi++;
					nIgnore--;
				} // endw
				goto TstFind3;
			}
			else if((RBYTE_HIGH(eax)==VK_SPACE || RBYTE_HIGH(eax)==VK_TAB) && (!ecx || edx!=1 || prev!=1))
			{
				// Ignore whitespace
				while((*(BYTE *)(edi+ecx+sizeof(CHARS))==VK_SPACE || *(BYTE *)(edi+ecx+sizeof(CHARS))==VK_TAB) && ecx<((CHARS *)edi)->len)
				{
					ecx++;
					nIgnore++;
				} // endw
				prev = edx;
				goto TstFind3;
			} // endif
		} // endif
		if(RBYTE_LOW(eax)==RBYTE_HIGH(eax))
		{
			goto TstFind1;
		} // endif
		if(!fMC)
		{
			edx = RBYTE_LOW(eax);
			RBYTE_LOW(eax) = CaseTab[edx];
			if(RBYTE_LOW(eax)==RBYTE_HIGH(eax))
			{
				goto TstFind1;
			} // endif
		} // endif
		eax = 0;
		eax--;
TstFind2:
		if(fWW && !RBYTE_LOW(eax))
		{
			if(ecx<=((CHARS *)edi)->len)
			{
				eax = *(BYTE *)(edi+ecx+sizeof(CHARS));
				eax = eax+CharTab;
				RBYTE_LOW(eax) = *(BYTE *)eax;
				if(RBYTE_LOW(eax)==CT_CHAR)
				{
					eax = 0;
					eax--;
				}
				else
				{
					eax = 0;
				} // endif
			} // endif
		} // endif
		esi = temp2;
		ecx = temp1;
		return;

	}

	void TstLnDown(void)
	{
		edi = nLine;
		edi *= sizeof(LINE);
		if(edi<pMem->rpLineFree)
		{
			edi += pMem->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += pMem->hChars;
			if(!esi)
			{
				eax = ((CHARS *)edi)->len;
				lnlen = eax;
			} // endif
Nxt:
			eax = len[esi];
			eax += ecx;
			if(eax<=((CHARS *)edi)->len)
			{
				if(fWW && ecx)
				{
					eax = *(BYTE *)(edi+ecx+sizeof(CHARS)-1);
					if((BYTE)CharTab[eax]==CT_CHAR)
					{
						ecx++;
						goto Nxt;
					} // endif
				} // endif
				TstFind();
				if(RBYTE_LOW(eax)==0)
				{
					goto Found;
				} // endif
				if(!esi)
				{
					ecx++;
					goto Nxt;
				} // endif
			} // endif
		}
		else
		{
			// EOF
			if(fDir==1)
			{
				cpMin = -1;
				lnlen = 0;
			}
			else
			{
				cpMax = -1;
				lnlen = 0;
			} // endif
		} // endif
		eax = -1;
		return;
Found:
		if(!esi)
		{
			cpMin += ecx;
			lnlen -= ecx;
		} // endif
		eax = cpMin;
		return;

	}

	void TstLnUp(void)
	{
		edi = nLine;
		edi *= sizeof(LINE);
		if(edi<pMem->rpLineFree)
		{
			edi += pMem->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += pMem->hChars;
			if(!esi)
			{
				eax = ((CHARS *)edi)->len;
				lnlen = eax;
				if(cpMin<ecx)
				{
					cpMin -= ecx;
					goto NotFoundUp;
				} // endif
				cpMin -= ecx;
			}
			else
			{
				// carry with add operation
				if(edi<pMem->hChars)
				{
					goto NotFoundUp;
				} // endif
			} // endif
NxtUp:
			if(fWW && ecx)
			{
				eax = *(BYTE *)(edi+ecx+sizeof(CHARS)-1);
				if((BYTE)CharTab[eax]==CT_CHAR)
				{
					ecx--;
					if(R_SIGNED(ecx) >= 0)
					{
						goto NxtUp;
					} // endif
					goto NotFoundUp;
				} // endif
			} // endif
			TstFind();
			if(RBYTE_LOW(eax)==0)
			{
				goto FoundUp;
			} // endif
			if(!esi)
			{
				ecx--;
				if(R_SIGNED(ecx) >= 0)
				{
					goto NxtUp;
				} // endif
			} // endif
		}
		else
		{
			// EOF
			if(fDir==1)
			{
				cpMin = -1;
				lnlen = 0;
			}
			else
			{
				cpMax = -1;
				lnlen = 0;
			} // endif
		} // endif
NotFoundUp:
		eax = -1;
		return;
FoundUp:
		if(!esi)
		{
			cpMin += ecx;
			lnlen -= ecx;
		} // endif
		eax = cpMin;
		return;

	}

} // FindTheText

REG_T FindTextEx(EDIT *pMem, DWORD fFlag, REG_T lpFindTextEx)
{
	REG_T eax = 0, ecx, edx, esi;
	REG_T lpText;
	DWORD len;
	DWORD fMC;
	DWORD fWW;

	esi = lpFindTextEx;
	eax = ((FINDTEXTEX *)esi)->lpstrText;
	lpText = eax;
	eax = strlen(eax);
	if(eax)
	{
		len = eax;
		eax = 0;
		fMC = eax;
		fWW = eax;
		if(fFlag&FR_WHOLEWORD)
		{
			fWW++;
		} // endif
		if(fFlag&FR_MATCHCASE)
		{
			fMC++;
		} // endif
		eax = ((FINDTEXTEX *)esi)->chrg.cpMin;
		if(fFlag&FR_DOWN)
		{
			// Down
			eax = 0;
			if(fFlag&FR_IGNOREWHITESPACE)
			{
				eax++;
			} // endif
			ecx = ((FINDTEXTEX *)esi)->chrg.cpMax;
			if(ecx==-1)
			{
				ecx = -2;
			} // endif
			eax = FindTheText(pMem, lpText, fMC, fWW, eax, ((FINDTEXTEX *)esi)->chrg.cpMin, ecx, 1, &edx);
			len += edx;
		}
		else
		{
			// Up
			eax = 0;
			if(fFlag&FR_IGNOREWHITESPACE)
			{
				eax++;
			} // endif
			eax = FindTheText(pMem, lpText, fMC, fWW, eax, ((FINDTEXTEX *)esi)->chrg.cpMin, ((FINDTEXTEX *)esi)->chrg.cpMax, -1, &edx);
			len += edx;
		} // endif
		if(eax!=-1)
		{
			((FINDTEXTEX *)esi)->chrgText.cpMin = eax;
			edx = len;
			edx += eax;
			((FINDTEXTEX *)esi)->chrgText.cpMax = edx;
		} // endif
	}
	else
	{
		eax = -1;
	} // endif
	return eax;

} // FindTextEx

REG_T IsLine(EDIT *pMem, DWORD nLine, REG_T lpszTest)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T tmpesi;
	DWORD fCmnt;
	BOOL notfound;

	auto void TestLine(void);
	auto void SkipString(void);
	auto void SkipCmnt(void);
	auto void SkipSpc(void);
	auto void OptSkipWord(void);
	auto void SkipWord(void);
	auto void TestWord(void);

	notfound = FALSE;
	edi = nLine;
	edi *= sizeof(LINE);
	esi = lpszTest;
	eax = -1;
	if(edi<pMem->rpLineFree && *(BYTE *)esi)
	{
		edi = nLine;
		edi *= sizeof(LINE);
		TestLine();
	} // endif
	return eax;

	void TestLine(void)
	{
		REG_T temp1;
		ecx = 0;
		fCmnt = ecx;
		edi += pMem->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += pMem->hChars;
		if(((CHARS *)edi)->state&STATE_COMMENT)
		{
			RWORD(eax) = *(WORD *)esi;
			if(pMem->ccmntblocks==1 && RWORD(eax)!='/*' && RWORD(eax)!='*/')
			{
				goto Nf;
			}
			else if(pMem->ccmntblocks==2 && RWORD(eax)!='/\'' && RWORD(eax)!='\'/')
			{
				goto Nf;
			} // endif
		}
		else
		{
			SkipSpc();
			if(notfound)
			{
				goto Nf;
			} // endif
			if(eax!=0)
			{
				goto Nf;
			} // endif
		} // endif
Nxt:
		RWORD(eax) = *(WORD *)esi;
		if(RBYTE_HIGH(eax))
		{
			if(RWORD(eax)==' $')
			{
				SkipCmnt();
				if(notfound)
				{
					goto Nf;
				} // endif
				esi++;
				SkipWord();
				if(eax!=0)
				{
					goto Nf;
				} // endif
				RBYTE_LOW(eax) = *(BYTE *)esi;
				if(RBYTE_LOW(eax)==' ')
				{
					esi++;
					SkipSpc();
					if(notfound)
					{
						goto Nf;
					} // endif
					SkipCmnt();
					if(notfound)
					{
						goto Nf;
					} // endif
					if(eax!=0)
					{
						goto Nf;
					} // endif
				} // endif
			}
			else if(RWORD(eax)==' ?')
			{
				SkipCmnt();
				if(notfound)
				{
					goto Nf;
				} // endif
				esi += 2;
				temp1 = esi;
				TestWord();
				if(notfound)
				{
					goto Nf;
				} // endif
				esi = temp1;
				if(eax==0)
				{
					goto Found;
				} // endif
				esi--;
				SkipWord();
				if(eax!=0)
				{
					goto Nf;
				} // endif
				RBYTE_LOW(eax) = *(BYTE *)esi;
				if(RBYTE_LOW(eax)==' ')
				{
					esi++;
					SkipSpc();
					if(notfound)
					{
						goto Nf;
					} // endif
					SkipCmnt();
					if(notfound)
					{
						goto Nf;
					} // endif
					if(eax!=0)
					{
						goto Nf;
					} // endif
				} // endif
			}
			else if(RBYTE_LOW(eax)=='%')
			{
				SkipCmnt();
				if(notfound)
				{
					goto Nf;
				} // endif
				esi++;
				OptSkipWord();
				goto Nxt;
			}
			else if(RWORD(eax)=='\'/')
			{
				// comment init
				while(ecx<((CHARS *)edi)->len)
				{
					eax = *(BYTE *)(edi+ecx+sizeof(CHARS));
					eax = *(BYTE *)(eax+CharTab);
					if(eax==CT_STRING)
					{
						SkipString();
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='\'/')
					{
						ecx++;
						fCmnt++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/\'')
					{
						ecx++;
						fCmnt--;
					}
					else if(*(BYTE *)(edi+ecx+sizeof(CHARS))=='\'' && !fCmnt)
					{
						ecx = ((CHARS *)edi)->len;
					} // endif
					ecx++;
				} // endw
				if((SDWORD)fCmnt > 0)
				{
					eax = 0;
					goto Found;
				} // endif
				goto Nf;
			}
			else if(RWORD(eax)=='/\'')
			{
				// Comment end
				while(ecx<((CHARS *)edi)->len)
				{
					eax = *(BYTE *)(edi+ecx+sizeof(CHARS));
					eax = *(BYTE *)(eax+CharTab);
					if(eax==CT_STRING)
					{
						SkipString();
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/\'')
					{
						if(((CHARS *)edi)->state&STATE_COMMENT)
						{
							fCmnt--;
						} // endif
						ecx++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='\'/')
					{
						fCmnt++;
						ecx++;
					}
					else if(*(BYTE *)(edi+ecx+sizeof(CHARS))=='\'' && !fCmnt)
					{
						ecx = ((CHARS *)edi)->len;
					} // endif
					ecx++;
				} // endw
				if((SDWORD)fCmnt < 0)
				{
					eax = 0;
					goto Found;
				} // endif
				goto Nf;
			}
			else if(RWORD(eax)=='*/')
			{
				// comment init
				while(ecx<((CHARS *)edi)->len)
				{
					esi = *(BYTE *)(edi+ecx+sizeof(CHARS));
					esi = *(BYTE *)(esi+CharTab);
					if(esi==CT_STRING)
					{
						SkipString();
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='*/')
					{
						ecx++;
						fCmnt++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/*')
					{
						ecx++;
						fCmnt--;
					} // endif
					ecx++;
				} // endw
				if((SDWORD)fCmnt > 0)
				{
					eax = 0;
					goto Found;
				} // endif
				goto Nf;
			}
			else if(RWORD(eax)=='/*')
			{
				// Comment end
				while(ecx<((CHARS *)edi)->len)
				{
					if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/*')
					{
						fCmnt--;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=='*/')
					{
						fCmnt++;
					} // endif
					ecx++;
				} // endw
				if((SDWORD)fCmnt < 0)
				{
					eax = 0;
					goto Found;
				} // endif
				goto Nf;
			} // endif
			SkipCmnt();
			if(notfound)
			{
				goto Nf;
			} // endif
			TestWord();
			if(notfound)
			{
				goto Nf;
			} // endif
			if(eax!=0)
			{
				goto Nf;
			} // endif
			edx = 0;
		}
		else
		{
			SkipCmnt();
			if(notfound)
			{
				goto Nf;
			} // endif
			while(ecx<((CHARS *)edi)->len)
			{
				edx = 0;
				if(RBYTE_LOW(eax)==*(BYTE *)(edi+ecx+sizeof(CHARS)))
				{
					break;
				} // endif
				edx--;
				esi = *(BYTE *)(edi+ecx+sizeof(CHARS));
				esi = *(BYTE *)(esi+CharTab);
				if(esi==CT_CMNTCHAR)
				{
					break;
				}
				else if(esi==CT_CMNTDBLCHAR)
				{
					esi = *(BYTE *)(edi+ecx+sizeof(CHARS)+1);
					esi = *(BYTE *)(esi+CharTab);
					if(esi==CT_CMNTDBLCHAR)
					{
						break;
					} // endif
				}
				else if(esi==CT_STRING)
				{
					SkipString();
				} // endif
				ecx++;
			} // endw
		} // endif
		eax = edx;
Found:
		return;
Nf:
		eax = -1;
		return;

	}

	void SkipString(void)
	{
		REG_T temp1;
		temp1 = eax;
		RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		ecx++;
		while(ecx<((CHARS *)edi)->len)
		{
			if(RBYTE_LOW(eax)==*(BYTE *)(edi+ecx+sizeof(CHARS)))
			{
				break;
			} // endif
			ecx++;
		} // endw
		eax = temp1;
		return;

	}

	void SkipCmnt(void)
	{
		REG_T temp1;
		if(*(WORD *)(edi+ecx+sizeof(CHARS))=='\'/')
		{
			temp1 = eax;
			ecx++;
			while(ecx<((CHARS *)edi)->len)
			{
				ecx++;
				if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/\'')
				{
					break;
				} // endif
			} // endw
			if(*(WORD *)(edi+ecx+sizeof(CHARS))=='/\'')
			{
				ecx += 2;
			} // endif
			SkipSpc();
			eax = temp1;
		} // endif
		return;

	}

	void SkipSpc(void)
	{

SkipSpcStart:
		if(ecx<((CHARS *)edi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==VK_TAB || RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==':' || (RBYTE_LOW(eax)=='*' && pMem->ccmntblocks!=1))
			{
				ecx++;
				goto SkipSpcStart;
			}
			else if(RBYTE_LOW(eax)=='"')
			{
				SkipString();
				if(*(BYTE *)(edi+ecx+sizeof(CHARS))=='"')
				{
					ecx++;
				} // endif
				goto SkipSpcStart;
			}
			else if(RBYTE_LOW(eax)==bracketcont[0])
			{
				if(*(BYTE *)(edi+ecx+sizeof(CHARS)+1)==VK_RETURN)
				{
					nLine++;
					edi = nLine;
					edi *= sizeof(LINE);
					if(edi<pMem->rpLineFree)
					{
						edi += pMem->hLine;
						edi = ((LINE *)edi)->rpChars;
						edi += pMem->hChars;
						if(((CHARS *)edi)->state&STATE_COMMENT)
						{
							goto SkipSpcNf;
						} // endif
						ecx = 0;
						goto SkipSpcStart;
					}
					else
					{
						goto SkipSpcNf;
					} // endif
				} // endif
			} // endif
			eax = 0;
		}
		else
		{
			eax = -1;
		} // endif
		return;
SkipSpcNf:
		notfound = TRUE;
		return;

	}

	void OptSkipWord(void)
	{
		REG_T temp1;
		temp1 = ecx;
		while(ecx<((CHARS *)edi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)esi;
			esi++;
			RBYTE_HIGH(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
			ecx++;
			if(RBYTE_LOW(eax)==VK_SPACE && (RBYTE_HIGH(eax)==VK_SPACE || RBYTE_HIGH(eax)==VK_TAB))
			{
				eax = temp1;
				return;
			} // endif
			if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax)<='z')
			{
				RBYTE_LOW(eax) &= 0x5F;
			} // endif
			if(RBYTE_HIGH(eax)>='a' && RBYTE_HIGH(eax)<='z')
			{
				RBYTE_HIGH(eax) &= 0x5F;
			} // endif
			if(RBYTE_LOW(eax)!=RBYTE_HIGH(eax))
			{
				while(*(BYTE *)(esi-1)!=VK_SPACE)
				{
					esi++;
				} // endw
				break;
			} // endif
		} // endw
		ecx = temp1;
		return;

	}

	void SkipWord(void)
	{

SkipWordStart:
		if(ecx<((CHARS *)edi)->len)
		{
			eax = *(BYTE *)(edi+ecx+sizeof(CHARS));
			if(eax!=VK_TAB && eax!=' ' && eax!=':')
			{
				eax = eax+CharTab;
				RBYTE_LOW(eax) = *(BYTE *)eax;
				if(RBYTE_LOW(eax)==CT_CHAR || RBYTE_LOW(eax)==CT_HICHAR)
				{
					ecx++;
					goto SkipWordStart;
				}
				else
				{
					if(RBYTE_LOW(eax)==CT_CMNTCHAR)
					{
						ecx = ((CHARS *)edi)->len;
					} // endif
					eax = 0;
					eax--;
					return;
				} // endif
			} // endif
			eax = 0;
		}
		else
		{
			eax = 0;
			eax--;
		} // endif
		return;

	}

	void TestWord(void)
	{
		REG_T temp1;
		goto TestWordStart;

anon_1:
		esi++;
		ecx++;
		RBYTE_LOW(eax) = *(BYTE *)esi;
		if(ecx>=((CHARS *)edi)->len && RBYTE_LOW(eax))
		{
			eax = 0;
			eax--;
			return;
		} // endif

TestWordStart:
		RWORD(eax) = *(WORD *)esi;
		if(RBYTE_LOW(eax)==0)
		{
			goto anon_2;
		} // endif
		if(RBYTE_LOW(eax)==' ')
		{
			RWORD(eax) = *(WORD *)(edi+ecx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
			{
				SkipSpc();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				ecx--;
				goto anon_1;
			}
			else if(RBYTE_LOW(eax)=='(')
			{
				ecx--;
				goto anon_1;
			}
			else
			{
				eax = 0;
				eax--;
				return;
			} // endif
		}
		else if(RWORD(eax)=='(#')
		{
			esi++;
			while(ecx<((CHARS *)edi)->len)
			{
				if(*(BYTE *)(edi+ecx+sizeof(CHARS))=='(')
				{
					break;
				} // endif
				ecx++;
			} // endw
			eax = 0;
			if(*(BYTE *)(edi+ecx+sizeof(CHARS))!='(')
			{
				eax--;
			} // endif
			return;
		}
		else if(RWORD(eax)=='$$')
		{
			esi += 3;
			while(ecx<((CHARS *)edi)->len)
			{
				temp1 = esi;
				TestWord();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				if(!eax)
				{
					eax = temp1;
					eax = 0;
					return;
				} // endif
				esi = temp1;
				ecx++;
			} // endw
		}
		else if(RWORD(eax)=='!$')
		{
			esi += 3;
			while(ecx<((CHARS *)edi)->len)
			{
				temp1 = esi;
				SkipSpc();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				TestWord();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				if(!eax)
				{
					SkipSpc();
					if(notfound)
					{
						goto TestWordNf;
					} // endif
					SkipCmnt();
					if(notfound)
					{
						goto TestWordNf;
					} // endif
					eax = temp1;
					RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
					if(RBYTE_LOW(eax)==VK_RETURN || ecx==((CHARS *)edi)->len)
					{
						eax = 0;
					}
					else
					{
						eax = RBYTE_LOW(eax);
						eax = eax+CharTab;
						RBYTE_LOW(eax) = *(BYTE *)eax;
						if(RBYTE_LOW(eax)==CT_CMNTCHAR)
						{
							eax = 0;
						}
						else
						{
							eax = 0;
							eax--;
						} // endif
					} // endif
					return;
				} // endif
				esi = temp1;
				ecx++;
			} // endw
		}
		else if(RBYTE_LOW(eax)=='*')
		{
			eax = 0;
			temp1 = ecx;
			ecx = *(BYTE *)(edi+ecx+sizeof(CHARS));
			ecx = *(BYTE *)(ecx+CharTab);
			if(ecx!=CT_CHAR)
			{
				eax--;
			} // endif
			ecx = temp1;
			return;
		}
		else if(RBYTE_LOW(eax)=='!')
		{
			if(*(BYTE *)(esi-1)!=' ')
			{
				RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
				if(RBYTE_LOW(eax)!=' ' && RBYTE_LOW(eax)!=VK_TAB && RBYTE_LOW(eax)!=VK_RETURN)
				{
					eax = 0;
					eax--;
					return;
				} // endif
			} // endif
			SkipSpc();
			if(notfound)
			{
				goto TestWordNf;
			} // endif
			SkipCmnt();
			if(notfound)
			{
				goto TestWordNf;
			} // endif
			if(ecx==((CHARS *)edi)->len)
			{
				eax = 0;
				return;
			} // endif
			esi++;
			tmpesi = esi;
			RWORD(eax) = *(WORD *)esi;
			while(TRUE)
			{
				temp1 = ecx;
				TestWord();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				edx = temp1;
				eax++;
				if(eax)
				{
					break;
				} // endif
				esi = tmpesi;
				ecx = edx;
				SkipWord();
				if(eax)
				{
					ecx++;
				} // endif
				SkipSpc();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound)
				{
					goto TestWordNf;
				} // endif
				eax = 0;
				if(ecx>=((CHARS *)edi)->len)
				{
					break;
				} // endif
			} // endw
			return;
		}
		else if(RWORD(eax)==' $')
		{
			SkipWord();
			SkipSpc();
			if(notfound)
			{
				goto TestWordNf;
			} // endif
			SkipCmnt();
			if(notfound)
			{
				goto TestWordNf;
			} // endif
			esi++;
			esi++;
			goto TestWordStart;
		} // endif
		RBYTE_HIGH(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax)<='z')
		{
			RBYTE_LOW(eax) &= 0x5F;
		} // endif
		if(RBYTE_HIGH(eax)>='a' && RBYTE_HIGH(eax)<='z')
		{
			RBYTE_HIGH(eax) &= 0x5F;
		} // endif
		if(RBYTE_LOW(eax)=='$')
		{
			goto anon_2;
		} // endif
		if(RBYTE_LOW(eax)==RBYTE_HIGH(eax))
		{
			goto anon_1;
		} // endif
		eax = 0;
		eax--;
		return;
anon_2:
		if(RBYTE_LOW(eax)=='$')
		{
			eax = 0;
			if(ecx<((CHARS *)edi)->len)
			{
				temp1 = ecx;
				ecx = *(BYTE *)(edi+ecx+sizeof(CHARS));
				ecx = *(BYTE *)(ecx+CharTab);
				if(ecx==CT_CHAR || ecx==CT_HICHAR)
				{
					eax++;
				} // endif
				ecx = temp1;
			} // endif
			eax--;
		}
		else
		{
			eax = 0;
			if(ecx<((CHARS *)edi)->len)
			{
				temp1 = ecx;
				ecx = *(BYTE *)(edi+ecx+sizeof(CHARS));
				ecx = *(BYTE *)(ecx+CharTab);
				if(ecx==CT_CHAR || ecx==CT_HICHAR)
				{
					eax--;
				} // endif
				ecx = temp1;
			}
			else if(ecx>((CHARS *)edi)->len)
			{
				eax--;
			} // endif
		} // endif

TestWordNf:
		return;

	}

} // IsLine

REG_T SetBookMark(EDIT *pMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, edx;

	edx = nLine;
	edx *= sizeof(LINE);
	eax = 0;
	if(edx<pMem->rpLineFree)
	{
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		eax = nType;
		eax <<= 4;
		eax &= STATE_BMMASK;
		((CHARS *)edx)->state &= -1 ^ STATE_BMMASK;
		((CHARS *)edx)->state |= eax;
		nBmid++;
		if(!(((CHARS *)edx)->state&STATE_HIDDEN))
		{
			eax = nBmid;
			((CHARS *)edx)->bmid = eax;
		} // endif
	} // endif
	return eax;

} // SetBookMark

REG_T GetBookMark(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx;

	eax = 0;
	eax--;
	edx = nLine;
	edx *= sizeof(LINE);
	if(edx<pMem->rpLineFree)
	{
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		eax = ((CHARS *)edx)->state;
		eax &= STATE_BMMASK;
		eax >>= 4;
	} // endif
	return eax;

} // GetBookMark

REG_T ClearBookMarks(EDIT *pMem, DWORD nType)
{
	REG_T eax = 0, ecx, edx, edi;

	nType &= 15;
	eax = nType;
	eax <<= 4;
	edi = 0;
	while(edi<pMem->rpLineFree)
	{
		edx = edi;
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		ecx = ((CHARS *)edx)->state;
		ecx &= STATE_BMMASK;
		if(eax==ecx)
		{
			((CHARS *)edx)->state &= -1 ^ STATE_BMMASK;
			if(!(((CHARS *)edx)->state&STATE_HIDDEN))
			{
				((CHARS *)edx)->bmid = 0;
			} // endif
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // ClearBookMarks

REG_T NextBookMark(EDIT *pMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, ecx, edx, edi;
	DWORD fExpand;

	eax = nType;
	nType &= 15;
	eax &= 0x80000000;
	fExpand = eax;
	edi = nLine;
	edi++;
	edi *= sizeof(LINE);
	eax = 0;
	eax--;
	while(edi<pMem->rpLineFree)
	{
		edx = edi;
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		ecx = ((CHARS *)edx)->state;
		ecx &= STATE_BMMASK;
		ecx >>= 4;
		if(ecx==nType)
		{
			eax = edi;
			eax /= sizeof(LINE);
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextBookMark

REG_T NextBreakpoint(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx, edi;

	edi = nLine;
	edi++;
	edi *= sizeof(LINE);
	eax = 0;
	eax--;
	while(edi<pMem->rpLineFree)
	{
		edx = edi;
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		if(((CHARS *)edx)->state&STATE_BREAKPOINT)
		{
			eax = edi;
			eax /= sizeof(LINE);
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextBreakpoint

REG_T NextError(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx, edi;

	edi = nLine;
	edi++;
	edi *= sizeof(LINE);
	eax = 0;
	eax--;
	while(edi<pMem->rpLineFree)
	{
		edx = edi;
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		if(((CHARS *)edx)->errid)
		{
			eax = edi;
			eax /= sizeof(LINE);
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextError

REG_T PreviousBookMark(EDIT *pMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, ecx, edx, edi;
	DWORD fExpand;

	eax = nType;
	nType &= 15;
	eax &= 0x80000000;
	fExpand = eax;
	eax = 0;
	eax--;
	edi = nLine;
	edi--;
	edi *= sizeof(LINE);
	while(R_SIGNED(edi) >= 0)
	{
anon_3:
		edx = edi;
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		ecx = ((CHARS *)edx)->state;
		ecx &= STATE_BMMASK;
		ecx >>= 4;
		if(ecx==nType)
		{
			eax = edi;
			eax /= sizeof(LINE);
			break;
		} // endif
		edi -= sizeof(LINE);
	} // endw
	return eax;

} // PreviousBookMark

REG_T LockLine(EDIT *pMem, DWORD nLine, DWORD fLock)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fLock)
		{
			((CHARS *)eax)->state |= STATE_LOCKED;
		}
		else
		{
			((CHARS *)eax)->state &= -1 ^ STATE_LOCKED;
		} // endif
	} // endif
	return eax;

} // LockLine

REG_T IsLineLocked(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx;

	eax = 0;
	if(!(pMem->fstyle&STYLE_READONLY))
	{
		edx = nLine;
		edx *= sizeof(LINE);
		if(edx<pMem->rpLineFree)
		{
			edx += pMem->hLine;
			edx = ((LINE *)edx)->rpChars;
			edx += pMem->hChars;
			eax = ((CHARS *)edx)->state;
			eax &= STATE_LOCKED;
		} // endif
	}
	else
	{
		eax++;
	} // endif
	return eax;

} // IsLineLocked

REG_T HideLine(EDIT *pMem, DWORD nLine, DWORD fHide)
{
	REG_T eax = 0, ecx, edx;

	auto void SetYP(void);

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fHide)
		{
			if(!(((CHARS *)eax)->state&STATE_HIDDEN))
			{
				ecx = ((CHARS *)eax)->len;
				if(*(BYTE *)(eax+ecx+sizeof(CHARS)-1)==0x0D)
				{
					((CHARS *)eax)->state |= STATE_HIDDEN;
					pMem->nHidden++;
					SetYP();
					eax = 0;
					eax++;
					goto Ex;
				} // endif
			} // endif
		}
		else
		{
			if(((CHARS *)eax)->state&STATE_HIDDEN)
			{
				((CHARS *)eax)->state &= -1 ^ STATE_HIDDEN;
				pMem->nHidden--;
				SetYP();
				eax = 0;
				eax++;
				goto Ex;
			} // endif
		} // endif
	} // endif
	eax = 0;
Ex:
	return eax;

	void SetYP(void)
	{
		edx = nLine;
		eax = 0;
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
		return;

	}

} // HideLine

REG_T IsLineHidden(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_HIDDEN;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineHidden

REG_T NoBlockLine(EDIT *pMem, DWORD nLine, DWORD fNoBlock)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fNoBlock)
		{
			((CHARS *)eax)->state |= STATE_NOBLOCK;
		}
		else
		{
			((CHARS *)eax)->state &= -1 ^ STATE_NOBLOCK;
		} // endif
	} // endif
	return eax;

} // NoBlockLine

REG_T IsLineNoBlock(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_NOBLOCK;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineNoBlock

REG_T AltHiliteLine(EDIT *pMem, DWORD nLine, DWORD fAltHilite)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fAltHilite)
		{
			((CHARS *)eax)->state |= STATE_ALTHILITE;
		}
		else
		{
			((CHARS *)eax)->state &= -1 ^ STATE_ALTHILITE;
		} // endif
	} // endif
	return eax;

} // AltHiliteLine

REG_T IsLineAltHilite(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_ALTHILITE;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineAltHilite

REG_T SetBreakpoint(EDIT *pMem, DWORD nLine, DWORD fBreakpoint)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fBreakpoint)
		{
			((CHARS *)eax)->state |= STATE_BREAKPOINT;
		}
		else
		{
			((CHARS *)eax)->state &= -1 ^ STATE_BREAKPOINT;
		} // endif
	} // endif
	return eax;

} // SetBreakpoint

REG_T SetError(EDIT *pMem, DWORD nLine, DWORD nErrID)
{
	REG_T eax = 0, edx;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		edx = nErrID;
		((CHARS *)eax)->errid = edx;
	} // endif
	return eax;

} // SetError

REG_T GetError(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx;

	edx = nLine;
	edx *= sizeof(LINE);
	eax = 0;
	if(edx<pMem->rpLineFree)
	{
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		eax = ((CHARS *)edx)->errid;
	} // endif
	return eax;

} // GetError

REG_T SetRedText(EDIT *pMem, DWORD nLine, DWORD fRed)
{
	REG_T eax = 0;

	eax = nLine;
	eax *= sizeof(LINE);
	if(eax<pMem->rpLineFree)
	{
		eax += pMem->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += pMem->hChars;
		if(fRed)
		{
			((CHARS *)eax)->state |= STATE_REDTEXT;
		}
		else
		{
			((CHARS *)eax)->state &= -1 ^ STATE_REDTEXT;
		} // endif
	} // endif
	return eax;

} // SetRedText

REG_T GetLineState(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, edx;

	edx = nLine;
	edx *= sizeof(LINE);
	eax = 0;
	if(edx<pMem->rpLineFree)
	{
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		eax = ((CHARS *)edx)->state;
	} // endif
	return eax;

} // GetLineState

REG_T IsSelectionLocked(EDIT *pMem, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ecx, edx;
	REG_T temp1;
	DWORD nLineMax;

	eax = cpMin;
	if(eax>cpMax)
	{
		temp1 = eax;
		eax = cpMax;
		cpMax = temp1;
		cpMin = eax;
	} // endif
	eax = GetCharPtr(pMem, cpMax, &ecx, &edx);
	nLineMax = edx;
	eax = GetCharPtr(pMem, cpMin, &ecx, &edx);
	while(edx<=nLineMax)
	{
		temp1 = edx;
		eax = IsLineLocked(pMem, edx);
		edx = temp1;
		if(eax!=0)
		{
			goto Ex;
		} // endif
		edx++;
	} // endw
Ex:
	return eax;

} // IsSelectionLocked


REG_T TrimSpace(EDIT *pMem, DWORD nLine, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, edi;
	REG_T temp1, temp2, temp3;
	DWORD cp;

	edi = nLine;
	eax = GetCpFromLine(pMem, edi);
	cp = eax;
	edi *= sizeof(LINE);
	edx = 0;
	ecx = 0;
	if(edi<pMem->rpLineFree)
	{
		edi += pMem->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += pMem->hChars;
		edx = ((CHARS *)edi)->len;
		if(edx)
		{
			if(fLeft)
			{
				// Left trim (Not implemented)
			}
			else
			{
				// Right trim
				temp1 = edx;
				RBYTE_LOW(eax) = *(BYTE *)(edi+edx+sizeof(CHARS)-1);
				temp2 = eax;
				if(RBYTE_LOW(eax)==0x0D)
				{
					edx--;
				} // endif
				ecx = edx;
anon_4:
				RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS)-1);
				if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
				{
					ecx--;
					if(ecx!=0)
					{
						goto anon_4;
					} // endif
				} // endif
				eax = cp;
				eax += ecx;
				edx -= ecx;
				temp3 = edx;
				edx = edi+ecx+sizeof(CHARS);
				ecx = temp3;
				if(ecx)
				{
					temp3 = ecx;
					eax = SaveUndo(pMem, UNDO_DELETEBLOCK, eax, edx, ecx);
					ecx = temp3;
				} // endif
				eax = temp2;
				if(ecx)
				{
					((CHARS *)edi)->len -= ecx;
					edx = ((CHARS *)edi)->len;
					if(RBYTE_LOW(eax)==0x0D)
					{
						*(BYTE *)(edi+edx+sizeof(CHARS)-1) = RBYTE_LOW(eax);
					} // endif
					((CHARS *)edi)->state &= -1 ^ (STATE_CHANGED | STATE_CHANGESAVED);
					((CHARS *)edi)->state |= STATE_CHANGED;
				} // endif
				edx = temp1;
				edx -= ecx;
			} // endif
		} // endif
	} // endif
	if(ecx)
	{
		temp1 = edx;
		eax = 0;
		pMem->edta.topyp = eax;
		pMem->edta.topln = eax;
		pMem->edta.topcp = eax;
		pMem->edtb.topyp = eax;
		pMem->edtb.topln = eax;
		pMem->edtb.topcp = eax;
		if(!pMem->fChanged)
		{
			pMem->fChanged = TRUE;
			eax = InvalidateRect(pMem->hsta, NULL, TRUE);
		} // endif
		eax = GetTopFromYp(pMem, pMem->edta.hwnd, pMem->edta.cpy);
		eax = GetTopFromYp(pMem, pMem->edtb.hwnd, pMem->edtb.cpy);
		eax = InvalidateLine(pMem, pMem->edta.hwnd, nLine);
		eax = InvalidateLine(pMem, pMem->edtb.hwnd, nLine);
		pMem->nchange++;
		edx = temp1;
	} // endif
Ex:
	eax = edx;
	return eax;

} // TrimSpace

REG_T SkipSpace(EDIT *pMem, DWORD cp, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
	if(!fLeft)
	{
anon_5:
		if(edx<((CHARS *)esi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
			{
				edx++;
				goto anon_5;
			} // endif
		} // endif
	}
	else
	{
anon_6:
		if(edx)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS)-1);
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
			{
				edx--;
				goto anon_6;
			} // endif
		} // endif
	} // endif
	eax = pMem->cpLine;
	eax += edx;
	return eax;

} // SkipSpace

REG_T SkipWhiteSpace(EDIT *pMem, DWORD cp, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
	if(!fLeft)
	{
anon_7:
		if(edx<((CHARS *)esi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
			eax = IsChar(RBYTE_LOW(eax));
			if(RBYTE_LOW(eax)!=1)
			{
				edx++;
				goto anon_7;
			} // endif
		} // endif
	}
	else
	{
anon_8:
		if(edx)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
			eax = IsChar(RBYTE_LOW(eax));
			if(RBYTE_LOW(eax)!=1)
			{
				edx--;
				goto anon_8;
			} // endif
		} // endif
	} // endif
	eax = pMem->cpLine;
	eax += edx;
	return eax;

} // SkipWhiteSpace

REG_T GetWordStart(EDIT *pMem, DWORD cp, DWORD nType)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
anon_9:
	if(edx)
	{
		RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS)-1);
		if(RBYTE_LOW(eax)=='.' && nType)
		{
			edx--;
			goto anon_9;
		}
		else if(RBYTE_LOW(eax)=='>' && nType==2 && edx>2)
		{
			if(*(BYTE *)(esi+edx+sizeof(CHARS)-2)=='-')
			{
				edx--;
				edx--;
				goto anon_9;
			} // endif
		}
		else if(RBYTE_LOW(eax)==')' && nType==2)
		{
			ecx = 0;
			while(edx>1)
			{
				RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS)-1);
				if(RBYTE_LOW(eax)==')')
				{
					ecx++;
				}
				else if(RBYTE_LOW(eax)=='(')
				{
					ecx--;
					if(!ecx)
					{
						edx--;
						break;
					} // endif
				} // endif
				edx--;
			} // endw
			goto anon_9;
		}
		else
		{
			eax = IsChar(RBYTE_LOW(eax));
		} // endif
		if(RBYTE_LOW(eax)==1)
		{
			edx--;
			goto anon_9;
		} // endif
	} // endif
	eax = pMem->cpLine;
	eax += edx;
	return eax;

} // GetWordStart

REG_T GetLineStart(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	eax = pMem->cpLine;
	return eax;

} // GetLineStart

REG_T GetTabPos(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
	eax = 0;
	ecx = 0;
	while(R_SIGNED(ecx) < R_SIGNED(edx))
	{
		eax++;
		if(*(BYTE *)(esi+ecx+sizeof(CHARS))==VK_TAB || eax==pMem->nTab)
		{
			eax = 0;
		} // endif
		ecx++;
	} // endw
	return eax;

} // GetTabPos

REG_T GetWordEnd(EDIT *pMem, DWORD cp, DWORD nType)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
anon_10:
	if(edx<((CHARS *)esi)->len)
	{
		RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
		if(RBYTE_LOW(eax)=='.' && nType)
		{
			edx++;
			goto anon_10;
		}
		else if(RBYTE_LOW(eax)=='-' && nType==2 && *(BYTE *)(esi+edx+sizeof(CHARS)+1)=='>')
		{
			edx++;
			edx++;
			goto anon_10;
		}
		else if(RBYTE_LOW(eax)=='(' && nType==2)
		{
			ecx = 0;
			while(edx<((CHARS *)esi)->len)
			{
				RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
				if(RBYTE_LOW(eax)=='(')
				{
					ecx++;
				}
				else if(RBYTE_LOW(eax)==')')
				{
					ecx--;
					if(!ecx)
					{
						edx++;
						break;
					} // endif
				} // endif
				edx++;
			} // endw
			goto anon_10;
		}
		else
		{
			eax = IsChar(RBYTE_LOW(eax));
		} // endif
		if(RBYTE_LOW(eax)==1)
		{
			edx++;
			goto anon_10;
		} // endif
	} // endif
	eax = pMem->cpLine;
	eax += edx;
	return eax;


} // GetWordEnd

REG_T GetLineEnd(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, esi;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	esi = pMem->rpChars;
	esi += pMem->hChars;
	edx = eax;
anon_11:
	if(edx<((CHARS *)esi)->len)
	{
		RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
		eax = IsChar(RBYTE_LOW(eax));
		if(RBYTE_LOW(eax)==1)
		{
			edx++;
			goto anon_11;
		} // endif
	} // endif
	eax = pMem->cpLine;
	eax += ((CHARS *)esi)->len;
	eax--;
	if(*(BYTE *)(esi+eax+sizeof(CHARS))==VK_RETURN)
	{
		eax--;
	} // endif
	return eax;

} // GetLineEnd

REG_T StreamIn(EDIT *pMem, REG_T lParam)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	REG_T temp1;
	REG_T hCMem;
	DWORD dwRead;
	DWORD fUnicode;

	auto void ReadChars(void);

	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, MAXSTREAM*3+4096);
	hCMem = eax;
	eax = GlobalLock(hCMem);
	edi = 0;
	dwRead = edi;
	fUnicode = edi;
anon_12:
	esi = hCMem;
	esi += MAXSTREAM;
	ReadChars();
	if(eax!=0)
	{
		goto anon_13;
	} // endif
	if(dwRead)
	{
		if(!fUnicode)
		{
			eax = *(WORD *)esi;
			if(eax==0x0FEFF && dwRead>=2)
			{
				// Unicode
				eax = 2;
				pMem->funicode = TRUE;
				fUnicode = eax;
				esi += eax;
				dwRead -= eax;
			}
			else
			{
				fUnicode = 1;
				pMem->funicode = FALSE;
			} // endif
		} // endif
		if(fUnicode==2)
		{
			edx = dwRead;
			edx /= 2;
			eax = WideCharToMultiByte(CP_ACP, 0, esi, edx, hCMem, MAXSTREAM, NULL, NULL);
			dwRead = eax;
			esi = hCMem;
		} // endif
		ecx = 0;
		while(ecx<dwRead)
		{
			temp1 = ecx;
			eax = *(BYTE *)(esi+ecx);
			if(eax!=0x0A)
			{
				eax = InsertChar(pMem, edi, eax);
			} // endif
			ecx = temp1;
			edi++;
			ecx++;
		} // endw
		goto anon_12;
	} // endif
anon_13:
	eax = GlobalUnlock(hCMem);
	eax = GlobalFree(hCMem);
	pMem->nHidden = 0;
	return eax;

	void ReadChars(void)
	{
		edx = lParam;
		((EDITSTREAM *)edx)->dwError = 0;
		eax = &dwRead;
		eax = ((EDITSTREAMCALLBACK)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, esi, MAXSTREAM, eax);
		return;

	}

} // StreamIn

REG_T StreamOut(EDIT *pMem, REG_T lParam)
{
	REG_T eax = 0, ecx, edx, esi, edi;
	DWORD dwWrite;
	REG_T hCMem;
	DWORD nChars;

	auto void StreamUnicode(void);
	auto void StreamAnsi(void);
	auto void FillCMem(void);

	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, MAXSTREAM*3+4096);
	hCMem = eax;
	eax = GlobalLock(hCMem);
	esi = pMem->hLine;
	if(pMem->funicode)
	{
		// Save as unicode
		eax = hCMem;
		*(WORD *)eax = 0x0FEFF;
		nChars = 2;
		StreamAnsi();
anon_14:
		FillCMem();
		if(ecx==0)
		{
			goto Ex;
		} // endif
		StreamUnicode();
		if(eax==0)
		{
			goto anon_14;
		} // endif
	}
	else
	{
		// Save as ansi
anon_15:
		FillCMem();
		if(ecx==0)
		{
			goto Ex;
		} // endif
		StreamAnsi();
		if(eax==0)
		{
			goto anon_15;
		} // endif
	} // endif
Ex:
	eax = GlobalUnlock(hCMem);
	eax = GlobalFree(hCMem);
	return eax;

	void StreamUnicode(void)
	{
		eax = hCMem;
		eax += MAXSTREAM+1024;
		eax = MultiByteToWideChar(CP_ACP, 0, hCMem, nChars, eax, MAXSTREAM+1024);
		edx = lParam;
		((EDITSTREAM *)edx)->dwError = 0;
		eax = &dwWrite;
		ecx = nChars;
		ecx *= 2;
		edx = hCMem;
		edx += MAXSTREAM+1024;
		eax = ((EDITSTREAMCALLBACK)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, edx, ecx, eax);
		return;

	}

	void StreamAnsi(void)
	{
		edx = lParam;
		((EDITSTREAM *)edx)->dwError = 0;
		// lea		eax,dwWrite
		// push	eax
		// push	nChars
		// push	hCMem
		// mov		eax,((EDITSTREAM *)edx)->dwCookie
		// push	eax
		// call	((EDITSTREAM *)edx)->pfnCallback
		eax = &dwWrite;
		eax = ((EDITSTREAMCALLBACK)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, hCMem, nChars, eax);
		return;

	}

	void FillCMem(void)
	{
		REG_T temp1, temp2, temp3;
		ecx = 0;
		edx = 0;
		nChars = ecx;
		eax = esi;
		eax -= (REG_T)pMem->hLine;
		if(eax<pMem->rpLineFree)
		{
			temp1 = esi;
			edi = hCMem;
			esi = ((LINE *)esi)->rpChars;
			esi += pMem->hChars;
			while(ecx<((CHARS *)esi)->len)
			{
				RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
				*(BYTE *)edi = RBYTE_LOW(eax);
				ecx++;
				edi++;
				nChars++;
				if(RBYTE_LOW(eax)==0x0D)
				{
					*(BYTE *)edi = 0x0A;
					edx++;
					edi++;
					nChars++;
				} // endif
				if(nChars>=MAXSTREAM)
				{
					temp2 = ecx;
					temp3 = edx;
					if(pMem->funicode)
					{
						StreamUnicode();
					}
					else
					{
						StreamAnsi();
					} // endif
					edx = temp3;
					ecx = temp2;
					edi = hCMem;
					nChars = 0;
				} // endif
			} // endw
			esi = temp1;
			esi += sizeof(LINE);
		} // endif
		ecx += edx;
		return;


	}

} // StreamOut

REG_T HiliteLine(EDIT *pMem, DWORD nLine, DWORD nColor)
{
	REG_T eax = 0, edx;

	edx = nLine;
	edx *= sizeof(LINE);
	if(edx<pMem->rpLineFree)
	{
		edx += pMem->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += pMem->hChars;
		((CHARS *)edx)->state &= -1 ^ STATE_HILITEMASK;
		eax = nColor;
		eax &= STATE_HILITEMASK;
		((CHARS *)edx)->state |= eax;
		eax = InvalidateLine(pMem, pMem->edta.hwnd, nLine);
		eax = InvalidateLine(pMem, pMem->edtb.hwnd, nLine);
	} // endif
	eax = 0;
	return eax;

} // HiliteLine

REG_T SelChange(EDIT *pMem, DWORD nType)
{
	REG_T eax = 0, ecx, edx;
	RASELCHANGE sc;

	if(pMem->cpbrst!=-1)
	{
		pMem->cpbrst = -1;
		pMem->cpbren = -1;
		eax = InvalidateEdit(pMem, pMem->edta.hwnd);
		eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
	} // endif
	eax = GetCharPtr(pMem, pMem->cpMin, &ecx, &edx);
	edx = pMem->ID;
	eax = pMem->hwnd;
	sc.nmhdr.hwndFrom = eax;
	sc.nmhdr.idFrom = edx;
	sc.nmhdr.code = EN_SELCHANGE;
	if(!(pMem->nMode&MODE_BLOCK))
	{
		eax = pMem->cpMin;
		sc.chrg.cpMin = eax;
		eax = pMem->cpMax;
		sc.chrg.cpMax = eax;
	}
	else
	{
		eax = pMem->cpLine;
		eax += pMem->blrg.clMin;
		sc.chrg.cpMin = eax;
		sc.chrg.cpMax = eax;
	} // endif
	eax = nType;
	sc.seltyp = RWORD(eax);
	eax = pMem->line;
	sc.line = eax;
	eax = pMem->cpLine;
	sc.cpLine = eax;
	eax = pMem->rpChars;
	eax += pMem->hChars;
	sc.lpLine = eax;
	eax = pMem->rpLineFree;
	eax /= sizeof(LINE);
	eax--;
	sc.nlines = eax;
	eax = pMem->nHidden;
	sc.nhidden = eax;
	eax = pMem->nchange;
	eax -= pMem->nlastchange;
	if(eax)
	{
		pMem->nlastchange += eax;
		eax = TRUE;
	} // endif
	sc.fchanged = eax;
	ecx = pMem->nPageBreak;
	eax = 0;
	if(ecx)
	{
		eax = pMem->line;
		eax /= ecx;
	} // endif
	sc.npage = eax;
	eax = pMem->nWordGroup;
	sc.nWordGroup = eax;
	if(!pMem->nsplitt)
	{
		eax = pMem->cpMin;
		pMem->edta.cp = eax;
		pMem->edtb.cp = eax;
	} // endif
	eax = pMem->line;
	if(eax!=pMem->lastline)
	{
		if(pMem->fhilite)
		{
			eax = HiliteLine(pMem, pMem->lastline, 0);
			eax = HiliteLine(pMem, pMem->line, pMem->fhilite);
		} // endif
		eax = pMem->line;
		pMem->lastline = eax;
	} // endif
	eax = SendMessage(pMem->hpar, WM_NOTIFY, pMem->ID, &sc);
	return eax;

} // SelChange

REG_T AutoIndent(EDIT *pMem)
{
	REG_T eax = 0, edx, esi;
	REG_T temp1, temp2, temp3;
	DWORD nLine;

	eax = GetLineFromCp(pMem, pMem->cpMin);
	if(eax)
	{
		nLine = eax;
		edx = 0;
		temp1 = pMem->fOvr;
		pMem->fOvr = FALSE;
anon_16:
		eax = nLine;
		esi = pMem->hLine;
		esi = esi+eax*sizeof(LINE)-sizeof(LINE);
		esi = ((LINE *)esi)->rpChars;
		esi += pMem->hChars;
		if(edx<((CHARS *)esi)->len)
		{
			eax = *(BYTE *)(esi+edx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
			{
				temp2 = edx;
				temp3 = eax;
				eax = InsertChar(pMem, pMem->cpMin, eax);
				eax = temp3;
				eax = SaveUndo(pMem, UNDO_INSERT, pMem->cpMin, eax, 1);
				eax = pMem->cpMin;
				eax++;
				pMem->cpMin = eax;
				pMem->cpMax = eax;
				edx = temp2;
				edx++;
				goto anon_16;
			} // endif
		} // endif
		pMem->fOvr = temp1;
	} // endif
	return eax;

} // AutoIndent

REG_T IsCharPos(EDIT *pMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, esi;
	DWORD nMax;

	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	nMax = eax;
	esi = pMem->rpChars;
	esi += pMem->hChars;
	if(!(((CHARS *)esi)->state&STATE_COMMENT))
	{
		ecx = 0;
		while(ecx<nMax)
		{
			if(pMem->ccmntblocks==1 && *(WORD *)(esi+ecx+sizeof(CHARS))=='*/')
			{
				ecx += 2;
				while(ecx<nMax)
				{
					if(*(WORD *)(esi+ecx+sizeof(CHARS))=='/*')
					{
						break;
					} // endif
					ecx++;
				} // endw
				if(*(WORD *)(esi+ecx+sizeof(CHARS))=='/*')
				{
					ecx += 2;
				}
				else
				{
					// On comment block
					eax = 1;
					goto Ex;
				} // endif
			}
			else if(pMem->ccmntblocks==2 && *(WORD *)(esi+ecx+sizeof(CHARS))=='\'/')
			{
				ecx += 2;
				while(ecx<nMax)
				{
					if(*(WORD *)(esi+ecx+sizeof(CHARS))=='/\'')
					{
						break;
					} // endif
					ecx++;
				} // endw
				if(*(WORD *)(esi+ecx+sizeof(CHARS))=='/\'')
				{
					ecx += 2;
				}
				else
				{
					// On comment block
					eax = 1;
					goto Ex;
				} // endif
			}
			else
			{
				eax = *(BYTE *)(esi+ecx+sizeof(CHARS));
				RBYTE_LOW(eax) = *(BYTE *)(eax+CharTab);
				if(RBYTE_LOW(eax)==CT_CMNTCHAR)
				{
					eax = 2;
					goto Ex;
				}
				else if(RBYTE_LOW(eax)==CT_CMNTDBLCHAR)
				{
					RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
					RBYTE_HIGH(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS)+1);
					if(RBYTE_LOW(eax)==RBYTE_HIGH(eax) || RBYTE_HIGH(eax)=='*')
					{
						eax = 2;
						goto Ex;
					} // endif
				}
				else if(RBYTE_LOW(eax)==CT_STRING)
				{
					RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS));
					while(ecx<nMax)
					{
						ecx++;
						if(RBYTE_LOW(eax)==*(BYTE *)(esi+ecx+sizeof(CHARS)))
						{
							break;
						} // endif
					} // endw
					if(ecx>=nMax)
					{
						eax = 3;
						goto Ex;
					} // endif
				} // endif
				ecx++;
			} // endif
		} // endw
		eax = 0;
	}
	else
	{
		// On comment block
		eax = 1;
	} // endif
Ex:
	return eax;

} // IsCharPos

REG_T BracketMatchRight(EDIT *pMem, DWORD nChr, DWORD nMatch, DWORD cp)
{
	REG_T eax = 0, ecx, edx, edi;
	REG_T temp1;
	DWORD nCount;

	nCount = 0;
	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	edx = eax;
	edi = pMem->hChars;
	edi += pMem->rpChars;
	while(edx<=((CHARS *)edi)->len)
	{
		RBYTE_LOW(eax) = (BYTE)nMatch;
		RBYTE_HIGH(eax) = (BYTE)nChr;
		RBYTE_LOW(ecx) = bracketcont[0];
		RBYTE_HIGH(ecx) = bracketcont[1];
		if(RBYTE_LOW(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp1 = edx;
			eax = IsCharPos(pMem, cp);
			edx = temp1;
			if(!eax)
			{
				nCount--;
				if(nCount==0)
				{
					eax = edx;
					eax += pMem->cpLine;
					return eax;
				} // endif
			} // endif
		}
		else if(RBYTE_HIGH(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp1 = edx;
			eax = IsCharPos(pMem, cp);
			edx = temp1;
			if(!eax)
			{
				nCount++;
			} // endif
		}
		else if((RBYTE_LOW(ecx)==*(BYTE *)(edi+edx+sizeof(CHARS)) || RBYTE_HIGH(ecx)==*(BYTE *)(edi+edx+sizeof(CHARS))) && edx<=((CHARS *)edi)->len)
		{
			if(*(BYTE *)(edi+edx+sizeof(CHARS))!=VK_RETURN)
			{
				temp1 = edx;
				eax = IsCharPos(pMem, cp);
				edx = temp1;
				edx++;
				cp++;
				if(!eax)
				{
					while((*(BYTE *)(edi+edx+sizeof(CHARS))==VK_SPACE || *(BYTE *)(edi+edx+sizeof(CHARS))==VK_TAB) && edx<((CHARS *)edi)->len)
					{
						edx++;
						cp++;
					} // endw
				} // endif
				if(*(BYTE *)(edi+edx+sizeof(CHARS))==VK_RETURN)
				{
					cp++;
					eax = cp;
					eax = GetCharPtr(pMem, eax, &ecx, &edx);
					edx = eax;
					edi = pMem->hChars;
					edi += pMem->rpChars;
					edx = 0;
				} // endif
			}
			else
			{
				cp++;
				eax = cp;
				eax = GetCharPtr(pMem, eax, &ecx, &edx);
				edx = eax;
				edi = pMem->hChars;
				edi += pMem->rpChars;
				edx = 0;
			} // endif
			edx--;
			cp--;
		} // endif
		edx++;
		cp++;
	} // endw
	eax = 0;
	eax--;
	return eax;

} // BracketMatchRight

REG_T BracketMatchLeft(EDIT *pMem, DWORD nChr, DWORD nMatch, DWORD cp)
{
	REG_T eax = 0, ecx, edx, edi;
	REG_T temp1, temp2;
	DWORD nCount;

	nCount = 0;
	temp1 = cp;
	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	edx = eax;
	edi = pMem->hChars;
	edi += pMem->rpChars;
	while(R_SIGNED(edx) >= 0)
	{
		RBYTE_LOW(eax) = (BYTE)nMatch;
		RBYTE_HIGH(eax) = (BYTE)nChr;
		if(RBYTE_LOW(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp2 = edx;
			eax = IsCharPos(pMem, cp);
			edx = temp2;
			if(!eax)
			{
				nCount--;
				if(nCount==0)
				{
					eax = edx;
					eax += pMem->cpLine;
					goto Ex;
				} // endif
			} // endif
		}
		else if(RBYTE_HIGH(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp2 = edx;
			eax = IsCharPos(pMem, cp);
			edx = temp2;
			if(!eax)
			{
				nCount++;
			} // endif
		} // endif
		if(!edx && pMem->line)
		{
			cp--;
			eax = GetCharPtr(pMem, cp, &ecx, &edx);
			edx = eax;
			edi = pMem->hChars;
			edi += pMem->rpChars;
			while((*(BYTE *)(edi+edx+sizeof(CHARS))==VK_SPACE || *(BYTE *)(edi+edx+sizeof(CHARS))==VK_TAB) && edx!=0)
			{
				edx--;
				cp--;
			} // endw
			temp2 = edx;
			eax = IsCharPos(pMem, cp);
			edx = temp2;
			if(!eax)
			{
				if(bracketcont[0]!=VK_RETURN)
				{
					if(edx)
					{
						edx--;
						RBYTE_LOW(eax) = *(BYTE *)(edi+edx+sizeof(CHARS));
						if(RBYTE_LOW(eax)!=bracketcont[0] && RBYTE_LOW(eax)!=bracketcont[1])
						{
							break;
						} // endif
					} // endif
				} // endif
			} // endif
			cp++;
			edx++;
		} // endif
		edx--;
		cp--;
	} // endw
	eax = 0;
	eax--;
Ex:
	cp = temp1;
	temp1 = eax;
	eax = GetCharPtr(pMem, cp, &ecx, &edx);
	eax = temp1;
	return eax;

} // BracketMatchLeft

REG_T BracketMatch(EDIT *pMem, DWORD nChr, DWORD cp)
{
	REG_T eax = 0, ecx;
	REG_T temp1;

	if(pMem->cpbrst!=-1 || pMem->cpbren!=-1)
	{
		eax = InvalidateEdit(pMem, pMem->edta.hwnd);
		eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
		pMem->cpbrst = -1;
		pMem->cpbren = -1;
	} // endif
	RBYTE_LOW(eax) = (BYTE)nChr;
	ecx = 0;
	while((BYTE)bracketstart[ecx])
	{
		if(RBYTE_LOW(eax)==bracketstart[ecx])
		{
			temp1 = ecx;
			eax = IsCharPos(pMem, cp);
			ecx = temp1;
			if(eax!=0)
			{
				goto Ex;
			} // endif
			eax = (BYTE)bracketend[ecx];
			eax = BracketMatchRight(pMem, nChr, eax, cp);
			pMem->cpbren = eax;
			eax = cp;
			pMem->cpbrst = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = pMem->cpbren;
			goto Ex;
		} // endif
		ecx++;
	} // endw
	ecx = 0;
	while((BYTE)bracketend[ecx])
	{
		if(RBYTE_LOW(eax)==bracketend[ecx])
		{
			temp1 = ecx;
			eax = IsCharPos(pMem, cp);
			ecx = temp1;
			if(eax!=0)
			{
				goto Ex;
			} // endif
			eax = (BYTE)bracketstart[ecx];
			eax = BracketMatchLeft(pMem, nChr, eax, cp);
			pMem->cpbrst = eax;
			eax = cp;
			pMem->cpbren = eax;
			eax = InvalidateEdit(pMem, pMem->edta.hwnd);
			eax = InvalidateEdit(pMem, pMem->edtb.hwnd);
			eax = pMem->cpbrst;
			goto Ex;
		} // endif
		ecx++;
	} // endw
	eax = -1;
Ex:
	return eax;

} // BracketMatch

REG_T GetLineBegin(EDIT *pMem, DWORD nLine)
{
	REG_T eax = 0, ecx, esi, edi;

	eax = nLine;
	if(eax)
	{
		while(nLine)
		{
			nLine--;
			edi = nLine;
			edi *= sizeof(LINE);
			edi += pMem->hLine;
			esi = ((LINE *)edi)->rpChars;
			esi += pMem->hChars;
			ecx = ((CHARS *)esi)->len;
			if(ecx<2)
			{
				break;
			} // endif
			RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS)-2);
			if(RBYTE_LOW(eax)!=bracketcont[0] && RBYTE_LOW(eax)!=bracketcont[1])
			{
				break;
			} // endif
		} // endw
		eax = nLine;
		eax++;
	} // endif
	return eax;

} // GetLineBegin

