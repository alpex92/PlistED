#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qDebug>
#include <QUndoGroup>
#include <QFileDialog>
#include <QDropEvent>

#include "domparser.h"
#include "editortabswidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionNew_activated();
    void on_actionOpen_activated();
    void on_actionClose_activated();
    void on_actionClose_all_activated();

    void on_actionAdd_activated();
    void on_actionRemove_activated();

    void on_actionSave_activated();
    void on_actionSave_as_activated();

    void on_actionExpand_all_activated();
    void on_actionAbout_activated();

    void tabWidget_currentChanged(int index);
    void on_menu_aboutToShow();

    void onCleanChanged(bool clean);

    void openRecentFile();

    void onTabCloseRequest(int i = -1);

private:

    void setExpandText(EditorTab *tab);

    void openFiles(QStringList list = QStringList());

    void savePlist(QString filePath);
    void openPlist(QString filePath = "");

    enum { MaxRecentFiles = 5 };
    void setRecentFiles(const QString &fileName);
    void updateRecentFiles();

    Ui::MainWindow *ui;

    EditorTabsWidget *tabWidget;
    QUndoGroup *undoGroup;

protected:

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // MAINWINDOW_H
