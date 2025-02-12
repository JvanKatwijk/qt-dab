/****************************************************************************
** Meta object code from reading C++ file 'airspy-2.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt-devices/airspy-2/airspy-2.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'airspy-2.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8airspy_2E_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN8airspy_2E = QtMocHelpers::stringData(
    "airspy_2",
    "new_tabSetting",
    "",
    "set_linearity",
    "value",
    "set_sensitivity",
    "set_lna_gain",
    "set_mixer_gain",
    "set_vga_gain",
    "set_lna_agc",
    "set_mixer_agc",
    "set_rf_bias",
    "switch_tab",
    "set_xmlDump",
    "handle_convQuality"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN8airspy_2E[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   86,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    1,   89,    2, 0x08,    3 /* Private */,
       5,    1,   92,    2, 0x08,    5 /* Private */,
       6,    1,   95,    2, 0x08,    7 /* Private */,
       7,    1,   98,    2, 0x08,    9 /* Private */,
       8,    1,  101,    2, 0x08,   11 /* Private */,
       9,    1,  104,    2, 0x08,   13 /* Private */,
      10,    1,  107,    2, 0x08,   15 /* Private */,
      11,    1,  110,    2, 0x08,   17 /* Private */,
      12,    1,  113,    2, 0x08,   19 /* Private */,
      13,    0,  116,    2, 0x08,   21 /* Private */,
      14,    1,  117,    2, 0x08,   22 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject airspy_2::staticMetaObject = { {
    QMetaObject::SuperData::link<deviceHandler::staticMetaObject>(),
    qt_meta_stringdata_ZN8airspy_2E.offsetsAndSizes,
    qt_meta_data_ZN8airspy_2E,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN8airspy_2E_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<airspy_2, std::true_type>,
        // method 'new_tabSetting'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_linearity'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_sensitivity'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_lna_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_mixer_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_vga_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_lna_agc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_mixer_agc'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_rf_bias'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'switch_tab'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_xmlDump'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_convQuality'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void airspy_2::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<airspy_2 *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->new_tabSetting((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->set_linearity((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->set_sensitivity((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->set_lna_gain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->set_mixer_gain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->set_vga_gain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->set_lna_agc((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 7: _t->set_mixer_agc((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->set_rf_bias((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->switch_tab((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 10: _t->set_xmlDump(); break;
        case 11: _t->handle_convQuality((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (airspy_2::*)(int );
            if (_q_method_type _q_method = &airspy_2::new_tabSetting; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *airspy_2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *airspy_2::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN8airspy_2E.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_airspyWidget"))
        return static_cast< Ui_airspyWidget*>(this);
    return deviceHandler::qt_metacast(_clname);
}

int airspy_2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = deviceHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void airspy_2::new_tabSetting(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
