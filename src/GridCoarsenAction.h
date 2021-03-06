#pragma once

#include "Action.h"
#include "GridKeyword.h"

class CWPhastDoc;
class CGridActor;

class CGridCoarsenAction : public CAction
{
public:
	CGridCoarsenAction(CWPhastDoc *document, CGridActor *actor, int min[3], int max[3], int parts[3], const CGridKeyword &gridKeyword, bool skipFirstExecute = false);
	~CGridCoarsenAction(void);

	virtual void Execute();
	virtual void UnExecute();

	void Apply();

protected:
	CWPhastDoc    *Document;
	CGridActor    *Actor;
	int            Min[3];
	int            Max[3];
	int            Parts[3];
	bool           SkipFirstExecute;
	CGridKeyword   GridKeyword;
};
