/****************************************************************************
** Meta object code from reading C++ file 'radio.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.10)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../radio.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.10. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RadioInterface_t {
    QByteArrayData data[131];
    char stringdata0[2357];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RadioInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RadioInterface_t qt_meta_stringdata_RadioInterface = {
    {
QT_MOC_LITERAL(0, 0, 14), // "RadioInterface"
QT_MOC_LITERAL(1, 15, 14), // "set_newChannel"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 18), // "set_newPresetIndex"
QT_MOC_LITERAL(4, 50, 11), // "showQuality"
QT_MOC_LITERAL(5, 62, 18), // "show_rsCorrections"
QT_MOC_LITERAL(6, 81, 15), // "show_clockError"
QT_MOC_LITERAL(7, 97, 20), // "set_CorrectorDisplay"
QT_MOC_LITERAL(8, 118, 13), // "addtoEnsemble"
QT_MOC_LITERAL(9, 132, 14), // "nameofEnsemble"
QT_MOC_LITERAL(10, 147, 16), // "show_frameErrors"
QT_MOC_LITERAL(11, 164, 13), // "show_rsErrors"
QT_MOC_LITERAL(12, 178, 14), // "show_aacErrors"
QT_MOC_LITERAL(13, 193, 15), // "show_ficSuccess"
QT_MOC_LITERAL(14, 209, 9), // "setSynced"
QT_MOC_LITERAL(15, 219, 9), // "showLabel"
QT_MOC_LITERAL(16, 229, 16), // "handle_motObject"
QT_MOC_LITERAL(17, 246, 12), // "sendDatagram"
QT_MOC_LITERAL(18, 259, 14), // "handle_tdcdata"
QT_MOC_LITERAL(19, 274, 21), // "changeinConfiguration"
QT_MOC_LITERAL(20, 296, 8), // "newAudio"
QT_MOC_LITERAL(21, 305, 9), // "setStereo"
QT_MOC_LITERAL(22, 315, 18), // "set_streamSelector"
QT_MOC_LITERAL(23, 334, 15), // "No_Signal_Found"
QT_MOC_LITERAL(24, 350, 16), // "show_motHandling"
QT_MOC_LITERAL(25, 367, 11), // "setSyncLost"
QT_MOC_LITERAL(26, 379, 10), // "closeEvent"
QT_MOC_LITERAL(27, 390, 12), // "QCloseEvent*"
QT_MOC_LITERAL(28, 403, 5), // "event"
QT_MOC_LITERAL(29, 409, 9), // "clockTime"
QT_MOC_LITERAL(30, 419, 17), // "startAnnouncement"
QT_MOC_LITERAL(31, 437, 16), // "stopAnnouncement"
QT_MOC_LITERAL(32, 454, 8), // "newFrame"
QT_MOC_LITERAL(33, 463, 11), // "set_epgData"
QT_MOC_LITERAL(34, 475, 16), // "epgTimer_timeOut"
QT_MOC_LITERAL(35, 492, 16), // "switchVisibility"
QT_MOC_LITERAL(36, 509, 8), // "QWidget*"
QT_MOC_LITERAL(37, 518, 10), // "nrServices"
QT_MOC_LITERAL(38, 529, 21), // "handle_presetSelector"
QT_MOC_LITERAL(39, 551, 22), // "handle_contentSelector"
QT_MOC_LITERAL(40, 574, 14), // "http_terminate"
QT_MOC_LITERAL(41, 589, 12), // "show_channel"
QT_MOC_LITERAL(42, 602, 16), // "handle_timeTable"
QT_MOC_LITERAL(43, 619, 20), // "handle_contentButton"
QT_MOC_LITERAL(44, 640, 19), // "handle_detailButton"
QT_MOC_LITERAL(45, 660, 18), // "handle_resetButton"
QT_MOC_LITERAL(46, 679, 17), // "handle_scanButton"
QT_MOC_LITERAL(47, 697, 17), // "handle_etiHandler"
QT_MOC_LITERAL(48, 715, 16), // "handle_snrButton"
QT_MOC_LITERAL(49, 732, 21), // "handle_spectrumButton"
QT_MOC_LITERAL(50, 754, 25), // "handle_devicewidgetButton"
QT_MOC_LITERAL(51, 780, 25), // "handle_clearScan_Selector"
QT_MOC_LITERAL(52, 806, 21), // "handle_scanListButton"
QT_MOC_LITERAL(53, 828, 23), // "handle_sourcedumpButton"
QT_MOC_LITERAL(54, 852, 22), // "handle_framedumpButton"
QT_MOC_LITERAL(55, 875, 22), // "handle_audiodumpButton"
QT_MOC_LITERAL(56, 898, 24), // "handle_prevServiceButton"
QT_MOC_LITERAL(57, 923, 24), // "handle_nextServiceButton"
QT_MOC_LITERAL(58, 948, 22), // "handle_channelSelector"
QT_MOC_LITERAL(59, 971, 24), // "handle_nextChannelButton"
QT_MOC_LITERAL(60, 996, 24), // "handle_prevChannelButton"
QT_MOC_LITERAL(61, 1021, 21), // "handle_scanListSelect"
QT_MOC_LITERAL(62, 1043, 16), // "TerminateProcess"
QT_MOC_LITERAL(63, 1060, 17), // "updateTimeDisplay"
QT_MOC_LITERAL(64, 1078, 15), // "channel_timeOut"
QT_MOC_LITERAL(65, 1094, 7), // "doStart"
QT_MOC_LITERAL(66, 1102, 9), // "newDevice"
QT_MOC_LITERAL(67, 1112, 13), // "selectService"
QT_MOC_LITERAL(68, 1126, 11), // "QModelIndex"
QT_MOC_LITERAL(69, 1138, 16), // "setPresetService"
QT_MOC_LITERAL(70, 1155, 17), // "handle_muteButton"
QT_MOC_LITERAL(71, 1173, 18), // "muteButton_timeOut"
QT_MOC_LITERAL(72, 1192, 17), // "scheduler_timeOut"
QT_MOC_LITERAL(73, 1210, 19), // "handle_dlTextButton"
QT_MOC_LITERAL(74, 1230, 19), // "handle_configButton"
QT_MOC_LITERAL(75, 1250, 21), // "handle_scheduleButton"
QT_MOC_LITERAL(76, 1272, 17), // "handle_httpButton"
QT_MOC_LITERAL(77, 1290, 12), // "handle_onTop"
QT_MOC_LITERAL(78, 1303, 18), // "handle_autoBrowser"
QT_MOC_LITERAL(79, 1322, 22), // "handle_transmitterTags"
QT_MOC_LITERAL(80, 1345, 19), // "color_contentButton"
QT_MOC_LITERAL(81, 1365, 18), // "color_detailButton"
QT_MOC_LITERAL(82, 1384, 17), // "color_resetButton"
QT_MOC_LITERAL(83, 1402, 16), // "color_scanButton"
QT_MOC_LITERAL(84, 1419, 20), // "color_spectrumButton"
QT_MOC_LITERAL(85, 1440, 15), // "color_snrButton"
QT_MOC_LITERAL(86, 1456, 24), // "color_devicewidgetButton"
QT_MOC_LITERAL(87, 1481, 20), // "color_scanListButton"
QT_MOC_LITERAL(88, 1502, 22), // "color_sourcedumpButton"
QT_MOC_LITERAL(89, 1525, 16), // "color_muteButton"
QT_MOC_LITERAL(90, 1542, 23), // "color_prevChannelButton"
QT_MOC_LITERAL(91, 1566, 23), // "color_nextChannelButton"
QT_MOC_LITERAL(92, 1590, 23), // "color_prevServiceButton"
QT_MOC_LITERAL(93, 1614, 23), // "color_nextServiceButton"
QT_MOC_LITERAL(94, 1638, 18), // "color_dlTextButton"
QT_MOC_LITERAL(95, 1657, 20), // "color_scheduleButton"
QT_MOC_LITERAL(96, 1678, 18), // "color_configButton"
QT_MOC_LITERAL(97, 1697, 16), // "color_httpButton"
QT_MOC_LITERAL(98, 1714, 27), // "color_set_coordinatesButton"
QT_MOC_LITERAL(99, 1742, 21), // "color_loadTableButton"
QT_MOC_LITERAL(100, 1764, 16), // "color_skinButton"
QT_MOC_LITERAL(101, 1781, 16), // "color_fontButton"
QT_MOC_LITERAL(102, 1798, 18), // "color_portSelector"
QT_MOC_LITERAL(103, 1817, 12), // "showSpectrum"
QT_MOC_LITERAL(104, 1830, 8), // "show_tii"
QT_MOC_LITERAL(105, 1839, 17), // "show_tii_spectrum"
QT_MOC_LITERAL(106, 1857, 8), // "show_snr"
QT_MOC_LITERAL(107, 1866, 9), // "show_null"
QT_MOC_LITERAL(108, 1876, 6), // "showIQ"
QT_MOC_LITERAL(109, 1883, 15), // "showCorrelation"
QT_MOC_LITERAL(110, 1899, 12), // "QVector<int>"
QT_MOC_LITERAL(111, 1912, 22), // "handle_muteTimeSetting"
QT_MOC_LITERAL(112, 1935, 25), // "handle_switchDelaySetting"
QT_MOC_LITERAL(113, 1961, 23), // "handle_orderAlfabetical"
QT_MOC_LITERAL(114, 1985, 22), // "handle_orderServiceIds"
QT_MOC_LITERAL(115, 2008, 25), // "handle_ordersubChannelIds"
QT_MOC_LITERAL(116, 2034, 23), // "handle_scanmodeSelector"
QT_MOC_LITERAL(117, 2058, 26), // "handle_saveServiceSelector"
QT_MOC_LITERAL(118, 2085, 22), // "handle_skipList_button"
QT_MOC_LITERAL(119, 2108, 22), // "handle_skipFile_button"
QT_MOC_LITERAL(120, 2131, 23), // "handle_tii_detectorMode"
QT_MOC_LITERAL(121, 2155, 19), // "handle_LoggerButton"
QT_MOC_LITERAL(122, 2175, 28), // "handle_set_coordinatesButton"
QT_MOC_LITERAL(123, 2204, 19), // "handle_portSelector"
QT_MOC_LITERAL(124, 2224, 18), // "handle_epgSelector"
QT_MOC_LITERAL(125, 2243, 21), // "handle_transmSelector"
QT_MOC_LITERAL(126, 2265, 25), // "handle_eti_activeSelector"
QT_MOC_LITERAL(127, 2291, 17), // "handle_saveSlides"
QT_MOC_LITERAL(128, 2309, 19), // "handle_skinSelector"
QT_MOC_LITERAL(129, 2329, 9), // "loadTable"
QT_MOC_LITERAL(130, 2339, 17) // "handle_fontSelect"

    },
    "RadioInterface\0set_newChannel\0\0"
    "set_newPresetIndex\0showQuality\0"
    "show_rsCorrections\0show_clockError\0"
    "set_CorrectorDisplay\0addtoEnsemble\0"
    "nameofEnsemble\0show_frameErrors\0"
    "show_rsErrors\0show_aacErrors\0"
    "show_ficSuccess\0setSynced\0showLabel\0"
    "handle_motObject\0sendDatagram\0"
    "handle_tdcdata\0changeinConfiguration\0"
    "newAudio\0setStereo\0set_streamSelector\0"
    "No_Signal_Found\0show_motHandling\0"
    "setSyncLost\0closeEvent\0QCloseEvent*\0"
    "event\0clockTime\0startAnnouncement\0"
    "stopAnnouncement\0newFrame\0set_epgData\0"
    "epgTimer_timeOut\0switchVisibility\0"
    "QWidget*\0nrServices\0handle_presetSelector\0"
    "handle_contentSelector\0http_terminate\0"
    "show_channel\0handle_timeTable\0"
    "handle_contentButton\0handle_detailButton\0"
    "handle_resetButton\0handle_scanButton\0"
    "handle_etiHandler\0handle_snrButton\0"
    "handle_spectrumButton\0handle_devicewidgetButton\0"
    "handle_clearScan_Selector\0"
    "handle_scanListButton\0handle_sourcedumpButton\0"
    "handle_framedumpButton\0handle_audiodumpButton\0"
    "handle_prevServiceButton\0"
    "handle_nextServiceButton\0"
    "handle_channelSelector\0handle_nextChannelButton\0"
    "handle_prevChannelButton\0handle_scanListSelect\0"
    "TerminateProcess\0updateTimeDisplay\0"
    "channel_timeOut\0doStart\0newDevice\0"
    "selectService\0QModelIndex\0setPresetService\0"
    "handle_muteButton\0muteButton_timeOut\0"
    "scheduler_timeOut\0handle_dlTextButton\0"
    "handle_configButton\0handle_scheduleButton\0"
    "handle_httpButton\0handle_onTop\0"
    "handle_autoBrowser\0handle_transmitterTags\0"
    "color_contentButton\0color_detailButton\0"
    "color_resetButton\0color_scanButton\0"
    "color_spectrumButton\0color_snrButton\0"
    "color_devicewidgetButton\0color_scanListButton\0"
    "color_sourcedumpButton\0color_muteButton\0"
    "color_prevChannelButton\0color_nextChannelButton\0"
    "color_prevServiceButton\0color_nextServiceButton\0"
    "color_dlTextButton\0color_scheduleButton\0"
    "color_configButton\0color_httpButton\0"
    "color_set_coordinatesButton\0"
    "color_loadTableButton\0color_skinButton\0"
    "color_fontButton\0color_portSelector\0"
    "showSpectrum\0show_tii\0show_tii_spectrum\0"
    "show_snr\0show_null\0showIQ\0showCorrelation\0"
    "QVector<int>\0handle_muteTimeSetting\0"
    "handle_switchDelaySetting\0"
    "handle_orderAlfabetical\0handle_orderServiceIds\0"
    "handle_ordersubChannelIds\0"
    "handle_scanmodeSelector\0"
    "handle_saveServiceSelector\0"
    "handle_skipList_button\0handle_skipFile_button\0"
    "handle_tii_detectorMode\0handle_LoggerButton\0"
    "handle_set_coordinatesButton\0"
    "handle_portSelector\0handle_epgSelector\0"
    "handle_transmSelector\0handle_eti_activeSelector\0"
    "handle_saveSlides\0handle_skinSelector\0"
    "loadTable\0handle_fontSelect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RadioInterface[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
     124,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  634,    2, 0x06 /* Public */,
       3,    1,  637,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,  640,    2, 0x0a /* Public */,
       5,    2,  647,    2, 0x0a /* Public */,
       6,    1,  652,    2, 0x0a /* Public */,
       7,    1,  655,    2, 0x0a /* Public */,
       8,    2,  658,    2, 0x0a /* Public */,
       9,    2,  663,    2, 0x0a /* Public */,
      10,    1,  668,    2, 0x0a /* Public */,
      11,    1,  671,    2, 0x0a /* Public */,
      12,    1,  674,    2, 0x0a /* Public */,
      13,    1,  677,    2, 0x0a /* Public */,
      14,    1,  680,    2, 0x0a /* Public */,
      15,    1,  683,    2, 0x0a /* Public */,
      16,    5,  686,    2, 0x0a /* Public */,
      17,    1,  697,    2, 0x0a /* Public */,
      18,    2,  700,    2, 0x0a /* Public */,
      19,    0,  705,    2, 0x0a /* Public */,
      20,    2,  706,    2, 0x0a /* Public */,
      21,    1,  711,    2, 0x0a /* Public */,
      22,    1,  714,    2, 0x0a /* Public */,
      23,    0,  717,    2, 0x0a /* Public */,
      24,    1,  718,    2, 0x0a /* Public */,
      25,    0,  721,    2, 0x0a /* Public */,
      26,    1,  722,    2, 0x0a /* Public */,
      29,    9,  725,    2, 0x0a /* Public */,
      30,    2,  744,    2, 0x0a /* Public */,
      31,    2,  749,    2, 0x0a /* Public */,
      32,    1,  754,    2, 0x0a /* Public */,
      33,    4,  757,    2, 0x0a /* Public */,
      34,    0,  766,    2, 0x0a /* Public */,
      35,    1,  767,    2, 0x0a /* Public */,
      37,    1,  770,    2, 0x0a /* Public */,
      38,    1,  773,    2, 0x0a /* Public */,
      39,    1,  776,    2, 0x0a /* Public */,
      40,    0,  779,    2, 0x0a /* Public */,
      41,    1,  780,    2, 0x0a /* Public */,
      42,    0,  783,    2, 0x08 /* Private */,
      43,    0,  784,    2, 0x08 /* Private */,
      44,    0,  785,    2, 0x08 /* Private */,
      45,    0,  786,    2, 0x08 /* Private */,
      46,    0,  787,    2, 0x08 /* Private */,
      47,    0,  788,    2, 0x08 /* Private */,
      48,    0,  789,    2, 0x08 /* Private */,
      49,    0,  790,    2, 0x08 /* Private */,
      50,    0,  791,    2, 0x08 /* Private */,
      51,    1,  792,    2, 0x08 /* Private */,
      52,    0,  795,    2, 0x08 /* Private */,
      53,    0,  796,    2, 0x08 /* Private */,
      54,    0,  797,    2, 0x08 /* Private */,
      55,    0,  798,    2, 0x08 /* Private */,
      56,    0,  799,    2, 0x08 /* Private */,
      57,    0,  800,    2, 0x08 /* Private */,
      58,    1,  801,    2, 0x08 /* Private */,
      59,    0,  804,    2, 0x08 /* Private */,
      60,    0,  805,    2, 0x08 /* Private */,
      61,    1,  806,    2, 0x08 /* Private */,
      62,    0,  809,    2, 0x08 /* Private */,
      63,    0,  810,    2, 0x08 /* Private */,
      64,    0,  811,    2, 0x08 /* Private */,
      65,    1,  812,    2, 0x08 /* Private */,
      66,    1,  815,    2, 0x08 /* Private */,
      67,    1,  818,    2, 0x08 /* Private */,
      69,    0,  821,    2, 0x08 /* Private */,
      70,    0,  822,    2, 0x08 /* Private */,
      71,    0,  823,    2, 0x08 /* Private */,
      72,    1,  824,    2, 0x08 /* Private */,
      73,    0,  827,    2, 0x08 /* Private */,
      74,    0,  828,    2, 0x08 /* Private */,
      75,    0,  829,    2, 0x08 /* Private */,
      76,    0,  830,    2, 0x08 /* Private */,
      77,    1,  831,    2, 0x08 /* Private */,
      78,    1,  834,    2, 0x08 /* Private */,
      79,    1,  837,    2, 0x08 /* Private */,
      80,    0,  840,    2, 0x08 /* Private */,
      81,    0,  841,    2, 0x08 /* Private */,
      82,    0,  842,    2, 0x08 /* Private */,
      83,    0,  843,    2, 0x08 /* Private */,
      84,    0,  844,    2, 0x08 /* Private */,
      85,    0,  845,    2, 0x08 /* Private */,
      86,    0,  846,    2, 0x08 /* Private */,
      87,    0,  847,    2, 0x08 /* Private */,
      88,    0,  848,    2, 0x08 /* Private */,
      89,    0,  849,    2, 0x08 /* Private */,
      90,    0,  850,    2, 0x08 /* Private */,
      91,    0,  851,    2, 0x08 /* Private */,
      92,    0,  852,    2, 0x08 /* Private */,
      93,    0,  853,    2, 0x08 /* Private */,
      94,    0,  854,    2, 0x08 /* Private */,
      95,    0,  855,    2, 0x08 /* Private */,
      96,    0,  856,    2, 0x08 /* Private */,
      97,    0,  857,    2, 0x08 /* Private */,
      98,    0,  858,    2, 0x08 /* Private */,
      99,    0,  859,    2, 0x08 /* Private */,
     100,    0,  860,    2, 0x08 /* Private */,
     101,    0,  861,    2, 0x08 /* Private */,
     102,    0,  862,    2, 0x08 /* Private */,
     103,    1,  863,    2, 0x08 /* Private */,
     104,    2,  866,    2, 0x08 /* Private */,
     105,    0,  871,    2, 0x08 /* Private */,
     106,    1,  872,    2, 0x08 /* Private */,
     107,    1,  875,    2, 0x08 /* Private */,
     108,    1,  878,    2, 0x08 /* Private */,
     109,    3,  881,    2, 0x08 /* Private */,
     111,    1,  888,    2, 0x08 /* Private */,
     112,    1,  891,    2, 0x08 /* Private */,
     113,    0,  894,    2, 0x08 /* Private */,
     114,    0,  895,    2, 0x08 /* Private */,
     115,    0,  896,    2, 0x08 /* Private */,
     116,    1,  897,    2, 0x08 /* Private */,
     117,    1,  900,    2, 0x08 /* Private */,
     118,    0,  903,    2, 0x08 /* Private */,
     119,    0,  904,    2, 0x08 /* Private */,
     120,    1,  905,    2, 0x08 /* Private */,
     121,    1,  908,    2, 0x08 /* Private */,
     122,    0,  911,    2, 0x08 /* Private */,
     123,    0,  912,    2, 0x08 /* Private */,
     124,    1,  913,    2, 0x08 /* Private */,
     125,    1,  916,    2, 0x08 /* Private */,
     126,    1,  919,    2, 0x08 /* Private */,
     127,    1,  922,    2, 0x08 /* Private */,
     128,    0,  925,    2, 0x08 /* Private */,
     129,    0,  926,    2, 0x08 /* Private */,
     130,    0,  927,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QByteArray, QMetaType::QString, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::QString, QMetaType::QString,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 36,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 68,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 110,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RadioInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RadioInterface *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->set_newChannel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_newPresetIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->showQuality((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 3: _t->show_rsCorrections((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->show_clockError((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->set_CorrectorDisplay((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->addtoEnsemble((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->nameofEnsemble((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->show_frameErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->show_rsErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->show_aacErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->show_ficSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->setSynced((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->showLabel((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->handle_motObject((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5]))); break;
        case 15: _t->sendDatagram((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->handle_tdcdata((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->changeinConfiguration(); break;
        case 18: _t->newAudio((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 19: _t->setStereo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->set_streamSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->No_Signal_Found(); break;
        case 22: _t->show_motHandling((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->setSyncLost(); break;
        case 24: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 25: _t->clockTime((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8])),(*reinterpret_cast< int(*)>(_a[9]))); break;
        case 26: _t->startAnnouncement((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 27: _t->stopAnnouncement((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 28: _t->newFrame((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->set_epgData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 30: _t->epgTimer_timeOut(); break;
        case 31: _t->switchVisibility((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 32: _t->nrServices((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->handle_presetSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 34: _t->handle_contentSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 35: _t->http_terminate(); break;
        case 36: _t->show_channel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->handle_timeTable(); break;
        case 38: _t->handle_contentButton(); break;
        case 39: _t->handle_detailButton(); break;
        case 40: _t->handle_resetButton(); break;
        case 41: _t->handle_scanButton(); break;
        case 42: _t->handle_etiHandler(); break;
        case 43: _t->handle_snrButton(); break;
        case 44: _t->handle_spectrumButton(); break;
        case 45: _t->handle_devicewidgetButton(); break;
        case 46: _t->handle_clearScan_Selector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 47: _t->handle_scanListButton(); break;
        case 48: _t->handle_sourcedumpButton(); break;
        case 49: _t->handle_framedumpButton(); break;
        case 50: _t->handle_audiodumpButton(); break;
        case 51: _t->handle_prevServiceButton(); break;
        case 52: _t->handle_nextServiceButton(); break;
        case 53: _t->handle_channelSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 54: _t->handle_nextChannelButton(); break;
        case 55: _t->handle_prevChannelButton(); break;
        case 56: _t->handle_scanListSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 57: _t->TerminateProcess(); break;
        case 58: _t->updateTimeDisplay(); break;
        case 59: _t->channel_timeOut(); break;
        case 60: _t->doStart((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 61: _t->newDevice((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 62: _t->selectService((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 63: _t->setPresetService(); break;
        case 64: _t->handle_muteButton(); break;
        case 65: _t->muteButton_timeOut(); break;
        case 66: _t->scheduler_timeOut((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 67: _t->handle_dlTextButton(); break;
        case 68: _t->handle_configButton(); break;
        case 69: _t->handle_scheduleButton(); break;
        case 70: _t->handle_httpButton(); break;
        case 71: _t->handle_onTop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 72: _t->handle_autoBrowser((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 73: _t->handle_transmitterTags((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 74: _t->color_contentButton(); break;
        case 75: _t->color_detailButton(); break;
        case 76: _t->color_resetButton(); break;
        case 77: _t->color_scanButton(); break;
        case 78: _t->color_spectrumButton(); break;
        case 79: _t->color_snrButton(); break;
        case 80: _t->color_devicewidgetButton(); break;
        case 81: _t->color_scanListButton(); break;
        case 82: _t->color_sourcedumpButton(); break;
        case 83: _t->color_muteButton(); break;
        case 84: _t->color_prevChannelButton(); break;
        case 85: _t->color_nextChannelButton(); break;
        case 86: _t->color_prevServiceButton(); break;
        case 87: _t->color_nextServiceButton(); break;
        case 88: _t->color_dlTextButton(); break;
        case 89: _t->color_scheduleButton(); break;
        case 90: _t->color_configButton(); break;
        case 91: _t->color_httpButton(); break;
        case 92: _t->color_set_coordinatesButton(); break;
        case 93: _t->color_loadTableButton(); break;
        case 94: _t->color_skinButton(); break;
        case 95: _t->color_fontButton(); break;
        case 96: _t->color_portSelector(); break;
        case 97: _t->showSpectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 98: _t->show_tii((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 99: _t->show_tii_spectrum(); break;
        case 100: _t->show_snr((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 101: _t->show_null((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 102: _t->showIQ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 103: _t->showCorrelation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QVector<int>(*)>(_a[3]))); break;
        case 104: _t->handle_muteTimeSetting((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 105: _t->handle_switchDelaySetting((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 106: _t->handle_orderAlfabetical(); break;
        case 107: _t->handle_orderServiceIds(); break;
        case 108: _t->handle_ordersubChannelIds(); break;
        case 109: _t->handle_scanmodeSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 110: _t->handle_saveServiceSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 111: _t->handle_skipList_button(); break;
        case 112: _t->handle_skipFile_button(); break;
        case 113: _t->handle_tii_detectorMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 114: _t->handle_LoggerButton((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 115: _t->handle_set_coordinatesButton(); break;
        case 116: _t->handle_portSelector(); break;
        case 117: _t->handle_epgSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 118: _t->handle_transmSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 119: _t->handle_eti_activeSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 120: _t->handle_saveSlides((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 121: _t->handle_skinSelector(); break;
        case 122: _t->loadTable(); break;
        case 123: _t->handle_fontSelect(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWidget* >(); break;
            }
            break;
        case 103:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RadioInterface::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RadioInterface::set_newChannel)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (RadioInterface::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RadioInterface::set_newPresetIndex)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RadioInterface::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RadioInterface.data,
    qt_meta_data_RadioInterface,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RadioInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RadioInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RadioInterface.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RadioInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 124)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 124;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 124)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 124;
    }
    return _id;
}

// SIGNAL 0
void RadioInterface::set_newChannel(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void RadioInterface::set_newPresetIndex(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
