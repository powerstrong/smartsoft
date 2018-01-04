//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2012 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
//
// BCGPStatic.cpp : implementation file
//

#include "stdafx.h"
#include "Bcgglobals.h"
#include "BCGPVisualManager.h"
#include "BCGPStatic.h"
#include "BCGPDlgImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CBCGPStatic, CStatic)

/////////////////////////////////////////////////////////////////////////////
// CBCGPStatic

CBCGPStatic::CBCGPStatic()
{
	m_bOnGlass = FALSE;
	m_bVisualManagerStyle = FALSE;
	m_clrText = (COLORREF)-1;
	m_bBackstageMode = FALSE;
	m_hFont	= NULL;
}

CBCGPStatic::~CBCGPStatic()
{
}

BEGIN_MESSAGE_MAP(CBCGPStatic, CStatic)
	//{{AFX_MSG_MAP(CBCGPStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLAERO, OnBCGSetControlAero)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLVMMODE, OnBCGSetControlVMMode)
	ON_REGISTERED_MESSAGE(BCGM_ONSETCONTROLBACKSTAGEMODE, OnBCGSetControlBackStageMode)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGPStatic message handlers

BOOL CBCGPStatic::OnEraseBkgnd(CDC* pDC) 
{
	if (!m_bOnGlass)
	{
		return CStatic::OnEraseBkgnd (pDC);
	}

	return TRUE;
}
//*****************************************************************************
void CBCGPStatic::OnPaint() 
{
	if (!m_bVisualManagerStyle && !m_bOnGlass)
	{
		Default ();
		return;
	}

	const DWORD dwStyle = GetStyle ();

	if ((dwStyle & SS_ICON) == SS_ICON ||
		(dwStyle & SS_BLACKRECT) == SS_BLACKRECT ||
		(dwStyle & SS_GRAYRECT) == SS_GRAYRECT ||
		(dwStyle & SS_WHITERECT) == SS_WHITERECT ||
		(dwStyle & SS_BLACKFRAME) == SS_BLACKFRAME ||
		(dwStyle & SS_GRAYFRAME) == SS_GRAYFRAME ||
		(dwStyle & SS_WHITEFRAME) == SS_WHITEFRAME ||
		(dwStyle & SS_USERITEM) == SS_USERITEM ||
		(dwStyle & SS_ETCHEDHORZ) == SS_ETCHEDHORZ ||
		(dwStyle & SS_ETCHEDVERT) == SS_ETCHEDVERT)
	{
		Default ();
		return;
	}

	CPaintDC dc(this); // device context for painting

	CBCGPMemDC memDC (dc, this);
	CDC* pDC = &memDC.GetDC ();

	CRect rectText;
	GetClientRect (rectText);

	globalData.DrawParentBackground (this, pDC);

	if (m_hFont != NULL && ::GetObjectType (m_hFont) != OBJ_FONT)
	{
		m_hFont = NULL;
	}

	CFont* pOldFont = m_hFont == NULL ?
		(CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT) :
		pDC->SelectObject (CFont::FromHandle (m_hFont));

	ASSERT(pOldFont != NULL);

	UINT uiDTFlags = DT_WORDBREAK;

	if (dwStyle & SS_CENTER)
	{
		uiDTFlags |= DT_CENTER;
	}
	else if (dwStyle & SS_RIGHT)
	{
		uiDTFlags |= DT_RIGHT;
	}

	if (dwStyle & SS_NOPREFIX)
	{
		uiDTFlags |= DT_NOPREFIX;
	}

	if ((dwStyle & SS_CENTERIMAGE) == SS_CENTERIMAGE)
	{
		uiDTFlags |= DT_SINGLELINE | DT_VCENTER;
	}

	COLORREF clrText = m_clrText == (COLORREF)-1 ? ((m_bBackstageMode && !globalData.IsHighContastMode()) ? RGB(0, 0, 0) : globalData.clrBarText) : m_clrText;
	if (!IsWindowEnabled ())
	{
#ifndef _BCGSUITE_
		clrText = CBCGPVisualManager::GetInstance ()->GetToolbarDisabledTextColor ();
#else
		clrText = globalData.clrGrayedText;
#endif
	}

	CString strText;
	GetWindowText (strText);

	if (strText.Find(_T('\t')) >= 0)
	{
		uiDTFlags |= (DT_TABSTOP | 0x40);
	}

	if (!m_bOnGlass)
	{
		COLORREF clrTextOld = pDC->SetTextColor (clrText);
		pDC->SetBkMode (TRANSPARENT);
		pDC->DrawText (strText, rectText, uiDTFlags);
		pDC->SetTextColor (clrTextOld);
	}
	else
	{
		CBCGPVisualManager::GetInstance ()->DrawTextOnGlass (pDC, strText, rectText, uiDTFlags, 6, IsWindowEnabled () ? m_clrText : globalData.clrGrayedText);
	}

	pDC->SelectObject (pOldFont);
}
//*****************************************************************************
LRESULT CBCGPStatic::OnBCGSetControlAero (WPARAM wp, LPARAM)
{
	m_bOnGlass = (BOOL) wp;
	return 0;
}
//**************************************************************************
LRESULT CBCGPStatic::OnBCGSetControlVMMode (WPARAM wp, LPARAM)
{
	m_bVisualManagerStyle = (BOOL) wp;
	return 0;
}
//*****************************************************************************
LRESULT CBCGPStatic::OnSetText (WPARAM, LPARAM)
{
	if (m_bBackstageMode && !globalData.IsHighContastMode())
	{
		SetRedraw(FALSE);
	}

	LRESULT lr = Default ();

	if (m_bBackstageMode && !globalData.IsHighContastMode())
	{
		SetRedraw(TRUE);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	}

	if (GetParent () != NULL)
	{
		CRect rect;
		GetWindowRect (rect);

		GetParent ()->ScreenToClient (&rect);
		GetParent ()->RedrawWindow (rect);
	}

	return lr;
}
//*****************************************************************************
void CBCGPStatic::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);

	if (GetParent () != NULL)
	{
		CRect rect;
		GetWindowRect (rect);

		GetParent ()->ScreenToClient (&rect);
		GetParent ()->RedrawWindow (rect);
	}
}
//*****************************************************************************
void CBCGPStatic::OnNcPaint() 
{
#ifndef _BCGSUITE_
	const DWORD dwStyle = GetStyle ();
	BOOL bIsSeparator = (dwStyle & SS_ETCHEDHORZ) == SS_ETCHEDHORZ || (dwStyle & SS_ETCHEDVERT) == SS_ETCHEDVERT;

	if (!bIsSeparator || !CBCGPVisualManager::GetInstance ()->IsOwnerDrawDlgSeparator(this))
	{
		Default();
		return;
	}

	CWindowDC dc(this);

	CRect rect;
	GetWindowRect (rect);

	rect.bottom -= rect.top;
	rect.right -= rect.left;
	rect.left = rect.top = 0;

	CBCGPVisualManager::GetInstance ()->OnDrawDlgSeparator(&dc, this, rect, (dwStyle & SS_ETCHEDVERT) != SS_ETCHEDVERT);
#else
	Default();
#endif
}
//**************************************************************************
LRESULT CBCGPStatic::OnBCGSetControlBackStageMode (WPARAM, LPARAM)
{
	m_bBackstageMode = TRUE;
	return 0;
}
//*****************************************************************************
LRESULT CBCGPStatic::OnSetFont (WPARAM wParam, LPARAM)
{
	m_hFont = (HFONT) wParam;
	return Default();
}
