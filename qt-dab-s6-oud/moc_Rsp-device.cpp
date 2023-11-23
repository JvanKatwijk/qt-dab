/****************************************************************************
** Meta object code from reading C++ file 'Rsp-device.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../qt-devices/sdrplay-handler-v3/Rsp-device.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Rsp-device.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Rsp_device_t {
    QByteArrayData data[7];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Rsp_device_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Rsp_device_t qt_meta_stringdata_Rsp_device = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Rsp_device"
QT_MOC_LITERAL(1, 11, 20), // "set_lnabounds_signal"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 21), // "set_deviceName_signal"
QT_MOC_LITERAL(4, 55, 24), // "set_antennaSelect_signal"
QT_MOC_LITERAL(5, 80, 17), // "set_nrBits_signal"
QT_MOC_LITERAL(6, 98, 12) // "show_lnaGain"

    },
    "Rsp_device\0set_lnabounds_signal\0\0"
    "set_deviceName_signal\0set_antennaSelect_signal\0"
    "set_nrBits_signal\0show_lnaGain"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Rsp_device[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       3,    1,   44,    2, 0x06 /* Public */,
       4,    1,   47,    2, 0x06 /* Public */,
       5,    1,   50,    2, 0x06 /* Public */,
       6,    1,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void Rsp_device::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Rsp_device *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_lnabounds_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->set_deviceName_signal((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->set_antennaSelect_signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_nrBits_signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->show_lnaGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Rsp_device::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Rsp_device::set_lnabounds_signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Rsp_device::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Rsp_device::set_deviceName_signal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Rsp_device::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Rsp_device::set_antennaSelect_signal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Rsp_device::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Rsp_device::set_nrBits_signal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Rsp_device::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Rsp_device::show_lnaGain)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Rsp_device::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Rsp_device.data,
    qt_meta_data_Rsp_device,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Rsp_device::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Rsp_device::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Rsp_device.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Rsp_device::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Rsp_device::set_lnabounds_signal(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Rsp_device::set_deviceName_signal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Rsp_device::set_antennaSelect_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Rsp_device::set_nrBits_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Rsp_device::show_lnaGain(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
