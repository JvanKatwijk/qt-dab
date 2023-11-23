/****************************************************************************
** Meta object code from reading C++ file 'mp2processor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../includes/backend/audio/mp2processor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mp2processor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mp2Processor_t {
    QByteArrayData data[5];
    char stringdata0[49];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mp2Processor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mp2Processor_t qt_meta_stringdata_mp2Processor = {
    {
QT_MOC_LITERAL(0, 0, 12), // "mp2Processor"
QT_MOC_LITERAL(1, 13, 16), // "show_frameErrors"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 8), // "newAudio"
QT_MOC_LITERAL(4, 40, 8) // "isStereo"

    },
    "mp2Processor\0show_frameErrors\0\0newAudio\0"
    "isStereo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mp2Processor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    4,   32,    2, 0x06 /* Public */,
       4,    1,   41,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void mp2Processor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<mp2Processor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->show_frameErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->newAudio((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 2: _t->isStereo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (mp2Processor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp2Processor::show_frameErrors)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (mp2Processor::*)(int , int , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp2Processor::newAudio)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (mp2Processor::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&mp2Processor::isStereo)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject mp2Processor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_mp2Processor.data,
    qt_meta_data_mp2Processor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *mp2Processor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mp2Processor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mp2Processor.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "frameProcessor"))
        return static_cast< frameProcessor*>(this);
    return QObject::qt_metacast(_clname);
}

int mp2Processor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void mp2Processor::show_frameErrors(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void mp2Processor::newAudio(int _t1, int _t2, bool _t3, bool _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void mp2Processor::isStereo(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
