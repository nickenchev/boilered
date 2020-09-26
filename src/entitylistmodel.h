#ifndef ENTITYLISTMODEL_H
#define ENTITYLISTMODEL_H

#include <QAbstractListModel>

namespace Boiler
{
    class EntityComponentSystem;
}

class EntityListModel : public QAbstractListModel
{
    Boiler::EntityComponentSystem &ecs;

public:
    EntityListModel(Boiler::EntityComponentSystem &ecs);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // ENTITYLISTMODEL_H
