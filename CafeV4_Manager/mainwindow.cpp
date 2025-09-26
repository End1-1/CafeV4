#include "core.h"
#include "ui_mainwindow.h"
#include "dlglogin.h"
#include "dlgsettingspassword.h"
#include "mainwindow.h"
#include "dlgrecalculation.h"
#include "dlgsalarydoc.h"
#include "dlgabout.h"
#include "qsqlcache.h"
#include "gsettings.h"
#include "qbasesqlwindow.h"
#include "qsinchronizedatabasechanges.h"
#include "gmenu.h"
#include "qimportfromas.h"
#include "gsalerreportcommon.h"
#include "grmenu.h"
#include "gdocinstore.h"
#include "gsalarydoc.h"
#include "gfullfinalmovement.h"
#include "gemployes.h"
#include "qemployesgroups.h"
#include "qsale.h"
#include "qmenunames.h"
#include "qhall.h"
#include "stafffood.h"
#include "qdishestypes.h"
#include "qstorages.h"
#include "dlgconnection.h"
#include "qfoodgroup.h"
#include "qfoodunits.h"
#include "monthlyselfcost.h"
#include "qfoods.h"
#include "qprintschema.h"
#include "qdishesgroupofgroup.h"
#include "qtotalrecipes.h"
#include "gremoveorders.h"
//#include "stafffood.h"
#include <QLibrary>
#include <QShortcut>
#include "dlgcorrectoutstoreofsales.h"
#include "gtabletcontrol.h"
#include "qgroupquery.h"
#include "ggroupqueries.h"
#include "gdiscountcard.h"
#include "qdishremovereason.h"
#include "cnfmaindb.h"
#include <QToolButton>

