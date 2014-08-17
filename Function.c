#include <windows.h>
#include "Data.h"

REG_T FindTheText(DWORD hMem, DWORD pFind, DWORD fMC, DWORD fWW, DWORD fWhiteSpace, DWORD cpMin, DWORD cpMax, DWORD fDir)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;
	DWORD lnlen;
	DWORD lpFind[15];
	DWORD len[15];
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
		lpFind[esi*4] = 0;
		len[esi*4] = 0;
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
			len[edx*4] = ecx;
			ecx = 0;
			edx++;
			lpFind[edx*4] = edi;
			edi--;
			ecx--;
		} // endif
		esi++;
		edi++;
		ecx++;
	} // endw
	*(BYTE *)edi = 0;
	len[edx*4] = ecx;
	ebx = hMem;
	if(fDir==1)
	{
		// Down
		eax = GetCharPtr(ebx, cpMin);
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
			while(len[esi*4])
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
		eax = GetCharPtr(ebx, cpMin);
		nLine = edx;
		ecx = eax;
		edx = cpMin;
		eax = -1;
		while(R_SIGNED(edx) >= (SDWORD)cpMax)
		{
			nIgnore = 0;
			temp1 = nLine;
			esi = 0;
			while(len[esi*4])
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
			edi *= 4;
			eax = -1;
			if(edi>=((EDIT *)ebx)->rpLineFree)
			{
				break;
			} // endif
			eax = GetCpFromLine(ebx, nLine);
			cpMin = eax;
			edi += ((EDIT *)ebx)->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += ((EDIT *)ebx)->hChars;
			ecx = ((CHARS *)edi)->len;
			cpMin += ecx;
			edx = cpMin;
			eax = -1;
		} // endw
	} // endif
	edx = nIgnore;
	return eax;

	void TstFind(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		prev = 1;
		temp1 = ecx;
		temp2 = esi;
		esi = lpFind[esi*4];
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		edi = nLine;
		edi *= 4;
		if(edi<((EDIT *)ebx)->rpLineFree)
		{
			edi += ((EDIT *)ebx)->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += ((EDIT *)ebx)->hChars;
			if(!esi)
			{
				eax = ((CHARS *)edi)->len;
				lnlen = eax;
			} // endif
Nxt:
			eax = len[esi*4];
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		edi = nLine;
		edi *= 4;
		if(edi<((EDIT *)ebx)->rpLineFree)
		{
			edi += ((EDIT *)ebx)->hLine;
			edi = ((LINE *)edi)->rpChars;
			edi += ((EDIT *)ebx)->hChars;
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
				if(edi<((EDIT *)ebx)->hChars)
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

REG_T FindTextEx(DWORD hMem, DWORD fFlag, DWORD lpFindTextEx)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD lpText;
	DWORD len;
	DWORD fMC;
	DWORD fWW;

	ebx = hMem;
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
			eax = FindTheText(ebx, lpText, fMC, fWW, eax, ((FINDTEXTEX *)esi)->chrg.cpMin, ecx, 1);
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
			eax = FindTheText(ebx, lpText, fMC, fWW, eax, ((FINDTEXTEX *)esi)->chrg.cpMin, ((FINDTEXTEX *)esi)->chrg.cpMax, -1);
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

REG_T IsLine(DWORD hMem, DWORD nLine, DWORD lpszTest)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD tmpesi;
	DWORD fCmnt;
	// LOCAL	espsave:DWORD
	DWORD esisave;
	DWORD notfound;

	auto void TestLine(void);
	auto void SkipString(void);
	auto void SkipCmnt(void);
	auto void SkipSpc(void);
	auto void OptSkipWord(void);
	auto void SkipWord(void);
	auto void TestWord(void);

	// mov		eax,esp
	// sub		eax,4
	// mov		espsave,eax
	notfound = 0;
	ebx = hMem;
	edi = nLine;
	edi *= 4;
	esi = lpszTest;
	if(edi<((EDIT *)ebx)->rpLineFree && *(BYTE *)esi)
	{
		while(*(BYTE *)esi)
		{
			esisave = esi;
			edi = nLine;
			edi *= 4;
			TestLine();
			if(notfound!=0)
			{
				break;
			} // endif
			if(eax!=-1)
			{
				break;
			} // endif
			esi = esisave;
			eax = strlen(esi);
			esi = esi+eax+1;
			eax = -1;
		} // endw
	}
	else
	{
		eax = -1;
	} // endif
	return eax;

	void TestLine(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		fCmnt = ecx;
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		if(((CHARS *)edi)->state&STATE_COMMENT)
		{
			RWORD(eax) = *(WORD *)esi;
			if(((EDIT *)ebx)->ccmntblocks==1 && RWORD(eax)!="/*" && RWORD(eax)!="*/")
			{
				goto Nf;
			}
			else if(((EDIT *)ebx)->ccmntblocks==2 && RWORD(eax)!="/'" && RWORD(eax)!="'/")
			{
				goto Nf;
			} // endif
		}
		else
		{
			SkipSpc();
			if(notfound!=0)
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
				if(notfound!=0)
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
					if(notfound!=0)
					{
						goto Nf;
					} // endif
					SkipCmnt();
					if(notfound!=0)
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
				if(notfound!=0)
				{
					goto Nf;
				} // endif
				esi += 2;
				temp1 = esi;
				TestWord();
				if(notfound!=0)
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
					if(notfound!=0)
					{
						goto Nf;
					} // endif
					SkipCmnt();
					if(notfound!=0)
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
				if(notfound!=0)
				{
					goto Nf;
				} // endif
				esi++;
				OptSkipWord();
				goto Nxt;
			}
			else if(RWORD(eax)=="'/")
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
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="'/")
					{
						ecx++;
						fCmnt++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/'")
					{
						ecx++;
						fCmnt--;
					}
					else if(*(BYTE *)(edi+ecx+sizeof(CHARS))=="'" && !fCmnt)
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
			else if(RWORD(eax)=="/'")
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
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/'")
					{
						if(((CHARS *)edi)->state&STATE_COMMENT)
						{
							fCmnt--;
						} // endif
						ecx++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="'/")
					{
						fCmnt++;
						ecx++;
					}
					else if(*(BYTE *)(edi+ecx+sizeof(CHARS))=="'" && !fCmnt)
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
			else if(RWORD(eax)=="*/")
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
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="*/")
					{
						ecx++;
						fCmnt++;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/*")
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
			else if(RWORD(eax)=="/*")
			{
				// Comment end
				while(ecx<((CHARS *)edi)->len)
				{
					if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/*")
					{
						fCmnt--;
					}
					else if(*(WORD *)(edi+ecx+sizeof(CHARS))=="*/")
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
			if(notfound!=0)
			{
				goto Nf;
			} // endif
			TestWord();
			if(notfound!=0)
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
			if(notfound!=0)
			{
				goto Nf;
			} // endif
			while(ecx<((CHARS *)edi)->len)
			{
				edx = 0;
				if(RBYTE_LOW(eax)==[edi+ecx+sizeof(CHARS)])
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = eax;
		RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
		ecx++;
		while(ecx<((CHARS *)edi)->len)
		{
			if(RBYTE_LOW(eax)==[edi+ecx+sizeof(CHARS)])
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		if(*(WORD *)(edi+ecx+sizeof(CHARS))=="'/")
		{
			temp1 = eax;
			ecx++;
			while(ecx<((CHARS *)edi)->len)
			{
				ecx++;
				if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/'")
				{
					break;
				} // endif
			} // endw
			if(*(WORD *)(edi+ecx+sizeof(CHARS))=="/'")
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

SkipSpcStart:
		if(ecx<((CHARS *)edi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(edi+ecx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==VK_TAB || RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==':' || (RBYTE_LOW(eax)=='*' && ((EDIT *)ebx)->ccmntblocks!=1))
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
			else if(RBYTE_LOW(eax)==(BYTE)bracketcont)
			{
				if(*(BYTE *)(edi+ecx+sizeof(CHARS)+1)==VK_RETURN)
				{
					nLine++;
					edi = nLine;
					edi *= 4;
					if(edi<((EDIT *)ebx)->rpLineFree)
					{
						edi += ((EDIT *)ebx)->hLine;
						edi = ((LINE *)edi)->rpChars;
						edi += ((EDIT *)ebx)->hChars;
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
		// mov		esp,espsave
		notfound = 1;
		return;

	}

	void OptSkipWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
				if(notfound!=0)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound!=0)
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
				if(notfound!=0)
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
				if(notfound!=0)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound!=0)
				{
					goto TestWordNf;
				} // endif
				TestWord();
				if(notfound!=0)
				{
					goto TestWordNf;
				} // endif
				if(!eax)
				{
					SkipSpc();
					if(notfound!=0)
					{
						goto TestWordNf;
					} // endif
					SkipCmnt();
					if(notfound!=0)
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
			if(notfound!=0)
			{
				goto TestWordNf;
			} // endif
			SkipCmnt();
			if(notfound!=0)
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
				if(notfound!=0)
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
				if(notfound!=0)
				{
					goto TestWordNf;
				} // endif
				SkipCmnt();
				if(notfound!=0)
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
			if(notfound!=0)
			{
				goto TestWordNf;
			} // endif
			SkipCmnt();
			if(notfound!=0)
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

REG_T SetBookMark(DWORD hMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = nLine;
	edx *= 4;
	eax = 0;
	if(edx<((EDIT *)ebx)->rpLineFree)
	{
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
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

REG_T GetBookMark(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = 0;
	eax--;
	edx = nLine;
	edx *= 4;
	if(edx<((EDIT *)ebx)->rpLineFree)
	{
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)edx)->state;
		eax &= STATE_BMMASK;
		eax >>= 4;
	} // endif
	return eax;

} // GetBookMark

REG_T ClearBookMarks(DWORD hMem, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	nType &= 15;
	eax = nType;
	eax <<= 4;
	edi = 0;
	while(edi<((EDIT *)ebx)->rpLineFree)
	{
		edx = edi;
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
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

REG_T NextBookMark(DWORD hMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD fExpand;

	ebx = hMem;
	eax = nType;
	nType &= 15;
	eax &= 0x80000000;
	fExpand = eax;
	edi = nLine;
	edi++;
	edi *= 4;
	eax = 0;
	eax--;
	while(edi<((EDIT *)ebx)->rpLineFree)
	{
		edx = edi;
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		ecx = ((CHARS *)edx)->state;
		ecx &= STATE_BMMASK;
		ecx >>= 4;
		if(ecx==nType)
		{
			eax = edi;
			eax /= 4;
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextBookMark

REG_T NextBreakpoint(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edi = nLine;
	edi++;
	edi *= 4;
	eax = 0;
	eax--;
	while(edi<((EDIT *)ebx)->rpLineFree)
	{
		edx = edi;
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		if(((CHARS *)edx)->state&STATE_BREAKPOINT)
		{
			eax = edi;
			eax /= 4;
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextBreakpoint

REG_T NextError(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edi = nLine;
	edi++;
	edi *= 4;
	eax = 0;
	eax--;
	while(edi<((EDIT *)ebx)->rpLineFree)
	{
		edx = edi;
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		if(((CHARS *)edx)->errid)
		{
			eax = edi;
			eax /= 4;
			break;
		} // endif
		edi += sizeof(LINE);
	} // endw
	return eax;

} // NextError

REG_T PreviousBookMark(DWORD hMem, DWORD nLine, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD fExpand;

	ebx = hMem;
	eax = nType;
	nType &= 15;
	eax &= 0x80000000;
	fExpand = eax;
	eax = 0;
	eax--;
	edi = nLine;
	edi--;
	edi *= 4;
	while(R_SIGNED(edi) >= 0)
	{
anon_3:
		edx = edi;
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		ecx = ((CHARS *)edx)->state;
		ecx &= STATE_BMMASK;
		ecx >>= 4;
		if(ecx==nType)
		{
			eax = edi;
			eax /= 4;
			break;
		} // endif
		edi -= sizeof(LINE);
	} // endw
	return eax;

} // PreviousBookMark

REG_T LockLine(DWORD hMem, DWORD nLine, DWORD fLock)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
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

REG_T IsLineLocked(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = 0;
	if(!(((EDIT *)ebx)->fstyle&STYLE_READONLY))
	{
		edx = nLine;
		edx *= 4;
		if(edx<((EDIT *)ebx)->rpLineFree)
		{
			edx += ((EDIT *)ebx)->hLine;
			edx = ((LINE *)edx)->rpChars;
			edx += ((EDIT *)ebx)->hChars;
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

REG_T HideLine(DWORD hMem, DWORD nLine, DWORD fHide)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	auto void SetYP(void);

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		if(fHide)
		{
			if(!(((CHARS *)eax)->state&STATE_HIDDEN))
			{
				ecx = ((CHARS *)eax)->len;
				if(*(BYTE *)(eax+ecx+sizeof(CHARS)-1)==0x0D)
				{
					((CHARS *)eax)->state |= STATE_HIDDEN;
					((EDIT *)ebx)->nHidden++;
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
				((EDIT *)ebx)->nHidden--;
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		edx = nLine;
		eax = 0;
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
		return;

	}

} // HideLine

REG_T IsLineHidden(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_HIDDEN;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineHidden

REG_T NoBlockLine(DWORD hMem, DWORD nLine, DWORD fNoBlock)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
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

REG_T IsLineNoBlock(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_NOBLOCK;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineNoBlock

REG_T AltHiliteLine(DWORD hMem, DWORD nLine, DWORD fAltHilite)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
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

REG_T IsLineAltHilite(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)eax)->state;
		eax &= STATE_ALTHILITE;
	}
	else
	{
		eax = 0;
	} // endif
	return eax;

} // IsLineAltHilite

REG_T SetBreakpoint(DWORD hMem, DWORD nLine, DWORD fBreakpoint)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
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

REG_T SetError(DWORD hMem, DWORD nLine, DWORD nErrID)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
		edx = nErrID;
		((CHARS *)eax)->errid = edx;
	} // endif
	return eax;

} // SetError

REG_T GetError(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = nLine;
	edx *= 4;
	eax = 0;
	if(edx<((EDIT *)ebx)->rpLineFree)
	{
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)edx)->errid;
	} // endif
	return eax;

} // GetError

REG_T SetRedText(DWORD hMem, DWORD nLine, DWORD fRed)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = nLine;
	eax *= 4;
	if(eax<((EDIT *)ebx)->rpLineFree)
	{
		eax += ((EDIT *)ebx)->hLine;
		eax = ((LINE *)eax)->rpChars;
		eax += ((EDIT *)ebx)->hChars;
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

REG_T GetLineState(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = nLine;
	edx *= 4;
	eax = 0;
	if(edx<((EDIT *)ebx)->rpLineFree)
	{
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		eax = ((CHARS *)edx)->state;
	} // endif
	return eax;

} // GetLineState

REG_T IsSelectionLocked(DWORD hMem, DWORD cpMin, DWORD cpMax)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLineMax;

	ebx = hMem;
	eax = cpMin;
	if(eax>cpMax)
	{
		temp1 = eax;
		eax = cpMax;
		cpMax = temp1;
		cpMin = eax;
	} // endif
	eax = GetCharPtr(ebx, cpMax);
	nLineMax = edx;
	eax = GetCharPtr(ebx, cpMin);
	while(edx<=nLineMax)
	{
		temp1 = edx;
		eax = IsLineLocked(ebx, edx);
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


REG_T TrimSpace(DWORD hMem, DWORD nLine, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD cp;

	ebx = hMem;
	edi = nLine;
	eax = GetCpFromLine(ebx, edi);
	cp = eax;
	edi *= 4;
	edx = 0;
	if(edi<((EDIT *)ebx)->rpLineFree)
	{
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		edx = ((CHARS *)edi)->len;
		if(edx)
		{
			if(fLeft)
			{
				// Left trim (Not implemented)
				ecx = 0;
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
					eax = SaveUndo(ebx, UNDO_DELETEBLOCK, eax, edx, ecx);
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
		((EDIT *)ebx)->edta.topyp = eax;
		((EDIT *)ebx)->edta.topln = eax;
		((EDIT *)ebx)->edta.topcp = eax;
		((EDIT *)ebx)->edtb.topyp = eax;
		((EDIT *)ebx)->edtb.topln = eax;
		((EDIT *)ebx)->edtb.topcp = eax;
		if(!((EDIT *)ebx)->fChanged)
		{
			((EDIT *)ebx)->fChanged = TRUE;
			eax = InvalidateRect(((EDIT *)ebx)->hsta, NULL, TRUE);
		} // endif
		eax = GetTopFromYp(ebx, ((EDIT *)ebx)->edta.hwnd, ((EDIT *)ebx)->edta.cpy);
		eax = GetTopFromYp(ebx, ((EDIT *)ebx)->edtb.hwnd, ((EDIT *)ebx)->edtb.cpy);
		eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, nLine);
		eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, nLine);
		((EDIT *)ebx)->nchange++;
		edx = temp1;
	} // endif
Ex:
	eax = edx;
	return eax;

} // TrimSpace

REG_T SkipSpace(DWORD hMem, DWORD cp, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
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
	eax = ((EDIT *)ebx)->cpLine;
	eax += edx;
	return eax;

} // SkipSpace

REG_T SkipWhiteSpace(DWORD hMem, DWORD cp, DWORD fLeft)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	edx = eax;
	if(!fLeft)
	{
anon_7:
		if(edx<((CHARS *)esi)->len)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
			eax = IsChar();
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
			eax = IsChar();
			if(RBYTE_LOW(eax)!=1)
			{
				edx--;
				goto anon_8;
			} // endif
		} // endif
	} // endif
	eax = ((EDIT *)ebx)->cpLine;
	eax += edx;
	return eax;

} // SkipWhiteSpace

REG_T GetWordStart(DWORD hMem, DWORD cp, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
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
				if(RBYTE_LOW(eax)==")")
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
			eax = IsChar();
		} // endif
		if(RBYTE_LOW(eax)==1)
		{
			edx--;
			goto anon_9;
		} // endif
	} // endif
	eax = ((EDIT *)ebx)->cpLine;
	eax += edx;
	return eax;

} // GetWordStart

REG_T GetLineStart(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	eax = ((EDIT *)ebx)->cpLine;
	return eax;

} // GetLineStart

REG_T GetTabPos(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	edx = eax;
	eax = 0;
	ecx = 0;
	while(R_SIGNED(ecx) < R_SIGNED(edx))
	{
		eax++;
		if(*(BYTE *)(esi+ecx+sizeof(CHARS))==VK_TAB || eax==((EDIT *)ebx)->nTab)
		{
			eax = 0;
		} // endif
		ecx++;
	} // endw
	return eax;

} // GetTabPos

REG_T GetWordEnd(DWORD hMem, DWORD cp, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
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
				if(RBYTE_LOW(eax)=="(")
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
			eax = IsChar();
		} // endif
		if(RBYTE_LOW(eax)==1)
		{
			edx++;
			goto anon_10;
		} // endif
	} // endif
	eax = ((EDIT *)ebx)->cpLine;
	eax += edx;
	return eax;


} // GetWordEnd

REG_T GetLineEnd(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	edx = eax;
anon_11:
	if(edx<((CHARS *)esi)->len)
	{
		RBYTE_LOW(eax) = *(BYTE *)(esi+edx+sizeof(CHARS));
		eax = IsChar();
		if(RBYTE_LOW(eax)==1)
		{
			edx++;
			goto anon_11;
		} // endif
	} // endif
	eax = ((EDIT *)ebx)->cpLine;
	eax += ((CHARS *)esi)->len;
	eax--;
	if(*(BYTE *)(esi+eax+sizeof(CHARS))==VK_RETURN)
	{
		eax--;
	} // endif
	return eax;

} // GetLineEnd

REG_T StreamIn(DWORD hMem, DWORD lParam)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD hCMem;
	DWORD dwRead;
	DWORD fUnicode;

	auto void ReadChars(void);

	ebx = hMem;
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
				((EDIT *)ebx)->funicode = TRUE;
				fUnicode = eax;
				esi += eax;
				dwRead -= eax;
			}
			else
			{
				fUnicode = 1;
				((EDIT *)ebx)->funicode = FALSE;
			} // endif
		} // endif
		if(fUnicode==2)
		{
			edx = dwRead;
			edx /= 1;
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
				eax = InsertChar(ebx, edi, eax);
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
	((EDIT *)ebx)->nHidden = 0;
	return eax;

	void ReadChars(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		edx = lParam;
		((EDITSTREAM *)edx)->dwError = 0;
		// lea		eax,dwRead
		// push	eax
		// mov		eax,MAXSTREAM
		// push	eax
		// mov		eax,esi
		// push	eax
		// push	((EDITSTREAM *)edx)->dwCookie
		// call	((EDITSTREAM *)edx)->pfnCallback
		eax = &dwRead;
		eax = ((EDITSTREAMCALLBACKPTR)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, esi, MAXSTREAM, eax);
		return;

	}

} // StreamIn

REG_T StreamOut(DWORD hMem, DWORD lParam)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD dwWrite;
	DWORD hCMem;
	DWORD nChars;

	auto void StreamUnicode(void);
	auto void StreamAnsi(void);
	auto void FillCMem(void);

	ebx = hMem;
	eax = xGlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, MAXSTREAM*3+4096);
	hCMem = eax;
	eax = GlobalLock(hCMem);
	esi = ((EDIT *)ebx)->hLine;
	if(((EDIT *)ebx)->funicode)
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
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = hCMem;
		eax += MAXSTREAM+1024;
		eax = MultiByteToWideChar(CP_ACP, 0, hCMem, nChars, eax, MAXSTREAM+1024);
		edx = lParam;
		((EDITSTREAM *)edx)->dwError = 0;
		// lea		eax,dwWrite
		// push	eax
		// mov		eax,nChars
		// shl		eax,1
		// push	eax
		// mov		eax,hCMem
		// add		eax,MAXSTREAM+1024
		// push	eax
		// mov		eax,((EDITSTREAM *)edx)->dwCookie
		// push	eax
		// call	((EDITSTREAM *)edx)->pfnCallback
		eax = &dwWrite;
		ecx = nChars;
		ecx *= 1;
		edx = hCMem;
		edx += MAXSTREAM+1024;
		eax = ((EDITSTREAMCALLBACKPTR)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, edx, ecx, eax);
		return;

	}

	void StreamAnsi(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
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
		eax = ((EDITSTREAMCALLBACKPTR)((EDITSTREAM *)edx)->pfnCallback)(((EDITSTREAM *)edx)->dwCookie, hCMem, nChars, eax);
		return;

	}

	void FillCMem(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		edx = 0;
		nChars = ecx;
		eax = esi;
		eax -= ((EDIT *)ebx)->hLine;
		if(eax<((EDIT *)ebx)->rpLineFree)
		{
			temp1 = esi;
			edi = hCMem;
			esi = ((LINE *)esi)->rpChars;
			esi += ((EDIT *)ebx)->hChars;
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
					if(((EDIT *)ebx)->funicode)
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

REG_T HiliteLine(DWORD hMem, DWORD nLine, DWORD nColor)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	edx = nLine;
	edx *= 4;
	if(edx<((EDIT *)ebx)->rpLineFree)
	{
		edx += ((EDIT *)ebx)->hLine;
		edx = ((LINE *)edx)->rpChars;
		edx += ((EDIT *)ebx)->hChars;
		((CHARS *)edx)->state &= -1 ^ STATE_HILITEMASK;
		eax = nColor;
		eax &= STATE_HILITEMASK;
		((CHARS *)edx)->state |= eax;
		eax = InvalidateLine(ebx, ((EDIT *)ebx)->edta.hwnd, nLine);
		eax = InvalidateLine(ebx, ((EDIT *)ebx)->edtb.hwnd, nLine);
	} // endif
	eax = 0;
	return eax;

} // HiliteLine

REG_T SelChange(DWORD hMem, DWORD nType)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	RASELCHANGE sc;

	ebx = hMem;
	if(((EDIT *)ebx)->cpbrst!=-1)
	{
		((EDIT *)ebx)->cpbrst = -1;
		((EDIT *)ebx)->cpbren = -1;
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
	} // endif
	eax = GetCharPtr(hMem, ((EDIT *)ebx)->cpMin);
	edx = ((EDIT *)ebx)->ID;
	eax = ((EDIT *)ebx)->hwnd;
	sc.nmhdr.hwndFrom = eax;
	sc.nmhdr.idFrom = edx;
	sc.nmhdr.code = EN_SELCHANGE;
	if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
	{
		eax = ((EDIT *)ebx)->cpMin;
		sc.chrg.cpMin = eax;
		eax = ((EDIT *)ebx)->cpMax;
		sc.chrg.cpMax = eax;
	}
	else
	{
		eax = ((EDIT *)ebx)->cpLine;
		eax += ((EDIT *)ebx)->blrg.clMin;
		sc.chrg.cpMin = eax;
		sc.chrg.cpMax = eax;
	} // endif
	eax = nType;
	sc.seltyp = RWORD(eax);
	eax = ((EDIT *)ebx)->line;
	sc.line = eax;
	eax = ((EDIT *)ebx)->cpLine;
	sc.cpLine = eax;
	eax = ((EDIT *)ebx)->rpChars;
	eax += ((EDIT *)ebx)->hChars;
	sc.lpLine = eax;
	eax = ((EDIT *)ebx)->rpLineFree;
	eax /= 4;
	eax--;
	sc.nlines = eax;
	eax = ((EDIT *)ebx)->nHidden;
	sc.nhidden = eax;
	eax = ((EDIT *)ebx)->nchange;
	eax -= ((EDIT *)ebx)->nlastchange;
	if(eax)
	{
		((EDIT *)ebx)->nlastchange += eax;
		eax = TRUE;
	} // endif
	sc.fchanged = eax;
	ecx = ((EDIT *)ebx)->nPageBreak;
	eax = 0;
	if(ecx)
	{
		eax = ((EDIT *)ebx)->line;
		// xor		edx,edx
		eax /= ecx;
	} // endif
	sc.npage = eax;
	eax = ((EDIT *)ebx)->nWordGroup;
	sc.nWordGroup = eax;
	if(!((EDIT *)ebx)->nsplitt)
	{
		eax = ((EDIT *)ebx)->cpMin;
		((EDIT *)ebx)->edta.cp = eax;
		((EDIT *)ebx)->edtb.cp = eax;
	} // endif
	eax = ((EDIT *)ebx)->line;
	if(eax!=((EDIT *)ebx)->lastline)
	{
		if(((EDIT *)ebx)->fhilite)
		{
			eax = HiliteLine(ebx, ((EDIT *)ebx)->lastline, 0);
			eax = HiliteLine(ebx, ((EDIT *)ebx)->line, ((EDIT *)ebx)->fhilite);
		} // endif
		eax = ((EDIT *)ebx)->line;
		((EDIT *)ebx)->lastline = eax;
	} // endif
	eax = SendMessage(((EDIT *)ebx)->hpar, WM_NOTIFY, ((EDIT *)ebx)->ID, &sc);
	return eax;

} // SelChange

REG_T AutoIndent(DWORD hMem)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLine;

	ebx = hMem;
	eax = GetLineFromCp(ebx, ((EDIT *)ebx)->cpMin);
	if(eax)
	{
		nLine = eax;
		edx = 0;
		temp1 = ((EDIT *)ebx)->fOvr;
		((EDIT *)ebx)->fOvr = FALSE;
anon_16:
		eax = nLine;
		esi = ((EDIT *)ebx)->hLine;
		esi = esi+eax*sizeof(LINE)-sizeof(LINE);
		esi = ((LINE *)esi)->rpChars;
		esi += ((EDIT *)ebx)->hChars;
		if(edx<((CHARS *)esi)->len)
		{
			eax = *(BYTE *)(esi+edx+sizeof(CHARS));
			if(RBYTE_LOW(eax)==' ' || RBYTE_LOW(eax)==VK_TAB)
			{
				temp2 = edx;
				temp3 = eax;
				eax = InsertChar(ebx, ((EDIT *)ebx)->cpMin, eax);
				eax = temp3;
				eax = SaveUndo(ebx, UNDO_INSERT, ((EDIT *)ebx)->cpMin, eax, 1);
				eax = ((EDIT *)ebx)->cpMin;
				eax++;
				((EDIT *)ebx)->cpMin = eax;
				((EDIT *)ebx)->cpMax = eax;
				edx = temp2;
				edx++;
				goto anon_16;
			} // endif
		} // endif
		((EDIT *)ebx)->fOvr = temp1;
	} // endif
	return eax;

} // AutoIndent

REG_T IsCharPos(DWORD hMem, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nMax;

	ebx = hMem;
	eax = GetCharPtr(ebx, cp);
	nMax = eax;
	esi = ((EDIT *)ebx)->rpChars;
	esi += ((EDIT *)ebx)->hChars;
	if(!(((CHARS *)esi)->state&STATE_COMMENT))
	{
		ecx = 0;
		while(ecx<nMax)
		{
			if(((EDIT *)ebx)->ccmntblocks==1 && *(WORD *)(esi+ecx+sizeof(CHARS))=="*/")
			{
				ecx += 2;
				while(ecx<nMax)
				{
					if(*(WORD *)(esi+ecx+sizeof(CHARS))=="/*")
					{
						break;
					} // endif
					ecx++;
				} // endw
				if(*(WORD *)(esi+ecx+sizeof(CHARS))=="/*")
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
			else if(((EDIT *)ebx)->ccmntblocks==2 && *(WORD *)(esi+ecx+sizeof(CHARS))=="'/")
			{
				ecx += 2;
				while(ecx<nMax)
				{
					if(*(WORD *)(esi+ecx+sizeof(CHARS))=="/'")
					{
						break;
					} // endif
					ecx++;
				} // endw
				if(*(WORD *)(esi+ecx+sizeof(CHARS))=="/'")
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

REG_T BracketMatchRight(DWORD hMem, DWORD nChr, DWORD nMatch, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nCount;

	ebx = hMem;
	nCount = 0;
	eax = GetCharPtr(ebx, cp);
	edx = eax;
	edi = ((EDIT *)ebx)->hChars;
	edi += ((EDIT *)ebx)->rpChars;
	while(edx<=((CHARS *)edi)->len)
	{
		RBYTE_LOW(eax) = (BYTE)nMatch;
		RBYTE_HIGH(eax) = (BYTE)nChr;
		RBYTE_LOW(ecx) = (BYTE)bracketcont;
		RBYTE_HIGH(ecx) = (BYTE)bracketcont+1;
		if(RBYTE_LOW(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp1 = edx;
			eax = IsCharPos(ebx, cp);
			edx = temp1;
			if(!eax)
			{
				nCount--;
				if(nCount==0)
				{
					eax = edx;
					eax += ((EDIT *)ebx)->cpLine;
					return eax;
				} // endif
			} // endif
		}
		else if(RBYTE_HIGH(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp1 = edx;
			eax = IsCharPos(ebx, cp);
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
				eax = IsCharPos(ebx, cp);
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
					eax = GetCharPtr(ebx, eax);
					edx = eax;
					edi = ((EDIT *)ebx)->hChars;
					edi += ((EDIT *)ebx)->rpChars;
					edx = 0;
				} // endif
			}
			else
			{
				cp++;
				eax = cp;
				eax = GetCharPtr(ebx, eax);
				edx = eax;
				edi = ((EDIT *)ebx)->hChars;
				edi += ((EDIT *)ebx)->rpChars;
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

REG_T BracketMatchLeft(DWORD hMem, DWORD nChr, DWORD nMatch, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nCount;

	ebx = hMem;
	nCount = 0;
	temp1 = cp;
	eax = GetCharPtr(ebx, cp);
	edx = eax;
	edi = ((EDIT *)ebx)->hChars;
	edi += ((EDIT *)ebx)->rpChars;
	while(R_SIGNED(edx) >= 0)
	{
		RBYTE_LOW(eax) = (BYTE)nMatch;
		RBYTE_HIGH(eax) = (BYTE)nChr;
		if(RBYTE_LOW(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp2 = edx;
			eax = IsCharPos(ebx, cp);
			edx = temp2;
			if(!eax)
			{
				nCount--;
				if(nCount==0)
				{
					eax = edx;
					eax += ((EDIT *)ebx)->cpLine;
					goto Ex;
				} // endif
			} // endif
		}
		else if(RBYTE_HIGH(eax)==*(BYTE *)(edi+edx+sizeof(CHARS)))
		{
			temp2 = edx;
			eax = IsCharPos(ebx, cp);
			edx = temp2;
			if(!eax)
			{
				nCount++;
			} // endif
		} // endif
		if(!edx && ((EDIT *)ebx)->line)
		{
			cp--;
			eax = GetCharPtr(ebx, cp);
			edx = eax;
			edi = ((EDIT *)ebx)->hChars;
			edi += ((EDIT *)ebx)->rpChars;
			while((*(BYTE *)(edi+edx+sizeof(CHARS))==VK_SPACE || *(BYTE *)(edi+edx+sizeof(CHARS))==VK_TAB) && edx!=0)
			{
				edx--;
				cp--;
			} // endw
			temp2 = edx;
			eax = IsCharPos(ebx, cp);
			edx = temp2;
			if(!eax)
			{
				if((BYTE)bracketcont!=VK_RETURN)
				{
					if(edx)
					{
						edx--;
						RBYTE_LOW(eax) = *(BYTE *)(edi+edx+sizeof(CHARS));
						if(RBYTE_LOW(eax)!=(BYTE)bracketcont && RBYTE_LOW(eax)!=(BYTE)bracketcont+1)
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
	eax = GetCharPtr(ebx, cp);
	eax = temp1;
	return eax;

} // BracketMatchLeft

REG_T BracketMatch(DWORD hMem, DWORD nChr, DWORD cp)
{
	REG_T eax = 0, ecx, edx, ebx;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	ebx = hMem;
	if(((EDIT *)ebx)->cpbrst!=-1 || ((EDIT *)ebx)->cpbren!=-1)
	{
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
		eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
		eax = 0;
		eax--;
		((EDIT *)ebx)->cpbrst = eax;
		((EDIT *)ebx)->cpbren = eax;
	} // endif
	RBYTE_LOW(eax) = (BYTE)nChr;
	ecx = 0;
	while((BYTE)bracketstart[ecx])
	{
		if(RBYTE_LOW(eax)==bracketstart[ecx])
		{
			temp1 = ecx;
			eax = IsCharPos(ebx, cp);
			ecx = temp1;
			if(eax!=0)
			{
				goto Ex;
			} // endif
			eax = (BYTE)bracketend[ecx];
			eax = BracketMatchRight(ebx, nChr, eax, cp);
			((EDIT *)ebx)->cpbren = eax;
			eax = cp;
			((EDIT *)ebx)->cpbrst = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = ((EDIT *)ebx)->cpbren;
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
			eax = IsCharPos(ebx, cp);
			ecx = temp1;
			if(eax!=0)
			{
				goto Ex;
			} // endif
			eax = (BYTE)bracketstart[ecx];
			eax = BracketMatchLeft(ebx, nChr, eax, cp);
			((EDIT *)ebx)->cpbrst = eax;
			eax = cp;
			((EDIT *)ebx)->cpbren = eax;
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edta.hwnd);
			eax = InvalidateEdit(ebx, ((EDIT *)ebx)->edtb.hwnd);
			eax = ((EDIT *)ebx)->cpbrst;
			goto Ex;
		} // endif
		ecx++;
	} // endw
	eax = -1;
Ex:
	return eax;

} // BracketMatch

REG_T GetLineBegin(DWORD hMem, DWORD nLine)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

	eax = nLine;
	if(eax)
	{
		while(nLine)
		{
			nLine--;
			eax = nLine;
			edi = eax*4;
			edi += ((EDIT *)ebx)->hLine;
			esi = ((LINE *)edi)->rpChars;
			esi += ((EDIT *)ebx)->hChars;
			ecx = ((CHARS *)esi)->len;
			if(ecx<2)
			{
				break;
			} // endif
			RBYTE_LOW(eax) = *(BYTE *)(esi+ecx+sizeof(CHARS)-2);
			if(RBYTE_LOW(eax)!=bracketcont && RBYTE_LOW(eax) !=bracketcont[1])
			{
				break;
			} // endif
		} // endw
		eax = nLine;
		eax++;
	} // endif
	return eax;

} // GetLineBegin

