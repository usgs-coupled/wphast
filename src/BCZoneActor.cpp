#include "StdAfx.h"
#include "BCZoneActor.h"
#include "resource.h"

#include "property.h"

#include <vtkObjectFactory.h> // reqd by vtkStandardNewMacro
#include <vtkPropAssembly.h>

#include "PropertyTreeControlBar.h"
#include "FluxPropsPage.h"
#include "FluxPropsPage2.h"
#include "LeakyPropsPage.h"
#include "SpecifiedHeadPropsPage.h"
#include "ETSLayout/ETSLayout.h"

#include "WPhastDoc.h"
#include "SetBCAction.h"
#include "ZoneCreateAction.h"
#include "FlowOnly.h"
#include "Protect.h"
#include "Global.h"
#include "Utilities.h"
#include "GridKeyword.h"

vtkCxxRevisionMacro(CBCZoneActor, "$Revision$");
vtkStandardNewMacro(CBCZoneActor);

// Note: No header files should follow the following three lines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const char CBCZoneActor::szHeading[] = "BC";
double CBCZoneActor::s_color[3][3];
vtkProperty* CBCZoneActor::s_Property[3] = {0, 0, 0};
vtkProperty* CBCZoneActor::s_OutlineProperty[3] = {0, 0, 0};

CBCZoneActor::CBCZoneActor(void)
{
	static StaticInit init; // constructs/destructs s_Property[]
	// props set in this->UpdateProperty
}

CBCZoneActor::~CBCZoneActor(void)
{
}

void CBCZoneActor::Create(CWPhastDoc* pWPhastDoc, const CBC& bc, LPCTSTR desc)
{
	ASSERT(bc.polyh && ::AfxIsValidAddress(bc.polyh, sizeof(Polyhedron)));
	CGridKeyword gk;
	pWPhastDoc->GetGridKeyword(gk);
	CZoneCreateAction<CBCZoneActor>* pAction = new CZoneCreateAction<CBCZoneActor>(
		pWPhastDoc,
		bc.polyh,
		gk.m_grid_origin,
		gk.m_grid_angle,
		desc
		);
	pAction->GetZoneActor()->SetData(bc);
	pWPhastDoc->Execute(pAction);
}

void CBCZoneActor::Insert(CPropertyTreeControlBar* pTreeControlBar, HTREEITEM hInsertAfter)
{
	CTreeCtrl* pTreeCtrl = pTreeControlBar->GetTreeCtrl();
	HTREEITEM htiBC = pTreeControlBar->GetBCNode();
	this->InsertAt(pTreeCtrl, htiBC, hInsertAfter);

	// set visible checkmark
	CTreeCtrlNode node(this->m_hti, pTreeControlBar->GetTreeCtrlEx());
	pTreeControlBar->SetNodeCheck(node, BST_CHECKED);
}

void CBCZoneActor::InsertAt(CTreeCtrl* pTreeCtrl, HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	CString str = this->GetTreeHeading();
	this->m_hti = pTreeCtrl->InsertItem(str, hParent, hInsertAfter);
	pTreeCtrl->SetItemData(this->m_hti, (DWORD_PTR)this);
	pTreeCtrl->SelectItem(this->m_hti); // might want to move this
	this->Update(pTreeCtrl, this->m_hti);
}

CString CBCZoneActor::GetTreeHeading(void)
{
	CString str;
	// UNDEFINED, SPECIFIED, FLUX, LEAKY
	switch (this->m_bc.bc_type)
	{
		case BC_info::BC_UNDEFINED:
			str.Format(_T("UNDEFINED %s"), this->GetNameDesc());
			break;
		case BC_info::BC_SPECIFIED:
			str.Format(_T("SPECIFIED_HEAD %s"), this->GetNameDesc());
			break;
		case BC_info::BC_FLUX:
			str.Format(_T("FLUX %s"), this->GetNameDesc());
			break;
		case BC_info::BC_LEAKY:
			str.Format(_T("LEAKY %s"), this->GetNameDesc());
			break;
		default:
			str.Format(_T("UNKNOWN_BC %s"), this->GetNameDesc());
			break;
	}
	return str;
}

void CBCZoneActor::UpdateTree(CPropertyTreeControlBar* pTreeControlBar)
{
	this->Update(pTreeControlBar->GetTreeCtrl());
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent)
{
	ASSERT(htiParent == this->GetHTreeItem());
	this->Update(pTreeCtrl);
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl)
{
	this->Update(pTreeCtrl, this->GetHTreeItem(), this->GetBC());
}