typedef QString (*actionName)();
typedef int (*call)(QWidget *);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar_3->setVisible(false);
    m_connectionUrl = new QLabel(tr("No connected"));
    ui->statusBar->addWidget(m_connectionUrl);
    ___mainWindow = this;
    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    ___toolBar = ui->toolBar;
    ___toolBarBtn = ui->toolBar_2;
    logout();
    m_tabWidget = ui->tab;

    QBaseSqlWindow::init(___toolBarBtn, ___toolBar);
    QGroupQuery::m_toolBarBtn = ___toolBarBtn;
    QGroupQuery::m_toolBarWnd = ___toolBar;;

    QToolButton *btn = new QToolButton(ui->tab);
    btn->setMinimumSize(20, 20);
    btn->setIcon(QIcon(":/img/menu.png"));
    connect(btn, &QToolButton::clicked, this, &MainWindow::showMenu);
    ui->tab->setCornerWidget(btn, Qt::TopLeftCorner);
    btn->show();

    QShortcut *hotEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(hotEsc, SIGNAL(activated()), this, SLOT(escapeKey()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this,SLOT(search()));
    new QShortcut(QKeySequence(Qt::Key_F3), this, SLOT(searchNext()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLogin_triggered()
{
    logout();
    dlgLogin *login = new dlgLogin(this);
    if (login->exec() == QDialog::Accepted) {
        QSqlCache::clear();
        QBaseSqlWindow::setUsername(___ff_user->fullName);
#ifdef QT_DEBUG
        m_connectionUrl->setText(QSystem::objectName() + " / " + ___ff_user->fullName + " / DEBUG");
#else
        m_connectionUrl->setText(QSystem::objectName() + " / " + ___ff_user->fullName);
#endif
        ui->actionLogout->setEnabled(true);
        ui->actionChange_password->setEnabled(true);
        ui->menuCatalog->setEnabled(true);
        ui->menuActions->setEnabled(true);
        ui->menuReports->setEnabled(true);
        ui->menuOther_Reports->setEnabled(true);

        ui->actionSinchronize_database_changes->setEnabled(___ff_user->roleRead(ROLE_M_SYNC_WITH_AS) && QSystem::isMain());
        ui->actionGroup_queries->setEnabled(QSystem::isMain());

        ui->actionNew_store_order->setEnabled(___ff_user->roleWrite(ROLE_M_STORE_DOCUMENTS));
        ui->actionRecount_output_of_dishes->setEnabled(___ff_user->roleRead(ROLE_M_CALC_OUTPUT));
        ui->actionNew_salary_document->setEnabled(___ff_user->roleWrite(ROLE_M_SALARY));
        ui->actionGet_store_orders_from_AS->setEnabled(___ff_user->roleRead(ROLE_M_SYNC_WITH_AS));
        ui->actionRecount_output_of_dishes->setEnabled(___ff_user->roleRead(ROLE_M_STORE_DOCUMENTS));
        ui->actionCorrect_output_sales_storages->setEnabled(___ff_user->roleRead(ROLE_M_STORE_DOCUMENTS));

        ui->actionStore_documents->setEnabled(___ff_user->roleRead(ROLE_M_STORE_DOCUMENTS));
        ui->actionSalary->setEnabled(___ff_user->roleRead(ROLE_M_SALARY));

        ui->actionRecipes_2->setEnabled(___ff_user->roleRead(ROLE_M_MENU));
        ui->actionRemoved_orders->setEnabled(___ff_user->roleRead(ROLE_M_CANCELATION));
        ui->actionMovement_of_goods->setEnabled(___ff_user->roleRead(ROLE_M_STORE_DOCUMENTS));
        ui->actionGroup_queries->setEnabled(___ff_user->roleRead(ROLE_M_GROUP_QUERIES));

        ui->menuEmployes->setEnabled(___ff_user->roleRead(ROLE_M_COMMON_DIRECTORY));
        ui->menuMenu->setEnabled(___ff_user->roleRead(ROLE_M_MENU));
        ui->menuGoods->setEnabled(___ff_user->roleRead(ROLE_M_MENU));
        ui->menuHall->setEnabled(___ff_user->roleRead(ROLE_M_COMMON_DIRECTORY));
        ui->actionStorages->setEnabled(___ff_user->roleRead(ROLE_M_COMMON_DIRECTORY));
        ui->actionPartners->setEnabled(___ff_user->roleRead(ROLE_M_COMMON_DIRECTORY));
        ui->menuConfigurations->setEnabled(___ff_user->roleRead(ROLE_M_COMMON_DIRECTORY));
        ui->menuActions->setEnabled(ui->menuActions->isEnabled() && !___ff_user->roleRead(ROLE_M_VIEWER));
        ui->actionSales->setEnabled(___ff_user->roleRead(ROLE_M_SALES));
        ui->actionDiscount_cards->setEnabled(___ff_user->roleRead(ROLE_M_DISCOUNT));

        deleteMdiChilds();

        for (QMap<QAction*, QString>::iterator it = m_costumReports.begin(); it != m_costumReports.end(); it++)
            ui->menuOther_Reports->removeAction(qobject_cast<QAction*>(it.key()));
        m_costumReports.clear();
        QDir d = QSystem::appPath() + "\\plugins\\";
        QStringList files = d.entryList();
        for (QStringList::const_iterator i = files.constBegin(); i != files.constEnd(); i++) {
            if (*i == "." || *i == "..")
                continue;
            QLibrary l(d.path() + "/" + *i);
            l.load();
            actionName an = (actionName)(l.resolve("actionName"));
            if (!an)
                continue;
            call c = (call) (l.resolve("call"));
            c(this);
            QAction *a = ui->menuOther_Reports->addAction(an());
            connect(a, SIGNAL(triggered()), SLOT(costumAction()));
            m_costumReports[a] = *i;
        }
    }
    delete login;
}

void MainWindow::showMenu()
{
    GridWidget *gw = dynamic_cast<GridWidget*>(ui->tab->currentWidget());
    if (gw) {
        gw->showMenu();
    }
}

void MainWindow::costumAction()
{

}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::logout()
{
    setToolbarButtonsInvisible();
    deleteMdiChilds();
    m_connectionUrl->setText(tr("Not connected"));
    ui->actionLogout->setEnabled(false);
    ui->actionChange_password->setEnabled(false);
    ui->menuCatalog->setEnabled(false);
    ui->menuActions->setEnabled(false);
    ui->menuReports->setEnabled(false);
    ui->menuOther_Reports->setEnabled(false);
}

void MainWindow::deleteMdiChilds()
{
    while (ui->tab->count() > 0) {
        ui->tab->widget(0)->deleteLater();
        ui->tab->removeTab(0);
    }
}

void MainWindow::on_actionLogout_triggered()
{
    logout();
}

void MainWindow::setToolbarButtonsInvisible()
{
    QList<QAction*> actions = ui->toolBar_2->actions();
    for (QList<QAction*>::iterator it = actions.begin(); it != actions.end(); it++)
        if (*it)
            (*it)->setVisible(false);
}

void MainWindow::on_actionNew_store_order_triggered()
{
    createWindow<StoreOrder>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionDishes_triggered()
{
    createWindow<GMenu>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionStore_documents_triggered()
{
    createWindow<GDocInStore>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionEmployes_triggered()
{
    createWindow<GEmployes>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionRecount_output_of_dishes_triggered()
{
    DlgRecalculation *d = new DlgRecalculation(this);
    d->exec();
    delete d;
}

void MainWindow::on_actionNew_salary_document_triggered()
{

    DlgSalaryDoc *d = createWindow<DlgSalaryDoc>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionSalary_triggered()
{
    createWindow<GSalaryDoc>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionAbout_triggered()
{
    DlgAbout *d = new DlgAbout(this);
    d->exec();
    delete d;
}

void MainWindow::on_actionCustomers_triggered()
{
}

void MainWindow::on_actionNew_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionNew();
}

MdiWindow *MainWindow::activeBaseGrid()
{
    QWidget *s = ui->tab->currentWidget();
    if (!s)
        return 0;
    MdiWindow *b = qobject_cast<MdiWindow*>(s);
    return b;
}

void MainWindow::continueCreateWindow(QWidget *t, QAction *a)
{
    if (a == nullptr) {
        a = new QAction();
    }
    MdiWindow *mw = dynamic_cast<MdiWindow*>(t);
    if (mw) {
        mw->setToolbarButtons(___toolBarBtn);
    }
    GridWidget *gw = dynamic_cast<GridWidget*>(t);
    if (gw) {
        gw->setTitle(a->text());
        gw->prepare();
        gw->go();
    }
    m_tabWidget->addTab(t, a->icon(), a->text());
    m_tabWidget->setCurrentWidget(t);
}

void MainWindow::search()
{
    MdiWindow *b = activeBaseGrid();
    if (b) {
        b->actionSearch();
    } else {
        GridWidget *gw = dynamic_cast<GridWidget*>(ui->tab->currentWidget());
        if (gw) {
            gw->search();
        }
    }
}

void MainWindow::searchNext()
{
    MdiWindow *b = activeBaseGrid();
    if (b) {
        b->actionSearchNext();
    } else {
        GridWidget *gw = dynamic_cast<GridWidget*>(ui->tab->currentWidget());
        if (gw) {
            gw->search();
        }
    }
}

void MainWindow::escapeKey()
{
    GridWidget *gw = dynamic_cast<GridWidget*>(ui->tab->currentWidget());
    if (gw) {
        gw->escapeKey();
    }
}

void MainWindow::on_actionEdit_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionEdit();
}

void MainWindow::on_actionRefresh_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionRefresh();
}

void MainWindow::on_actionDelete_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionDelete();
}

void MainWindow::on_actionSales_settings_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_SALES_SETTINGS);
}

void MainWindow::on_actionSave_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionSave();
}

void MainWindow::on_actionCopy_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_COPY);
}

