#include "maya_ui.h"
#include <maya/MGlobal.h>
#include <maya/MArgList.h>

maya_ui::maya_ui() : MPxCommand()
{
}

maya_ui::~maya_ui()
{
}

MStatus maya_ui::doIt(const MArgList& args)
{
	return MStatus::kSuccess;
}