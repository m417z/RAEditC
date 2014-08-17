#include <windows.h>
#include <commctrl.h>
#include "Data.h"

REG_T DrawLine(DWORD hMem, DWORD lpChars, DWORD nLine, DWORD cp, DWORD hDC, DWORD lpRect)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DRAWTEXTPARAMS dtp;
	DWORD cpMin;
	DWORD cpMax;
	DWORD lpCR;
	RECT rect;
	RECT srect;
	DWORD lCol;
	DWORD rcleft;
	DWORD fCmnt;
	DWORD fStr;
	DWORD fWrd;
	DWORD wCol;
	DWORD fEnd;
	DWORD fTmp;
	DWORD nGroup;
	DWORD fLc;
	DWORD fChr;
	BYTE tmp;
	DWORD fDot;
	DWORD fRed;
	DWORD fBack;
	DWORD bCol;
	DWORD fOpr;
	DWORD fNum;
	DWORD fCmntNest;
	DWORD nStr;
	DWORD nCmnt;
	DWORD nStringMode;

	auto void SetBack(void);
	auto void DrawWord(void);
	auto void DrawTabMarker(void);
	auto void ScanWord(void);
	auto void GetWord(void);
	auto void GetNum(void);
	auto void TestWord(void);
	auto void CmpWord(void);
	auto void SetCaseWord(void);
	auto void BackFill(void);
	auto void DrawCmntBack(void);
	auto void DrawSelBck(void);

	ebx = hMem;
	eax = ((EDIT *)ebx)->nWordGroup;
	nGroup = eax;
	eax = ((EDIT *)ebx)->fstyleex;
	eax >>= 3;
	eax &= 3;
	nStringMode = eax;
	fRed = 0;
	eax = '.';
	eax = *(BYTE *)(eax+CharTab);
	eax &= 1;
	fDot = eax;
	esi = lpChars;
	edi = ((CHARS *)esi)->len;
	if(!(((CHARS *)esi)->state&STATE_HIDDEN))
	{
		nCmnt = 0;
		eax = CopyRect(&rect, lpRect);
		eax = rect.top;
		eax += ((EDIT *)ebx)->fntinfo.fntht;
		rect.bottom = eax;
		edx = ((CHARS *)esi)->state;
		if(edx&STATE_ALTHILITE)
		{
			nGroup++;
		} // endif
		if(edx&STATE_REDTEXT)
		{
			fRed++;
		} // endif
		eax = edx;
		eax &= STATE_COMMENT;
		fCmnt = eax;
		eax = edx;
		eax &= STATE_COMMENTNEST;
		fCmntNest = eax;
		edx &= STATE_HILITEMASK;
		if(edx)
		{
			if(edx==STATE_HILITE1)
			{
				edx = ((EDIT *)ebx)->br.hBrHilite1;
			}
			else if(edx==STATE_HILITE2)
			{
				edx = ((EDIT *)ebx)->br.hBrHilite2;
			}
			else if(edx==STATE_HILITE3)
			{
				edx = ((EDIT *)ebx)->br.hBrHilite3;
			} // endif
			eax = FillRect(hDC, &rect, edx);
		} // endif
		eax = rect.top;
		edx = eax;
		edx += ((EDIT *)ebx)->fntinfo.fntht;
		if(edi && R_SIGNED(eax) < (SDWORD)rect.bottom && R_SIGNED(edx) > 0)
		{
			esi += sizeof(CHARS);
			RBYTE_LOW(eax) = *(BYTE *)(esi+edi-1);
			lpCR = 0;
			if(RBYTE_LOW(eax)==0x0D)
			{
				eax = esi+edi-1;
				lpCR = eax;
				*(BYTE *)eax = ' ';
			} // endif
			if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
			{
				eax = ((EDIT *)ebx)->cpMin;
				ecx = ((EDIT *)ebx)->cpMax;
				if(!((EDIT *)ebx)->fHideSel && eax!=ecx)
				{
					if(eax>cp)
					{
						eax -= cp;
					}
					else
					{
						eax = 0;
					} // endif
					if(ecx>cp)
					{
						ecx -= cp;
					}
					else
					{
						ecx = 0;
					} // endif
				}
				else
				{
					eax = 0;
					ecx = 0;
				} // endif
			}
			else
			{
				ecx = ((EDIT *)ebx)->blrg.lnMin;
				edx = ((EDIT *)ebx)->blrg.lnMax;
				if(ecx>edx)
				{
					temp1 = ecx;
					ecx = edx;
					edx = temp1;
				} // endif
				eax = nLine;
				if(eax>=ecx && eax<=edx)
				{
					eax = GetBlockCp(ebx, nLine, ((EDIT *)ebx)->blrg.clMin);
					eax -= cp;
					temp1 = eax;
					eax = GetBlockCp(ebx, nLine, ((EDIT *)ebx)->blrg.clMax);
					eax -= cp;
					ecx = temp1;
				}
				else
				{
					eax = 0;
					ecx = 0;
				} // endif
			} // endif
			if(eax>ecx)
			{
				temp1 = eax;
				eax = ecx;
				ecx = temp1;
			} // endif
			cpMin = eax;
			cpMax = ecx;
			dtp.cbSize = sizeof(dtp);
			eax = ((EDIT *)ebx)->nTab;
			dtp.iTabLength = eax;
			dtp.iLeftMargin = 0;
			dtp.iRightMargin = 0;
			dtp.uiLengthDrawn = 0;
			eax = rect.left;
			rcleft = eax;
			lCol = -1;
			fStr = 0;
			fWrd = 0;
			fEnd = 0;
			fChr = 0;
			fNum = 0;
			fOpr = 0;
			nStr = 0;
			nCmnt = 0;
			edx = ((CHARS *)(esi-sizeof(CHARS)))->state;
			eax = edx;
			eax &= STATE_COMMENT;
			fCmnt = eax;
			if(eax)
			{
				nCmnt = 1;
			} // endif
			edx &= 3;
			DrawSelBck();
			eax = ((EDIT *)ebx)->fstyle;
			if(eax&STYLE_NOHILITE)
			{
				fEnd = 99;
			} // endif
			if(fCmnt)
			{
				DrawCmntBack();
			} // endif
			ecx = edi;
			edi = 0;
			while(edi<ecx)
			{
				temp1 = ecx;
				fBack = 0;
				fOpr = 0;
				if(edi>=2 && ((EDIT *)ebx)->ccmntblocks && fCmnt && !fCmntNest && !nStr && nCmnt)
				{
					eax = *(WORD *)(esi+edi-2);
					if(((eax=='/*' && ((EDIT *)ebx)->ccmntblocks==1) || (eax=="/'" && ((EDIT *)ebx)->ccmntblocks==2) || (RBYTE_HIGH(eax)=="}" && ((EDIT *)ebx)->ccmntblocks==3)))
					{
						fCmnt = 0;
						nCmnt--;
					} // endif
				} // endif
				if(fEnd==99)
				{
					eax = ((EDIT *)ebx)->clr.txtcol;
				}
				else if(fEnd==1)
				{
					eax = *(BYTE *)(esi+edi);
					RBYTE_LOW(eax) = *(BYTE *)(eax+CharTab);
					if(RBYTE_LOW(eax)==CT_CMNTCHAR)
					{
						fEnd = 0;
						fStr = 0;
						fCmnt = eax;
						eax = ((EDIT *)ebx)->clr.cmntcol;
					}
					else if(RBYTE_LOW(eax)==CT_CMNTDBLCHAR)
					{
						eax = *(WORD *)(esi+edi);
						if(RBYTE_LOW(eax)==RBYTE_HIGH(eax) || RBYTE_HIGH(eax)=='*')
						{
							fEnd = 0;
							fStr = 0;
							fCmnt = eax;
							eax = ((EDIT *)ebx)->clr.cmntcol;
						}
						else
						{
							eax = ((EDIT *)ebx)->clr.strcol;
						} // endif
					}
					else if(RBYTE_LOW(eax)==CT_CMNTINITCHAR)
					{
						eax = *(WORD *)(esi+edi);
						if(RBYTE_HIGH(eax)=="'")
						{
							fEnd = 0;
							fStr = 0;
							fCmnt = eax;
							eax = ((EDIT *)ebx)->clr.cmntcol;
						}
						else
						{
							eax = ((EDIT *)ebx)->clr.strcol;
						} // endif
					}
					else
					{
						eax = ((EDIT *)ebx)->clr.strcol;
					} // endif
				}
				else if(fCmnt)
				{
					eax = ((EDIT *)ebx)->clr.cmntback;
					SetBack();
					eax = ((EDIT *)ebx)->clr.cmntcol;
				}
				else if(fWrd)
				{
					eax = wCol;
				}
				else if(fStr)
				{
					eax = *(BYTE *)(esi+edi);
					if(eax==fStr)
					{
						eax = *(BYTE *)(esi+edi-1);
						if(nStringMode==2 && eax=='\\')
						{
							eax = ((EDIT *)ebx)->clr.strback;
							SetBack();
							eax = ((EDIT *)ebx)->clr.strcol;
							wCol = eax;
						}
						else
						{
							fStr = 0;
							eax = ((EDIT *)ebx)->fstyleex;
							eax >>= 3;
							eax &= 3;
							nStringMode = eax;
							eax = ((EDIT *)ebx)->clr.oprback;
							SetBack();
							eax = ((EDIT *)ebx)->clr.oprcol;
						} // endif
					}
					else
					{
						eax = ((EDIT *)ebx)->clr.strback;
						SetBack();
						eax = ((EDIT *)ebx)->clr.strcol;
						wCol = eax;
					} // endif
				}
				else
				{
					eax = *(BYTE *)(esi+edi);
					edx = *(WORD *)(esi+edi);
					RBYTE_LOW(eax) = *(BYTE *)(eax+CharTab);
					if(RBYTE_LOW(eax)==CT_CHAR || edx=='h&' || edx=='H&')
					{
						ScanWord();
						if(fNum)
						{
							temp2 = eax;
							eax = ((EDIT *)ebx)->clr.numback;
							SetBack();
							eax = temp2;
						} // endif
						if(fEnd==2)
						{
							DrawCmntBack();
						} // endif
					}
					else if(RBYTE_LOW(eax)==CT_HICHAR)
					{
						ScanWord();
						if(eax==((EDIT *)ebx)->clr.txtcol)
						{
							fWrd = 1;
							eax = ((EDIT *)ebx)->clr.oprcol;
						} // endif
					}
					else if(RBYTE_LOW(eax)==CT_OPER)
					{
						eax = ((EDIT *)ebx)->clr.oprback;
						SetBack();
						eax = ((EDIT *)ebx)->clr.oprcol;
						fOpr = 1;
					}
					else if(RBYTE_LOW(eax)==CT_CMNTCHAR)
					{
						fCmnt = eax;
						eax = ((EDIT *)ebx)->clr.cmntback;
						SetBack();
						eax = ((EDIT *)ebx)->clr.cmntcol;
						DrawCmntBack();
					}
					else if(RBYTE_LOW(eax)==CT_STRING)
					{
						if(edi && nStringMode==1)
						{
							eax = *(BYTE *)(esi+edi-1);
							if(eax=='!')
							{
								nStringMode = 2;
							} // endif
						} // endif
						eax = *(BYTE *)(esi+edi);
						fStr = eax;
						fOpr = 1;
						eax = ((EDIT *)ebx)->clr.oprback;
						SetBack();
						eax = ((EDIT *)ebx)->clr.oprcol;
					}
					else if(RBYTE_LOW(eax)==CT_CMNTDBLCHAR)
					{
						eax = *(WORD *)(esi+edi);
						if(RBYTE_LOW(eax)==RBYTE_HIGH(eax) || RBYTE_HIGH(eax)=='*')
						{
							if(RBYTE_HIGH(eax)=='*')
							{
								nCmnt++;
							} // endif
							fCmnt = eax;
							eax = ((EDIT *)ebx)->clr.cmntback;
							SetBack();
							eax = ((EDIT *)ebx)->clr.cmntcol;
							DrawCmntBack();
						}
						else
						{
							eax = ((EDIT *)ebx)->clr.oprcol;
						} // endif
					}
					else if(RBYTE_LOW(eax)==CT_CMNTINITCHAR)
					{
						eax = *(WORD *)(esi+edi);
						if(RBYTE_HIGH(eax)=="'" || RBYTE_LOW(eax)=='{')
						{
							nCmnt++;
							fCmnt = eax;
							eax = ((EDIT *)ebx)->clr.cmntback;
							SetBack();
							eax = ((EDIT *)ebx)->clr.cmntcol;
							DrawCmntBack();
						}
						else
						{
							eax = ((EDIT *)ebx)->clr.oprcol;
						} // endif
					}
					else
					{
						eax = ((EDIT *)ebx)->clr.txtcol;
					} // endif
				} // endif
				if(edi>=2 && fWrd!=0)
				{
					if(*(WORD *)(edi+esi-2)=='>-')
					{
						eax = ((EDIT *)ebx)->clr.txtcol;
						wCol = eax;
					} // endif
				} // endif
				if(fRed)
				{
					eax = 0x0FF;
				} // endif
				if(fStr && !fOpr)
				{
					temp2 = eax;
					eax = ((EDIT *)ebx)->clr.strback;
					SetBack();
					eax = temp2;
				} // endif
				DrawWord();
				if(fEnd==1 && !fWrd)
				{
					fStr = 1;
				}
				else if(fEnd==2 && !fWrd)
				{
					fCmnt = 1;
				} // endif
				eax = rect.right;
				rect.left = eax;
				edx = lpRect;
				edx = ((RECT *)edx)->right;
				rect.right = edx;
				ecx = temp1;
				if(R_SIGNED(eax) > R_SIGNED(edx))
				{
					goto anon_1;
				} // endif
			} // endw
anon_1:
			eax = lpCR;
			if(eax)
			{
				*(BYTE *)eax = 0x0D;
			} // endif
		} // endif
	} // endif
	return eax;

	void SetBack(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		if(eax!=((EDIT *)ebx)->clr.bckcol)
		{
			bCol = eax;
			eax = SetBkColor(hDC, eax);
			fBack = TRUE;
		}
		else
		{
			fBack = FALSE;
		} // endif
		return;

	}

	void DrawWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = eax;
		eax = TRANSPARENT;
		if(fBack)
		{
			eax = OPAQUE;
		} // endif
		eax = SetBkMode(hDC, eax);
		eax = temp1;
		edx = *(BYTE *)(esi+edi);
		if(edx==VK_TAB)
		{
			ecx = ((EDIT *)ebx)->fntinfo.tabwt;
			eax = rect.left;
			eax -= rcleft;
			// xor		edx,edx
			eax /= ecx;
			eax *= ecx;
			eax += rcleft;
			while(*(BYTE *)(esi+edi)==VK_TAB && edi<((CHARS *)(esi-sizeof(CHARS)))->len)
			{
				DrawTabMarker();
				eax += ecx;
				edi++;
				rect.right = eax;
				if(fBack && (edi<=cpMin || edi>cpMax))
				{
					temp1 = eax;
					temp2 = ecx;
					temp3 = edx;
					eax = CreateSolidBrush(bCol);
					temp4 = eax;
					BackFill();
					eax = temp4;
					eax = DeleteObject(eax);
					edx = temp3;
					ecx = temp2;
					eax = temp1;
					temp1 = rect.right;
					rect.left = temp1;
				} // endif
				if(fWrd)
				{
					fWrd--;
				} // endif
			} // endw
		}
		else if(edx==VK_SPACE)
		{
			ecx = ((EDIT *)ebx)->fntinfo.spcwt;
			eax = rect.left;
			edx = ((EDIT *)ebx)->fntinfo.tabwt;
			edx += eax;
			edx = eax;
			while(*(BYTE *)(esi+edi)==VK_SPACE && edi<((CHARS *)(esi-sizeof(CHARS)))->len)
			{
				if(eax==edx)
				{
					edx += ((EDIT *)ebx)->fntinfo.tabwt;
					DrawTabMarker();
				} // endif
				eax += ecx;
				edi++;
				rect.right = eax;
				if(fBack && (edi<=cpMin || edi>cpMax))
				{
					temp1 = eax;
					temp2 = ecx;
					temp3 = edx;
					eax = CreateSolidBrush(bCol);
					temp4 = eax;
					BackFill();
					eax = temp4;
					eax = DeleteObject(eax);
					edx = temp3;
					ecx = temp2;
					eax = temp1;
					temp1 = rect.right;
					rect.left = temp1;
				} // endif
				if(fWrd)
				{
					fWrd--;
				} // endif
			} // endw
		}
		else
		{
			if(edx=='"')
			{
				if(nStr)
				{
					nStr--;
				}
				else
				{
					nStr++;
				} // endif
			} // endif
			fChr = TRUE;
			if(!fWrd)
			{
				temp1 = eax;
				GetWord();
				fWrd = ecx;
				eax = temp1;
			} // endif
			ecx = fWrd;
			ecx += edi;
			if(edi>=cpMax || ecx<cpMin)
			{
				// Word outside selection
				ecx = fWrd;
			}
			else if(edi<cpMin && ecx>=cpMin)
			{
				// Word starts before selection, ends in selection
				ecx = cpMin;
				ecx -= edi;
			}
			else if(edi>=cpMin && ecx<=cpMax)
			{
				// Word is in selection
				temp1 = eax;
				temp2 = ecx;
				if(fBack)
				{
					eax = SetBkMode(hDC, TRANSPARENT);
				} // endif
				ecx = temp2;
				eax = temp1;
				ecx = fWrd;
				eax &= 0x03000000;
				eax |= ((EDIT *)ebx)->clr.seltxtcol;
			}
			else
			{
				// Part of word is selected
				temp1 = eax;
				temp2 = ecx;
				if(fBack)
				{
					eax = SetBkMode(hDC, TRANSPARENT);
				} // endif
				ecx = temp2;
				eax = temp1;
				ecx = cpMax;
				ecx -= edi;
				eax &= 0x03000000;
				eax |= ((EDIT *)ebx)->clr.seltxtcol;
			} // endif
			if(eax!=lCol)
			{
				temp1 = ecx;
				lCol = eax;
				eax &= 0x0FFFFFF;
				eax = SetTextColor(hDC, eax);
				ecx = temp1;
			} // endif
			eax = *(BYTE *)(esi+edi);
			if(eax>0x80 && ((EDIT *)ebx)->fntinfo.fDBCS)
			{
				eax = TextOut(hDC, rect.left, rect.top, esi+edi, 2);
				eax = DrawTextEx(hDC, esi+edi, 2, &rect, DT_EDITCONTROL | DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP, &dtp);
				edi++;
				edi++;
				if(fWrd)
				{
					fWrd--;
				} // endif
				if(fWrd)
				{
					fWrd--;
				} // endif
			}
			else
			{
				if(ecx)
				{
					fTmp = ecx;
					fWrd -= ecx;
				}
				else
				{
					fTmp = 1;
					if(fWrd)
					{
						fWrd--;
					} // endif
				} // endif
				if(((EDIT *)ebx)->fntinfo.monospace)
				{
					eax = fTmp;
					edx = ((EDIT *)ebx)->fntinfo.fntwt;
					eax *= edx;
					eax += rect.left;
					rect.right = eax;
				}
				else
				{
					eax = GetTextWidth(ebx, hDC, esi+edi, fTmp, &rect);
				} // endif
				if((SDWORD)rect.right > 0)
				{
					temp1 = rect.top;
					eax = ((EDIT *)ebx)->fntinfo.linespace;
					eax /= 1;
					rect.top += eax;
					eax = lCol;
					eax >>= 24;
					eax &= 3;
					ecx = cp;
					ecx += edi;
					if(ecx==((EDIT *)ebx)->cpbrst || ecx==((EDIT *)ebx)->cpbren)
					{
						eax = ((EDIT *)ebx)->clr.numcol;
						if(((EDIT *)ebx)->cpbrst==-1 || ((EDIT *)ebx)->cpbren==-1)
						{
							eax = SetBkMode(hDC, OPAQUE);
							eax = SetBkColor(hDC, ((EDIT *)ebx)->clr.hicol1);
							eax = ((EDIT *)ebx)->clr.cmntcol;
						} // endif
						if(eax!=lCol)
						{
							lCol = eax;
							eax &= 0x0FFFFFF;
							eax = SetTextColor(hDC, eax);
						} // endif
						// Bold
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						eax = SetBkMode(hDC, TRANSPARENT);
						rect.left++;
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						rect.left++;
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						rect.left -= 2;
						eax = -1;
					} // endif
					if(!eax)
					{
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
					}
					else if(eax==1)
					{
						// Bold
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						rect.left++;
						eax = SetBkMode(hDC, TRANSPARENT);
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						rect.left--;
					}
					else if(eax==2)
					{
						// Italic
						temp2 = rect.top;
						eax = ((EDIT *)ebx)->fntinfo.italic;
						rect.top -= eax;
						eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hIFont);
						temp3 = eax;
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						eax = temp3;
						eax = SelectObject(hDC, eax);
						rect.top = temp2;
					}
					else if(eax==3)
					{
						// Bold italic
						temp2 = rect.top;
						eax = ((EDIT *)ebx)->fntinfo.italic;
						rect.top -= eax;
						eax = SelectObject(hDC, ((EDIT *)ebx)->fnt.hIFont);
						temp3 = eax;
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						rect.left++;
						eax = SetBkMode(hDC, TRANSPARENT);
						eax = TextOut(hDC, rect.left, rect.top, esi+edi, fTmp);
						eax = temp3;
						eax = SelectObject(hDC, eax);
						rect.left--;
						rect.top = temp2;
					} // endif
					rect.top = temp1;
				} // endif
				edi += fTmp;
			} // endif
		} // endif
		return;

	}

	void DrawTabMarker(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		if(edi && !fChr)
		{
			temp1 = eax;
			temp2 = ecx;
			temp3 = edx;
			temp4 = esi;
			temp5 = edi;
			esi = eax+2;
			eax -= rcleft;
			if(eax)
			{
				ecx = ((EDIT *)ebx)->fntinfo.fntht;
				ecx /= 1;
				edi = rect.top;
				while(ecx)
				{
					temp6 = ecx;
					edi++;
					eax = SetPixel(hDC, esi, edi, ((EDIT *)ebx)->clr.hicol3);
					edi++;
					ecx = temp6;
					ecx--;
				} // endw
			} // endif
			edi = temp5;
			esi = temp4;
			edx = temp3;
			ecx = temp2;
			eax = temp1;
		} // endif
		return;

	}

	void ScanWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		fNum = ecx;
		GetWord();
		fWrd = ecx;
		if(ecx)
		{
			TestWord();
			if(!ecx)
			{
				GetNum();
				if(!ecx)
				{
					eax = ((EDIT *)ebx)->clr.txtcol;
				}
				else
				{
					fWrd = ecx;
					fNum = 1;
					eax = ((EDIT *)ebx)->clr.numcol;
				} // endif
			} // endif
		}
		else
		{
			eax = ((EDIT *)ebx)->clr.txtcol;
		} // endif
		wCol = eax;
		return;

	}

	void GetWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = 0;
		edx = CharTab;
		if(*(WORD *)(edi+esi)=='h&')
		{
			goto anon_2;
		} // endif
		if(*(WORD *)(edi+esi)=='H&')
		{
			goto anon_2;
		} // endif
		// .if *(WORD *)(edi+esi)=='>-'
		// inc		ecx
		// jmp		@f
		// .endif
		eax = edi+esi;
		eax = *(BYTE *)(eax+ecx);
		if(*(BYTE *)(eax+edx)==3)
		{
			goto anon_2;
		} // endif
		ecx--;