void MainWindow::on_actionPaste_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_PASTE);
}

void MainWindow::on_actionSinchronize_database_changes_triggered()
{
    createWindow<QSinchronizeDatabaseChanges>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionRecipe_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_RECIPE);
}

void MainWindow::on_actionMenu_2_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_MENU);
}

void MainWindow::on_actionFilter_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionFilter();
}

void MainWindow::on_actionMagic_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_MAGIC);
}

void MainWindow::on_actionPrint_preview_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionPrintPreview();
}

void MainWindow::on_actionPrint_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionPrint();
}

void MainWindow::on_actionExport_to_Excel_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionExcel("");
}

void MainWindow::on_actionGet_store_orders_from_AS_triggered()
{
    createWindow<QImportFromAS>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionView_prices_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b) {
        b->actionCostum(ACTION_VIEW_PRICES);
        b->actionCostum(ACTION_GROUP_SALARY);
    }
}

void MainWindow::on_actionMovement_of_goods_triggered()
{
    createWindow<GFullFinalMovement>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionSet_password_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_SET_PASSWORD);
}

void MainWindow::on_actionClear_password_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_CLEAR_PASSWORD);
}

void MainWindow::on_actionAccess_control_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_ACCESS_CONTROL);
}

void MainWindow::on_actionEmployes_groups_triggered()
{
    createWindow<QEmployesGroups>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionUp_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_UP);
}