void CBCZoneActor::Update(CTreeCtrl* pTreeCtrl, HTREEITEM htiParent, const CBC& crBC)
{
	CZoneActor::Update(pTreeCtrl, htiParent);

	// update description
	//
	pTreeCtrl->SetItemText(htiParent, this->GetTreeHeading());

	CBC relative(crBC);
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	if (pFrame)
	{
		ASSERT_VALID(pFrame);
		CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
		ASSERT_VALID(pDoc);

		CGlobal::PathsAbsoluteToRelative(pDoc->GetDefaultPathName(), pDoc, relative);
	}

	switch (relative.bc_type)
	{
		case BC_info::BC_UNDEFINED:
			ASSERT(FALSE);
			break;

		case BC_info::BC_SPECIFIED:
			if (relative.face_defined)
			{
				switch(relative.cell_face)
				{
				case CF_X:
					pTreeCtrl->InsertItem("exterior_cells_only  X", htiParent);
					break;
				case CF_Y:
					pTreeCtrl->InsertItem("exterior_cells_only  Y", htiParent);
					break;
				case CF_Z:
					pTreeCtrl->InsertItem("exterior_cells_only  Z", htiParent);
					break;
				case CF_ALL:
					pTreeCtrl->InsertItem("exterior_cells_only  ALL", htiParent);
					break;
				default:
					ASSERT(FALSE);
					break;
				}
			}

			// head
			relative.m_bc_head.InsertItem(pTreeCtrl, "head", htiParent);

			// associated_solution
			if (relative.bc_solution_type == ST_ASSOCIATED)
			{
				relative.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);
			}

			// fixed_solution
			if (relative.bc_solution_type == ST_FIXED)
			{
				relative.m_bc_solution.InsertItem(pTreeCtrl, "fixed_solution", htiParent);
			}
			break;

		case BC_info::BC_FLUX:
			// flux
			relative.m_bc_flux.InsertItem(pTreeCtrl, "flux", htiParent);

			// associated_solution
			relative.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);

			// face
			if (relative.face_defined)
			{
				switch (relative.face)
				{
					case 0:
						pTreeCtrl->InsertItem(_T("face X"), htiParent);						
						break;
					case 1:
						pTreeCtrl->InsertItem(_T("face Y"), htiParent);						
						break;
					case 2:
						pTreeCtrl->InsertItem(_T("face Z"), htiParent);						
						break;
				}
			}
			break;

		case BC_info::BC_LEAKY:
			// face
			if (relative.face_defined)
			{
				switch (relative.face)
				{
					case 0:
						pTreeCtrl->InsertItem(_T("face X"), htiParent);						
						break;
					case 1:
						pTreeCtrl->InsertItem(_T("face Y"), htiParent);						
						break;
					case 2:
						pTreeCtrl->InsertItem(_T("face Z"), htiParent);						
						break;
				}
			}

			// thickness
			if (relative.bc_thick && relative.bc_thick->type != PROP_UNDEFINED)
			{
				static_cast<Cproperty*>(relative.bc_thick)->Insert(pTreeCtrl, htiParent, "thickness");
			}

			// hydraulic_conductivity
			if (relative.bc_k && relative.bc_k->type != PROP_UNDEFINED)
			{
				static_cast<Cproperty*>(relative.bc_k)->Insert(pTreeCtrl, htiParent, "hydraulic_conductivity");
			}

			// elevation
			if (relative.bc_z_user && relative.bc_z_user->type != PROP_UNDEFINED)
			{
				static_cast<Cproperty*>(relative.bc_z_user)->Insert(pTreeCtrl, htiParent, "elevation");
			}

			// z_coordinate_system
			if (relative.bc_z_coordinate_system_user == PHAST_Transform::MAP)
			{
				pTreeCtrl->InsertItem(_T("z_coordinate_system    map"), htiParent);						
			}

			// head
			relative.m_bc_head.InsertItem(pTreeCtrl, "head", htiParent);

			// associated_solution
			relative.m_bc_solution.InsertItem(pTreeCtrl, "associated_solution", htiParent);

			break;

		default:
			ASSERT(FALSE);
			break;
	}
}

void CBCZoneActor::Edit(CTreeCtrl* pTreeCtrl)
{
	this->Edit(pTreeCtrl, 1);
}