anon_2:
		ecx++;
		eax = edi+esi;
		eax = *(BYTE *)(eax+ecx);
		if(*(BYTE *)(eax+edx)==1)
		{
			goto anon_2;
		} // endif
		return;

	}

	void GetNum(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = ebx;
		ebx = 0;
		ecx = 0;
		eax = *(BYTE *)(edi+esi);
		edx = *(BYTE *)(edi+esi+1);
		if((eax>='0' && eax<='9') || (eax=='&' && (edx=='h' || edx=='H')))
		{
			if(eax=='0' && (edx=='x' || edx=='X'))
			{
				ecx++;
				ebx = 0x80000000;
			}
			else if(edx=='h' || edx=='H')
			{
				ecx++;
				// mov		ebx,0x80000000
			} // endif
			edx = CharTab;
anon_3:
			ecx++;
			eax = edi+esi;
			eax = *(BYTE *)(eax+ecx);
			if(*(BYTE *)(eax+edx)==1)
			{
				if((eax>='0' && eax<='9') || (eax>='A' && eax<='F') || (eax>='a' && eax<='f'))
				{
					ebx++;
					goto anon_3;
				}
				else if(eax=='H' || eax=='h')
				{
					ecx++;
					eax = edi+esi;
					eax = *(BYTE *)(eax+ecx);
					if(*(BYTE *)(eax+edx)==1 || R_SIGNED(ebx) < 0)
					{
						ecx = 0;
					}
					else
					{
						ebx = 0;
					} // endif
				}
				else
				{
					ecx = 0;
				} // endif
			} // endif
			if(ebx==0x80000000)
			{
				ecx = 0;
			} // endif
		} // endif
		ebx = temp1;
		return;

	}

	void TestWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = ebx;
		eax = *(BYTE *)(esi+edi);
		fLc = 0;
		if(eax>='a' && eax<='z')
		{
			fLc = 1;
			eax &= 0x5F;
		} // endif
		ebx = hWrdMem;
		edx = *(DWORD *)(ebx+eax*4);
