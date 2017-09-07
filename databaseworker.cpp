#include "databaseworker.h"

DatabaseWorker::DatabaseWorker(QString path)
{
    infoDB = QSqlDatabase::addDatabase("QSQLITE");

    //if file exists - just open database
    //else - create and open database

    if(QFile::exists(path))
    {
        infoDB.setDatabaseName(path);
        infoDB.open();
    }
    else
    {
        infoDB.setDatabaseName(path);
        infoDB.open();
        QSqlQuery create_Values_table("CREATE TABLE Values_table (ID integer NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,firstInitX varchar(15) NOT NULL,firstInitY varchar(15) NOT NULL, secondInitX varchar(15) NOT NULL, secondInitY varchar(15) NOT NULL,firstAngle varchar(15) NOT NULL, firstV varchar(15) NOT NULL, secondAngle varchar(15) NOT NULL, secondV varchar(15) NOT NULL)");
        create_Values_table.exec();
        QSqlQuery create_Objects_table("CREATE TABLE Objects_table(O_ID integer NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,firstX varchar(15) NOT NULL, firstY varchar(15) NOT NULL,secondX varchar(15) NOT NULL, secondY varchar(15) NOT NULL,firstT varchar(15) NOT NULL, firstD varchar(15) NOT NULL,secondT varchar(15) NOT NULL, secondD varchar(15) NOT NULL,intersectX varchar(15), intersectY varchar(15), Values_id integer NOT NULL,isNew integer NOT NULL, FOREIGN KEY (Values_id) REFERENCES Values_table (ID))");
        create_Objects_table.exec();
    }
}

void DatabaseWorker::DeleteSessionValues()
{
    QSqlQuery delete_session_values("UPDATE Objects_table SET isNew = 0 WHERE isNEW = 1");
    delete_session_values.exec();
}

void DatabaseWorker::InsertValues(QStringList values)
{
    QString firstInitX = values.at(0);
    QString firstInitY = values.at(1);
    QString secondInitX = values.at(2);
    QString secondInitY = values.at(3);
    QString firstAngle = values.at(4);
    QString firstV = values.at(5);
    QString secondAngle = values.at(6);
    QString secondV = values.at(7);

    QSqlQuery getRowsCount("select count(*) from Values_table where firstInitX = '" + firstInitX + "' AND firstInitY = '" + firstInitY + "' AND secondInitX = '" + secondInitX + "' AND secondInitY = '" + secondInitY + "' AND firstAngle = '" + firstAngle + "' AND firstV = '" + firstV + "' AND secondAngle = '" + secondAngle + "' AND secondV = '" + secondV + "'");
    getRowsCount.exec();
    getRowsCount.next();
    int count = getRowsCount.value(0).toInt();
    if(count == 0)
    {
        QSqlQuery insertOperation;
        insertOperation.exec("insert into Values_table(firstInitX,firstInitY,secondInitX,secondInitY,firstAngle,firstV,secondAngle,secondV) values('" + firstInitX + "'," + firstInitY + ",'" + secondInitX + "','" + secondInitY + "','" + firstAngle + "','" + firstV + "','" + secondAngle + "','" + secondV + "')");
        qDebug() << insertOperation.lastError().text();
    }
}

