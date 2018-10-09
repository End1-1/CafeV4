#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core.h"
#include "mdiwindow.h"
#include <QMdiSubWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    template <class T>
    static T* createWindow() {
        T *t = new T();
        dynamic_cast<MdiWindow*>(t)->setToolbarButtons(___toolBarBtn);
        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow*>(t->parent());
        qApp->processEvents();
        if (subWindow) {
            subWindow->resize(t->minimumWidth() == 0 ? 1000 : t->minimumWidth(), t->minimumHeight() == 0 ? 700 : t->minimumHeight());
            subWindow->move(30 * ___mdiArea->subWindowList().count(), 30 * ___mdiArea->subWindowList().count());
        }
        t->showMaximized();
        return t;
    }

public slots:
    void on_actionLogin_triggered();
    
private slots:
    void costumAction();
    void on_actionQuit_triggered();
    void on_actionLogout_triggered();
    void on_actionNew_store_order_triggered();
    void on_actionDishes_triggered();
    void on_actionStore_documents_triggered();
    void on_actionEmployes_triggered();
    void on_actionRecount_output_of_dishes_triggered();
    void on_actionNew_salary_document_triggered();
    void on_actionSalary_triggered();
    void on_actionAbout_triggered();
    void on_actionCustomers_triggered();
    void on_actionNew_triggered();
    void on_mdiArea_subWindowActivated(QMdiSubWindow *arg1);
    void on_actionEdit_triggered();
    void on_actionRefresh_triggered();
    void on_actionDelete_triggered();
    void on_actionSales_settings_triggered();
    void on_actionSave_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionSinchronize_database_changes_triggered();
    void on_actionRecipe_triggered();
    void on_actionMenu_2_triggered();
    void on_actionFilter_triggered();
    void on_actionMagic_triggered();
    void on_actionPrint_preview_triggered();
    void on_actionPrint_triggered();
    void on_actionExport_to_Excel_triggered();
    void on_actionGet_store_orders_from_AS_triggered();
    void on_actionView_prices_triggered();
    void on_actionMovement_of_goods_triggered();
    void on_actionSet_password_triggered();
    void on_actionClear_password_triggered();
    void on_actionAccess_control_triggered();
    void on_actionEmployes_groups_triggered();
    void on_actionUp_triggered();
    void on_actionDown_triggered();
    void on_actionMultiselection_triggered();
    void on_actionSales_triggered();
    void on_actionHistory_2_triggered();
    void on_actionMenus_triggered();
    void on_actionHalls_triggered();
    void on_actionDishes_types_triggered();
    void on_actionStorages_triggered();
    void on_actionMain_database_triggered();
    void on_actionFood_groups_triggered();
    void on_actionFood_units_triggered();
    void on_actionFood_names_triggered();
    void on_actionPrinter_schemas_triggered();
    void on_actionGroup_of_types_triggered();
    void on_actionRecipes_2_triggered();
    void on_actionSearch_triggered();
    void on_actionRemoved_orders_triggered();
    void on_actionSearch_Next_triggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMap<QAction*, QString> m_costumReports;
    QLabel *m_connectionUrl;
    void setToolbarButtonsInvisible();
    void logout();
    void deleteMdiChilds();
    MdiWindow *activeBaseGrid();

private slots:
    void search();
    void on_actionTablet_control_triggered();
    void on_actionCorrect_output_sales_storages_triggered();
    void on_actionCorrect_output_store_of_sales_table_triggered();
    void on_actionImage_triggered();
    void on_actionGroup_queries_triggered();
    void on_actionStation_configuration_triggered();
    void on_actionGroup_queries_2_triggered();
    void on_actionDiscount_cards_triggered();
    void on_actionDishes_remove_states_triggered();
    void on_actionRecipes_triggered();
    void on_actionInventoiztionDocuments_triggered();
    void on_actionNew_menu_triggered();
    void on_actionNew_recipes_triggered();
    void on_actionNew_menu_names_triggered();
};

#endif // MAINWINDOW_H
