#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QObject>
#include <QtSql>

class DatabaseWorker : public QObject
{
    Q_OBJECT
public:
    DatabaseWorker(QString path);
    QSqlDatabase infoDB;

signals:
    
public slots:
    void DeleteSessionValues();
    void InsertValues(QStringList values);
    void UpdateValues(QString value,int pos);
    void InsertObjects(QStringList data,QStringList values);
    QStringList GetLastData();
    bool haveLastData();
};

#endif // DATABASEWORKER_H
