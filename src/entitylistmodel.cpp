#include "entitylistmodel.h"
#include "core/entitycomponentsystem.h"

using namespace Boiler;

EntityListModel::EntityListModel(EntityComponentSystem &ecs) : ecs(ecs)
{
}

int EntityListModel::rowCount(const QModelIndex&) const
{
    return ecs.getEntityWorld().count();
}

QVariant EntityListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        Entity entity = ecs.getEntityWorld().getEntities().at(index.row());
        return QString("Entity (%1)").arg(entity.getId());
    }
    return QVariant();
}