TestWord1:
		if(edx)
		{
			eax = ((WORDINFO *)(ebx+edx))->color;
			eax >>= 28;
			if(eax!=nGroup)
			{
				goto TestWord2;
			} // endif
			if(ecx==((WORDINFO *)(ebx+edx))->len)
			{
				goto anon_4;
			} // endif
TestWord2:
			edx = ((WORDINFO *)(ebx+edx))->rpprev;
			goto TestWord1;
anon_4:
			RBYTE_HIGH(eax) = (BYTE)((WORDINFO *)(ebx+edx))->fend;
			CmpWord();
			if(eax!=0)
			{
				edx = ((WORDINFO *)(ebx+edx))->rpprev;
				goto TestWord1;
			} // endif
			if(RBYTE_HIGH(eax) & 4)
			{
				RBYTE_HIGH(eax) &= ~ 4;
				SetCaseWord();
			} // endif
			*(BYTE *)(&fEnd) = RBYTE_HIGH(eax);
			eax = ((WORDINFO *)(ebx+edx))->color;
		}
		else
		{
			ecx = 0;
		} // endif
		ebx = temp1;
		return;

	}

	void CmpWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = ecx;
		temp2 = ebx;
		temp3 = esi;
		ebx = ebx+edx+sizeof(WORDINFO);
		esi = esi+edi;
		RBYTE_LOW(eax) = *(BYTE *)esi;
		if(RBYTE_LOW(eax)=='.' && edi && fDot)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi-1);
			if(RBYTE_LOW(eax)!=' ' && RBYTE_LOW(eax)!=VK_TAB)
			{
				goto CmpWord2;
			} // endif
		}
		else if(edi)
		{
			RBYTE_LOW(eax) = *(BYTE *)(esi-1);
			if(RBYTE_LOW(eax)=='.' && fDot)
			{
				goto CmpWord2;
			} // endif
		} // endif
