#ifndef EDITORTAB_H
#define EDITORTAB_H

#include <QWidget>
#include <QFileInfo>
#include <QUndoStack>
#include "dommodel.h"
#include "commands.h"

namespace Ui {
class EditorTab;
}

class EditorTab : public QWidget
{
    Q_OBJECT
    
public:

    explicit EditorTab(DomModel *model, QWidget *parent = 0);
    ~EditorTab();

    bool isExpanded();
    void expand();

    DomModel *getModel();
    void setModel(DomModel *m);

    QString getPath();
    QString getFileName();
    void setPath(QString p);

    QModelIndex currentIndex();

    QUndoStack *getUndoStack();

public slots:

     void onItemAded(const QModelIndex &index);
     void on_treeView_expanded();
     void on_treeView_collapsed();

private slots:

     void editorDataAboutToBeSet(const QModelIndex &index, QString val);


private:

    QUndoStack *undoStack;

    Ui::EditorTab *ui;
    bool treeExpanded;
    DomModel *model;
    QFileInfo fileInfo;

    void clearModel();

};

#endif // EDITORTAB_H
