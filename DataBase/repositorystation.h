#ifndef REPOSITORYSTATION_H
#define REPOSITORYSTATION_H

#include "baserepository.h"

class RepositoryStation : public BaseRepository
{
public:
    explicit RepositoryStation(QSqlDatabase db);

    bool insert(const StationStruct &item, int *outId = nullptr);
    QVector<StationStruct> selectAll() const;
    QVector<StationStruct> selectByFilter(const QVariantMap &filters) const;
    QVector<StationStruct> selectByAnyFilter(const QVariantMap &filters) const;
    bool update(int id, const QMap<QString, QVariant> &fields);
    bool remove(int id);
    bool removeAll();
    bool clearAllAndRestartID();

private:
    StationStruct mapQueryToItem(const QSqlQuery &query) const;
};

#endif // REPOSITORYSTATION_H
