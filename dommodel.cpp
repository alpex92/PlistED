
 #include <QtGui>
 #include <QtXml>

 #include "dommodel.h"

 DomModel::DomModel(QObject *parent)
     : QAbstractItemModel(parent)
 {
     rootItem = new DomItem(0, "Root");
     changed = false;
 }

 DomModel::~DomModel()
 {
     delete rootItem;
 }

 DomItem *DomModel::getRoot()
 {
     return rootItem;
 }

 int DomModel::columnCount(const QModelIndex &/*parent*/) const
 {
     return 3;
 }

 QVariant DomModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     DomItem *item = itemForIndex(index);

     QString name = item->getName();
     QString type = item->getType();
     QString value = item->getValue();

     switch (index.column()) {
         case 0:
             return name;
         case 1:
             return type;
         case 2:
             return value;
         default:
             return QVariant();
     }
 }

 bool DomModel::setData(const QModelIndex & index, const QVariant & value, int role)
 {
    if (!index.isValid()) return false;
    else
    {
        DomItem *item = itemForIndex(index);

        QString str = value.toString();

        switch (index.column())
        {
            case 0:
                item->setName(str);
                break;
            case 1:
                item->setType(str);
                break;
            case 2:
                item->setValue(str);
        };

        emit dataChanged(index, index);
        return true;
    }

 }

 Qt::ItemFlags DomModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable| Qt::ItemIsEditable;
 }

 QVariant DomModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         switch (section) {
             case 0:
                 return tr("Key");
             case 1:
                 return tr("Type");
             case 2:
                 return tr("Value");
             default:
                 return QVariant();
         }
     }

     return QVariant();
 }

 QModelIndex DomModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     DomItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = itemForIndex(parent);

     DomItem *childItem = parentItem->child(row);

     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex DomModel::parent(const QModelIndex &child) const
 {
     if (!child.isValid()) return QModelIndex();

     DomItem *childItem = itemForIndex(child);
     DomItem *parentItem = childItem->parent();

     if (!parentItem || parentItem == rootItem)
         return QModelIndex();

     int row = parentItem->row();

     if (parentItem) return createIndex(row, 0, parentItem);

     return QModelIndex();
 }

 int DomModel::rowCount(const QModelIndex &parent) const
 {
     if (parent.column() > 0)
         return 0;

     DomItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = itemForIndex(parent);

     return parentItem->childCount();
 }

 QModelIndex DomModel::addItem(const QModelIndex &parent, int row, ItemState *state)
 {
     if (parent.isValid())
     {
         QModelIndex index = this->index(parent.row(), 0, parent.parent());
         DomItem *item = itemForIndex(index);

         if (row == -1) row = item->childCount();
         DomItem * child = NULL;

         beginInsertRows(index, row, row);

         if (state != NULL)
         {
             child = state->getState()->clone();
         }
         child = item->addChild(row, child);

         endInsertRows();
         emit itemAdded(index);

         return this->index(child->row(), 0, index);
     }
     return QModelIndex();
 }

 void DomModel::removeItem(const QModelIndex &index)
 {
     if (index.isValid())
     {
         DomItem *item = itemForIndex(index);
         int row = index.row();
         QModelIndex parent = index.parent();
         beginRemoveRows(parent, row, row);
         item->removeFromParent(index.row());
         endRemoveRows();
     }
 }

 ItemState *DomModel::saveItemState(const QModelIndex &index)
 {
     return new ItemState(itemForIndex(index));
 }

 DomItem *DomModel::itemForIndex(const QModelIndex &index)
 {
     return static_cast<DomItem*>(index.internalPointer());
 }

 bool DomModel::itemNotPlist(const QModelIndex &index)
 {
    return itemForIndex(QModelIndex(index))->getName() != "plist";
 }


