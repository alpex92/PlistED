#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include "dommodel.h"
#include <QModelIndex>

class AddCommand : public QUndoCommand
{
public:
    AddCommand(DomModel *model, const QModelIndex &index, QUndoCommand *parent = 0);
    void undo();
    void redo();

private:
    DomModel *m_model;
    QModelIndex m_index;
    QModelIndex m_parent;
};

class RemoveCommand : public QUndoCommand
{
public:
    RemoveCommand(DomModel *model, const QModelIndex &index, QUndoCommand *parent = 0);
    ~RemoveCommand();

    void undo();
    void redo();

private:
    DomModel *m_model;
    QModelIndex m_index;
    QModelIndex m_parent;

    int m_row;
    ItemState *m_state;
};

class EditCommand : public QUndoCommand
{
public:
    EditCommand(QString val, DomModel *model, const QModelIndex &index, QUndoCommand *parent = 0);

    void undo();
    void redo();

private:
    DomModel *m_model;
    QModelIndex m_index;

    QString m_oldVal;
    QString m_newVal;
};

#endif // COMMANDS_H
