
// CartoTypeDemoDoc.cpp : implementation of the CCartoTypeDemoDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CartoTypeDemo.h"
#endif

#include "CartoTypeDemoDoc.h"

#include <propkey.h>

#ifdef CARTOTYPE_VERIFY_ENCRYPTED_FILE
#include <cartotype_encrypted_stream.h>
#include <cartotype_twofish_encryption.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCartoTypeDemoDoc

IMPLEMENT_DYNCREATE(CCartoTypeDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CCartoTypeDemoDoc, CDocument)
END_MESSAGE_MAP()


// CCartoTypeDemoDoc construction/destruction

CCartoTypeDemoDoc::CCartoTypeDemoDoc()
{
	// TODO: add one-time construction code here

}

CCartoTypeDemoDoc::~CCartoTypeDemoDoc()
    {
    }

// CCartoTypeDemoDoc serialization

void CCartoTypeDemoDoc::Serialize(CArchive& ar)
    {
    if (ar.IsStoring())
        {
        // TODO: add storing code here
        }
    else
        {
        // TODO: add loading code here
        }
    }

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CCartoTypeDemoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CCartoTypeDemoDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CCartoTypeDemoDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCartoTypeDemoDoc diagnostics

#ifdef _DEBUG
void CCartoTypeDemoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCartoTypeDemoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCartoTypeDemoDoc commands

BOOL CCartoTypeDemoDoc::OnOpenDocument(LPCTSTR lpszPathName)
    {
    m_map_data_set.reset();
    CartoType::Result error;
    CCartoTypeDemoApp* app = (CCartoTypeDemoApp*)AfxGetApp();
    CartoType::String filename;
    SetString(filename,lpszPathName);

    if (filename.Last(10) == ".ctm1_ctci")
        {
        std::string key("password");
        m_map_data_set = CartoType::FrameworkMapDataSet::New(error,app->Engine(),filename,&key);
        }
    else
        m_map_data_set = CartoType::FrameworkMapDataSet::New(error,app->Engine(),filename);

    if (error)
        {
        ::CString text("Error opening map ");
        text += lpszPathName;
        app->ShowError((LPCTSTR)text,error);
        return FALSE;
        }
    return TRUE;
    }


BOOL CCartoTypeDemoDoc::OnNewDocument()
    {
    // TODO: Add your specialized code here and/or call the base class

    return CDocument::OnNewDocument();
    }
