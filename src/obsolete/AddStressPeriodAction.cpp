#include "StdAfx.h"
#include "AddStressPeriodAction.h"

#include "WPhastDoc.h"
#include "PropertyTreeControlBar.h"

CAddStressPeriodAction::CAddStressPeriodAction(CWPhastDoc* pDoc, const CTimeControl& tc)
: m_pDoc(pDoc)
, m_tc(tc)
, m_nStressPeriod(-1)
{
	ASSERT_VALID(this->m_pDoc);
}

CAddStressPeriodAction::~CAddStressPeriodAction(void)
{
}

void CAddStressPeriodAction::Execute()
{
// COMMENT: {4/8/2005 6:53:23 PM}	this->m_pDoc->AddStressPeriod(this->m_tc);
// COMMENT: {4/8/2005 6:53:23 PM}	this->m_nStressPeriod = this->m_pDoc->GetStressPeriodCount();
}

void CAddStressPeriodAction::UnExecute()
{
// COMMENT: {4/8/2005 6:53:25 PM}	this->m_pDoc->RemoveStressPeriod(this->m_nStressPeriod);
}

