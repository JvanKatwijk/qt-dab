/****************************************************************************
** Meta object code from reading C++ file 'pluto-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../qt-devices/pluto-handler-2/pluto-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pluto-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_plutoHandler_t {
    QByteArrayData data[9];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_plutoHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_plutoHandler_t qt_meta_stringdata_plutoHandler = {
    {
QT_MOC_LITERAL(0, 0, 12), // "plutoHandler"
QT_MOC_LITERAL(1, 13, 13), // "new_gainValue"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 12), // "new_agcValue"
QT_MOC_LITERAL(4, 41, 15), // "set_gainControl"
QT_MOC_LITERAL(5, 57, 14), // "set_agcControl"
QT_MOC_LITERAL(6, 72, 18), // "toggle_debugButton"
QT_MOC_LITERAL(7, 91, 10), // "set_filter"
QT_MOC_LITERAL(8, 102, 11) // "set_xmlDump"

    },
    "plutoHandler\0new_gainValue\0\0new_agcValue\0"
    "set_gainControl\0set_agcControl\0"
    "toggle_debugButton\0set_filter\0set_xmlDump"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_plutoHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       3,    1,   52,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   55,    2, 0x08 /* Private */,
       5,    1,   58,    2, 0x08 /* Private */,
       6,    0,   61,    2, 0x08 /* Private */,
       7,    0,   62,    2, 0x08 /* Private */,
       8,    0,   63,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void plutoHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<plutoHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->new_gainValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->new_agcValue((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->set_gainControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_agcControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->toggle_debugButton(); break;
        case 5: _t->set_filter(); break;
        case 6: _t->set_xmlDump(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (plutoHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&plutoHandler::new_gainValue)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (plutoHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&plutoHandler::new_agcValue)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject plutoHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_plutoHandler.data,
    qt_meta_data_plutoHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *plutoHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *plutoHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_plutoHandler.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_plutoWidget"))
        return static_cast< Ui_plutoWidget*>(this);
    return QThread::qt_metacast(_clname);
}

int plutoHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void plutoHandler::new_gainValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void plutoHandler::new_agcValue(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
