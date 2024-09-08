#ifndef SQLDRIVERPLUGIN_H
#define SQLDRIVERPLUGIN_H

#include <QSqlDriverPlugin>
#include <QSqlResult>
#include <QSqlDriver>
#include <QSqlRecord>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

//===========================================================================================

class MDB_DriverPlugin : public QSqlDriverPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QSqlDriverFactoryInterface_iid FILE "QSQL_MDB.json")

public:
  explicit MDB_DriverPlugin(QObject *parent = nullptr);

private:
  QSqlDriver *create(const QString &key) override;
};

//===========================================================================================

class MDB_Driver;

class MDB_Result : public QSqlResult
{
public:

    MDB_Result(const QSqlDriver *driver)
        : QSqlResult(driver)
    {
      drv = (MDB_Driver*)driver;
    }
    ~MDB_Result() {}

protected:
    QVariant data(int /* index */) override;
    bool isNull(int /* index */) override { return false; }
    bool reset(const QString & /* query */) override;
    bool fetch(int /* index */) override { return false; }
    bool fetchFirst() override { return false; }
    bool fetchLast() override { return false; }
    int size() override;
    int numRowsAffected() override { return 0; }
    QSqlRecord record() const override { return QSqlRecord(); }

    QVector<QString> findResultOut();

    MDB_Driver *drv;
    QVector<QString> out_cursor;
    QString act_coll_name;
};

//===========================================================================================

class MDB_Driver : public QSqlDriver
{
public:
    MDB_Driver() {}

    bool hasFeature(DriverFeature /* feature */) const override;
    bool open(const QString & /* db */,
              const QString & /* user */,
              const QString & /* password */,
              const QString & /* host */,
              int /* port */,
              const QString & /* options */) override;
    void close() override;

    mongocxx::v_noabi::database db;

    QSqlResult *createResult() const override { return new MDB_Result(this); }
};

#endif // SQLDRIVERPLUGIN_H