void CBCZoneActor::Edit(CTreeCtrl* pTreeCtrl, int nStressPeriod)
{
	CString str;
	str.Format(_T("%s Properties"), this->GetName());
// COMMENT: {7/13/2009 4:51:42 PM}	ETSLayoutPropertySheet props(str);
	CPropertySheet props(str);

	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	ASSERT_VALID(pFrame);
	CWPhastDoc* pDoc = reinterpret_cast<CWPhastDoc*>(pFrame->GetActiveDocument());
	ASSERT_VALID(pDoc);

	ASSERT(this->GetBC().polyh);

	if (this->m_bc.bc_type == BC_info::BC_FLUX)
	{
		CFluxPropsPage2 fluxProps;
		props.AddPage(&fluxProps);
		fluxProps.SetProperties(this->GetBC());
		fluxProps.SetFlowOnly(pDoc->GetFlowOnly());
		fluxProps.SetDesc(this->GetDesc());
		fluxProps.SetUnits(pDoc->GetUnits());
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			fluxProps.GetProperties(bc);
			ASSERT(bc.polyh);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, fluxProps.GetDesc()));
		}
	}
	else if (this->m_bc.bc_type == BC_info::BC_LEAKY)
	{
		CLeakyPropsPage leakyProps;
		props.AddPage(&leakyProps);
		leakyProps.SetProperties(this->GetBC());		
		leakyProps.SetFlowOnly(pDoc->GetFlowOnly());
		leakyProps.SetDesc(this->GetDesc());
		leakyProps.SetUnits(pDoc->GetUnits());
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			leakyProps.GetProperties(bc);
			ASSERT(bc.polyh);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, leakyProps.GetDesc()));
		}
	}
	else if (this->m_bc.bc_type == BC_info::BC_SPECIFIED)
	{
		CSpecifiedHeadPropsPage specified;
		props.AddPage(&specified);
		specified.SetProperties(this->GetBC());		
		specified.SetFlowOnly(pDoc->GetFlowOnly());
		specified.SetDesc(this->GetDesc());
		specified.SetUnits(pDoc->GetUnits());
		if (props.DoModal() == IDOK)
		{
			CBC bc;
			specified.GetProperties(bc);
			ASSERT(bc.polyh);
			pDoc->Execute(new CSetBCAction(this, pTreeCtrl, bc, specified.GetDesc()));
		}		
	}
	else 
	{
		ASSERT(FALSE);
	}
}

CBC CBCZoneActor::GetBC(void)const
{
	return this->m_bc;
}

CBC CBCZoneActor::GetData(void)const
{
	return this->GetBC();
}

void CBCZoneActor::SetBC(const CBC& rBC)
{
	CProtect p(this->m_bc.polyh);

	this->m_bc = rBC;

	// set color etc.
	this->UpdateProperty();
}

void CBCZoneActor::SetData(const CBC& rBC)
{
	this->SetBC(rBC);
}

HTREEITEM CBCZoneActor::GetHTreeItem(void)const
{
	return this->m_hti;
}

void CBCZoneActor::SetHTreeItem(HTREEITEM htItem)
{
	this->m_hti = htItem;
}

void CBCZoneActor::Serialize(bool bStoring, hid_t loc_id, const CWPhastDoc* pWPhastDoc)
{
	ASSERT(this->GetName()); // must have name
	TRACE("%s\n", this->GetName());

	CZoneActor::Serialize(bStoring, loc_id, pWPhastDoc);

	if (bStoring)
	{
		// serialize bc
		this->m_bc.Serialize(bStoring, loc_id);
	}
	else
	{
		// serialize bc
		this->m_bc.Serialize(bStoring, loc_id);

		// set color etc
		this->UpdateProperty();
	}
}

void CBCZoneActor::Add(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC())
	{
		ASSERT(!pPropAssembly->GetParts()->IsItemPresent(this));
		pPropAssembly->AddPart(this);
		ASSERT(pPropAssembly->GetParts()->IsItemPresent(this));
		pWPhastDoc->AddPropAssembly(pPropAssembly);
	}
#ifdef _DEBUG
	else
	{
		ASSERT(FALSE);
	}
#endif
}

void CBCZoneActor::Remove(CWPhastDoc *pWPhastDoc)
{
	if (!pWPhastDoc) { ASSERT(FALSE); return; }
	if (vtkPropAssembly *pPropAssembly = pWPhastDoc->GetPropAssemblyBC())
	{
		pPropAssembly->RemovePart(this);
		// VTK HACK
		// This is req'd because ReleaseGraphicsResources is not called when
		// vtkPropAssembly::RemovePart(vtkProp *prop) is called
		pWPhastDoc->ReleaseGraphicsResources(this);
	}
#ifdef _DEBUG
	else
	{
		ASSERT(FALSE);
	}
#endif
}

