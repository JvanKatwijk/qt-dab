/****************************************************************************
** Meta object code from reading C++ file 'radio.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "radio.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radio.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14RadioInterfaceE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN14RadioInterfaceE = QtMocHelpers::stringData(
    "RadioInterface",
    "select_ensemble_font",
    "",
    "select_ensemble_fontSize",
    "select_ensemble_fontColor",
    "channelSignal",
    "handle_correlationSelector",
    "show_dcOffset",
    "startScanning",
    "stopScanning",
    "show_quality",
    "show_rsCorrections",
    "show_clock_error",
    "show_Corrector",
    "add_to_ensemble",
    "name_of_ensemble",
    "nrServices",
    "show_frameErrors",
    "show_rsErrors",
    "show_aacErrors",
    "show_ficQuality",
    "show_ficBER",
    "set_synced",
    "show_label",
    "handle_motObject",
    "sendDatagram",
    "handle_tdcdata",
    "changeinConfiguration",
    "newAudio",
    "localSelect_SS",
    "setStereo",
    "set_streamSelector",
    "no_signal_found",
    "show_mothandling",
    "set_sync_lost",
    "closeEvent",
    "QCloseEvent*",
    "event",
    "clockTime",
    "start_announcement",
    "stop_announcement",
    "newFrame",
    "set_epgData",
    "epgTimer_timeOut",
    "handle_presetSelect",
    "handle_contentSelector",
    "http_terminate",
    "show_channel",
    "handle_iqSelector",
    "show_spectrum",
    "handle_tiiThreshold",
    "show_tiiData",
    "QList<tiiData>",
    "show_tii_spectrum",
    "show_snr",
    "show_null",
    "showIQ",
    "show_correlation",
    "QList<int>",
    "show_stdDev",
    "showPeakLevel",
    "handle_techFrame_closed",
    "handle_configFrame_closed",
    "handle_deviceFrame_closed",
    "handle_newDisplayFrame_closed",
    "doStart",
    "newDevice",
    "handle_scheduleButton",
    "handle_devicewidgetButton",
    "handle_resetButton",
    "handle_dlTextButton",
    "handle_snrButton",
    "handle_sourcedumpButton",
    "scheduler_timeOut",
    "show_changeLabel",
    "notInOld",
    "notInNew",
    "show_pauzeSlide",
    "handle_timeTable",
    "handle_contentButton",
    "handle_detailButton",
    "handle_scanButton",
    "handle_etiHandler",
    "handle_spectrumButton",
    "handle_scanListButton",
    "handle_presetButton",
    "handle_framedumpButton",
    "handle_audiodumpButton",
    "handle_prevServiceButton",
    "handle_nextServiceButton",
    "handle_channelSelector",
    "handle_nextChannelButton",
    "handle_prevChannelButton",
    "handle_muteButton",
    "handle_folderButton",
    "handle_scanListSelect",
    "TerminateProcess",
    "updateTimeDisplay",
    "channel_timeOut",
    "start_background_task",
    "setPresetService",
    "muteButton_timeOut",
    "handle_configButton",
    "handle_httpButton",
    "setVolume",
    "handle_snrLabel",
    "handle_tiiButton",
    "handle_labelColor",
    "color_scanButton",
    "color_presetButton",
    "color_spectrumButton",
    "color_scanListButton",
    "color_prevServiceButton",
    "color_nextServiceButton",
    "color_configButton",
    "color_httpButton",
    "color_tiiButton",
    "set_transmitters_local",
    "handle_LoggerButton",
    "handle_set_coordinatesButton",
    "handle_eti_activeSelector",
    "handle_loadTable",
    "selectDecoder",
    "handle_aboutLabel"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN14RadioInterfaceE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
     116,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  710,    2, 0x06,    1 /* Public */,
       3,    0,  711,    2, 0x06,    2 /* Public */,
       4,    0,  712,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    1,  713,    2, 0x0a,    4 /* Public */,
       6,    1,  716,    2, 0x0a,    6 /* Public */,
       7,    1,  719,    2, 0x0a,    8 /* Public */,
       8,    0,  722,    2, 0x0a,   10 /* Public */,
       9,    0,  723,    2, 0x0a,   11 /* Public */,
      10,    3,  724,    2, 0x0a,   12 /* Public */,
      11,    2,  731,    2, 0x0a,   16 /* Public */,
      12,    1,  736,    2, 0x0a,   19 /* Public */,
      13,    2,  739,    2, 0x0a,   21 /* Public */,
      14,    3,  744,    2, 0x0a,   24 /* Public */,
      15,    2,  751,    2, 0x0a,   28 /* Public */,
      16,    1,  756,    2, 0x0a,   31 /* Public */,
      17,    1,  759,    2, 0x0a,   33 /* Public */,
      18,    1,  762,    2, 0x0a,   35 /* Public */,
      19,    1,  765,    2, 0x0a,   37 /* Public */,
      20,    2,  768,    2, 0x0a,   39 /* Public */,
      21,    1,  773,    2, 0x0a,   42 /* Public */,
      22,    1,  776,    2, 0x0a,   44 /* Public */,
      23,    1,  779,    2, 0x0a,   46 /* Public */,
      24,    5,  782,    2, 0x0a,   48 /* Public */,
      25,    1,  793,    2, 0x0a,   54 /* Public */,
      26,    2,  796,    2, 0x0a,   56 /* Public */,
      27,    2,  801,    2, 0x0a,   59 /* Public */,
      28,    4,  806,    2, 0x0a,   62 /* Public */,
      29,    2,  815,    2, 0x0a,   67 /* Public */,
      30,    1,  820,    2, 0x0a,   70 /* Public */,
      31,    1,  823,    2, 0x0a,   72 /* Public */,
      32,    0,  826,    2, 0x0a,   74 /* Public */,
      33,    1,  827,    2, 0x0a,   75 /* Public */,
      34,    0,  830,    2, 0x0a,   77 /* Public */,
      35,    1,  831,    2, 0x0a,   78 /* Public */,
      38,    9,  834,    2, 0x0a,   80 /* Public */,
      39,    3,  853,    2, 0x0a,   90 /* Public */,
      40,    2,  860,    2, 0x0a,   94 /* Public */,
      41,    1,  865,    2, 0x0a,   97 /* Public */,
      42,    4,  868,    2, 0x0a,   99 /* Public */,
      43,    0,  877,    2, 0x0a,  104 /* Public */,
      44,    2,  878,    2, 0x0a,  105 /* Public */,
      45,    1,  883,    2, 0x0a,  108 /* Public */,
      46,    0,  886,    2, 0x0a,  110 /* Public */,
      47,    1,  887,    2, 0x0a,  111 /* Public */,
      48,    0,  890,    2, 0x0a,  113 /* Public */,
      49,    1,  891,    2, 0x0a,  114 /* Public */,
      50,    1,  894,    2, 0x0a,  116 /* Public */,
      51,    2,  897,    2, 0x0a,  118 /* Public */,
      53,    0,  902,    2, 0x0a,  121 /* Public */,
      54,    1,  903,    2, 0x0a,  122 /* Public */,
      55,    2,  906,    2, 0x0a,  124 /* Public */,
      56,    1,  911,    2, 0x0a,  127 /* Public */,
      57,    3,  914,    2, 0x0a,  129 /* Public */,
      59,    1,  921,    2, 0x0a,  133 /* Public */,
      60,    2,  924,    2, 0x0a,  135 /* Public */,
      61,    0,  929,    2, 0x0a,  138 /* Public */,
      62,    0,  930,    2, 0x0a,  139 /* Public */,
      63,    0,  931,    2, 0x0a,  140 /* Public */,
      64,    0,  932,    2, 0x0a,  141 /* Public */,
      65,    1,  933,    2, 0x0a,  142 /* Public */,
      66,    1,  936,    2, 0x0a,  144 /* Public */,
      67,    0,  939,    2, 0x0a,  146 /* Public */,
      68,    0,  940,    2, 0x0a,  147 /* Public */,
      69,    0,  941,    2, 0x0a,  148 /* Public */,
      70,    0,  942,    2, 0x0a,  149 /* Public */,
      71,    0,  943,    2, 0x0a,  150 /* Public */,
      72,    0,  944,    2, 0x0a,  151 /* Public */,
      73,    1,  945,    2, 0x0a,  152 /* Public */,
      74,    2,  948,    2, 0x0a,  154 /* Public */,
      77,    0,  953,    2, 0x08,  157 /* Private */,
      78,    0,  954,    2, 0x08,  158 /* Private */,
      79,    0,  955,    2, 0x08,  159 /* Private */,
      80,    0,  956,    2, 0x08,  160 /* Private */,
      81,    0,  957,    2, 0x08,  161 /* Private */,
      82,    0,  958,    2, 0x08,  162 /* Private */,
      83,    0,  959,    2, 0x08,  163 /* Private */,
      84,    0,  960,    2, 0x08,  164 /* Private */,
      85,    0,  961,    2, 0x08,  165 /* Private */,
      86,    0,  962,    2, 0x08,  166 /* Private */,
      87,    0,  963,    2, 0x08,  167 /* Private */,
      88,    0,  964,    2, 0x08,  168 /* Private */,
      89,    0,  965,    2, 0x08,  169 /* Private */,
      90,    1,  966,    2, 0x08,  170 /* Private */,
      91,    0,  969,    2, 0x08,  172 /* Private */,
      92,    0,  970,    2, 0x08,  173 /* Private */,
      93,    0,  971,    2, 0x08,  174 /* Private */,
      94,    0,  972,    2, 0x08,  175 /* Private */,
      95,    1,  973,    2, 0x08,  176 /* Private */,
      96,    0,  976,    2, 0x08,  178 /* Private */,
      97,    0,  977,    2, 0x08,  179 /* Private */,
      98,    0,  978,    2, 0x08,  180 /* Private */,
      99,    1,  979,    2, 0x08,  181 /* Private */,
     100,    0,  982,    2, 0x08,  183 /* Private */,
     101,    0,  983,    2, 0x08,  184 /* Private */,
     102,    0,  984,    2, 0x08,  185 /* Private */,
     103,    0,  985,    2, 0x08,  186 /* Private */,
     104,    1,  986,    2, 0x08,  187 /* Private */,
     105,    0,  989,    2, 0x08,  189 /* Private */,
     106,    0,  990,    2, 0x08,  190 /* Private */,
     107,    0,  991,    2, 0x08,  191 /* Private */,
     108,    0,  992,    2, 0x08,  192 /* Private */,
     109,    0,  993,    2, 0x08,  193 /* Private */,
     110,    0,  994,    2, 0x08,  194 /* Private */,
     111,    0,  995,    2, 0x08,  195 /* Private */,
     112,    0,  996,    2, 0x08,  196 /* Private */,
     113,    0,  997,    2, 0x08,  197 /* Private */,
     114,    0,  998,    2, 0x08,  198 /* Private */,
     115,    0,  999,    2, 0x08,  199 /* Private */,
     116,    0, 1000,    2, 0x08,  200 /* Private */,
     117,    1, 1001,    2, 0x0a,  201 /* Public */,
     118,    1, 1004,    2, 0x0a,  203 /* Public */,
     119,    0, 1007,    2, 0x0a,  205 /* Public */,
     120,    1, 1008,    2, 0x0a,  206 /* Public */,
     121,    0, 1011,    2, 0x0a,  208 /* Public */,
     122,    1, 1012,    2, 0x0a,  209 /* Public */,
     123,    0, 1015,    2, 0x0a,  211 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::QString, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 36,   37,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 52, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 58,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QStringList, QMetaType::QStringList,   75,   76,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RadioInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ZN14RadioInterfaceE.offsetsAndSizes,
    qt_meta_data_ZN14RadioInterfaceE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN14RadioInterfaceE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RadioInterface, std::true_type>,
        // method 'select_ensemble_font'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'select_ensemble_fontSize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'select_ensemble_fontColor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'channelSignal'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_correlationSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_dcOffset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'startScanning'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stopScanning'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_quality'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'show_rsCorrections'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_clock_error'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_Corrector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'add_to_ensemble'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'name_of_ensemble'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'nrServices'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_frameErrors'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_rsErrors'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_aacErrors'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_ficQuality'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_ficBER'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'set_synced'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'show_label'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_motObject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'sendDatagram'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_tdcdata'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'changeinConfiguration'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList &, std::false_type>,
        // method 'newAudio'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'localSelect_SS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setStereo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_streamSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'no_signal_found'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_mothandling'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'set_sync_lost'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closeEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QCloseEvent *, std::false_type>,
        // method 'clockTime'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'start_announcement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'stop_announcement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'newFrame'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'set_epgData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'epgTimer_timeOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_presetSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_contentSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'http_terminate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_channel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_iqSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_spectrum'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_tiiThreshold'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_tiiData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<tiiData>, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_tii_spectrum'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'show_snr'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'show_null'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'showIQ'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'show_correlation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<int>, std::false_type>,
        // method 'show_stdDev'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'showPeakLevel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'handle_techFrame_closed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_configFrame_closed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_deviceFrame_closed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_newDisplayFrame_closed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'doStart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'newDevice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_scheduleButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_devicewidgetButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_resetButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_dlTextButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_snrButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_sourcedumpButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'scheduler_timeOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'show_changeLabel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QStringList, std::false_type>,
        // method 'show_pauzeSlide'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_timeTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_contentButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_detailButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_scanButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_etiHandler'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_spectrumButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_scanListButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_presetButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_framedumpButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_audiodumpButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_prevServiceButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_nextServiceButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_channelSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handle_nextChannelButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_prevChannelButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_muteButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_folderButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_scanListSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'TerminateProcess'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateTimeDisplay'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'channel_timeOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'start_background_task'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setPresetService'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'muteButton_timeOut'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_configButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_httpButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setVolume'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_snrLabel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_tiiButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_labelColor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_scanButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_presetButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_spectrumButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_scanListButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_prevServiceButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_nextServiceButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_configButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_httpButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'color_tiiButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'set_transmitters_local'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'handle_LoggerButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_set_coordinatesButton'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handle_eti_activeSelector'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_loadTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'selectDecoder'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handle_aboutLabel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RadioInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RadioInterface *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->select_ensemble_font(); break;
        case 1: _t->select_ensemble_fontSize(); break;
        case 2: _t->select_ensemble_fontColor(); break;
        case 3: _t->channelSignal((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->handle_correlationSelector((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->show_dcOffset((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 6: _t->startScanning(); break;
        case 7: _t->stopScanning(); break;
        case 8: _t->show_quality((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3]))); break;
        case 9: _t->show_rsCorrections((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 10: _t->show_clock_error((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->show_Corrector((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 12: _t->add_to_ensemble((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 13: _t->name_of_ensemble((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->nrServices((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 15: _t->show_frameErrors((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->show_rsErrors((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 17: _t->show_aacErrors((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->show_ficQuality((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 19: _t->show_ficBER((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 20: _t->set_synced((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 21: _t->show_label((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->handle_motObject((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[5]))); break;
        case 23: _t->sendDatagram((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->handle_tdcdata((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 25: _t->changeinConfiguration((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 26: _t->newAudio((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[4]))); break;
        case 27: _t->localSelect_SS((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 28: _t->setStereo((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 29: _t->set_streamSelector((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 30: _t->no_signal_found(); break;
        case 31: _t->show_mothandling((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->set_sync_lost(); break;
        case 33: _t->closeEvent((*reinterpret_cast< std::add_pointer_t<QCloseEvent*>>(_a[1]))); break;
        case 34: _t->clockTime((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[8])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[9]))); break;
        case 35: _t->start_announcement((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 36: _t->stop_announcement((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 37: _t->newFrame((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 38: _t->set_epgData((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[4]))); break;
        case 39: _t->epgTimer_timeOut(); break;
        case 40: _t->handle_presetSelect((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 41: _t->handle_contentSelector((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 42: _t->http_terminate(); break;
        case 43: _t->show_channel((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 44: _t->handle_iqSelector(); break;
        case 45: _t->show_spectrum((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 46: _t->handle_tiiThreshold((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 47: _t->show_tiiData((*reinterpret_cast< std::add_pointer_t<QList<tiiData>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 48: _t->show_tii_spectrum(); break;
        case 49: _t->show_snr((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 50: _t->show_null((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 51: _t->showIQ((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 52: _t->show_correlation((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[3]))); break;
        case 53: _t->show_stdDev((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 54: _t->showPeakLevel((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 55: _t->handle_techFrame_closed(); break;
        case 56: _t->handle_configFrame_closed(); break;
        case 57: _t->handle_deviceFrame_closed(); break;
        case 58: _t->handle_newDisplayFrame_closed(); break;
        case 59: _t->doStart((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 60: _t->newDevice((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 61: _t->handle_scheduleButton(); break;
        case 62: _t->handle_devicewidgetButton(); break;
        case 63: _t->handle_resetButton(); break;
        case 64: _t->handle_dlTextButton(); break;
        case 65: _t->handle_snrButton(); break;
        case 66: _t->handle_sourcedumpButton(); break;
        case 67: _t->scheduler_timeOut((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 68: _t->show_changeLabel((*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QStringList>>(_a[2]))); break;
        case 69: _t->show_pauzeSlide(); break;
        case 70: _t->handle_timeTable(); break;
        case 71: _t->handle_contentButton(); break;
        case 72: _t->handle_detailButton(); break;
        case 73: _t->handle_scanButton(); break;
        case 74: _t->handle_etiHandler(); break;
        case 75: _t->handle_spectrumButton(); break;
        case 76: _t->handle_scanListButton(); break;
        case 77: _t->handle_presetButton(); break;
        case 78: _t->handle_framedumpButton(); break;
        case 79: _t->handle_audiodumpButton(); break;
        case 80: _t->handle_prevServiceButton(); break;
        case 81: _t->handle_nextServiceButton(); break;
        case 82: _t->handle_channelSelector((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 83: _t->handle_nextChannelButton(); break;
        case 84: _t->handle_prevChannelButton(); break;
        case 85: _t->handle_muteButton(); break;
        case 86: _t->handle_folderButton(); break;
        case 87: _t->handle_scanListSelect((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 88: _t->TerminateProcess(); break;
        case 89: _t->updateTimeDisplay(); break;
        case 90: _t->channel_timeOut(); break;
        case 91: _t->start_background_task((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 92: _t->setPresetService(); break;
        case 93: _t->muteButton_timeOut(); break;
        case 94: _t->handle_configButton(); break;
        case 95: _t->handle_httpButton(); break;
        case 96: _t->setVolume((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 97: _t->handle_snrLabel(); break;
        case 98: _t->handle_tiiButton(); break;
        case 99: _t->handle_labelColor(); break;
        case 100: _t->color_scanButton(); break;
        case 101: _t->color_presetButton(); break;
        case 102: _t->color_spectrumButton(); break;
        case 103: _t->color_scanListButton(); break;
        case 104: _t->color_prevServiceButton(); break;
        case 105: _t->color_nextServiceButton(); break;
        case 106: _t->color_configButton(); break;
        case 107: _t->color_httpButton(); break;
        case 108: _t->color_tiiButton(); break;
        case 109: _t->set_transmitters_local((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 110: _t->handle_LoggerButton((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 111: _t->handle_set_coordinatesButton(); break;
        case 112: _t->handle_eti_activeSelector((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 113: _t->handle_loadTable(); break;
        case 114: _t->selectDecoder((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 115: _t->handle_aboutLabel(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 52:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 2:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (RadioInterface::*)();
            if (_q_method_type _q_method = &RadioInterface::select_ensemble_font; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (RadioInterface::*)();
            if (_q_method_type _q_method = &RadioInterface::select_ensemble_fontSize; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (RadioInterface::*)();
            if (_q_method_type _q_method = &RadioInterface::select_ensemble_fontColor; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *RadioInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RadioInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN14RadioInterfaceE.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RadioInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 116)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 116;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 116)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 116;
    }
    return _id;
}

// SIGNAL 0
void RadioInterface::select_ensemble_font()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RadioInterface::select_ensemble_fontSize()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RadioInterface::select_ensemble_fontColor()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
