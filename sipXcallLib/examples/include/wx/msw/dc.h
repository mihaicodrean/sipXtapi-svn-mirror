/////////////////////////////////////////////////////////////////////////////
// Name:        dc.h
// Purpose:     wxDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dc.h,v 1.38 2002/04/17 11:47:59 JS Exp $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DC_H_
#define _WX_DC_H_

#ifdef __GNUG__
    #pragma interface "dc.h"
#endif

#include "wx/defs.h"
#include "wx/dc.h"

// ---------------------------------------------------------------------------
// macros
// ---------------------------------------------------------------------------

#if wxUSE_DC_CACHEING
/*
 * Cached blitting, maintaining a cache
 * of bitmaps required for transparent blitting
 * instead of constant creation/deletion
 */

class wxDCCacheEntry: public wxObject
{
public:
    wxDCCacheEntry(WXHBITMAP hBitmap, int w, int h, int depth);
    wxDCCacheEntry(WXHDC hDC, int depth);
    ~wxDCCacheEntry();

    WXHBITMAP   m_bitmap;
    WXHDC       m_dc;
    int         m_width;
    int         m_height;
    int         m_depth;
};
#endif

class WXDLLEXPORT wxDC : public wxDCBase
{
public:
    wxDC();
    ~wxDC();

    // implement base class pure virtuals
    // ----------------------------------

    virtual void Clear();

    virtual bool StartDoc(const wxString& message);
    virtual void EndDoc();

    virtual void StartPage();
    virtual void EndPage();

    virtual void SetFont(const wxFont& font);
    virtual void SetPen(const wxPen& pen);
    virtual void SetBrush(const wxBrush& brush);
    virtual void SetBackground(const wxBrush& brush);
    virtual void SetBackgroundMode(int mode);
#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette);
#endif // wxUSE_PALETTE

    virtual void DestroyClippingRegion();

    virtual wxCoord GetCharHeight() const;
    virtual wxCoord GetCharWidth() const;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 wxFont *theFont = NULL) const;

    virtual bool CanDrawBitmap() const;
    virtual bool CanGetTextExtent() const;
    virtual int GetDepth() const;
    virtual wxSize GetPPI() const;

    virtual void SetMapMode(int mode);
    virtual void SetUserScale(double x, double y);
    virtual void SetSystemScale(double x, double y);
    virtual void SetLogicalScale(double x, double y);
    virtual void SetLogicalOrigin(wxCoord x, wxCoord y);
    virtual void SetDeviceOrigin(wxCoord x, wxCoord y);
    virtual void SetAxisOrientation(bool xLeftRight, bool yBottomUp);
    virtual void SetLogicalFunction(int function);

    // implementation from now on
    // --------------------------

    virtual void SetRop(WXHDC cdc);
    virtual void SelectOldObjects(WXHDC dc);

    wxWindow *GetWindow() const { return m_canvas; }
    void SetWindow(wxWindow *win)
    {
        m_canvas = win;

#if wxUSE_PALETTE
        // if we have palettes use the correct one for this window
        InitializePalette();
#endif // wxUSE_PALETTE
    }

    WXHDC GetHDC() const { return m_hDC; }
    void SetHDC(WXHDC dc, bool bOwnsDC = FALSE)
    {
        m_hDC = dc;
        m_bOwnsDC = bOwnsDC;
    }

    const wxBitmap& GetSelectedBitmap() const { return m_selectedBitmap; }
    wxBitmap& GetSelectedBitmap() { return m_selectedBitmap; }

    // update the internal clip box variables
    void UpdateClipBox();

#if wxUSE_DC_CACHEING
    static wxDCCacheEntry* FindBitmapInCache(WXHDC hDC, int w, int h);
    static wxDCCacheEntry* FindDCInCache(wxDCCacheEntry* notThis, WXHDC hDC);

    static void AddToBitmapCache(wxDCCacheEntry* entry);
    static void AddToDCCache(wxDCCacheEntry* entry);
    static void ClearCache();
#endif

protected:
    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             int style = wxFLOOD_SURFACE);

    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const;

    virtual void DoDrawPoint(wxCoord x, wxCoord y);
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2);

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc);
    virtual void DoDrawCheckMark(wxCoord x, wxCoord y,
                                 wxCoord width, wxCoord height);
    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea);

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height);
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius);
    virtual void DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height);

    virtual void DoCrossHair(wxCoord x, wxCoord y);

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y);
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = FALSE);

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y);
    virtual void DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y,
                                   double angle);

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                        wxDC *source, wxCoord xsrc, wxCoord ysrc,
                        int rop = wxCOPY, bool useMask = FALSE, wxCoord xsrcMask = -1, wxCoord ysrcMask = -1);

    // this is gnarly - we can't even call this function DoSetClippingRegion()
    // because of virtual function hiding
    virtual void DoSetClippingRegionAsRegion(const wxRegion& region);
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height);
    virtual void DoGetClippingRegion(wxCoord *x, wxCoord *y,
                                     wxCoord *width, wxCoord *height)
    {
        GetClippingBox(x, y, width, height);
    }

    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetSizeMM(int* width, int* height) const;

    virtual void DoDrawLines(int n, wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset);
    virtual void DoDrawPolygon(int n, wxPoint points[],
                               wxCoord xoffset, wxCoord yoffset,
                               int fillStyle = wxODDEVEN_RULE);


#if wxUSE_PALETTE
    // MSW specific, select a logical palette into the HDC
    // (tell windows to translate pixel from other palettes to our custom one
    // and vice versa)
    // Realize tells it to also reset the system palette to this one.
    void DoSelectPalette(bool realize = FALSE);

    // Find out what palette our parent window has, then select it into the dc
    void InitializePalette();
#endif // wxUSE_PALETTE

    // common part of DoDrawText() and DoDrawRotatedText()
    void DrawAnyText(const wxString& text, wxCoord x, wxCoord y);

    // common part of DoSetClippingRegion() and DoSetClippingRegionAsRegion()
    void SetClippingHrgn(WXHRGN hrgn);

    // MSW-specific member variables
    // -----------------------------

    // the window associated with this DC (may be NULL)
    wxWindow         *m_canvas;

    wxBitmap          m_selectedBitmap;

    // TRUE => DeleteDC() in dtor, FALSE => only ReleaseDC() it
    bool              m_bOwnsDC:1;

    // our HDC
    WXHDC             m_hDC;

    // Store all old GDI objects when do a SelectObject, so we can select them
    // back in (this unselecting user's objects) so we can safely delete the
    // DC.
    WXHBITMAP         m_oldBitmap;
    WXHPEN            m_oldPen;
    WXHBRUSH          m_oldBrush;
    WXHFONT           m_oldFont;

#if wxUSE_PALETTE
    WXHPALETTE        m_oldPalette;
#endif // wxUSE_PALETTE

#if wxUSE_DC_CACHEING
    static wxList     sm_bitmapCache;
    static wxList     sm_dcCache;
#endif

    DECLARE_DYNAMIC_CLASS(wxDC)
};

// ----------------------------------------------------------------------------
// wxDCTemp: a wxDC which doesn't free the given HDC (used by wxWindows
// only/mainly)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDCTemp : public wxDC
{
public:
    wxDCTemp(WXHDC hdc) { SetHDC(hdc); }
    virtual ~wxDCTemp() { SetHDC((WXHDC)NULL); }
};

#endif
    // _WX_DC_H_
