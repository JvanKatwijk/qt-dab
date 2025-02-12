/****************************************************************************
** Meta object code from reading C++ file 'sdrplay-handler-v3.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay-handler-v3.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17sdrplayHandler_v3E_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN17sdrplayHandler_v3E = QtMocHelpers::stringData(
    "sdrplayHandler_v3",
    "new_GRdBValue",
    "",
    "new_lnaValue",
    "new_agcSetting",
    "show_tuner_gain",
    "set_lnabounds_signal",
    "set_deviceName_signal",
    "set_serial_signal",
    "set_apiVersion_signal",
    "set_antennaSelect_signal",
    "overload_state_changed",
    "set_ifgainReduction",
    "set_lnagainReduction",
    "set_agcControl",
    "set_ppmControl",
    "set_selectAntenna",
    "set_selectTuner",
    "set_biasT",
    "set_notch",
    "report_overload_state",
    "display_gain",
    "set_lnabounds",
    "set_serial",
    "set_apiVersion",
    "set_xmlDump",
    "show_lnaGain"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN17sdrplayHandler_v3E[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  164,    2, 0x06,    1 /* Public */,
       3,    1,  167,    2, 0x06,    3 /* Public */,
       4,    1,  170,    2, 0x06,    5 /* Public */,
       5,    1,  173,    2, 0x06,    7 /* Public */,
       6,    2,  176,    2, 0x06,    9 /* Public */,
       7,    1,  181,    2, 0x06,   12 /* Public */,
       8,    1,  184,    2, 0x06,   14 /* Public */,
       9,    1,  187,    2, 0x06,   16 /* Public */,
      10,    1,  190,    2, 0x06,   18 /* Public */,
      11,    1,  193,    2, 0x06,   20 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    1,  196,    2, 0x08,   22 /* Private */,
      13,    1,  199,    2, 0x08,   24 /* Private */,
      14,    1,  202,    2, 0x08,   26 /* Private */,
      15,    1,  205,    2, 0x08,   28 /* Private */,
      16,    1,  208,    2, 0x08,   30 /* Private */,
      17,    1,  211,    2, 0x08,   32 /* Private */,
      18,    1,  214,    2, 0x08,   34 /* Private */,
      19,    1,  217,    2, 0x08,   36 /* Private */,
      20,    1,  220,    2, 0x08,   38 /* Private */,
      21,    1,  223,    2, 0x08,   40 /* Private */,
      22,    2,  226,    2, 0x0a,   42 /* Public */,
      23,    1,  231,    2, 0x0a,   45 /* Public */,
      24,    1,  234,    2, 0x0a,   47 /* Public */,
      25,    0,  237,    2, 0x0a,   49 /* Public */,
      26,    1,  238,    2, 0x0a,   50 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

Q_CONSTINIT const QMetaObject sdrplayHandler_v3::staticMetaObject = { {
    QMetaObject::SuperData::link<deviceHandler::staticMetaObject>(),
    qt_meta_stringdata_ZN17sdrplayHandler_v3E.offsetsAndSizes,
    qt_meta_data_ZN17sdrplayHandler_v3E,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN17sdrplayHandler_v3E_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<sdrplayHandler_v3, std::true_type>,
        // method 'new_GRdBValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'new_lnaValue'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'new_agcSetting'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'show_tuner_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'set_lnabounds_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_deviceName_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'set_serial_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'set_apiVersion_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'set_antennaSelect_signal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'overload_state_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_ifgainReduction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_lnagainReduction'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_agcControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_ppmControl'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_selectAntenna'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'set_selectTuner'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'set_biasT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_notch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'report_overload_state'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'display_gain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'set_lnabounds'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_serial'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'set_apiVersion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'set_xmlDump'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_lnaGain'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>
    >,
    nullptr
} };

void sdrplayHandler_v3::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<sdrplayHandler_v3 *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->new_GRdBValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->new_lnaValue((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->new_agcSetting((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->show_tuner_gain((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 4: _t->set_lnabounds_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 5: _t->set_deviceName_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->set_serial_signal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->set_apiVersion_signal((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 8: _t->set_antennaSelect_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 9: _t->overload_state_changed((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->set_ifgainReduction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->set_lnagainReduction((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 12: _t->set_agcControl((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->set_ppmControl((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 14: _t->set_selectAntenna((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->set_selectTuner((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->set_biasT((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->set_notch((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->report_overload_state((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->display_gain((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 20: _t->set_lnabounds((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 21: _t->set_serial((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->set_apiVersion((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 23: _t->set_xmlDump(); break;
        case 24: _t->show_lnaGain((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(int );
            if (_q_method_type _q_method = &sdrplayHandler_v3::new_GRdBValue; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(int );
            if (_q_method_type _q_method = &sdrplayHandler_v3::new_lnaValue; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(bool );
            if (_q_method_type _q_method = &sdrplayHandler_v3::new_agcSetting; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(double );
            if (_q_method_type _q_method = &sdrplayHandler_v3::show_tuner_gain; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(int , int );
            if (_q_method_type _q_method = &sdrplayHandler_v3::set_lnabounds_signal; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(const QString & );
            if (_q_method_type _q_method = &sdrplayHandler_v3::set_deviceName_signal; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(const QString & );
            if (_q_method_type _q_method = &sdrplayHandler_v3::set_serial_signal; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(float );
            if (_q_method_type _q_method = &sdrplayHandler_v3::set_apiVersion_signal; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(bool );
            if (_q_method_type _q_method = &sdrplayHandler_v3::set_antennaSelect_signal; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _q_method_type = void (sdrplayHandler_v3::*)(bool );
            if (_q_method_type _q_method = &sdrplayHandler_v3::overload_state_changed; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *sdrplayHandler_v3::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplayHandler_v3::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN17sdrplayHandler_v3E.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Ui_sdrplayWidget_v3"))
        return static_cast< Ui_sdrplayWidget_v3*>(this);
    return deviceHandler::qt_metacast(_clname);
}

int sdrplayHandler_v3::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = deviceHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 25;
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
void sdrplayHandler_v3::show_tuner_gain(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void sdrplayHandler_v3::set_lnabounds_signal(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void sdrplayHandler_v3::set_deviceName_signal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void sdrplayHandler_v3::set_serial_signal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void sdrplayHandler_v3::set_apiVersion_signal(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void sdrplayHandler_v3::set_antennaSelect_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void sdrplayHandler_v3::overload_state_changed(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
