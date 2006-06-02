#include "StdAfx.h"
#include "River.h"

#include "Global.h"

#include "WPhastMacros.h"

CLIPFORMAT CRiver::clipFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("WPhast:CRiver"));

CRiver::CRiver(void)
{
}

CRiver::CRiver(const River &r)
{
	this->n_user      = r.n_user;
	this->description = r.description;

	// points
	//
	for (int ip = 0; ip < r.count_points; ++ip)
	{
		const River_Point* point_ptr = &r.points[ip];
		CRiverPoint rp(*point_ptr);
		m_listPoints.push_back(rp);
	}	
}

CRiver::~CRiver(void)
{
}

void CRiver::Serialize(bool bStoring, hid_t loc_id)
{
	static const char szPoints[]        = "Points";
	static const char szPointsFormat[]  = "Point %d";
	static const char sz_count_points[] = "count_points";

	HDF_GETSET_MACRO(n_user, H5T_NATIVE_INT);
	HDF_STD_STRING_MACRO(description);

	hid_t point_id;
	herr_t status;

	CString ptName;

	if (bStoring)
	{
		size_t count_points = this->m_listPoints.size();
		VERIFY(0 <= CGlobal::HDFSerialize(bStoring, loc_id, sz_count_points, H5T_NATIVE_INT, 1, &count_points));

		std::list<CRiverPoint>::iterator iter = this->m_listPoints.begin();
		for (size_t i = 0; iter != this->m_listPoints.end(); ++iter, ++i)
		{
			// create name for point group
			ptName.Format(szPointsFormat, i);

			// Create the "Point %d" group
			point_id = ::H5Gcreate(loc_id, ptName, 0);
			ASSERT(point_id > 0);
			if (point_id > 0)
			{
				iter->Serialize(bStoring, point_id);
				status = ::H5Gclose(point_id);
				ASSERT(status >= 0);
			}
		}
	}
	else
	{
		size_t count_points;
		if (0 > CGlobal::HDFSerialize(bStoring, loc_id, sz_count_points, H5T_NATIVE_INT, 1, &count_points))
		{
			count_points = 0;
		}
		this->m_listPoints.clear();
		for (size_t i = 0; i < count_points; ++i)
		{
			// create name for point group
			ptName.Format(szPointsFormat, i);

			// Open the "Point %d" group
			point_id = ::H5Gopen(loc_id, ptName);
			ASSERT(point_id > 0);
			if (point_id > 0)
			{
				CRiverPoint pt;
				pt.Serialize(bStoring, point_id);
				this->m_listPoints.push_back(pt);

				status = ::H5Gclose(point_id);
				ASSERT(status >= 0);
			}
		}
	}
}

void CRiver::Serialize(CArchive& ar)
{
	int nVersion = 1;
	ARC_VERSION(ar, CRiver, nVersion);

	ARC_GETSET_MACRO(ar, n_user);
	ARC_STD_STRING_MACRO(ar, description);

	if (ar.IsStoring())
	{
		ASSERT(sizeof(ULONGLONG) >= sizeof(std::list<CRiverPoint>::size_type));
		ULONGLONG count_points = this->m_listPoints.size();
		ar << count_points;

		std::list<CRiverPoint>::iterator iter = this->m_listPoints.begin();
		for (; iter != this->m_listPoints.end(); ++iter)
		{
			iter->Serialize(ar);
		}
	}
	else
	{
		ASSERT(sizeof(ULONGLONG) >= sizeof(std::list<CRiverPoint>::size_type));
		ULONGLONG count_points;
		ar >> count_points;

		this->m_listPoints.clear();
		for (ULONGLONG i = 0; i < count_points; ++i)
		{
			CRiverPoint pt;
			pt.Serialize(ar);
			this->m_listPoints.push_back(pt);
		}
	}

	ARC_VERSION(ar, CRiver, nVersion);
}

CRiverPoint::CRiverPoint(void)
: x_defined(0)
, y_defined(0)
, z_defined(0)
, depth_defined(0)
, k_defined(0)
, width_defined(0)
, thickness_defined(0)
, x(0.0)
, y(0.0)
, z(0.0)
{
}

CRiverPoint::CRiverPoint(const River_Point& rp)
: x_defined(rp.x_defined)
, y_defined(rp.y_defined)
, z_defined(rp.z_input_defined)
, depth_defined(rp.depth_defined)
, k_defined(rp.k_defined)
, width_defined(rp.width_defined)
, thickness_defined(rp.thickness_defined)
{
	if (this->x_defined)         this->x         = rp.x;
	if (this->y_defined)         this->y         = rp.y;
	if (this->z_defined)         this->z         = rp.z;
	if (this->depth_defined)     this->depth     = rp.depth;
	if (this->k_defined)         this->k         = rp.k;
	if (this->width_defined)     this->width     = rp.width;
	if (this->thickness_defined) this->thickness = rp.thickness;

	if (rp.head_defined && rp.head)
	{
		for (int i = 0; i < rp.head->count_properties; ++i)
		{
			Ctime t(rp.head->properties[i]->time);
			CRiverState rpt(rp.head->properties[i]->property->v[0]);
			this->Insert(t, rpt);
		}
	}
	if (rp.solution_defined && rp.solution)
	{
		for (int i = 0; i < rp.solution->count_properties; ++i)
		{
			Ctime t(rp.solution->properties[i]->time);
			CRiverState rpt((int)rp.solution->properties[i]->property->v[0]);
			this->Insert(t, rpt);
		}
	}
}