anon_5:
		RBYTE_LOW(eax) = *(BYTE *)(esi+ecx-1);
		if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax) <='z' && RBYTE_HIGH(eax)!=3)
		{
			RBYTE_LOW(eax) &= 0x5F;
		} // endif
		tmp = RBYTE_LOW(eax);
		RBYTE_LOW(eax) = *(BYTE *)(ebx+ecx-1);
		if(RBYTE_LOW(eax)>='a' && RBYTE_LOW(eax) <='z' && RBYTE_HIGH(eax)!=3)
		{
			RBYTE_LOW(eax) &= 0x5F;
		} // endif
		if(RBYTE_LOW(eax)!=tmp)
		{
			eax = 1;
			goto CmpWord2;
		} // endif
CmpWord1:
		ecx--;
		if(ecx!=0)
		{
			goto anon_5;
		} // endif
		eax = 0;
CmpWord2:
		esi = temp3;
		ebx = temp2;
		ecx = temp1;
		return;

	}

	void SetCaseWord(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = ecx;
		temp2 = ebx;
		temp3 = esi;
		ebx = ebx+edx+sizeof(WORDINFO);
		esi = esi+edi;
anon_6:
		RBYTE_LOW(eax) = *(BYTE *)(ebx+ecx-1);
		*(BYTE *)(esi+ecx-1) = RBYTE_LOW(eax);
		ecx--;
		if(ecx!=0)
		{
			goto anon_6;
		} // endif
		esi = temp3;
		ebx = temp2;
		ecx = temp1;
		return;

	}

	void BackFill(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		temp1 = rect.left;
		temp2 = rect.right;
		if((SDWORD)rect.left < 0)
		{
			rect.left = 0;
		} // endif
		if((SDWORD)rect.right > 2048)
		{
			rect.right = 2048;
		} // endif
		eax = FillRect(hDC, &rect, eax);
		rect.right = temp2;
		rect.left = temp1;
		return;

	}

	void DrawCmntBack(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		if(((EDIT *)ebx)->fstyle&STYLE_HILITECOMMENT)
		{
			temp1 = eax;
			temp2 = rect.left;
			temp3 = rect.right;
			eax = rect.left;
			edx = rect.right;
			if(R_SIGNED(edx) <= (SDWORD)srect.left || R_SIGNED(eax) >= (SDWORD)srect.right)
			{
				// Whole line
				eax = ((EDIT *)ebx)->br.hBrHilite1;
				BackFill();
			}
			else if(R_SIGNED(eax) < (SDWORD)srect.left)
			{
				// Middle
				eax = srect.left;
				rect.right = eax;
				eax = ((EDIT *)ebx)->br.hBrHilite1;
				BackFill();
				rect.right = 2048;
				eax = srect.right;
				rect.left = eax;
				eax = ((EDIT *)ebx)->br.hBrHilite1;
				BackFill();
			}
			else if(R_SIGNED(eax) < (SDWORD)srect.right)
			{
				// Right
				eax = srect.right;
				rect.left = eax;
				rect.right = 2048;
				eax = ((EDIT *)ebx)->br.hBrHilite1;
				BackFill();
			} // endif
			rect.right = temp3;
			rect.left = temp2;
			eax = temp1;
		} // endif
		return;

	}

	void DrawSelBck(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		srect.left = 4096;
		srect.right = 4096;
		if(!(((EDIT *)ebx)->nMode&MODE_BLOCK))
		{
			eax = cpMin;
			if(eax!=cpMax)
			{
				if(!cpMin && edi<=cpMax)
				{
					// Whole line
					eax = GetTextWidth(ebx, hDC, esi, edi, &rect);
					eax = ((EDIT *)ebx)->br.hBrSelBck;
					BackFill();
					eax = CopyRect(&srect, &rect);
				}
				else if(!cpMin)
				{
					// Left part
					eax = GetTextWidth(ebx, hDC, esi, cpMax, &rect);
					eax = ((EDIT *)ebx)->br.hBrSelBck;
					BackFill();
					eax = CopyRect(&srect, &rect);
				}
				else if(edi>cpMin)
				{
					// Right or middle part
					eax = GetTextWidth(ebx, hDC, esi, cpMin, &rect);
					temp1 = rect.right;
					ecx = cpMax;
					if(ecx>edi)
					{
						ecx = edi;
					} // endif
					eax = GetTextWidth(ebx, hDC, esi, ecx, &rect);
					rect.left = temp1;
					eax = ((EDIT *)ebx)->br.hBrSelBck;
					BackFill();
					eax = CopyRect(&srect, &rect);
					eax = rcleft;
					rect.left = eax;
				} // endif
			} // endif
		}
		else
		{
			ecx = ((EDIT *)ebx)->blrg.lnMin;
			edx = ((EDIT *)ebx)->blrg.lnMax;
			if(ecx>edx)
			{
				temp1 = ecx;
				ecx = edx;
				edx = temp1;
			} // endif
			eax = nLine;
			if(eax>=ecx && eax<=edx)
			{
				ecx = ((EDIT *)ebx)->fntinfo.fntwt;
				eax = ((EDIT *)ebx)->blrg.clMin;
				if(eax>((EDIT *)ebx)->blrg.clMax)
				{
					eax = ((EDIT *)ebx)->blrg.clMax;
				} // endif
				eax *= ecx;
				rect.left += eax;
				eax = ((EDIT *)ebx)->blrg.clMax;
				if(eax<((EDIT *)ebx)->blrg.clMin)
				{
					eax -= ((EDIT *)ebx)->blrg.clMin;
					eax = -eax;
				}
				else
				{
					eax -= ((EDIT *)ebx)->blrg.clMin;
				} // endif
				eax *= ecx;
				eax += rect.left;
				eax++;
				rect.right = eax;
				eax = rect.top;
				eax += ((EDIT *)ebx)->fntinfo.fntht;
				rect.bottom = eax;
				eax = ((EDIT *)ebx)->br.hBrSelBck;
				BackFill();
				eax = CopyRect(&srect, &rect);
				eax = rcleft;
				rect.left = eax;
			} // endif
		} // endif
		return;

	}

} // DrawLine

