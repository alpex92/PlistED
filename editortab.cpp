#include "editortab.h"
#include "ui_editortab.h"

#include "comboboxdelegate.h"
#include "lineeditdelegate.h"

EditorTab::EditorTab(DomModel *m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditorTab)
{
    ui->setupUi(this);

    treeExpanded = false;

    model = NULL;

    setModel(m);

    undoStack = new QUndoStack();

    QTreeView *view = ui->treeView;

    LineEditDelegate *delegate1 = new LineEditDelegate(view);

    ComboBoxDelegate *delegate2 = new ComboBoxDelegate(view);

    view->setItemDelegateForColumn(0, delegate1);

    view->setItemDelegateForColumn(1, delegate2);

    view->setItemDelegateForColumn(2, delegate1);

    view->expandToDepth(0);

    connect(model, SIGNAL(itemAdded(const QModelIndex&)), this, SLOT(onItemAded(const QModelIndex&)));

    connect(delegate1, SIGNAL(dataChanged(const QModelIndex&, QString)), this, SLOT(editorDataAboutToBeSet(const QModelIndex &, QString)));
    connect(delegate2, SIGNAL(dataChanged(const QModelIndex&, QString)), this, SLOT(editorDataAboutToBeSet(const QModelIndex &, QString)));
}

EditorTab::~EditorTab()
{
    clearModel();
    delete undoStack;
    delete ui;
}

void EditorTab::onItemAded(const QModelIndex &index)
{
    ui->treeView->expand(index);
}

bool EditorTab::isExpanded()
{
    return treeExpanded;
}

void EditorTab::expand()
{
    if (treeExpanded == false)
    {
        treeExpanded = true;
        ui->treeView->expandAll();
    }
    else
    {
        treeExpanded = false;
        ui->treeView->expandToDepth(0);
    }

    on_treeView_expanded();
}
QModelIndex EditorTab::currentIndex()
{
    QModelIndex i = ui->treeView->currentIndex();
    return QModelIndex(i);
}

DomModel *EditorTab::getModel()
{
    return model;
}

void EditorTab::setModel(DomModel *m)
{
    if (model != NULL) clearModel();
    this->model = m;
    ui->treeView->setModel(m);
}

void EditorTab::clearModel()
{
    ui->treeView->setModel(NULL);
    delete model;
    model = NULL;
}

void EditorTab::on_treeView_expanded()
{
    ui->treeView->resizeColumnToContents(0);
}

void  EditorTab::on_treeView_collapsed()
{
    on_treeView_expanded();
}

QUndoStack *EditorTab::getUndoStack()
{
   return undoStack;
}

QString EditorTab::getPath()
{
    return fileInfo.filePath();
}

void EditorTab::setPath(QString p)
{
    fileInfo.setFile(p);
}

QString EditorTab::getFileName()
{
    //return path.section("/", -1, -1);
    return fileInfo.fileName();
}

void EditorTab::editorDataAboutToBeSet(const QModelIndex &index, QString val)
{
    if (model->itemNotPlist(QModelIndex(index)))
    {
        QUndoCommand *editCommand = new EditCommand(val, model, QModelIndex(index));
        undoStack->push(editCommand);
    }
}
