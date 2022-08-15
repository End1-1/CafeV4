#include "grmetypes.h"

GrMeTypes::GrMeTypes(QObject *parent) :
    GrSelectorModel(parent)
{
    mSql = "select id, name from me_types order by 2 ";
    mFields.append(tr("Code"));
    mFields.append(tr("Name"));
}
