/****************************************************************************
** Meta object code from reading C++ file 'hackrf-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../qt-devices/hackrf-handler/hackrf-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hackrf-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_hackrfHandler_t {
    QByteArrayData data[12];
    char stringdata0[173];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_hackrfHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_hackrfHandler_t qt_meta_stringdata_hackrfHandler = {
    {
QT_MOC_LITERAL(0, 0, 13), // "hackrfHandler"
QT_MOC_LITERAL(1, 14, 16), // "signal_antEnable"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "signal_ampEnable"
QT_MOC_LITERAL(4, 49, 15), // "signal_vgaValue"
QT_MOC_LITERAL(5, 65, 15), // "signal_lnaValue"
QT_MOC_LITERAL(6, 81, 14), // "handle_LNAGain"
QT_MOC_LITERAL(7, 96, 14), // "handle_VGAGain"
QT_MOC_LITERAL(8, 111, 12), // "handle_biasT"
QT_MOC_LITERAL(9, 124, 12), // "handle_Ampli"
QT_MOC_LITERAL(10, 137, 20), // "handle_ppmCorrection"
QT_MOC_LITERAL(11, 158, 14) // "handle_xmlDump"

    },
    "hackrfHandler\0signal_antEnable\0\0"
    "signal_ampEnable\0signal_vgaValue\0"
    "signal_lnaValue\0handle_LNAGain\0"
    "handle_VGAGain\0handle_biasT\0handle_Ampli\0"
    "handle_ppmCorrection\0handle_xmlDump"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_hackrfHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       3,    1,   67,    2, 0x06 /* Public */,
       4,    1,   70,    2, 0x06 /* Public */,
       5,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   76,    2, 0x08 /* Private */,
       7,    1,   79,    2, 0x08 /* Private */,
       8,    1,   82,    2, 0x08 /* Private */,
       9,    1,   85,    2, 0x08 /* Private */,
      10,    1,   88,    2, 0x08 /* Private */,
      11,    0,   91,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void hackrfHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<hackrfHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_antEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->signal_ampEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->signal_vgaValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->signal_lnaValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->handle_LNAGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->handle_VGAGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->handle_biasT((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->handle_Ampli((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->handle_ppmCorrection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->handle_xmlDump(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (hackrfHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&hackrfHandler::signal_antEnable)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (hackrfHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&hackrfHandler::signal_ampEnable)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (hackrfHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&hackrfHandler::signal_vgaValue)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (hackrfHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&hackrfHandler::signal_lnaValue)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject hackrfHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_hackrfHandler.data,
    qt_meta_data_hackrfHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *hackrfHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *hackrfHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_hackrfHandler.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_hackrfWidget"))
        return static_cast< Ui_hackrfWidget*>(this);
    return QObject::qt_metacast(_clname);
}

int hackrfHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void hackrfHandler::signal_antEnable(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void hackrfHandler::signal_ampEnable(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void hackrfHandler::signal_vgaValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void hackrfHandler::signal_lnaValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
