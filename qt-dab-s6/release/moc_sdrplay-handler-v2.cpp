/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler-v2.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler-v2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_sdrplayHandler_t {
    QByteArrayData data[15];
    char stringdata0[225];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sdrplayHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sdrplayHandler_t qt_meta_stringdata_sdrplayHandler = {
    {
QT_MOC_LITERAL(0, 0, 14), // "sdrplayHandler"
QT_MOC_LITERAL(1, 15, 13), // "new_GRdBValue"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 12), // "new_lnaValue"
QT_MOC_LITERAL(4, 43, 14), // "new_agcSetting"
QT_MOC_LITERAL(5, 58, 19), // "set_ifgainReduction"
QT_MOC_LITERAL(6, 78, 20), // "set_lnagainReduction"
QT_MOC_LITERAL(7, 99, 14), // "set_agcControl"
QT_MOC_LITERAL(8, 114, 16), // "set_debugControl"
QT_MOC_LITERAL(9, 131, 14), // "set_ppmControl"
QT_MOC_LITERAL(10, 146, 17), // "set_antennaSelect"
QT_MOC_LITERAL(11, 164, 15), // "set_tunerSelect"
QT_MOC_LITERAL(12, 180, 11), // "set_xmlDump"
QT_MOC_LITERAL(13, 192, 10), // "voidSignal"
QT_MOC_LITERAL(14, 203, 21) // "biasT_selectorHandler"

    },
    "sdrplayHandler\0new_GRdBValue\0\0"
    "new_lnaValue\0new_agcSetting\0"
    "set_ifgainReduction\0set_lnagainReduction\0"
    "set_agcControl\0set_debugControl\0"
    "set_ppmControl\0set_antennaSelect\0"
    "set_tunerSelect\0set_xmlDump\0voidSignal\0"
    "biasT_selectorHandler"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sdrplayHandler[] = {

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

void sdrplayHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<sdrplayHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->new_GRdBValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->new_lnaValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->new_agcSetting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->set_ifgainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->set_lnagainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->set_agcControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->set_debugControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->set_ppmControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->set_antennaSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->set_tunerSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->set_xmlDump(); break;
        case 11: _t->voidSignal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->biasT_selectorHandler((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (sdrplayHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler::new_GRdBValue)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler::new_lnaValue)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler::new_agcSetting)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject sdrplayHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_sdrplayHandler.data,
    qt_meta_data_sdrplayHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *sdrplayHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplayHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sdrplayHandler.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_sdrplayWidget"))
        return static_cast< Ui_sdrplayWidget*>(this);
    return QObject::qt_metacast(_clname);
}

int sdrplayHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void sdrplayHandler::new_GRdBValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sdrplayHandler::new_lnaValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void sdrplayHandler::new_agcSetting(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
