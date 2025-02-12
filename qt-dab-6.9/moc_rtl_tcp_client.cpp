/****************************************************************************
** Meta object code from reading C++ file 'rtl_tcp_client.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt-devices/rtl_tcp/rtl_tcp_client.h"
#include <QtNetwork/QSslPreSharedKeyAuthenticator>
#include <QtNetwork/QSslError>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rtl_tcp_client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14rtl_tcp_clientE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN14rtl_tcp_clientE = QtMocHelpers::stringData(
    "rtl_tcp_client",
    "sendGain",
    "",
    "set_fCorrection",
    "readData",
    "wantConnect",
    "setDisconnect",
    "setBiasT",
    "setBandwidth",
    "setPort",
    "setAddress",
    "setAgcMode"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN14rtl_tcp_clientE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x08,    1 /* Private */,
       3,    1,   77,    2, 0x08,    3 /* Private */,
       4,    0,   80,    2, 0x08,    5 /* Private */,
       5,    0,   81,    2, 0x08,    6 /* Private */,
       6,    0,   82,    2, 0x08,    7 /* Private */,
       7,    1,   83,    2, 0x08,    8 /* Private */,
       8,    1,   86,    2, 0x08,   10 /* Private */,
       9,    1,   89,    2, 0x08,   12 /* Private */,
      10,    0,   92,    2, 0x08,   14 /* Private */,
      11,    1,   93,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject rtl_tcp_client::staticMetaObject = { {
    QMetaObject::SuperData::link<deviceHandler::staticMetaObject>(),
    qt_meta_stringdata_ZN14rtl_tcp_clientE.offsetsAndSizes,
    qt_meta_data_ZN14rtl_tcp_clientE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN14rtl_tcp_clientE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<rtl_tcp_client, std::true_type>,
        // method 'sendGain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_fCorrection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'readData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wantConnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setDisconnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setBiasT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setBandwidth'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setAddress'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setAgcMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void rtl_tcp_client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<rtl_tcp_client *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sendGain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->set_fCorrection((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 2: _t->readData(); break;
        case 3: _t->wantConnect(); break;
        case 4: _t->setDisconnect(); break;
        case 5: _t->setBiasT((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->setBandwidth((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->setPort((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->setAddress(); break;
        case 9: _t->setAgcMode((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *rtl_tcp_client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *rtl_tcp_client::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN14rtl_tcp_clientE.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_rtl_tcp_widget"))
        return static_cast< Ui_rtl_tcp_widget*>(this);
    return deviceHandler::qt_metacast(_clname);
}

int rtl_tcp_client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = deviceHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
