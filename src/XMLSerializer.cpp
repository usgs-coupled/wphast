#include "StdAfx.h"
#include "XMLSerializer.h"

#include <map>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/DOMBuilderImpl.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include "xml/StrX.h"
#include "xml/XStr.h"

#include "WPhastDoc.h"
#include "NewModel.h"
#include "SiteMap.h" // no longer used - kept only for backward compatibility
#include "MapActor.h"

// Note: these could be replaced by either of the following if using VS2005
// const XMLCh gLS[] = XMLStrL("LS");
// const XMLCh gLS[] = L"LS";
//

const XMLCh gLS[]          = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

const XMLCh gWPhast[]      = {xercesc::chLatin_W, xercesc::chLatin_P, xercesc::chLatin_h,
                              xercesc::chLatin_a, xercesc::chLatin_s, xercesc::chLatin_t,
                              xercesc::chNull};

const XMLCh gSiteMap[]     = {xercesc::chLatin_S, xercesc::chLatin_i, xercesc::chLatin_t,
                              xercesc::chLatin_e, xercesc::chLatin_M, xercesc::chLatin_a,
                              xercesc::chLatin_p, xercesc::chNull};

const XMLCh gSrc[]         = {xercesc::chLatin_s, xercesc::chLatin_r, xercesc::chLatin_c,
                              xercesc::chNull};

const XMLCh gWorldfile[]   = {xercesc::chLatin_w, xercesc::chLatin_o, xercesc::chLatin_r,
                              xercesc::chLatin_l, xercesc::chLatin_d, xercesc::chLatin_f,
                              xercesc::chLatin_i, xercesc::chLatin_l, xercesc::chLatin_e,
                              xercesc::chNull};

const XMLCh gAngle[]       = {xercesc::chLatin_a, xercesc::chLatin_n, xercesc::chLatin_g,
                              xercesc::chLatin_l, xercesc::chLatin_e, xercesc::chNull};

const XMLCh gPlacement_x[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_x, xercesc::chNull};

const XMLCh gPlacement_y[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_y, xercesc::chNull};

const XMLCh gPlacement_z[] = {xercesc::chLatin_p, xercesc::chLatin_l, xercesc::chLatin_a,
                              xercesc::chLatin_c, xercesc::chLatin_e, xercesc::chLatin_m,
                              xercesc::chLatin_e, xercesc::chLatin_n, xercesc::chLatin_t,
                              xercesc::chUnderscore, xercesc::chLatin_z, xercesc::chNull};

CXMLSerializer::CXMLSerializer(void)
: bInitialized(true)
, formatTarget(0)
{
	// Initialize the XML4C2 system
	try
	{
		xercesc::XMLPlatformUtils::Initialize();
		formatTarget = new xercesc::MemBufFormatTarget();
	}
	catch(const xercesc::XMLException&)
	{
		bInitialized = false;
	}
}

CXMLSerializer::~CXMLSerializer(void)
{
	delete formatTarget;

	// cleanup the XML4C2 system
	xercesc::XMLPlatformUtils::Terminate();
}

const char* CXMLSerializer::writeDOM(CWPhastDoc* wphastDoc, const char* prefix, LPCTSTR lpszPathName)
{
	if (!bInitialized || formatTarget == 0)
	{
		return 0;
	}

	// get impl
	//
	if (xercesc::DOMImplementation *impl = xercesc::DOMImplementationRegistry::getDOMImplementation(gLS))
	{
		// Create the DOM document
		// ie
		// <WPhast>
		//     <image src="bitmap.bmp" worldfile="bitmap.bpw"/>
		// </WPhast>
		//
		xercesc::DOMDocument* doc = impl->createDocument(0, gWPhast, 0);

		// Add comment node
		//
		this->AddCommentNode(doc, wphastDoc, lpszPathName);

		// TODO check for writeability of current directory
		//

		// Add SiteMap node
		//
		this->AddSiteMapNode(doc, wphastDoc, prefix);

		// Create the DOM writer
		//
		xercesc::DOMWriter *writer = ((xercesc::DOMImplementationLS*)impl)->createDOMWriter();

		// hardcode the encoding to "UTF-8"
		//
		writer->setEncoding(X("UTF-8"));

		// set pretty output (add whitespace)
		//
		if (writer->canSetFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true))
		{
			writer->setFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);
		}

		// write the doc
		//
		writer->writeNode(formatTarget, *doc);

		// cleanup
		//
		doc->release();
	}

	return (char*) formatTarget->getRawBuffer();
}

