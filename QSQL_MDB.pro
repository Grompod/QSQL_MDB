QT += sql

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    sqldriverplugin.cpp

HEADERS += \
    sqldriverplugin.h


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/sqldrivers
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    QSQL_MDB.json \
    QSQL_MDB.pro.user

INCLUDEPATH += D:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/mongo-cxx-driver/src/mongocxx/include/mongocxx/v_noabi
DEPENDPATH += D:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/mongo-cxx-driver/src/mongocxx/include/mongocxx/v_noabi

LIBS += -LD:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/build-mongo-cxx-driver-Default_dd144c-Debug/src/bsoncxx/ -llibbsoncxx.dll
LIBS += -LD:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/build-mongo-cxx-driver-Default_dd144c-Debug/src/mongocxx/ -llibmongocxx.dll

INCLUDEPATH += D:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/mongo-cxx-driver/src/bsoncxx/include/bsoncxx/v_noabi
DEPENDPATH += D:/MONGOCXX_DRIVER/mongo-cxx-driver-r3.10.1/mongo-cxx-driver/src/bsoncxx/include/bsoncxx/v_noabi


