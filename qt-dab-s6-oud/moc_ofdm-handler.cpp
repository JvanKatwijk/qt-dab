/****************************************************************************
** Meta object code from reading C++ file 'ofdm-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../ofdm-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ofdm-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ofdmHandler_t {
    QByteArrayData data[13];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ofdmHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ofdmHandler_t qt_meta_stringdata_ofdmHandler = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ofdmHandler"
QT_MOC_LITERAL(1, 12, 10), // "set_synced"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 15), // "no_signal_found"
QT_MOC_LITERAL(4, 40, 13), // "set_sync_lost"
QT_MOC_LITERAL(5, 54, 8), // "show_tii"
QT_MOC_LITERAL(6, 63, 17), // "show_tii_spectrum"
QT_MOC_LITERAL(7, 81, 13), // "show_spectrum"
QT_MOC_LITERAL(8, 95, 8), // "show_snr"
QT_MOC_LITERAL(9, 104, 16), // "show_clock_error"
QT_MOC_LITERAL(10, 121, 9), // "show_null"
QT_MOC_LITERAL(11, 131, 12), // "show_channel"
QT_MOC_LITERAL(12, 144, 14) // "show_Corrector"

    },
    "ofdmHandler\0set_synced\0\0no_signal_found\0"
    "set_sync_lost\0show_tii\0show_tii_spectrum\0"
    "show_spectrum\0show_snr\0show_clock_error\0"
    "show_null\0show_channel\0show_Corrector"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ofdmHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       3,    0,   77,    2, 0x06 /* Public */,
       4,    0,   78,    2, 0x06 /* Public */,
       5,    2,   79,    2, 0x06 /* Public */,
       6,    0,   84,    2, 0x06 /* Public */,
       7,    1,   85,    2, 0x06 /* Public */,
       8,    1,   88,    2, 0x06 /* Public */,
       8,    5,   91,    2, 0x06 /* Public */,
       9,    1,  102,    2, 0x06 /* Public */,
      10,    1,  105,    2, 0x06 /* Public */,
      11,    1,  108,    2, 0x06 /* Public */,
      12,    2,  111,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    2,    2,

       0        // eod
};

void ofdmHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ofdmHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_synced((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->no_signal_found(); break;
        case 2: _t->set_sync_lost(); break;
        case 3: _t->show_tii((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->show_tii_spectrum(); break;
        case 5: _t->show_spectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->show_snr((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 7: _t->show_snr((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
        case 8: _t->show_clock_error((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->show_null((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->show_channel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->show_Corrector((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ofdmHandler::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::set_synced)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::no_signal_found)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::set_sync_lost)) {
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_spectrum)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (ofdmHandler::*)(float );
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_clock_error)) {
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
        {
            using _t = void (ofdmHandler::*)(int , float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ofdmHandler::show_Corrector)) {
                *result = 11;
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
void ofdmHandler::set_synced(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ofdmHandler::no_signal_found()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ofdmHandler::set_sync_lost()
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
void ofdmHandler::show_spectrum(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ofdmHandler::show_snr(float _t1)
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
void ofdmHandler::show_clock_error(int _t1)
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

// SIGNAL 11
void ofdmHandler::show_Corrector(int _t1, float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