void CBCZoneActor::SetStaticColor(int bc_type, COLORREF cr)
{
	switch (bc_type)
	{
		case BC_info::BC_SPECIFIED:
			CBCZoneActor::s_color[0][0] = (double)GetRValue(cr)/255.;
			CBCZoneActor::s_color[0][1] = (double)GetGValue(cr)/255.;
			CBCZoneActor::s_color[0][2] = (double)GetBValue(cr)/255.;
			if (CBCZoneActor::s_Property[0])
			{
				CBCZoneActor::s_Property[0]->SetColor(CBCZoneActor::s_color[0]);
			}
			if (CBCZoneActor::s_OutlineProperty[0])
			{
				CBCZoneActor::s_OutlineProperty[0]->SetColor(CBCZoneActor::s_color[0]);
				CBCZoneActor::s_OutlineProperty[0]->SetEdgeColor(CBCZoneActor::s_color[0]);
				CBCZoneActor::s_OutlineProperty[0]->SetAmbientColor(CBCZoneActor::s_color[0]);
			}
			break;
		case BC_info::BC_FLUX:
			CBCZoneActor::s_color[1][0] = (double)GetRValue(cr)/255.;
			CBCZoneActor::s_color[1][1] = (double)GetGValue(cr)/255.;
			CBCZoneActor::s_color[1][2] = (double)GetBValue(cr)/255.;
			if (CBCZoneActor::s_Property[1])
			{
				CBCZoneActor::s_Property[1]->SetColor(CBCZoneActor::s_color[1]);
			}
			if (CBCZoneActor::s_OutlineProperty[1])
			{
				CBCZoneActor::s_OutlineProperty[1]->SetColor(CBCZoneActor::s_color[1]);
				CBCZoneActor::s_OutlineProperty[1]->SetEdgeColor(CBCZoneActor::s_color[1]);
				CBCZoneActor::s_OutlineProperty[1]->SetAmbientColor(CBCZoneActor::s_color[1]);
			}
			break;
		case BC_info::BC_LEAKY:
			CBCZoneActor::s_color[2][0] = (double)GetRValue(cr)/255.;
			CBCZoneActor::s_color[2][1] = (double)GetGValue(cr)/255.;
			CBCZoneActor::s_color[2][2] = (double)GetBValue(cr)/255.;
			if (CBCZoneActor::s_Property[2])
			{
				CBCZoneActor::s_Property[2]->SetColor(CBCZoneActor::s_color[2]);
			}
			if (CBCZoneActor::s_OutlineProperty[2])
			{
				CBCZoneActor::s_OutlineProperty[2]->SetColor(CBCZoneActor::s_color[2]);
				CBCZoneActor::s_OutlineProperty[2]->SetEdgeColor(CBCZoneActor::s_color[2]);
				CBCZoneActor::s_OutlineProperty[2]->SetAmbientColor(CBCZoneActor::s_color[2]);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
	}
}

void CBCZoneActor::UpdateProperty()
{
	switch (this->m_bc.bc_type)
	{
		case BC_info::BC_SPECIFIED:
			ASSERT(CBCZoneActor::s_Property[0]);
			if (CBCZoneActor::s_Property[0])
			{
				this->CubeActor->SetProperty(CBCZoneActor::s_Property[0]);
			}
			ASSERT(CBCZoneActor::s_OutlineProperty[0]);
			if (CBCZoneActor::s_OutlineProperty[0])
			{
				this->OutlineActor->SetProperty(CBCZoneActor::s_OutlineProperty[0]);
			}
			break;
		case BC_info::BC_FLUX:
			ASSERT(CBCZoneActor::s_Property[1]);
			if (CBCZoneActor::s_Property[1])
			{
				this->CubeActor->SetProperty(CBCZoneActor::s_Property[1]);
			}
			ASSERT(CBCZoneActor::s_OutlineProperty[1]);
			if (CBCZoneActor::s_OutlineProperty[1])
			{
				this->OutlineActor->SetProperty(CBCZoneActor::s_OutlineProperty[1]);
			}
			break;
		case BC_info::BC_LEAKY:
			ASSERT(CBCZoneActor::s_Property[2]);
			if (CBCZoneActor::s_Property[2])
			{
				this->CubeActor->SetProperty(CBCZoneActor::s_Property[2]);
			}
			ASSERT(CBCZoneActor::s_OutlineProperty[2]);
			if (CBCZoneActor::s_OutlineProperty[2])
			{
				this->OutlineActor->SetProperty(CBCZoneActor::s_OutlineProperty[2]);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
	}
	if (this->GetPolyhedronType() == Polyhedron::PRISM)
	{
		size_t npolys = this->SolidPerimeterActors.size();
		for (size_t poly = 0; poly < npolys; ++poly)
		{
			this->TopActors[poly]->SetProperty(this->CubeActor->GetProperty());
			this->SolidPerimeterActors[poly]->SetProperty(this->CubeActor->GetProperty());
			this->BottomActors[poly]->SetProperty(this->CubeActor->GetProperty());

			this->TopOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
			this->OutlinePerimeterActors[poly]->SetProperty(this->OutlineActor->GetProperty());
			this->BottomOutlineActors[poly]->SetProperty(this->OutlineActor->GetProperty());
		}
	}
}

Polyhedron*& CBCZoneActor::GetPolyhedron(void)
{
	return this->m_bc.polyh;
}
