#include "grselectormodel.h"

GrSelectorModel::GrSelectorModel(QObject *parent) :
    QObject(parent)
{
    mResult = false;
    mMultiselection = false;
}