void DatabaseWorker::InsertObjects(QStringList data,QStringList values)
{
    QString firstX = data.at(0);
    QString firstY = data.at(1);
    QString secondX = data.at(2);
    QString secondY = data.at(3);
    QString firstT = data.at(4);
    QString firstD = data.at(5);
    QString secondT = data.at(6);
    QString secondD = data.at(7);
    QString intersectX = data.at(8);
    QString intersectY = data.at(9);

    QString firstInitX = values.at(0);
    QString firstInitY = values.at(1);
    QString secondInitX = values.at(2);
    QString secondInitY = values.at(3);
    QString firstAngle = values.at(4);
    QString firstV = values.at(5);
    QString secondAngle = values.at(6);
    QString secondV = values.at(7);

    qDebug() << firstX << firstY << secondX << secondY << firstT << firstD << secondT << secondD << intersectX << intersectY;
    qDebug() << firstInitX << firstInitY << secondInitX << secondInitY << firstAngle << firstV << secondAngle << secondV;

    QSqlQuery getID("select ID from Values_table where firstInitX = '" + firstInitX + "' AND firstInitY = '" + firstInitY + "' AND secondInitX = '" + secondInitX + "' AND secondInitY = '" + secondInitY + "' AND firstAngle = '" + firstAngle + "' AND firstV = '" + firstV + "' AND secondAngle = '" + secondAngle + "' AND secondV = '" + secondV + "'");
    getID.exec();
    getID.next();
    QString ID = getID.value(0).toString();

    QSqlQuery insertOperation;
    insertOperation.exec("insert into Objects_table(firstX,firstY,secondX,secondY,firstT,firstD,secondT,secondD,intersectX,intersectY,Values_id,isNew) values('" + firstX + "','" + firstY + "','" + secondX + "','" + secondY + "','" + firstT + "','" + firstD + "','" + secondT + "','" + secondD  + "','" + intersectX + "','" + intersectY + "'," + ID + ",1)");
}

void DatabaseWorker::UpdateValues(QString value, int pos)
{
    QSqlQuery getID("select Values_id from Objects_table where isNew = 1");
    getID.exec();
    getID.next();
    QString ID = getID.value(0).toString();
    QString target;

    switch(pos)
    {
        case 0: {target = "firstInitX"; break;}
        case 1: {target = "firstInitY";break;}
        case 2: {target = "secondInitX";break;}
        case 3: {target = "secondInitY";break;}
        case 4: {target = "firstAngle";break;}
        case 5: {target = "firstV";break;}
        case 6: {target = "secondAngle";break;}
        case 7: {target = "secondV";break;}
    }

    QSqlQuery updateOperation("UPDATE Values_table SET " + target + "=" + value + " WHERE ID = " + ID);
    updateOperation.exec();
}

QStringList DatabaseWorker::GetLastData()
{
    QStringList list;
    QSqlQuery getRowsCount("select count(*) from Objects_table where isNew = 1");
    getRowsCount.exec();
    getRowsCount.next();
    int count = getRowsCount.value(0).toInt();
    if(count != 0)
    {
        QSqlQuery p("select firstX,firstY,secondX,secondY,firstT,firstD,secondT,secondD,intersectX,intersectY,Values_id from Objects_table where isNew = 1");
        p.exec();
        p.next();
        list.append(p.value(0).toString());
        list.append(p.value(1).toString());
        list.append(p.value(2).toString());
        list.append(p.value(3).toString());
        list.append(p.value(4).toString());
        list.append(p.value(5).toString());
        list.append(p.value(6).toString());
        list.append(p.value(7).toString());
        list.append(p.value(8).toString());
        list.append(p.value(9).toString());

        QString ID = p.value(10).toString();
        QSqlQuery vals("select firstInitX,firstInitY,secondInitX,secondInitY,firstAngle,firstV,secondAngle,secondV from Values_table where ID = " + ID);
        vals.exec();
        vals.next();
        list.append(vals.value(0).toString());
        list.append(vals.value(1).toString());
        list.append(vals.value(2).toString());
        list.append(vals.value(3).toString());
        list.append(vals.value(4).toString());
        list.append(vals.value(5).toString());
        list.append(vals.value(6).toString());
        list.append(vals.value(7).toString());
    }
    return list;
}

bool DatabaseWorker::haveLastData()
{
    QSqlQuery getRowsCount("select count(*) from Objects_table where isNew = 1");
    getRowsCount.exec();
    getRowsCount.next();
    int count = getRowsCount.value(0).toInt();
    if(count != 0)
    {
        return true;
    }
    return false;
}
