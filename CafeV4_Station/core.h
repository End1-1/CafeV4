#ifndef CORE_H
#define CORE_H

#include <QMessageBox>
#include <QDialog>
#include <QDebug>
#include <QDate>
#include <QTime>
#include <QWidget>
#include <QKeyEvent>
#include <QtGui>

#include "../core/connection.h"
#include "../core/order.h"
#include "../core/definitions.h"
#include "../core/hall.h"
#include "../core/menu.h"
#include "../core/stationauth.h"
#include "../core_gui/dlgmessagebox.h"
#include "../core/toolkit.h"
#include "../core/orderstoredoc.h"
#include "../core/settings.h"
#include "../core/ordermodifier.h"

#include "../core_gui/dlgprogress.h"
#include "../CafeV4_Manager/dlgmaindbsettings.h"

#define DBID "1"

//Пользователь ___userdata необходим для взаимодействия программы с веб-сервером
//extern UserData ___userdata;

//Пользователь ___staff существует после успешной авторизации до момента закрытия окна заказов.
//Все действия над заказом выполняются от имени этого пользователя
extern StationAuth ___staff;

//Объект хранит в себе данные о залах и столах.
extern Hall ___hall;

//Объект хранит данные меню
extern Menu ___menu;

extern Settings *___settings;

extern DlgProgress *___progressDialog;

extern OrderModifierList *___mod;

#endif
