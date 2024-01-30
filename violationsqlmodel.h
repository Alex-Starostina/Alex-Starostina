#ifndef VIOLATIONSQLMODEL_H
#define VIOLATIONSQLMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QPolygon>

class ViolationSqlModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ViolationSqlModel(QObject *parent = nullptr);
    int getId(const QModelIndex &index);
    int getTypeViolation(const QModelIndex &ind);

    void deleteFromModel(QModelIndex &ind);
    QPointF getCenter(const QModelIndex &ind);

private:
public slots:
    void init();
signals:
    void deleteBorderFromModel(int);

};

#endif // VIOLATIONSQLMODEL_H
