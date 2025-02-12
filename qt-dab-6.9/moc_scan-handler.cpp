/****************************************************************************
** Meta object code from reading C++ file 'scan-handler.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "support/scan-handler.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scan-handler.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11scanHandlerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN11scanHandlerE = QtMocHelpers::stringData(
    "scanHandler",
    "startScanning",
    "",
    "stopScanning",
    "handle_startKnop",
    "handle_stopKnop",
    "handle_scanMode",
    "handle_showKnop",
    "handle_clearKnop",
    "handle_defaultLoad",
    "handle_defaultStore",
    "handle_loadKnop",
    "handle_storeKnop"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN11scanHandlerE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    0,   81,    2, 0x06,    2 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    1,   84,    2, 0x08,    5 /* Private */,
       7,    0,   87,    2, 0x08,    7 /* Private */,
       8,    0,   88,    2, 0x08,    8 /* Private */,
       9,    0,   89,    2, 0x08,    9 /* Private */,
      10,    0,   90,    2, 0x08,   10 /* Private */,
      11,    0,   91,    2, 0x08,   11 /* Private */,
      12,    0,   92,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject scanHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN11scanHandlerE.offsetsAndSizes,
    qt_meta_data_ZN11scanHandlerE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN11scanHandlerE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<scanHandler, std::true_type>,
        // method 'startScanning'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopScanning'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_startKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_stopKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_scanMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_showKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_clearKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_defaultLoad'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_defaultStore'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_loadKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_storeKnop'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void scanHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<scanHandler *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startScanning(); break;
        case 1: _t->stopScanning(); break;
        case 2: _t->handle_startKnop(); break;
        case 3: _t->handle_stopKnop(); break;
        case 4: _t->handle_scanMode((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->handle_showKnop(); break;
        case 6: _t->handle_clearKnop(); break;
        case 7: _t->handle_defaultLoad(); break;
        case 8: _t->handle_defaultStore(); break;
        case 9: _t->handle_loadKnop(); break;
        case 10: _t->handle_storeKnop(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (scanHandler::*)();
            if (_q_method_type _q_method = &scanHandler::startScanning; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (scanHandler::*)();
            if (_q_method_type _q_method = &scanHandler::stopScanning; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *scanHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *scanHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN11scanHandlerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int scanHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void scanHandler::startScanning()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void scanHandler::stopScanning()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
