#ifndef CORE_H
#define CORE_H

#include <QDialog>
#include <QScrollBar>
#include <QInputDialog>
#include <QDate>
#include <QTime>
#include <QKeyEvent>

#include "../core/connection.h"
#include "../core_gui/dlgprogress.h"
#include "../core_gui/dlgmessagebox.h"
#include "../core/ordermodifier.h"
#include "../core/definitions.h"
#include "../core/cafenetwork.h"

#include "../core/stationauth.h"
#include "../core/hall2.h"
#include "../core/menu.h"
#include "../core/order.h"
#include "../core/toolkit.h"
#include "../core/settings.h"
#include "../core/printing.h"
#include "../CafeV4_Station/dlgdishcomment.h"
#include "../CafeV4_Manager/dlgmaindbsettings.h"
#include "../core_gui/dlglist.h"
#include "../core_gui/dlginput.h"

#define DB_ID "1"

extern Hall2 ___hall;
extern Menu ___menu;
extern DlgProgress *___dlgProgress;
extern Settings *___settings;
extern OrderModifierList *___mod;

#endif // CORE_H
