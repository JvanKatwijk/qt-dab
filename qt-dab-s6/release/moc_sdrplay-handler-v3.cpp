/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler-v3.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler-v3.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_sdrplayHandler_v3_t {
    QByteArrayData data[25];
    char stringdata0[396];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sdrplayHandler_v3_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sdrplayHandler_v3_t qt_meta_stringdata_sdrplayHandler_v3 = {
    {
QT_MOC_LITERAL(0, 0, 17), // "sdrplayHandler_v3"
QT_MOC_LITERAL(1, 18, 13), // "new_GRdBValue"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "new_lnaValue"
QT_MOC_LITERAL(4, 46, 14), // "new_agcSetting"
QT_MOC_LITERAL(5, 61, 20), // "set_lnabounds_signal"
QT_MOC_LITERAL(6, 82, 21), // "set_deviceName_signal"
QT_MOC_LITERAL(7, 104, 17), // "set_serial_signal"
QT_MOC_LITERAL(8, 122, 21), // "set_apiVersion_signal"
QT_MOC_LITERAL(9, 144, 24), // "set_antennaSelect_signal"
QT_MOC_LITERAL(10, 169, 19), // "set_ifgainReduction"
QT_MOC_LITERAL(11, 189, 20), // "set_lnagainReduction"
QT_MOC_LITERAL(12, 210, 14), // "set_agcControl"
QT_MOC_LITERAL(13, 225, 14), // "set_ppmControl"
QT_MOC_LITERAL(14, 240, 17), // "set_selectAntenna"
QT_MOC_LITERAL(15, 258, 9), // "set_biasT"
QT_MOC_LITERAL(16, 268, 13), // "set_lnabounds"
QT_MOC_LITERAL(17, 282, 10), // "set_nrBits"
QT_MOC_LITERAL(18, 293, 14), // "set_deviceName"
QT_MOC_LITERAL(19, 308, 10), // "set_serial"
QT_MOC_LITERAL(20, 319, 14), // "set_apiVersion"
QT_MOC_LITERAL(21, 334, 17), // "set_antennaSelect"
QT_MOC_LITERAL(22, 352, 18), // "show_tunerSelector"
QT_MOC_LITERAL(23, 371, 11), // "set_xmlDump"
QT_MOC_LITERAL(24, 383, 12) // "show_lnaGain"

    },
    "sdrplayHandler_v3\0new_GRdBValue\0\0"
    "new_lnaValue\0new_agcSetting\0"
    "set_lnabounds_signal\0set_deviceName_signal\0"
    "set_serial_signal\0set_apiVersion_signal\0"
    "set_antennaSelect_signal\0set_ifgainReduction\0"
    "set_lnagainReduction\0set_agcControl\0"
    "set_ppmControl\0set_selectAntenna\0"
    "set_biasT\0set_lnabounds\0set_nrBits\0"
    "set_deviceName\0set_serial\0set_apiVersion\0"
    "set_antennaSelect\0show_tunerSelector\0"
    "set_xmlDump\0show_lnaGain"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sdrplayHandler_v3[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  129,    2, 0x06 /* Public */,
       3,    1,  132,    2, 0x06 /* Public */,
       4,    1,  135,    2, 0x06 /* Public */,
       5,    2,  138,    2, 0x06 /* Public */,
       6,    1,  143,    2, 0x06 /* Public */,
       7,    1,  146,    2, 0x06 /* Public */,
       8,    1,  149,    2, 0x06 /* Public */,
       9,    1,  152,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  155,    2, 0x08 /* Private */,
      11,    1,  158,    2, 0x08 /* Private */,
      12,    1,  161,    2, 0x08 /* Private */,
      13,    1,  164,    2, 0x08 /* Private */,
      14,    1,  167,    2, 0x08 /* Private */,
      15,    1,  170,    2, 0x08 /* Private */,
      16,    2,  173,    2, 0x0a /* Public */,
      17,    1,  178,    2, 0x0a /* Public */,
      18,    1,  181,    2, 0x0a /* Public */,
      19,    1,  184,    2, 0x0a /* Public */,
      20,    1,  187,    2, 0x0a /* Public */,
      21,    1,  190,    2, 0x0a /* Public */,
      22,    1,  193,    2, 0x0a /* Public */,
      23,    0,  196,    2, 0x0a /* Public */,
      24,    1,  197,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void sdrplayHandler_v3::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<sdrplayHandler_v3 *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->new_GRdBValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->new_lnaValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->new_agcSetting((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->set_lnabounds_signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->set_deviceName_signal((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->set_serial_signal((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->set_apiVersion_signal((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 7: _t->set_antennaSelect_signal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->set_ifgainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->set_lnagainReduction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->set_agcControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->set_ppmControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->set_selectAntenna((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->set_biasT((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->set_lnabounds((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->set_nrBits((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->set_deviceName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->set_serial((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->set_apiVersion((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 19: _t->set_antennaSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->show_tunerSelector((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->set_xmlDump(); break;
        case 22: _t->show_lnaGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (sdrplayHandler_v3::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::new_GRdBValue)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::new_lnaValue)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::new_agcSetting)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::set_lnabounds_signal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::set_deviceName_signal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::set_serial_signal)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::set_apiVersion_signal)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (sdrplayHandler_v3::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&sdrplayHandler_v3::set_antennaSelect_signal)) {
                *result = 7;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject sdrplayHandler_v3::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_sdrplayHandler_v3.data,
    qt_meta_data_sdrplayHandler_v3,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *sdrplayHandler_v3::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplayHandler_v3::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sdrplayHandler_v3.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "Ui_sdrplayWidget_v3"))
        return static_cast< Ui_sdrplayWidget_v3*>(this);
    return QThread::qt_metacast(_clname);
}

int sdrplayHandler_v3::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void sdrplayHandler_v3::new_GRdBValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void sdrplayHandler_v3::new_lnaValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void sdrplayHandler_v3::new_agcSetting(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void sdrplayHandler_v3::set_lnabounds_signal(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void sdrplayHandler_v3::set_deviceName_signal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void sdrplayHandler_v3::set_serial_signal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void sdrplayHandler_v3::set_apiVersion_signal(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void sdrplayHandler_v3::set_antennaSelect_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