void MainWindow::on_actionDown_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_DOWN);
}

void MainWindow::on_actionMultiselection_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionMultiSelect(___toolBarBtn);
}

void MainWindow::on_actionSales_triggered()
{
    createWindow<QSale>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionHistory_2_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_HISTORY);
}

void MainWindow::on_actionMenus_triggered()
{
    createWindow<QMenuNames>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionHalls_triggered()
{
    createWindow<QHall>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionDishes_types_triggered()
{
    createWindow<QDishesTypes>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionStorages_triggered()
{
    createWindow<QStorages>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionMain_database_triggered()
{
    if (__cnfmaindb.fPassword.length() > 0) {
        dlgSettingsPassword *dp = new dlgSettingsPassword(__cnfmaindb.fPassword, this);
        if (dp->exec() != QDialog::Accepted) {
            delete dp;
            return;
        }
        delete dp;
    }

    DlgConnection *dc = new DlgConnection(this);
    dc->exec();
    delete dc;
}

void MainWindow::on_actionFood_groups_triggered()
{
    createWindow<QFoodGroup>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionFood_units_triggered()
{
    createWindow<QFoodUnits>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionFood_names_triggered()
{
    createWindow<QFoods>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionPrinter_schemas_triggered()
{
    createWindow<QPrintSchema>(static_cast<QAction*>(sender()));
}


void MainWindow::on_actionGroup_of_types_triggered()
{
    createWindow<QDishesGroupOfGroup>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionRecipes_2_triggered()
{
    createWindow<QTotalRecipes>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionSearch_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionSearch();
}

void MainWindow::on_actionRemoved_orders_triggered()
{
    createWindow<gremoveorders>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionSearch_Next_triggered()
{

}

void MainWindow::on_actionTablet_control_triggered()
{
    //GTabletControl *g = new GTabletControl(this);
    //g->show();
}

void MainWindow::on_actionCorrect_output_sales_storages_triggered()
{
    DlgCorrectOutStoreOfSales *d = new DlgCorrectOutStoreOfSales(this);
    d->exec();
    delete d;
}

void MainWindow::on_actionCorrect_output_store_of_sales_table_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_CORRECT_STORE);
}

void MainWindow::on_actionImage_triggered()
{
    MdiWindow *b = activeBaseGrid();
    if (b)
        b->actionCostum(ACTION_IMG);
}

void MainWindow::on_actionGroup_queries_triggered()
{
    createWindow<QGroupQuery>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionStation_configuration_triggered()
{
    createWindow<GSettings>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionGroup_queries_2_triggered()
{
    createWindow<GGroupQueries>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionDiscount_cards_triggered()
{
    createWindow<GDiscountCard>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionDishes_remove_states_triggered()
{
    createWindow<QDishRemoveReason>(static_cast<QAction*>(sender()));
}

void MainWindow::on_tab_currentChanged(int index)
{
    MdiWindow *m = dynamic_cast<MdiWindow*>(ui->tab->currentWidget());
    if (m) {
        m->setToolbarButtons(___toolBarBtn);
    } else {

    }
}

void MainWindow::on_tab_tabCloseRequested(int index)
{
    ui->tab->widget(index)->deleteLater();
    ui->tab->removeTab(index);
}

void MainWindow::on_actionNew_menu_names_triggered()
{
    createWindow<GRMenu>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionStaff_food_triggered()
{
    createWindow<StaffFood>(static_cast<QAction*>(sender()));
}

void MainWindow::on_actionMonthly_selfcost_triggered()
{
    createWindow<MonthlySelfcost>(static_cast<QAction*>(sender()));
}