void CRiverPoint::Insert(const Ctime& time, const CRiverState& state)
{
	CRiverState& rs = this->m_riverSchedule[time];

	if (state.head_defined)
	{
		rs.SetHead(state.head);
	}
	if (state.solution_defined)
	{
		rs.SetSolution(state.solution);
	}
}

bool CRiverPoint::IsAnyThingDefined(void)const
{
	return (this->depth_defined || this->k_defined || this->width_defined || this->thickness_defined || !this->m_riverSchedule.empty());
}

void CRiverPoint::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(x,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(y,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(z,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(depth,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(k,         H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(width,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(thickness, H5T_NATIVE_DOUBLE);

	static const char sz_schedule[] = "Schedule";

	if (bStoring)
	{
		CTimeSeries<CRiverState>::SerializeCreate(sz_schedule, this->m_riverSchedule, loc_id);
	}
	else
	{
		CTimeSeries<CRiverState>::SerializeOpen(sz_schedule, this->m_riverSchedule, loc_id);
	}
}

void CRiverPoint::Serialize(CArchive& ar)
{
	ARC_GETSET_DEFINED_MACRO(ar, x);
	ARC_GETSET_DEFINED_MACRO(ar, y);
	ARC_GETSET_DEFINED_MACRO(ar, z);
	ARC_GETSET_DEFINED_MACRO(ar, depth);
	ARC_GETSET_DEFINED_MACRO(ar, k);
	ARC_GETSET_DEFINED_MACRO(ar, width);
	ARC_GETSET_DEFINED_MACRO(ar, thickness);

	this->m_riverSchedule.Serialize(ar);
}

void CRiverState::Serialize(bool bStoring, hid_t loc_id)
{
	HDF_GETSET_DEFINED_MACRO(head,     H5T_NATIVE_DOUBLE);
	HDF_GETSET_DEFINED_MACRO(solution, H5T_NATIVE_INT   );
}

void CRiverState::Serialize(CArchive& ar)
{
	ARC_GETSET_DEFINED_MACRO(ar, head);
	ARC_GETSET_DEFINED_MACRO(ar, solution);
}

std::ostream& operator<< (std::ostream &os, const CRiver &a)
{
	os << "RIVER " << a.n_user;
	if (!a.description.empty())
	{
		os << " " << a.description;
	}
	os << "\n";

	std::list<CRiverPoint>::const_iterator it = a.m_listPoints.begin();
	for(; it != a.m_listPoints.end(); ++it)
	{
		os << (*it);
	}

	return os;
}

std::ostream& operator<< (std::ostream &os, const CRiverPoint &a)
{
	os << "\t-point " << a.x << " " << a.y << "\n";

	size_t nSolutions = 0;
	size_t nHeads     = 0;
	CTimeSeries<CRiverState>::const_iterator it = a.m_riverSchedule.begin();
	for(; it != a.m_riverSchedule.end(); ++it)
	{
		CRiverState state((*it).second);
		if (state.head_defined)
		{
			++nHeads;
		}
		if (state.solution_defined)
		{
			++nSolutions;
		}
	}

	// heads
	//
	if (nHeads)
	{
		os << "\t\t-head\n";
		it = a.m_riverSchedule.begin();
		for(; it != a.m_riverSchedule.end(); ++it)
		{
			Ctime time(it->first);
			CRiverState state(it->second);
			if (state.head_defined)
			{
				os << "\t\t\t" << time.value;
				if (time.type == UNITS && time.input && ::strlen(time.input))
				{
					os << " " << time.input;
				}
				os << "     " << state.head << "\n";
			}
		}
	}

	// solutions
	//
	if (nSolutions)
	{
		os << "\t\t-solution\n";
		it = a.m_riverSchedule.begin();
		for(; it != a.m_riverSchedule.end(); ++it)
		{
			Ctime time(it->first);
			CRiverState state(it->second);
			if (state.solution_defined)
			{
				os << "\t\t\t" << time.value;
				if (time.type == UNITS && time.input && ::strlen(time.input))
				{
					os << " " << time.input;
				}
				os << "     " << state.solution << "\n";
			}
		}
	}

	// width
	//
	if (a.width_defined)
	{
		os << "\t\t-width                       " << a.width << "\n";
	}

	// bed_hydraulic_conductivity
	//
	if (a.k_defined)
	{
		os << "\t\t-bed_hydraulic_conductivity  " << a.k << "\n";
	}

	// bed_hydraulic_conductivity
	//
	if (a.thickness_defined)
	{
		os << "\t\t-bed_thickness               " << a.thickness << "\n";
	}

	// depth
	//
	if (a.depth_defined)
	{
		os << "\t\t-depth                       " << a.depth << "\n";
	}

	/**************
	// bottom
	//
	if (a.z_defined)
	{
		os << "\t\t-bottom                      " << a.z << "\n";
	}
	***************/

	return os;
}