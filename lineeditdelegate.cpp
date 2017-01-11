#include "lineeditdelegate.h"

LineEditDelegate::LineEditDelegate(QObject *parent) {}

QWidget *LineEditDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/*option*/,
    const QModelIndex &/*index*/) const
{

    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = index.data().toString();
    lineEdit->setText(value);

    /*
    if (!index.parent().isValid()) lineEdit->setEnabled(false);
    else lineEdit->setEnabled(true);
    */
}

void LineEditDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model, const QModelIndex &index) const
{
    // get editor
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);

    // get old val form the model
    QString oldVal = model->data(index).toString();

    // get new val from editor
    QString newVal = lineEdit->text();

    // get index of type item
    QModelIndex typeIndex = model->index(index.row(), 1, index.parent());

    // get type val
    QString type = model->data(typeIndex).toString();

    // check new value
    QString val = (checkInput(type, newVal, index.column())) ? newVal : oldVal;

    // set data
    //model->setData(index, val, Qt::EditRole);
    emit dataChanged(index, val);
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

bool LineEditDelegate::checkInput(const QString &type, const QString &val, int col)
{
    bool ok;
    if (col != 2) ok = true;
    else
    {
        if (type == "integer")
        {
            val.toInt(&ok);
        }
        else
        {
            if(type == "real")
            {
                val.toFloat(&ok);
            }
            else
            {
                // we shouldn`t edit values of a dict or array
                ok = ((type == "array" || type == "dict") && col == 2) ? false : true;
            }
        }
    }
    return ok;
}
