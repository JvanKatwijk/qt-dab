/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler-v2.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler-v2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_sdrplayHandler_v2_t {
    QByteArrayData data[15];
    char stringdata0[268];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sdrplayHandler_v2_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sdrplayHandler_v2_t qt_meta_stringdata_sdrplayHandler_v2 = {
    {
QT_MOC_LITERAL(0, 0, 17), // "sdrplayHandler_v2"
QT_MOC_LITERAL(1, 18, 16), // "signal_GRdBValue"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 15), // "signal_lnaValue"
QT_MOC_LITERAL(4, 52, 17), // "signal_agcSetting"
QT_MOC_LITERAL(5, 70, 22), // "handle_ifgainReduction"
QT_MOC_LITERAL(6, 93, 23), // "handle_lnagainReduction"
QT_MOC_LITERAL(7, 117, 17), // "handle_agcControl"
QT_MOC_LITERAL(8, 135, 19), // "handle_debugControl"
QT_MOC_LITERAL(9, 155, 17), // "handle_ppmControl"
QT_MOC_LITERAL(10, 173, 20), // "handle_antennaSelect"
QT_MOC_LITERAL(11, 194, 18), // "handle_tunerSelect"
QT_MOC_LITERAL(12, 213, 14), // "handle_xmlDump"
QT_MOC_LITERAL(13, 228, 17), // "handle_voidSignal"
QT_MOC_LITERAL(14, 246, 21) // "handle_biasT_selector"

    },
    "sdrplayHandler_v2\0signal_GRdBValue\0\0"
    "signal_lnaValue\0signal_agcSetting\0"
    "handle_ifgainReduction\0handle_lnagainReduction\0"
    "handle_agcControl\0handle_debugControl\0"
    "handle_ppmControl\0handle_antennaSelect\0"
    "handle_tunerSelect\0handle_xmlDump\0"
    "handle_voidSignal\0handle_biasT_selector"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sdrplayHandler_v2[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       3,    1,   82,    2, 0x06 /* Public */,
       4,    1,   85,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   88,    2, 0x08 /* Private */,
       6,    1,   91,    2, 0x08 /* Private */,
       7,    1,   94,    2, 0x08 /* Private */,
       8,    1,   97,    2, 0x08 /* Private */,
       9,    1,  100,    2, 0x08 /* Private */,
      10,    1,  103,    2, 0x08 /* Private */,
      11,    1,  106,    2, 0x08 /* Private */,
      12,    0,  109,    2, 0x08 /* Private */,
      13,    1,  110,    2, 0x08 /* Private */,
      14,    1,  113,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void sdrplayHandler_v2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<sdrplayHandler_v2 *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->signal_GRdBValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->signal_lnaValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->signal_agcSetting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->handle_ifgainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->handle_lnagainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->handle_agcControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->handle_debugControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->handle_ppmControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->handle_antennaSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->handle_tunerSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->handle_xmlDump(); break;
        case 11: _t->handle_voidSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->handle_biasT_selector((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (sdrplayHandler_v2::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v2::signal_GRdBValue)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v2::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v2::signal_lnaValue)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v2::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v2::signal_agcSetting)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject sdrplayHandler_v2::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_sdrplayHandler_v2.data,
    qt_meta_data_sdrplayHandler_v2,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *sdrplayHandler_v2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplayHandler_v2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sdrplayHandler_v2.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_sdrplayWidget"))
        return static_cast< Ui_sdrplayWidget*>(this);
    return QObject::qt_metacast(_clname);
}

int sdrplayHandler_v2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void sdrplayHandler_v2::signal_GRdBValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sdrplayHandler_v2::signal_lnaValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void sdrplayHandler_v2::signal_agcSetting(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
