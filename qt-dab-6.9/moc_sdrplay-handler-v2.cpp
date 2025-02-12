/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler-v2.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler-v2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN17sdrplayHandler_v2E_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN17sdrplayHandler_v2E = QtMocHelpers::stringData(
    "sdrplayHandler_v2",
    "signal_GRdBValue",
    "",
    "signal_lnaValue",
    "signal_agcSetting",
    "handle_ifgainReduction",
    "handle_lnagainReduction",
    "handle_agcControl",
    "handle_debugControl",
    "handle_ppmControl",
    "handle_antennaSelect",
    "handle_tunerSelect",
    "handle_xmlDump",
    "handle_voidSignal",
    "handle_biasT_selector"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN17sdrplayHandler_v2E[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   92,    2, 0x06,    1 /* Public */,
       3,    1,   95,    2, 0x06,    3 /* Public */,
       4,    1,   98,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,  101,    2, 0x08,    7 /* Private */,
       6,    1,  104,    2, 0x08,    9 /* Private */,
       7,    1,  107,    2, 0x08,   11 /* Private */,
       8,    1,  110,    2, 0x08,   13 /* Private */,
       9,    1,  113,    2, 0x08,   15 /* Private */,
      10,    1,  116,    2, 0x08,   17 /* Private */,
      11,    1,  119,    2, 0x08,   19 /* Private */,
      12,    0,  122,    2, 0x08,   21 /* Private */,
      13,    1,  123,    2, 0x08,   22 /* Private */,
      14,    1,  126,    2, 0x08,   24 /* Private */,

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

Q_CONSTINIT const QMetaObject sdrplayHandler_v2::staticMetaObject = { {
    QMetaObject::SuperData::link<deviceHandler::staticMetaObject>(),
    qt_meta_stringdata_ZN17sdrplayHandler_v2E.offsetsAndSizes,
    qt_meta_data_ZN17sdrplayHandler_v2E,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN17sdrplayHandler_v2E_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<sdrplayHandler_v2, std::true_type>,
        // method 'signal_GRdBValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'signal_lnaValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'signal_agcSetting'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'handle_ifgainReduction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_lnagainReduction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_agcControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_debugControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_ppmControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_antennaSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_tunerSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_xmlDump'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_voidSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_biasT_selector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void sdrplayHandler_v2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<sdrplayHandler_v2 *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->signal_GRdBValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->signal_lnaValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->signal_agcSetting((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->handle_ifgainReduction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->handle_lnagainReduction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->handle_agcControl((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->handle_debugControl((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->handle_ppmControl((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->handle_antennaSelect((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->handle_tunerSelect((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->handle_xmlDump(); break;
        case 11: _t->handle_voidSignal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->handle_biasT_selector((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (sdrplayHandler_v2::*)(int );
            if (_q_method_type _q_method = &sdrplayHandler_v2::signal_GRdBValue; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v2::*)(int );
            if (_q_method_type _q_method = &sdrplayHandler_v2::signal_lnaValue; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v2::*)(bool );
            if (_q_method_type _q_method = &sdrplayHandler_v2::signal_agcSetting; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *sdrplayHandler_v2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplayHandler_v2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN17sdrplayHandler_v2E.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_sdrplayWidget"))
        return static_cast< Ui_sdrplayWidget*>(this);
    return deviceHandler::qt_metacast(_clname);
}

int sdrplayHandler_v2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = deviceHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
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
