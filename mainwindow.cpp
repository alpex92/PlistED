#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabWidget = new EditorTabsWidget(this);

    ui->centralWidget->layout()->addWidget(tabWidget);

    QApplication::setApplicationName("PlistED");
    QApplication::setOrganizationName("Alpex");

    undoGroup = new QUndoGroup(this);

    // create undo and redo actions
    QAction *actionUndo = undoGroup->createUndoAction(this, tr("Undo"));
    QAction *actionRedo = undoGroup->createRedoAction(this, tr("Redo"));

    // set shortcuts
    actionUndo->setShortcuts(QKeySequence::Undo);
    actionRedo->setShortcuts(QKeySequence::Redo);

    // add actions to menu
    ui->menuEdit->addAction(actionUndo);
    ui->menuEdit->addAction(actionRedo);

    connect(tabWidget, SIGNAL(currentChanged(int)),
            this, SLOT(tabWidget_currentChanged(int)));

    connect(undoGroup, SIGNAL(cleanChanged(bool)),
            this, SLOT(onCleanChanged(bool)));

    connect(ui->actionFile1, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    connect(ui->actionFile2, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    connect(ui->actionFile3, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    connect(ui->actionFile4, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    connect(ui->actionFile5, SIGNAL(triggered()), this, SLOT(openRecentFile()));

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabCloseRequest(int)));

    updateRecentFiles();

}

MainWindow::~MainWindow()
{
    delete undoGroup;
    delete ui;   
}

void MainWindow::on_actionNew_activated()
{
    // create new model
    DomModel *model = new DomModel();

    // add root
    DomItem *i = model->getRoot()->addChild();

    // set root data
    i->setName("plist");
    i->setType("dict");

    // create tab with model
    tabWidget->createTab(model);
}

void MainWindow::on_actionOpen_activated()
{
    openFiles();
}

void MainWindow::on_actionClose_activated()
{
    if (tabWidget->hasTabs()) onTabCloseRequest();
}

void MainWindow::on_actionClose_all_activated()
{
    if (tabWidget->hasTabs()) tabWidget->closeAllTabs();
}

void MainWindow::openFiles(QStringList list)
{
    if (list.isEmpty())
    {
        list = QFileDialog::getOpenFileNames(
                    this, tr("Select files to open"),
                    "", "Property list (*.plist)");
    }

    for (int i = 0; i < list.size(); ++i) openPlist(list[i]);
}

void MainWindow::openPlist(QString filePath)
{
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly))
        {
            QDomDocument document;

            if (document.setContent(&file))
            {
                qDebug() << QString("File %1 opened").arg(filePath);

                DomModel *model = DomParser::fromDom(document);

                tabWidget->createTab(model, filePath);
            }
            file.close();
        }

        setRecentFiles(filePath);
        updateRecentFiles();
    }
}

void MainWindow::onTabCloseRequest(int i)
{
    if (!undoGroup->isClean())
    {
        // make tab active
        if (i != -1) tabWidget->setCurrentIndex(i);

        // messageobox for save
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("The document has been modified."));
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret)
        {
           case QMessageBox::Cancel:
                // Cancel was clicked
           return;

           case QMessageBox::Save:
               on_actionSave_activated();
           break;

           case QMessageBox::Discard:
               // Don't Save was clicked
           break;
        }
    }

    // get current stack
    QUndoStack *stack = tabWidget->getCurentTab()->getUndoStack();

    // remove stack from group
    undoGroup->removeStack(stack);

    // close tab
    tabWidget->closeTab();
}

void MainWindow::savePlist(QString filePath)
{
    if (tabWidget->hasTabs())
    {
        EditorTab *tab = tabWidget->getCurentTab();

        // get parsed dom doc
        QDomDocument doc = DomParser::toDom(tab->getModel());

        // create and open file
        QFile file(filePath);
        file.open(QIODevice::WriteOnly);

        // create txt stream with file
        QTextStream out(&file);

        // write to file
        doc.save(out, 4);

        // close file
        file.close();

        // set new name
        tab->setPath(filePath);

        //get tab index
        int index = tabWidget->indexOf(tab);

        // get name
        QString name = tab->getFileName();

        // set text
        tabWidget->setTabText(index, name);

        // set stack clean
        undoGroup->activeStack()->clear();
        //undoGroup->activeStack()->setClean();
    }
}