void CXMLSerializer::AddCommentNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, LPCTSTR lpszPathName)
{
	// Create comment node
	//
	std::string comment(_T("Exported from "));
	std::string comment2(_T("Exported to   "));
	comment2 += lpszPathName;
	CString path = wphastDoc->GetPathName();
	if (path.IsEmpty())
	{
		comment += _T("an unsaved file");
	}
	else
	{
		comment += path;
	}
	xercesc::DOMElement* rootElem = doc->getDocumentElement();
	xercesc::DOMComment* com = doc->createComment(X(comment.c_str()));
	rootElem->appendChild(com);
	xercesc::DOMComment* com2 = doc->createComment(X(comment2.c_str()));
	rootElem->appendChild(com2);
}

void CXMLSerializer::AddSiteMapNode(xercesc::DOMDocument* doc, CWPhastDoc* wphastDoc, const char* prefix)
{
	CMapActor* mapActor = wphastDoc->m_pMapActor;
	if (!mapActor)
	{
		// nothing to do
		return;
	}

	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];

	VERIFY(::_tsplitpath_s(mapActor->SiteMap3.FileName.c_str(), szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	CString imageFileName(prefix);
	imageFileName += _T(".sitemap");
	imageFileName += szExt;
	if (!::CopyFile(mapActor->SiteMap3.FileName.c_str(), imageFileName, FALSE))
	{
		::AfxMessageBox(_T("Unable to write site map."));
	}
	CString strWorldFileName = CGlobal::GetWorldFileName(imageFileName);
	try 
	{
		CGlobal::WriteWorldFile(strWorldFileName, mapActor->SiteMap3.GetWorldTransform());
	}
	catch(...)
	{
		::AfxMessageBox(_T("Unable to write worldfile."));
	}

	std::ostringstream oss;
	oss.precision(DBL_DIG);
	oss << mapActor->SiteMap3.Angle;

	// SiteMap
	//
	xercesc::DOMElement* element = doc->createElement(gSiteMap);

	// src
	//
	element->setAttribute(gSrc, X(imageFileName));

	// worldfile
	//
	element->setAttribute(gWorldfile, X(strWorldFileName));

	ASSERT(doc->getDocumentElement() != (xercesc::DOMElement*)doc);
	ASSERT(doc->getDocumentElement()->getOwnerDocument() == doc);

	xercesc::DOMElement* rootElem = doc->getDocumentElement();
	rootElem->appendChild(element);
}

int CXMLSerializer::AddSiteMapNode(CMapActor* mapActor, const char* prefix, xercesc::DOMElement* element)
{
	return 0;
}

/**
  Prereqs: valid model.m_units and model.m_gridKeyword.m_grid[2]
*/
int CXMLSerializer::load(std::istream& is, CNewModel& model)
{
	// Instantiate the DOM parser.
	//
	const XMLCh gLS[] = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};

	// Get an implementation of the Load-Store (LS) interface.
	//
	xercesc::DOMImplementationLS* impl (
		static_cast<xercesc::DOMImplementationLS*>(
		xercesc::DOMImplementationRegistry::getDOMImplementation(gLS)));

	// Create a DOMBuilder.
	//
	xercesc::DOMBuilder* parser = impl->createDOMBuilder(xercesc::DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	// Create InputSource
	//
	std_input_source isrc(is);
	xercesc::Wrapper4InputSource wrap(&isrc, false);
	xercesc::DOMDocument* doc = parser->parse(wrap);

	// get minimum z value for sitemap
	//
	double pt[3] = {0.0, 0.0, 0.0};
	CGrid z(model.m_gridKeyword.m_grid[2]);
	z.Setup();
	pt[2] = z.coord[0] - .01 *(z.coord[z.count_coord - 1] - z.coord[0]);

	// Load SiteMap
	//
	CSiteMap siteMap;
	if (this->LoadSiteMap(doc, siteMap) == 0)
	{
		CSiteMap3 siteMap3;
		siteMap3.Angle    = model.m_gridKeyword.m_grid_angle;
		siteMap3.FileName = siteMap.m_fileName;
		for (int i = 0; i < 3; ++i)
		{
			siteMap3.Origin[i] = model.m_gridKeyword.m_grid_origin[i];
		}

		vtkTransform *trans = vtkTransform::New();
		const CUnits& units = model.m_units;
		trans->Scale(
			units.horizontal.input_to_si,
			units.horizontal.input_to_si,
			units.vertical.input_to_si);
		trans->TransformPoint(pt, pt);
		siteMap3.Origin[2] = pt[2];
		trans->Delete();

		siteMap3.Origin[2] = pt[2];
		siteMap3.SetWorldTransform(siteMap.GetWorldTransform());
		model.SetSiteMap3(siteMap3);
	}

	return 0;
}

int CXMLSerializer::LoadSiteMap(xercesc::DOMDocument* doc, CSiteMap& siteMap)
{
	xercesc::DOMElement* e = doc->getDocumentElement();
	if (!e) return 1;

	if (xercesc::XMLString::compareString(e->getTagName(), gWPhast) != 0) return 2; 

	xercesc::DOMNodeList* list = e->getElementsByTagName(gSiteMap);
	if (list->getLength() == 0) return 3;

	xercesc::DOMNode* node = list->item(0);
	if (node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE)
	{
		xercesc::DOMElement* siteElement = static_cast<xercesc::DOMElement*>(node);

		TCHAR szFullPathName[MAX_PATH];
		LPTSTR lpFilePart;

		// src
		//
		StrX src(siteElement->getAttribute(gSrc));
		if (::strlen(src.localForm()) == 0) return 4;
		::GetFullPathName(src.localForm(), MAX_PATH, szFullPathName, &lpFilePart);
		siteMap.m_fileName = szFullPathName;

		// worldfile
		//
		StrX worldfile(siteElement->getAttribute(gWorldfile));
		if (::strlen(worldfile.localForm()) == 0) return 5;
		::GetFullPathName(worldfile.localForm(), MAX_PATH, szFullPathName, &lpFilePart);
		CWorldTransform wtrans;
		wtrans.LoadWorldFile(szFullPathName);
		siteMap.SetWorldTransform(wtrans);

		// angle
		//
		{
			StrX angle(siteElement->getAttribute(gAngle));
			if (::strlen(angle.localForm()) != 0)
			{
				std::istringstream iss(angle.localForm());
				iss >> siteMap.m_angle;
			}
		}

		// placement_x
		//
		{
			StrX placement_x(siteElement->getAttribute(gPlacement_x));
			if (::strlen(placement_x.localForm()) != 0)
			{
				std::istringstream iss(placement_x.localForm());
				iss >> siteMap.m_placement[0];
			}
		}

		// placement_y
		//
		{
			StrX placement_y(siteElement->getAttribute(gPlacement_y));
			if (::strlen(placement_y.localForm()) != 0)
			{
				std::istringstream iss(placement_y.localForm());
				iss >> siteMap.m_placement[1];
			}
		}

		// placement_z
		//
		{
			StrX placement_z(siteElement->getAttribute(gPlacement_z));
			if (::strlen(placement_z.localForm()) != 0)
			{
				std::istringstream iss(placement_z.localForm());
				iss >> siteMap.m_placement[2];
			}
		}
	}

	return 0;
}
