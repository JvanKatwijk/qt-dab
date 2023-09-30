/****************************************************************************
** Meta object code from reading C++ file 'snr-viewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../viewers/snr-viewer/snr-viewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'snr-viewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_snrViewer_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_snrViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_snrViewer_t qt_meta_stringdata_snrViewer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "snrViewer"
QT_MOC_LITERAL(1, 10, 15), // "rightMouseClick"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 20), // "handle_snrDumpButton"
QT_MOC_LITERAL(4, 48, 13), // "set_snrHeight"
QT_MOC_LITERAL(5, 62, 13), // "set_snrLength"
QT_MOC_LITERAL(6, 76, 12) // "set_snrDelay"

    },
    "snrViewer\0rightMouseClick\0\0"
    "handle_snrDumpButton\0set_snrHeight\0"
    "set_snrLength\0set_snrDelay"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_snrViewer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x08 /* Private */,
       3,    0,   42,    2, 0x08 /* Private */,
       4,    1,   43,    2, 0x08 /* Private */,
       5,    1,   46,    2, 0x08 /* Private */,
       6,    1,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QPointF,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void snrViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<snrViewer *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->rightMouseClick((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 1: _t->handle_snrDumpButton(); break;
        case 2: _t->set_snrHeight((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_snrLength((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->set_snrDelay((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject snrViewer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_snrViewer.data,
    qt_meta_data_snrViewer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *snrViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *snrViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_snrViewer.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_snrWidget"))
        return static_cast< Ui_snrWidget*>(this);
    return QObject::qt_metacast(_clname);
}

int snrViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
