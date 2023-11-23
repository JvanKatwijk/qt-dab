/****************************************************************************
** Meta object code from reading C++ file 'lime-handler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../qt-devices/lime-handler/lime-handler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lime-handler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_limeHandler_t {
    QByteArrayData data[8];
    char stringdata0[80];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_limeHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_limeHandler_t qt_meta_stringdata_limeHandler = {
    {
QT_MOC_LITERAL(0, 0, 11), // "limeHandler"
QT_MOC_LITERAL(1, 12, 13), // "new_gainValue"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "setGain"
QT_MOC_LITERAL(4, 35, 10), // "setAntenna"
QT_MOC_LITERAL(5, 46, 11), // "set_xmlDump"
QT_MOC_LITERAL(6, 58, 10), // "set_filter"
QT_MOC_LITERAL(7, 69, 10) // "showErrors"

    },
    "limeHandler\0new_gainValue\0\0setGain\0"
    "setAntenna\0set_xmlDump\0set_filter\0"
    "showErrors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_limeHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   47,    2, 0x08 /* Private */,
       4,    1,   50,    2, 0x08 /* Private */,
       5,    0,   53,    2, 0x08 /* Private */,
       6,    1,   54,    2, 0x08 /* Private */,
       7,    2,   57,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

       0        // eod
};

void limeHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<limeHandler *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->new_gainValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setAntenna((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_xmlDump(); break;
        case 4: _t->set_filter((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->showErrors((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (limeHandler::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&limeHandler::new_gainValue)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject limeHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_limeHandler.data,
    qt_meta_data_limeHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *limeHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *limeHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_limeHandler.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "deviceHandler"))
        return static_cast< deviceHandler*>(this);
    if (!strcmp(_clname, "limeWidget"))
        return static_cast< limeWidget*>(this);
    return QThread::qt_metacast(_clname);
}

int limeHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void limeHandler::new_gainValue(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
