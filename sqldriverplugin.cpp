#include "sqldriverplugin.h"
#include <QString>
#include <QSqlError>

//==========================================================================================================

MDB_DriverPlugin::MDB_DriverPlugin(QObject *parent)
  : QSqlDriverPlugin(parent)
{
}

QSqlDriver *MDB_DriverPlugin::create(const QString &key)
{
  if (key == QLatin1String("QSQLMDB")){
      MDB_Driver *driver = new MDB_Driver();
      return driver;
    }
  else {return 0;}
}

//==========================================================================================================

bool MDB_Driver::hasFeature(DriverFeature feature) const
{
  switch (feature) {
    case QSqlDriver::Unicode:
    case QSqlDriver::Transactions:
    case QSqlDriver::PreparedQueries:
      return true;
    default:
      return false;
    }
}

//=========================================================================================================

bool MDB_Driver::open(const QString & db_name, const QString & user = "user",
                      const QString & password = "qwerty", const QString &host = "localhost",
                      int port = 27017, const QString & options = "nop")
{
  auto trash = password;
  trash = options;
  trash = user;
  QString newURI = "mongodb://" + host + ":" + port; //добавить проверку
  mongocxx::instance inst{};

  mongocxx::options::client client_options;
  auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
  client_options.server_api_opts(api);

  mongocxx::client conn{mongocxx::uri{newURI.toStdString()}, client_options};
  db = conn[db_name.toStdString()];

  setOpen(true);
  setOpenError(false);
  return true;
}

//==========================================================================================================

void MDB_Driver::close()
{
  if (isOpen())
    {
      setOpen(false);
      setOpenError(false);
    }
}

//==========================================================================================================

QVariant MDB_Result::data(int) {
  return QVariant();
}

//==========================================================================================================

bool MDB_Result::reset(const QString &query)
{
  if (!drv->isOpen() || drv->isOpenError())
    {
      setLastError(QSqlError("You need to open the DB for reset.",
                             "[QSQLMDB]", QSqlError::ConnectionError, 42));
      return false;
    }
  if (!query.startsWith("db."))
    {
      setLastError(QSqlError("Incorrect access to the database.",
                             "[QSQLMDB]", QSqlError::StatementError));
      return false;
    }
  QString q = query;
  act_coll_name = query.section('.', 1, 1);
  q.remove(0, act_coll_name.size()+3);

  if (q.startsWith("insertOne"))  //добавить проверки частные
    {
      mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
      q.remove(act_coll_name);
      q.remove("insertOne");
      q.remove(1);
      q.remove(-1);
      bsoncxx::document::value document = bsoncxx::from_json(q.toStdString()); //возможны проблемы с переводом в json
      coll.insert_one(document.view());
      q = "";
      return true;
    } else

    if (q.startsWith("insertMany"))
      {
        mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
        q.remove(act_coll_name);
        q.remove("insertMany");
        q.remove(1);
        q.remove(-1);
        q.remove(1);
        q.remove(-1);
        QStringList buff = q.split("}, {");
        for(QString &q_i : buff)
          {
            q_i.prepend("{");
            q_i.append("}");
            bsoncxx::document::value document = bsoncxx::from_json(q_i.toStdString()); //возможны проблемы с переводом в json
            coll.insert_one(document.view());
          }
        q = "";
        return true;
      } else

      if (q.startsWith("find"))
        {
          mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
          q.remove(act_coll_name);
          q.remove("find");
          q.remove(1);
          q.remove(-1);
          bsoncxx::document::value filter = bsoncxx::from_json(q.toStdString()); //возможны проблемы с переводом в json
          auto cursor = coll.find(filter.view());
          out_cursor.clear();
          for (auto&& docStr : cursor)
            {
              out_cursor.push_back(QString::fromStdString(bsoncxx::to_json(docStr)));
            }
          q = "";

          setSelect(true);
          setActive(true);
          return true;
        } else

        if (q.startsWith("deleteOne"))
          {
            mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
            q.remove(act_coll_name);
            q.remove("deleteOne");
            q.remove(1);
            q.remove(-1);
            bsoncxx::document::value document = bsoncxx::from_json(q.toStdString()); //возможны проблемы с переводом в json
            coll.delete_one(document.view());
            q = "";
            return true;
          } else

          if (q.startsWith("deleteMany"))
            {
              mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
              q.remove(act_coll_name);
              q.remove("deleteMany");
              q.remove(1);
              q.remove(-1);
              q.remove(1);
              q.remove(-1);
              QStringList buff = q.split("}, {");
              for(QString &q_i : buff)
                {
                  q_i.prepend("{");
                  q_i.append("}");
                  bsoncxx::document::value document = bsoncxx::from_json(q_i.toStdString()); //возможны проблемы с переводом в json
                  coll.delete_one(document.view());
                }
              q = "";
              return true;
            } else
            if (q.startsWith("updateOne"))
              {
                mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
                q.remove(act_coll_name);
                q.remove("updateOne");
                q.remove(1);
                q.remove(-1);
                QStringList buff = q.split("}, {");
                if(buff.size() != 2)
                  {
                    setLastError(QSqlError("Incorrect input for the command *updateOne*.",
                                           "[QSQLMDB]", QSqlError::StatementError));
                    return false;
                  }
                buff[0].append("}");
                buff[1].prepend("{");
                bsoncxx::document::value findDoc = bsoncxx::from_json(buff[0].toStdString()); //возможны проблемы с переводом в json
                bsoncxx::document::value update = bsoncxx::from_json(buff[1].toStdString());
                coll.update_one(findDoc.view(), update.view());
                q = "";
                return true;
              } else
              {
                setLastError(QSqlError("Incorrect SQL command.",
                                       "[QSQLMDB]", QSqlError::StatementError));
                return false;
              }
}

//==========================================================================================================

int MDB_Result::size() {
  mongocxx::collection coll = drv->db.collection(act_coll_name.toStdString());
  return coll.count_documents({});
}

//==========================================================================================================

QVector<QString> MDB_Result::findResultOut()
{
  if(isSelect() == true && isActive() == true)
    {
      setSelect(false);
      setActive(false);
      return out_cursor;
    } else
    {
      setLastError(QSqlError("There is no search result or the result is incorrect.",
                             "[QSQLMDB]", QSqlError::TransactionError));
      QVector<QString> Err;
      return Err;
    }
}

//==========================================================================================================
