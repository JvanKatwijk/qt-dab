/****************************************************************************
** Meta object code from reading C++ file 'airspy-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../qt-devices/airspy-handler/airspy-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'airspy-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_airspyHandler_t {
    QByteArrayData data[15];
    char stringdata0[179];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_airspyHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_airspyHandler_t qt_meta_stringdata_airspyHandler = {
    {
QT_MOC_LITERAL(0, 0, 13), // "airspyHandler"
QT_MOC_LITERAL(1, 14, 14), // "new_tabSetting"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 13), // "set_linearity"
QT_MOC_LITERAL(4, 44, 5), // "value"
QT_MOC_LITERAL(5, 50, 15), // "set_sensitivity"
QT_MOC_LITERAL(6, 66, 12), // "set_lna_gain"
QT_MOC_LITERAL(7, 79, 14), // "set_mixer_gain"
QT_MOC_LITERAL(8, 94, 12), // "set_vga_gain"
QT_MOC_LITERAL(9, 107, 11), // "set_lna_agc"
QT_MOC_LITERAL(10, 119, 13), // "set_mixer_agc"
QT_MOC_LITERAL(11, 133, 11), // "set_rf_bias"
QT_MOC_LITERAL(12, 145, 10), // "switch_tab"
QT_MOC_LITERAL(13, 156, 11), // "set_xmlDump"
QT_MOC_LITERAL(14, 168, 10) // "set_filter"

    },
    "airspyHandler\0new_tabSetting\0\0"
    "set_linearity\0value\0set_sensitivity\0"
    "set_lna_gain\0set_mixer_gain\0set_vga_gain\0"
    "set_lna_agc\0set_mixer_agc\0set_rf_bias\0"
    "switch_tab\0set_xmlDump\0set_filter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_airspyHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   77,    2, 0x08 /* Private */,
       5,    1,   80,    2, 0x08 /* Private */,
       6,    1,   83,    2, 0x08 /* Private */,
       7,    1,   86,    2, 0x08 /* Private */,
       8,    1,   89,    2, 0x08 /* Private */,
       9,    1,   92,    2, 0x08 /* Private */,
      10,    1,   95,    2, 0x08 /* Private */,
      11,    1,   98,    2, 0x08 /* Private */,
      12,    1,  101,    2, 0x08 /* Private */,
      13,    0,  104,    2, 0x08 /* Private */,
      14,    1,  105,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void airspyHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<airspyHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->new_tabSetting((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_linearity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_sensitivity((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_lna_gain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->set_mixer_gain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->set_vga_gain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->set_lna_agc((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->set_mixer_agc((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->set_rf_bias((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->switch_tab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->set_xmlDump(); break;
        case 11: _t->set_filter((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (airspyHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&airspyHandler::new_tabSetting)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject airspyHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_airspyHandler.data,
    qt_meta_data_airspyHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *airspyHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *airspyHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_airspyHandler.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_airspyWidget"))
        return static_cast< Ui_airspyWidget*>(this);
    return QObject::qt_metacast(_clname);
}

int airspyHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void airspyHandler::new_tabSetting(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