REG_T SetBlockMarkers(DWORD hMem, DWORD nLine, DWORD nMax)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	DWORD nLines;
	DWORD nLnMax;
	DWORD nLnSt;
	DWORD nLnEn;
	DWORD lpBlockDef;
	DWORD fcmnt;

	auto void BlockRoot(void);

	ebx = hMem;
	if(((EDIT *)ebx)->fstyleex&STYLEEX_BLOCKGUIDE)
	{
		fcmnt = 0;
		// Clear block markers
		edx = ((EDIT *)ebx)->rpLineFree;
		edx /= 4;
		edx--;
		nLnMax = edx;
		eax = nLine;
		edx = nMax;
		nLines = edx;
		while(eax<=nLnMax && nLines)
		{
			edx = eax;
			edx *= 4;
			edx += ((EDIT *)ebx)->hLine;
			edx = *(DWORD *)edx;
			edx += ((EDIT *)ebx)->hChars;
			((CHARS *)edx)->state &= -1 ^ (STATE_BLOCKSTART | STATE_BLOCK | STATE_BLOCKEND);
			if(!(((CHARS *)edx)->state&STATE_HIDDEN))
			{
				nLines--;
			} // endif
			eax++;
		} // endw
		edx = nMax;
		nLines = edx;
		// Find root block
		esi = -1;
Nxt:
		BlockRoot();
		if(nLnEn)
		{
			esi = nLnSt;
			esi++;
			if(esi<nLine)
			{
				esi = nLine;
			} // endif
			while(esi<=nLnEn && nLines)
			{
				edi = esi;
				edi *= 4;
				edi += ((EDIT *)ebx)->hLine;
				edi = *(DWORD *)edi;
				edi += ((EDIT *)ebx)->hChars;
				if(!(((CHARS *)edi)->state&STATE_HIDDEN))
				{
					if(((CHARS *)edi)->state&STATE_COMMENT)
					{
						fcmnt = edi;
					}
					else if(fcmnt)
					{
						edx = fcmnt;
						fcmnt = 0;
						if(esi==nLnEn)
						{
							((CHARS *)edx)->state &= -1 ^ (STATE_BLOCKSTART | STATE_BLOCK | STATE_BLOCKEND);
						} // endif
						((CHARS *)edx)->state |= STATE_BLOCKEND;
					} // endif
					((CHARS *)edi)->state &= -1 ^ (STATE_BLOCKSTART | STATE_BLOCK | STATE_BLOCKEND);
					if(esi<nLnEn)
					{
						((CHARS *)edi)->state |= STATE_BLOCK;
					} // endif
					eax = TestBlockEnd(ebx, esi);
					edx = esi;
					edx++;
					edx *= 4;
					if(eax!=-1 || edx==((EDIT *)ebx)->rpLineFree)
					{
						((CHARS *)edi)->state |= STATE_BLOCKEND;
					} // endif
					nLines--;
				} // endif
				esi++;
			} // endw
			if(esi<nLnMax && nLines)
			{
				edx = lpBlockDef;
				if(((RABLOCKDEF *)edx)->flag&BD_SEGMENTBLOCK)
				{
					esi--;
					nLines++;
					edi = esi;
					edi--;
					edi *= 4;
					edi += ((EDIT *)ebx)->hLine;
					edi = *(DWORD *)edi;
					edi += ((EDIT *)ebx)->hChars;
					((CHARS *)edi)->state &= -1 ^ (STATE_BLOCKSTART | STATE_BLOCK | STATE_BLOCKEND);
					((CHARS *)edi)->state |= STATE_BLOCKEND;
				} // endif
				esi--;
				goto Nxt;
			} // endif
		} // endif
	} // endif
	return eax;

	void BlockRoot(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;

BlockRootStart:
		nLnSt = 0;
		nLnEn = 0;
		eax = NextBookMark(ebx, esi, 1);
		if(eax!=-1)
		{
			esi = eax;
			eax = TestBlockStart(ebx, esi);
			if(eax!=-1)
			{
				lpBlockDef = eax;
				eax = GetBlock(ebx, esi, lpBlockDef);
				edx = lpBlockDef;
				if(!(((RABLOCKDEF *)edx)->flag&BD_INCLUDELAST))
				{
					eax++;
				} // endif
				eax += esi;
				if(eax>=nLine)
				{
					nLnSt = esi;
					nLnEn = eax;
				}
				else
				{
					if(R_SIGNED(eax) > R_SIGNED(esi))
					{
						esi = eax;
					} // endif
					if(((RABLOCKDEF *)edx)->flag&BD_SEGMENTBLOCK)
					{
						esi--;
					} // endif
					goto BlockRootStart;
				} // endif
			} // endif
		} // endif
		return;

	}

} // SetBlockMarkers

REG_T DrawChangedState(DWORD hMem, HDC hDC, DWORD lpLine, DWORD x, DWORD y)
{
	REG_T eax = 0, ecx, edx, ebx, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	HBRUSH hBr;
	RECT rect;

	ebx = hMem;
	if(((EDIT *)ebx)->fstyleex&STILEEX_LINECHANGED)
	{
		edi = lpLine;
		if(((CHARS *)edi)->state&STATE_CHANGESAVED)
		{
			eax = CreateSolidBrush(((EDIT *)ebx)->clr.changesaved);
			hBr = eax;
			eax = 0;
			eax -= x;
			eax += ((EDIT *)ebx)->linenrwt;
			eax += 20;
			rect.left = eax;
			eax += 5;
			rect.right = eax;
			eax = y;
			rect.top = eax;
			eax += ((EDIT *)ebx)->fntinfo.fntht;
			rect.bottom = eax;
			eax = FillRect(hDC, &rect, hBr);
			eax = DeleteObject(hBr);
		}
		else
		{
			if(((CHARS *)edi)->state&STATE_CHANGED)
			{
				eax = CreateSolidBrush(((EDIT *)ebx)->clr.changed);
				hBr = eax;
				eax = 0;
				eax -= x;
				eax += ((EDIT *)ebx)->linenrwt;
				eax += 20;
				rect.left = eax;
				eax += 5;
				rect.right = eax;
				eax = y;
				rect.top = eax;
				eax += ((EDIT *)ebx)->fntinfo.fntht;
				rect.bottom = eax;
				eax = FillRect(hDC, &rect, hBr);
				eax = DeleteObject(hBr);
			} // endif
		} // endif
	} // endif
	return eax;

} // DrawChangedState

