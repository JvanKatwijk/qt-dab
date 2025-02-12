/****************************************************************************
** Meta object code from reading C++ file 'spyserver-client.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt-devices/spy-server-16/spyserver-client.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spyserver-client.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16spyServer_clientE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN16spyServer_clientE = QtMocHelpers::stringData(
    "spyServer_client",
    "setConnection",
    "",
    "wantConnect",
    "setGain",
    "handle_autogain",
    "handle_checkTimer",
    "data_ready"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN16spyServer_clientE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x08,    1 /* Private */,
       3,    0,   51,    2, 0x08,    2 /* Private */,
       4,    1,   52,    2, 0x08,    3 /* Private */,
       5,    1,   55,    2, 0x08,    5 /* Private */,
       6,    0,   58,    2, 0x08,    7 /* Private */,
       7,    0,   59,    2, 0x0a,    8 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject spyServer_client::staticMetaObject = { {
    QMetaObject::SuperData::link<deviceHandler::staticMetaObject>(),
    qt_meta_stringdata_ZN16spyServer_clientE.offsetsAndSizes,
    qt_meta_data_ZN16spyServer_clientE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN16spyServer_clientE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<spyServer_client, std::true_type>,
        // method 'setConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wantConnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setGain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_autogain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_checkTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'data_ready'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void spyServer_client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<spyServer_client *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->setConnection(); break;
        case 1: _t->wantConnect(); break;
        case 2: _t->setGain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->handle_autogain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->handle_checkTimer(); break;
        case 5: _t->data_ready(); break;
        default: ;
        }
    }
}

const QMetaObject *spyServer_client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *spyServer_client::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN16spyServer_clientE.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_spyServer_widget"))
        return static_cast< Ui_spyServer_widget*>(this);
    return deviceHandler::qt_metacast(_clname);
}

int spyServer_client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = deviceHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
