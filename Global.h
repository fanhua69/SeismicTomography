
#ifndef tag_Global
#define tag_Global

#include "stdafx.h"
#include "NewMount.h"

bool GInitEquation(CProject &project,CDC *pDC=NULL);
bool GResolveEquation(CProject &project,int nGridWidth,int nGridHeight, CSize sizeScan, CDC *pDC,CTrace &trace);

#endif