// This proc does all the painting and drawing
REG_T RAEditPaint(HWND hWin)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	PAINTSTRUCT ps;
	HDC mDC;
	RECT rect;
	RECT rect1;
	RECT rect2;
	DWORD cp;
	BYTE buffer[32];
	DWORD hRgn1;
	RECT rcRgn1;
	POINT pt;

	auto void DrawBlockMarker(void);
	auto void DrawPageBreak(void);

	// Get the memory pointer
	eax = GetWindowLong(hWin, 0);
	ebx = eax;
	eax = GetFocus();
	if(eax==hWin)
	{
		if(!((EDIT *)ebx)->fCaretHide)
		{
			eax = HideCaret(hWin);
		} // endif
	} // endif
	eax = BeginPaint(hWin, &ps);
	if(((EDIT *)ebx)->linenrwt)
	{
		if(((EDIT *)ebx)->fstyle&STYLE_AUTOSIZELINENUM)
		{
			eax = 0;
			rect1.left = eax;
			rect1.top = eax;
			rect1.right = eax;
			rect1.bottom = eax;
			eax = SelectObject(ps.hdc, ((EDIT *)ebx)->fnt.hLnrFont);
			temp1 = eax;
			edx = ((EDIT *)ebx)->rpLineFree;
			edx /= 4;
			eax = DwToAscii(edx, &buffer);
			eax = DrawText(ps.hdc, &buffer, -1, &rect1, DT_CALCRECT | DT_SINGLELINE);
			eax = temp1;
			eax = SelectObject(ps.hdc, eax);
			eax = rect1.right;
			eax += 10;
			if(eax!=((EDIT *)ebx)->linenrwt)
			{
				((EDIT *)ebx)->linenrwt = eax;
				eax = hWin;
				if(eax==((EDIT *)ebx)->edta.hwnd)
				{
					esi = &((EDIT *)ebx)->edta;
				}
				else
				{
					esi = &((EDIT *)ebx)->edtb;
				} // endif
				// invoke SetCaret,ebx,((RAEDT *)esi)->cpy
				eax = GetCaretPoint(ebx, ((EDIT *)ebx)->cpMin, ((RAEDT *)esi)->cpy, &pt);
				eax = SetCaretPos(pt.x, pt.y);
			} // endif
		} // endif
	} // endif
	// Create a memory DC
	eax = CreateCompatibleDC(ps.hdc);
	mDC = eax;
	eax = GetClientRect(hWin, &rect);
	if(!(((EDIT *)ebx)->fstyle&STYLE_NOVSCROLL))
	{
		eax = SBWT;
		rect.right -= eax;
	} // endif
	eax = rect.right;
	if(eax<ps.rcPaint.right)
	{
		ps.rcPaint.right = eax;
	} // endif
	// Create a bitmap for the DC
	eax = ps.rcPaint.bottom;
	eax -= ps.rcPaint.top;
	rect1.bottom = eax;
	edx = ps.rcPaint.right;
	edx -= ps.rcPaint.left;
	rect1.right = edx;
	rect1.left = 0;
	rect1.top = 0;
	eax = CreateCompatibleBitmap(ps.hdc, edx, eax);
	// and select it
	eax = SelectObject(mDC, eax);
	temp1 = eax;
	// Select pen
	eax = SelectObject(mDC, ((EDIT *)ebx)->br.hPenSelbar);
	temp2 = eax;
	// Select the font into the DC
	eax = SelectObject(mDC, ((EDIT *)ebx)->fnt.hFont);
	temp3 = eax;
	// Draw text transparent
	eax = SetBkMode(mDC, TRANSPARENT);
	eax = ((EDIT *)ebx)->selbarwt;
	eax += ((EDIT *)ebx)->linenrwt;
	if(eax>ps.rcPaint.left)
	{
		eax -= ps.rcPaint.left;
		rect1.left = eax;
	} // endif
anon_7:
	eax = FillRect(mDC, &rect1, ((EDIT *)ebx)->br.hBrBck);
	eax = CopyRect(&rcRgn1, &rect1);
	eax = CreateRectRgn(rect1.left, rect1.top, rect1.right, rect1.bottom);
	hRgn1 = eax;
	if(rect1.left)
	{
		eax = rect1.left;
		rect1.right = eax;
		rect1.left = 0;
		eax = FillRect(mDC, &rect1, ((EDIT *)ebx)->br.hBrSelBar);
		rect1.right--;
		eax = MoveToEx(mDC, rect1.right, rect1.top, NULL);
		eax = LineTo(mDC, rect1.right, rect1.bottom);
	} // endif
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	temp4 = ((RAEDT *)esi)->cpy;
	eax = ((RAEDT *)esi)->topcp;
	cp = eax;
	esi = ((RAEDT *)esi)->topln;
	eax = rect.bottom;
	eax -= rect.top;
	ecx = ((EDIT *)ebx)->fntinfo.fntht;
	// xor		edx,edx
	eax /= ecx;
	eax++;
	eax = SetBlockMarkers(ebx, esi, eax);
	ecx = ((EDIT *)ebx)->fntinfo.fntht;
	eax = temp4;
	temp4 = eax;
	// xor		edx,edx
	eax /= ecx;
	eax *= ecx;
	edx = temp4;
	eax -= edx;
	rect.top = eax;
	// Draw rect a or b
	eax = ((EDIT *)ebx)->cpx;
	eax = -eax;
	eax += ((EDIT *)ebx)->selbarwt;
	eax += ((EDIT *)ebx)->linenrwt;
	eax++;
	rect.left = eax;
	eax = 0;
	edx = rect.top;
	while(R_SIGNED(edx) < (SDWORD)ps.rcPaint.bottom)
	{
		eax = CopyRect(&rect1, &rect);
		eax = ps.rcPaint.top;
		rect1.top -= eax;
		rect1.bottom -= eax;
		eax = ps.rcPaint.left;
		rect1.left -= eax;
		rect1.right -= eax;
anon_8:
		edi = esi;
		edi *= 4;
		if(edi>=((EDIT *)ebx)->rpLineFree)
		{
			goto anon_9;
		} // endif
		esi++;
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		edx = 0;
		if(!(((CHARS *)edi)->state&STATE_HIDDEN))
		{
			eax = rect.top;
			eax += ((EDIT *)ebx)->fntinfo.fntht;
			if(eax>ps.rcPaint.top)
			{
				eax = CreateRectRgn(rcRgn1.left, rect1.top, rcRgn1.right, rect1.bottom);
				temp4 = eax;
				eax = SelectClipRgn(mDC, eax);
				eax = temp4;
				eax = DeleteObject(eax);
				edx = esi;
				edx--;
				eax = DrawLine(ebx, edi, edx, cp, mDC, &rect1);
				eax = SelectClipRgn(mDC, hRgn1);
				if(((CHARS *)edi)->state&STATE_DIVIDERLINE)
				{
					if(!(((EDIT *)ebx)->fstyle&STYLE_NODIVIDERLINE))
					{
						eax = MoveToEx(mDC, rcRgn1.left, rect1.top, NULL);
						eax = LineTo(mDC, rcRgn1.right, rect1.top);
					} // endif
				} // endif
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				if(ps.rcPaint.left<eax)
				{
					eax = SelectClipRgn(mDC, NULL);
					DrawBlockMarker();
					if(((CHARS *)edi)->state&STATE_BREAKPOINT)
					{
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15+12;
						eax -= ps.rcPaint.left;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, 3, mDC, eax, edx, ILD_TRANSPARENT);
					} // endif
					if(((CHARS *)edi)->errid)
					{
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15+12;
						eax -= ps.rcPaint.left;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, 6, mDC, eax, edx, ILD_TRANSPARENT);
					} // endif
					eax = ((EDIT *)ebx)->lpBmCB;
					ecx = ((CHARS *)edi)->state;
					ecx &= STATE_BMMASK;
					if(ecx)
					{
						ecx >>= 4;
					}
					else if(eax)
					{
						// dec		esi
						// push	esi
						// inc		esi
						// push	((EDIT *)ebx)->hwnd
						// call	eax
						ecx = esi-1;
						eax = ((BOOKMARKPAINTCALLBACKPTR)eax)(((EDIT *)ebx)->hwnd, ecx);
						ecx = eax;
					} // endif
					if(ecx)
					{
						ecx--;
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15;
						eax -= ps.rcPaint.left;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, ecx, mDC, eax, edx, ILD_NORMAL);
					} // endif
					DrawPageBreak();
					if(((EDIT *)ebx)->linenrwt)
					{
						eax = SetBkMode(mDC, TRANSPARENT);
						eax = SetTextColor(mDC, ((EDIT *)ebx)->clr.lnrcol);
						eax = SelectObject(mDC, ((EDIT *)ebx)->fnt.hLnrFont);
						temp4 = eax;
						eax = ((EDIT *)ebx)->linenrwt;
						eax -= ps.rcPaint.left;
						eax -= 2;
						rect1.right = eax;
						eax -= ((EDIT *)ebx)->linenrwt;
						rect1.left = eax;
						eax = ((EDIT *)ebx)->fntinfo.fntht;
						eax += rect1.top;
						eax--;
						rect1.bottom = eax;
						eax = DwToAscii(esi, &buffer);
						eax = DrawText(mDC, &buffer, -1, &rect1, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
						eax = temp4;
						eax = SelectObject(mDC, eax);
					} // endif
				} // endif
			} // endif
			edx = ((EDIT *)ebx)->fntinfo.fntht;
		} // endif
		eax = ((CHARS *)edi)->len;
		cp += eax;
		if(edx==0)
		{
			goto anon_8;
		} // endif
		rect.top += edx;
		edx = rect.top;
	} // endw
