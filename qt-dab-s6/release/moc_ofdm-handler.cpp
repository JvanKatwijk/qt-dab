/****************************************************************************
** Meta object code from reading C++ file 'ofdm-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ofdm-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ofdm-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ofdmHandler_t {
    QByteArrayData data[12];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ofdmHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ofdmHandler_t qt_meta_stringdata_ofdmHandler = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ofdmHandler"
QT_MOC_LITERAL(1, 12, 9), // "setSynced"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 15), // "No_Signal_Found"
QT_MOC_LITERAL(4, 39, 11), // "setSyncLost"
QT_MOC_LITERAL(5, 51, 8), // "show_tii"
QT_MOC_LITERAL(6, 60, 17), // "show_tii_spectrum"
QT_MOC_LITERAL(7, 78, 13), // "show_Spectrum"
QT_MOC_LITERAL(8, 92, 8), // "show_snr"
QT_MOC_LITERAL(9, 101, 13), // "show_clockErr"
QT_MOC_LITERAL(10, 115, 9), // "show_null"
QT_MOC_LITERAL(11, 125, 12) // "show_channel"

    },
    "ofdmHandler\0setSynced\0\0No_Signal_Found\0"
    "setSyncLost\0show_tii\0show_tii_spectrum\0"
    "show_Spectrum\0show_snr\0show_clockErr\0"
    "show_null\0show_channel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ofdmHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       3,    0,   72,    2, 0x06 /* Public */,
       4,    0,   73,    2, 0x06 /* Public */,
       5,    2,   74,    2, 0x06 /* Public */,
       6,    0,   79,    2, 0x06 /* Public */,
       7,    1,   80,    2, 0x06 /* Public */,
       8,    1,   83,    2, 0x06 /* Public */,
       8,    5,   86,    2, 0x06 /* Public */,
       9,    1,   97,    2, 0x06 /* Public */,
      10,    1,  100,    2, 0x06 /* Public */,
      11,    1,  103,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void ofdmHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ofdmHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setSynced((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->No_Signal_Found(); break;
        case 2: _t->setSyncLost(); break;
        case 3: _t->show_tii((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->show_tii_spectrum(); break;
        case 5: _t->show_Spectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->show_snr((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->show_snr((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
        case 8: _t->show_clockErr((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->show_null((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->show_channel((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ofdmHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::setSynced)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::No_Signal_Found)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::setSyncLost)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_tii)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_tii_spectrum)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_Spectrum)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_snr)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(float , float , float , float , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_snr)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_clockErr)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_null)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_channel)) {
                *result = 10;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ofdmHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_ofdmHandler.data,
    qt_meta_data_ofdmHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ofdmHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ofdmHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ofdmHandler.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int ofdmHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void ofdmHandler::setSynced(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ofdmHandler::No_Signal_Found()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ofdmHandler::setSyncLost()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ofdmHandler::show_tii(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ofdmHandler::show_tii_spectrum()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void ofdmHandler::show_Spectrum(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ofdmHandler::show_snr(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ofdmHandler::show_snr(float _t1, float _t2, float _t3, float _t4, float _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ofdmHandler::show_clockErr(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void ofdmHandler::show_null(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void ofdmHandler::show_channel(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
