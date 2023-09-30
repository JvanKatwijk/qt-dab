/****************************************************************************
** Meta object code from reading C++ file 'rtl_tcp_client.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../qt-devices/rtl_tcp/rtl_tcp_client.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rtl_tcp_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_rtl_tcp_client_t {
    QByteArrayData data[9];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_rtl_tcp_client_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_rtl_tcp_client_t qt_meta_stringdata_rtl_tcp_client = {
    {
QT_MOC_LITERAL(0, 0, 14), // "rtl_tcp_client"
QT_MOC_LITERAL(1, 15, 8), // "sendGain"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 10), // "set_Offset"
QT_MOC_LITERAL(4, 36, 15), // "set_fCorrection"
QT_MOC_LITERAL(5, 52, 8), // "readData"
QT_MOC_LITERAL(6, 61, 13), // "setConnection"
QT_MOC_LITERAL(7, 75, 11), // "wantConnect"
QT_MOC_LITERAL(8, 87, 13) // "setDisconnect"

    },
    "rtl_tcp_client\0sendGain\0\0set_Offset\0"
    "set_fCorrection\0readData\0setConnection\0"
    "wantConnect\0setDisconnect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_rtl_tcp_client[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       3,    1,   52,    2, 0x08 /* Private */,
       4,    1,   55,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void rtl_tcp_client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<rtl_tcp_client *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sendGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_Offset((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_fCorrection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->readData(); break;
        case 4: _t->setConnection(); break;
        case 5: _t->wantConnect(); break;
        case 6: _t->setDisconnect(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject rtl_tcp_client::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_rtl_tcp_client.data,
    qt_meta_data_rtl_tcp_client,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *rtl_tcp_client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *rtl_tcp_client::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_rtl_tcp_client.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_rtl_tcp_widget"))
        return static_cast< Ui_rtl_tcp_widget*>(this);
    return QObject::qt_metacast(_clname);
}

int rtl_tcp_client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