anon_9:
	eax = ps.rcPaint.right;
	eax -= ps.rcPaint.left;
	edx = ps.rcPaint.bottom;
	edx -= ps.rcPaint.top;
	eax = BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, eax, edx, mDC, 0, 0, SRCCOPY);
	// Restore old font
	eax = temp3;
	eax = SelectObject(mDC, eax);
	// Restore old pen
	eax = temp2;
	eax = SelectObject(mDC, eax);
	// Restore old bitmap
	eax = temp1;
	eax = SelectObject(mDC, eax);
	// Delete created bitmap
	eax = DeleteObject(eax);
	// Delete created memory DC
	eax = DeleteDC(mDC);
	eax = EndPaint(hWin, &ps);
	eax = DeleteObject(hRgn1);
	eax = GetFocus();
	if(eax==hWin)
	{
		if(!((EDIT *)ebx)->fCaretHide)
		{
			eax = ShowCaret(hWin);
		} // endif
	} // endif
	return eax;

	void DrawBlockMarker(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = DrawChangedState(ebx, mDC, edi, ps.rcPaint.left, rect1.top);
		if(((CHARS *)edi)->state&STATE_BLOCK)
		{
			eax = 0;
			eax -= ps.rcPaint.left;
			eax += ((EDIT *)ebx)->linenrwt;
			eax += 15;
			edx = rect1.top;
			temp1 = eax;
			eax = MoveToEx(mDC, eax, edx, NULL);
			eax = temp1;
			edx = ((EDIT *)ebx)->fntinfo.fntht;
			edx += rect1.top;
			eax = LineTo(mDC, eax, edx);
		} // endif
		if(((CHARS *)edi)->state&STATE_BLOCKEND)
		{
			eax = 0;
			eax -= ps.rcPaint.left;
			eax += ((EDIT *)ebx)->linenrwt;
			eax += 15;
			edx = rect1.top;
			temp1 = eax;
			eax = MoveToEx(mDC, eax, edx, NULL);
			eax = temp1;
			edx = ((EDIT *)ebx)->fntinfo.fntht;
			edx /= 1;
			edx += rect1.top;
			temp1 = edx;
			eax = LineTo(mDC, eax, edx);
			edx = temp1;
			eax = 0;
			eax -= ps.rcPaint.left;
			eax += ((EDIT *)ebx)->linenrwt;
			eax += SELWT-4;
			eax = LineTo(mDC, eax, edx);
		} // endif
		return;

	}

	void DrawPageBreak(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = ((EDIT *)ebx)->nPageBreak;
		if(ecx)
		{
			eax = esi;
			// xor		edx,edx
			eax /= ecx;
			if(!edx)
			{
				eax = 0;
				eax -= ps.rcPaint.left;
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				edx += rect1.top;
				edx--;
				eax = MoveToEx(mDC, eax, edx, NULL);
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				eax -= ps.rcPaint.left;
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				edx += rect1.top;
				edx--;
				eax = LineTo(mDC, eax, edx);
			} // endif
		} // endif
		return;

	}

} // RAEditPaint