void MainWindow::on_actionSave_activated()
{
   if (tabWidget->hasTabs())
   {
       EditorTab *tab = tabWidget->getCurentTab();
       QString path = tab->getPath();
       if (!path.isEmpty()) savePlist(path);
       else on_actionSave_as_activated();
   }
}

void MainWindow::on_actionSave_as_activated()
{
    if (tabWidget->hasTabs())
    {
        QString str =  QFileDialog::getSaveFileName(
                    this, tr("Save as"), "", tr("Property list (*.plist)"));

        if (!str.isEmpty()) savePlist(str);
    }
}

void MainWindow::on_actionAdd_activated()
{
    if (tabWidget->hasTabs())
    {
        EditorTab *tab = tabWidget->getCurentTab();
        const QModelIndex index = tab->currentIndex();
        //if (index.isValid()) tab->getModel()->addItem(index);

        if (index.isValid())
        {
            QUndoCommand *addCommand = new AddCommand(tab->getModel(), index);
            undoGroup->activeStack()->push(addCommand);
        }
    }
}

void MainWindow::on_actionRemove_activated()
{
    if (tabWidget->hasTabs())
    {
        EditorTab *tab = tabWidget->getCurentTab();
        const QModelIndex index = tab->currentIndex();
        //if (index.isValid()) tab->getModel()->removeItem(index);
        if (index.isValid())
        {
            DomModel *model = tab->getModel();

            if (model->itemNotPlist(index))
            {
                QUndoCommand *removeCommand = new RemoveCommand(model, index);
                undoGroup->activeStack()->push(removeCommand);
            }
        }
    }
}

void MainWindow::on_actionExpand_all_activated()
{
    if (tabWidget->hasTabs())
    {
        EditorTab *tab = tabWidget->getCurentTab();

        tab->expand();

        setExpandText(tab);
    }
}

void MainWindow::on_actionAbout_activated()
{
    QMessageBox::about(this, tr("About PlistED"),
                         "PlistED Copyright 2012 Yaroslav Sushkov");
}

void MainWindow::tabWidget_currentChanged(int index)
{
    if (tabWidget->hasTabs())
    {
        // get tab widget
        EditorTab *tab = tabWidget->getCurentTab();

        setExpandText(tab);

        // set window title to filename
        this->setWindowFilePath(tabWidget->tabText(tabWidget->indexOf(tab)));

        // get undo stack
        QUndoStack *stack = tab->getUndoStack();

        // set active stack
        if (!undoGroup->stacks().contains(stack)) undoGroup->addStack(stack);
        undoGroup->setActiveStack(stack);
    }
    //?
    else this->setWindowFilePath(" ");
}

void MainWindow::onCleanChanged(bool clean)
{
    this->setWindowModified(!clean);
}

void MainWindow::setRecentFiles(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    // remove all inclusions
    files.removeAll(fileName);

    // add to begining
    files.prepend(fileName);

    while (files.size() > MaxRecentFiles)
    files.removeLast();

    settings.setValue("recentFileList", files);
}

void MainWindow::updateRecentFiles()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QAction *action = ui->menuRecent_files->actions().at(i);
        QString text = files.at(i);

        action->setText(text);
        action->setVisible(true);
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) openPlist(action->text());
}

void MainWindow::setExpandText(EditorTab *tab)
{
    QString text = (!tab->isExpanded()) ? tr("Expand all") : tr("Collapse all");
    ui->actionExpand_all->setIconText(text);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasFormat("text/uri-list")) event->accept();

    // application/xml application/x-plist
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) return;

    QStringList list;
    for (int i = 0; i < urls.size(); ++i)
    {
        list.append(urls.at(i).toLocalFile());
    }

    openFiles(list);
}

void MainWindow::on_menu_aboutToShow()
{
    bool enabled = tabWidget->hasTabs();
    ui->actionClose->setEnabled(enabled);
    ui->actionClose_all->setEnabled(enabled);
    ui->actionSave->setEnabled(enabled);
    ui->actionSave_as->setEnabled(enabled);
}