// This proc does all the painting and drawing
REG_T RAEditPaintNoBuff(HWND hWin)
{
	REG_T eax = 0, ecx, edx, ebx, esi, edi;
	REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
	PAINTSTRUCT ps;
	RECT rect;
	RECT rect1;
	RECT rect2;
	DWORD cp;
	BYTE buffer[32];
	DWORD hRgn1;
	POINT pt;

	auto void DrawBlockMarker(void);
	auto void DrawPageBreak(void);

	// Get the memory pointer
	eax = GetWindowLong(hWin, 0);
	ebx = eax;
	eax = GetFocus();
	if(eax==hWin)
	{
		if(!((EDIT *)ebx)->fCaretHide)
		{
			eax = HideCaret(hWin);
		} // endif
	} // endif
	eax = BeginPaint(hWin, &ps);
	if(((EDIT *)ebx)->linenrwt)
	{
		if(((EDIT *)ebx)->fstyle&STYLE_AUTOSIZELINENUM)
		{
			eax = 0;
			rect1.left = eax;
			rect1.top = eax;
			rect1.right = eax;
			rect1.bottom = eax;
			eax = SelectObject(ps.hdc, ((EDIT *)ebx)->fnt.hLnrFont);
			temp1 = eax;
			edx = ((EDIT *)ebx)->rpLineFree;
			edx /= 4;
			eax = DwToAscii(edx, &buffer);
			eax = DrawText(ps.hdc, &buffer, -1, &rect1, DT_CALCRECT | DT_SINGLELINE);
			eax = temp1;
			eax = SelectObject(ps.hdc, eax);
			eax = rect1.right;
			eax += 10;
			if(eax!=((EDIT *)ebx)->linenrwt)
			{
				((EDIT *)ebx)->linenrwt = eax;
				eax = hWin;
				if(eax==((EDIT *)ebx)->edta.hwnd)
				{
					esi = &((EDIT *)ebx)->edta;
				}
				else
				{
					esi = &((EDIT *)ebx)->edtb;
				} // endif
				// invoke SetCaret,ebx,((RAEDT *)esi)->cpy
				eax = GetCaretPoint(ebx, ((EDIT *)ebx)->cpMin, ((RAEDT *)esi)->cpy, &pt);
				eax = SetCaretPos(pt.x, pt.y);
			} // endif
		} // endif
	} // endif
	// Select pen
	eax = SelectObject(ps.hdc, ((EDIT *)ebx)->br.hPenSelbar);
	temp1 = eax;
	// Select the font into the DC
	eax = SelectObject(ps.hdc, ((EDIT *)ebx)->fnt.hFont);
	temp2 = eax;
	// Draw text transparent
	eax = SetBkMode(ps.hdc, TRANSPARENT);
	eax = GetClientRect(hWin, &rect);
	if(!(((EDIT *)ebx)->fstyle&STYLE_NOVSCROLL))
	{
		eax = SBWT;
		rect.right -= eax;
	} // endif
	eax = CopyRect(&rect1, &rect);
	eax = ((EDIT *)ebx)->selbarwt;
	eax += ((EDIT *)ebx)->linenrwt;
	rect1.left = eax;
	eax = CreateRectRgn(rect1.left, rect1.top, rect1.right, rect1.bottom);
	hRgn1 = eax;
	if(rect1.left)
	{
		eax = rect1.left;
		rect1.right = eax;
		rect1.left = 0;
		eax = FillRect(ps.hdc, &rect1, ((EDIT *)ebx)->br.hBrSelBar);
		rect1.right--;
		eax = MoveToEx(ps.hdc, rect1.right, rect1.top, NULL);
		eax = LineTo(ps.hdc, rect1.right, rect1.bottom);
	} // endif
	eax = hWin;
	if(eax==((EDIT *)ebx)->edta.hwnd)
	{
		esi = &((EDIT *)ebx)->edta;
	}
	else
	{
		esi = &((EDIT *)ebx)->edtb;
	} // endif
	temp3 = ((RAEDT *)esi)->cpy;
	eax = ((RAEDT *)esi)->topcp;
	cp = eax;
	esi = ((RAEDT *)esi)->topln;
	eax = rect.bottom;
	eax -= rect.top;
	ecx = ((EDIT *)ebx)->fntinfo.fntht;
	// xor		edx,edx
	eax /= ecx;
	eax++;
	eax = SetBlockMarkers(ebx, esi, eax);
	ecx = ((EDIT *)ebx)->fntinfo.fntht;
	eax = temp3;
	temp3 = eax;
	// xor		edx,edx
	eax /= ecx;
	eax *= ecx;
	edx = temp3;
	eax -= edx;
	rect.top = eax;
	// Draw rect a or b
	eax = ((EDIT *)ebx)->cpx;
	eax = -eax;
	eax += ((EDIT *)ebx)->selbarwt;
	eax += ((EDIT *)ebx)->linenrwt;
	rect.left = eax;
	eax = 0;
	edx = rect.top;
	while(R_SIGNED(edx) <= (SDWORD)ps.rcPaint.bottom)
	{
		eax = CopyRect(&rect1, &rect); // ps.rcPaint
		eax = rect1.top;
		eax += ((EDIT *)ebx)->fntinfo.fntht;
		rect1.bottom = eax;
		if(eax>=ps.rcPaint.top)
		{
			temp3 = rect1.left;
			eax = ((EDIT *)ebx)->selbarwt;
			eax += ((EDIT *)ebx)->linenrwt;
			rect1.left = eax;
			eax = FillRect(ps.hdc, &rect1, ((EDIT *)ebx)->br.hBrBck);
			rect1.left = temp3;
		} // endif
		rect1.left++;
anon_10:
		edx = ((EDIT *)ebx)->fntinfo.fntht;
		edi = esi;
		edi *= 4;
		if(edi>=((EDIT *)ebx)->rpLineFree)
		{
			goto anon_11;
		} // endif
		esi++;
		edi += ((EDIT *)ebx)->hLine;
		edi = ((LINE *)edi)->rpChars;
		edi += ((EDIT *)ebx)->hChars;
		edx = 0;
		if(!(((CHARS *)edi)->state&STATE_HIDDEN))
		{
			eax = rect1.top;
			eax += ((EDIT *)ebx)->fntinfo.fntht;
			if(eax>=ps.rcPaint.top)
			{
				rect1.bottom = eax;
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				eax = CreateRectRgn(eax, rect1.top, rect1.right, rect1.bottom);
				temp3 = eax;
				eax = SelectClipRgn(ps.hdc, eax);
				eax = temp3;
				eax = DeleteObject(eax);
				edx = esi;
				edx--;
				eax = DrawLine(ebx, edi, edx, cp, ps.hdc, &rect1);
				eax = SelectClipRgn(ps.hdc, hRgn1);
				if(((CHARS *)edi)->state&STATE_DIVIDERLINE)
				{
					if(!(((EDIT *)ebx)->fstyle&STYLE_NODIVIDERLINE))
					{
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax = MoveToEx(ps.hdc, eax, rect1.top, NULL);
						eax = LineTo(ps.hdc, rect1.right, rect1.top);
					} // endif
				} // endif
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				if(ps.rcPaint.left<eax)
				{
					eax = SelectClipRgn(ps.hdc, NULL);
					DrawBlockMarker();
					if(((CHARS *)edi)->state&STATE_BREAKPOINT)
					{
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15+12;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, 3, ps.hdc, eax, edx, ILD_TRANSPARENT);
					} // endif
					if(((CHARS *)edi)->errid)
					{
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15+12;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, 6, ps.hdc, eax, edx, ILD_TRANSPARENT);
					} // endif
					eax = ((EDIT *)ebx)->lpBmCB;
					ecx = ((CHARS *)edi)->state;
					ecx &= STATE_BMMASK;
					if(ecx)
					{
						ecx >>= 4;
					}
					else if(eax)
					{
						// dec		esi
						// push	esi
						// inc		esi
						// push	((EDIT *)ebx)->hwnd
						// call	eax
						ecx = esi-1;
						eax = ((BOOKMARKPAINTCALLBACKPTR)eax)(((EDIT *)ebx)->hwnd, ecx);
						ecx = eax;
					} // endif
					if(ecx)
					{
						ecx--;
						eax = ((EDIT *)ebx)->selbarwt;
						eax += ((EDIT *)ebx)->linenrwt;
						eax -= 15;
						edx = ((EDIT *)ebx)->fntinfo.fntht;
						// sub		edx,7
						edx /= 1;
						edx -= 5;
						edx += rect1.top;
						eax = ImageList_Draw(hIml, ecx, ps.hdc, eax, edx, ILD_NORMAL);
					} // endif
					DrawPageBreak();
					if(((EDIT *)ebx)->linenrwt)
					{
						eax = SetBkMode(ps.hdc, TRANSPARENT);
						eax = SetTextColor(ps.hdc, ((EDIT *)ebx)->clr.lnrcol);
						eax = SelectObject(ps.hdc, ((EDIT *)ebx)->fnt.hLnrFont);
						temp3 = eax;
						eax = ((EDIT *)ebx)->linenrwt;
						eax -= 2;
						rect1.right = eax;
						eax -= ((EDIT *)ebx)->linenrwt;
						rect1.left = eax;
						eax = ((EDIT *)ebx)->fntinfo.fntht;
						eax += rect1.top;
						eax--;
						rect1.bottom = eax;
						eax = DwToAscii(esi, &buffer);
						eax = DrawText(ps.hdc, &buffer, -1, &rect1, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
						eax = temp3;
						eax = SelectObject(ps.hdc, eax);
					} // endif
				} // endif
			} // endif
			edx = ((EDIT *)ebx)->fntinfo.fntht;
		} // endif
		eax = ((CHARS *)edi)->len;
		cp += eax;
		if(edx==0)
		{
			goto anon_10;
		} // endif
anon_11:
		rect.top += edx;
		edx = rect.top;
	} // endw
	// Restore old font
	eax = temp2;
	eax = SelectObject(ps.hdc, eax);
	// Restore old pen
	eax = temp1;
	eax = SelectObject(ps.hdc, eax);
	eax = EndPaint(hWin, &ps);
	eax = DeleteObject(hRgn1);
	eax = GetFocus();
	if(eax==hWin)
	{
		if(!((EDIT *)ebx)->fCaretHide)
		{
			eax = ShowCaret(hWin);
		} // endif
	} // endif
	return eax;

	void DrawBlockMarker(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		eax = DrawChangedState(ebx, ps.hdc, edi, 0, rect1.top);
		if(((CHARS *)edi)->state&STATE_BLOCK)
		{
			eax = ((EDIT *)ebx)->linenrwt;
			eax += 15;
			edx = rect1.top;
			temp1 = eax;
			eax = MoveToEx(ps.hdc, eax, edx, NULL);
			eax = temp1;
			edx = ((EDIT *)ebx)->fntinfo.fntht;
			edx += rect1.top;
			eax = LineTo(ps.hdc, eax, edx);
		} // endif
		if(((CHARS *)edi)->state&STATE_BLOCKEND)
		{
			eax = ((EDIT *)ebx)->linenrwt;
			eax += 15;
			edx = rect1.top;
			temp1 = eax;
			eax = MoveToEx(ps.hdc, eax, edx, NULL);
			eax = temp1;
			edx = ((EDIT *)ebx)->fntinfo.fntht;
			edx /= 1;
			edx += rect1.top;
			temp1 = edx;
			eax = LineTo(ps.hdc, eax, edx);
			edx = temp1;
			eax = ((EDIT *)ebx)->linenrwt;
			eax += SELWT-4;
			eax = LineTo(ps.hdc, eax, edx);
		} // endif
		return;

	}

	void DrawPageBreak(void)
	{
		REG_T temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8;
		ecx = ((EDIT *)ebx)->nPageBreak;
		if(ecx)
		{
			eax = esi;
			// xor		edx,edx
			eax /= ecx;
			if(!edx)
			{
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				edx += rect1.top;
				edx--;
				eax = MoveToEx(ps.hdc, 0, edx, NULL);
				eax = ((EDIT *)ebx)->selbarwt;
				eax += ((EDIT *)ebx)->linenrwt;
				edx = ((EDIT *)ebx)->fntinfo.fntht;
				edx += rect1.top;
				edx--;
				eax = LineTo(ps.hdc, eax, edx);
			} // endif
		} // endif
		return;

	}

} // RAEditPaintNoBuff


