/****************************************************************************
** Meta object code from reading C++ file 'radio.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "radio.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'radio.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RadioInterface_t {
    QByteArrayData data[138];
    char stringdata0[2508];
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
QT_MOC_LITERAL(4, 50, 12), // "show_quality"
QT_MOC_LITERAL(5, 63, 18), // "show_rsCorrections"
QT_MOC_LITERAL(6, 82, 16), // "show_clock_error"
QT_MOC_LITERAL(7, 99, 14), // "show_Corrector"
QT_MOC_LITERAL(8, 114, 15), // "add_to_ensemble"
QT_MOC_LITERAL(9, 130, 16), // "name_of_ensemble"
QT_MOC_LITERAL(10, 147, 16), // "show_frameErrors"
QT_MOC_LITERAL(11, 164, 13), // "show_rsErrors"
QT_MOC_LITERAL(12, 178, 14), // "show_aacErrors"
QT_MOC_LITERAL(13, 193, 15), // "show_ficSuccess"
QT_MOC_LITERAL(14, 209, 10), // "set_synced"
QT_MOC_LITERAL(15, 220, 10), // "show_label"
QT_MOC_LITERAL(16, 231, 16), // "handle_motObject"
QT_MOC_LITERAL(17, 248, 12), // "sendDatagram"
QT_MOC_LITERAL(18, 261, 14), // "handle_tdcdata"
QT_MOC_LITERAL(19, 276, 21), // "changeinConfiguration"
QT_MOC_LITERAL(20, 298, 8), // "newAudio"
QT_MOC_LITERAL(21, 307, 9), // "setStereo"
QT_MOC_LITERAL(22, 317, 18), // "set_streamSelector"
QT_MOC_LITERAL(23, 336, 15), // "no_signal_found"
QT_MOC_LITERAL(24, 352, 16), // "show_mothandling"
QT_MOC_LITERAL(25, 369, 13), // "set_sync_lost"
QT_MOC_LITERAL(26, 383, 10), // "closeEvent"
QT_MOC_LITERAL(27, 394, 12), // "QCloseEvent*"
QT_MOC_LITERAL(28, 407, 5), // "event"
QT_MOC_LITERAL(29, 413, 9), // "clockTime"
QT_MOC_LITERAL(30, 423, 18), // "start_announcement"
QT_MOC_LITERAL(31, 442, 17), // "stop_announcement"
QT_MOC_LITERAL(32, 460, 8), // "newFrame"
QT_MOC_LITERAL(33, 469, 11), // "set_epgData"
QT_MOC_LITERAL(34, 481, 16), // "epgTimer_timeOut"
QT_MOC_LITERAL(35, 498, 16), // "switchVisibility"
QT_MOC_LITERAL(36, 515, 8), // "QWidget*"
QT_MOC_LITERAL(37, 524, 10), // "nrServices"
QT_MOC_LITERAL(38, 535, 19), // "handle_presetSelect"
QT_MOC_LITERAL(39, 555, 22), // "handle_contentSelector"
QT_MOC_LITERAL(40, 578, 14), // "http_terminate"
QT_MOC_LITERAL(41, 593, 12), // "show_channel"
QT_MOC_LITERAL(42, 606, 17), // "handle_iqSelector"
QT_MOC_LITERAL(43, 624, 16), // "handle_timeTable"
QT_MOC_LITERAL(44, 641, 20), // "handle_contentButton"
QT_MOC_LITERAL(45, 662, 19), // "handle_detailButton"
QT_MOC_LITERAL(46, 682, 18), // "handle_resetButton"
QT_MOC_LITERAL(47, 701, 17), // "handle_scanButton"
QT_MOC_LITERAL(48, 719, 17), // "handle_etiHandler"
QT_MOC_LITERAL(49, 737, 16), // "handle_snrButton"
QT_MOC_LITERAL(50, 754, 21), // "handle_spectrumButton"
QT_MOC_LITERAL(51, 776, 25), // "handle_devicewidgetButton"
QT_MOC_LITERAL(52, 802, 25), // "handle_clearScan_Selector"
QT_MOC_LITERAL(53, 828, 21), // "handle_scanListButton"
QT_MOC_LITERAL(54, 850, 19), // "handle_presetButton"
QT_MOC_LITERAL(55, 870, 23), // "handle_sourcedumpButton"
QT_MOC_LITERAL(56, 894, 22), // "handle_framedumpButton"
QT_MOC_LITERAL(57, 917, 22), // "handle_audiodumpButton"
QT_MOC_LITERAL(58, 940, 24), // "handle_prevServiceButton"
QT_MOC_LITERAL(59, 965, 24), // "handle_nextServiceButton"
QT_MOC_LITERAL(60, 990, 22), // "handle_channelSelector"
QT_MOC_LITERAL(61, 1013, 24), // "handle_nextChannelButton"
QT_MOC_LITERAL(62, 1038, 24), // "handle_prevChannelButton"
QT_MOC_LITERAL(63, 1063, 17), // "handle_muteButton"
QT_MOC_LITERAL(64, 1081, 19), // "handle_dlTextButton"
QT_MOC_LITERAL(65, 1101, 21), // "handle_scanListSelect"
QT_MOC_LITERAL(66, 1123, 16), // "TerminateProcess"
QT_MOC_LITERAL(67, 1140, 17), // "updateTimeDisplay"
QT_MOC_LITERAL(68, 1158, 15), // "channel_timeOut"
QT_MOC_LITERAL(69, 1174, 7), // "doStart"
QT_MOC_LITERAL(70, 1182, 9), // "newDevice"
QT_MOC_LITERAL(71, 1192, 20), // "handle_serviceSelect"
QT_MOC_LITERAL(72, 1213, 11), // "QModelIndex"
QT_MOC_LITERAL(73, 1225, 16), // "setPresetService"
QT_MOC_LITERAL(74, 1242, 18), // "muteButton_timeOut"
QT_MOC_LITERAL(75, 1261, 17), // "scheduler_timeOut"
QT_MOC_LITERAL(76, 1279, 19), // "handle_configButton"
QT_MOC_LITERAL(77, 1299, 21), // "handle_scheduleButton"
QT_MOC_LITERAL(78, 1321, 17), // "handle_httpButton"
QT_MOC_LITERAL(79, 1339, 12), // "handle_onTop"
QT_MOC_LITERAL(80, 1352, 18), // "handle_autoBrowser"
QT_MOC_LITERAL(81, 1371, 22), // "handle_transmitterTags"
QT_MOC_LITERAL(82, 1394, 19), // "color_contentButton"
QT_MOC_LITERAL(83, 1414, 18), // "color_detailButton"
QT_MOC_LITERAL(84, 1433, 17), // "color_resetButton"
QT_MOC_LITERAL(85, 1451, 16), // "color_scanButton"
QT_MOC_LITERAL(86, 1468, 18), // "color_presetButton"
QT_MOC_LITERAL(87, 1487, 20), // "color_spectrumButton"
QT_MOC_LITERAL(88, 1508, 15), // "color_snrButton"
QT_MOC_LITERAL(89, 1524, 24), // "color_devicewidgetButton"
QT_MOC_LITERAL(90, 1549, 20), // "color_scanListButton"
QT_MOC_LITERAL(91, 1570, 22), // "color_sourcedumpButton"
QT_MOC_LITERAL(92, 1593, 16), // "color_muteButton"
QT_MOC_LITERAL(93, 1610, 23), // "color_prevChannelButton"
QT_MOC_LITERAL(94, 1634, 23), // "color_nextChannelButton"
QT_MOC_LITERAL(95, 1658, 23), // "color_prevServiceButton"
QT_MOC_LITERAL(96, 1682, 23), // "color_nextServiceButton"
QT_MOC_LITERAL(97, 1706, 18), // "color_dlTextButton"
QT_MOC_LITERAL(98, 1725, 20), // "color_scheduleButton"
QT_MOC_LITERAL(99, 1746, 18), // "color_configButton"
QT_MOC_LITERAL(100, 1765, 16), // "color_httpButton"
QT_MOC_LITERAL(101, 1782, 27), // "color_set_coordinatesButton"
QT_MOC_LITERAL(102, 1810, 21), // "color_loadTableButton"
QT_MOC_LITERAL(103, 1832, 16), // "color_skinButton"
QT_MOC_LITERAL(104, 1849, 16), // "color_fontButton"
QT_MOC_LITERAL(105, 1866, 18), // "color_portSelector"
QT_MOC_LITERAL(106, 1885, 13), // "show_spectrum"
QT_MOC_LITERAL(107, 1899, 8), // "show_tii"
QT_MOC_LITERAL(108, 1908, 17), // "show_tii_spectrum"
QT_MOC_LITERAL(109, 1926, 8), // "show_snr"
QT_MOC_LITERAL(110, 1935, 9), // "show_null"
QT_MOC_LITERAL(111, 1945, 6), // "showIQ"
QT_MOC_LITERAL(112, 1952, 16), // "show_correlation"
QT_MOC_LITERAL(113, 1969, 12), // "QVector<int>"
QT_MOC_LITERAL(114, 1982, 11), // "show_stdDev"
QT_MOC_LITERAL(115, 1994, 13), // "showPeakLevel"
QT_MOC_LITERAL(116, 2008, 22), // "handle_muteTimeSetting"
QT_MOC_LITERAL(117, 2031, 25), // "handle_switchDelaySetting"
QT_MOC_LITERAL(118, 2057, 23), // "handle_orderAlfabetical"
QT_MOC_LITERAL(119, 2081, 22), // "handle_orderServiceIds"
QT_MOC_LITERAL(120, 2104, 25), // "handle_ordersubChannelIds"
QT_MOC_LITERAL(121, 2130, 23), // "handle_scanmodeSelector"
QT_MOC_LITERAL(122, 2154, 26), // "handle_saveServiceSelector"
QT_MOC_LITERAL(123, 2181, 22), // "handle_skipList_button"
QT_MOC_LITERAL(124, 2204, 22), // "handle_skipFile_button"
QT_MOC_LITERAL(125, 2227, 23), // "handle_tii_detectorMode"
QT_MOC_LITERAL(126, 2251, 19), // "handle_LoggerButton"
QT_MOC_LITERAL(127, 2271, 28), // "handle_set_coordinatesButton"
QT_MOC_LITERAL(128, 2300, 19), // "handle_portSelector"
QT_MOC_LITERAL(129, 2320, 18), // "handle_epgSelector"
QT_MOC_LITERAL(130, 2339, 21), // "handle_transmSelector"
QT_MOC_LITERAL(131, 2361, 25), // "handle_eti_activeSelector"
QT_MOC_LITERAL(132, 2387, 17), // "handle_saveSlides"
QT_MOC_LITERAL(133, 2405, 19), // "handle_skinSelector"
QT_MOC_LITERAL(134, 2425, 16), // "handle_loadTable"
QT_MOC_LITERAL(135, 2442, 17), // "handle_fontSelect"
QT_MOC_LITERAL(136, 2460, 24), // "handle_dcRemovalSelector"
QT_MOC_LITERAL(137, 2485, 22) // "handle_decoderSelector"

    },
    "RadioInterface\0set_newChannel\0\0"
    "set_newPresetIndex\0show_quality\0"
    "show_rsCorrections\0show_clock_error\0"
    "show_Corrector\0add_to_ensemble\0"
    "name_of_ensemble\0show_frameErrors\0"
    "show_rsErrors\0show_aacErrors\0"
    "show_ficSuccess\0set_synced\0show_label\0"
    "handle_motObject\0sendDatagram\0"
    "handle_tdcdata\0changeinConfiguration\0"
    "newAudio\0setStereo\0set_streamSelector\0"
    "no_signal_found\0show_mothandling\0"
    "set_sync_lost\0closeEvent\0QCloseEvent*\0"
    "event\0clockTime\0start_announcement\0"
    "stop_announcement\0newFrame\0set_epgData\0"
    "epgTimer_timeOut\0switchVisibility\0"
    "QWidget*\0nrServices\0handle_presetSelect\0"
    "handle_contentSelector\0http_terminate\0"
    "show_channel\0handle_iqSelector\0"
    "handle_timeTable\0handle_contentButton\0"
    "handle_detailButton\0handle_resetButton\0"
    "handle_scanButton\0handle_etiHandler\0"
    "handle_snrButton\0handle_spectrumButton\0"
    "handle_devicewidgetButton\0"
    "handle_clearScan_Selector\0"
    "handle_scanListButton\0handle_presetButton\0"
    "handle_sourcedumpButton\0handle_framedumpButton\0"
    "handle_audiodumpButton\0handle_prevServiceButton\0"
    "handle_nextServiceButton\0"
    "handle_channelSelector\0handle_nextChannelButton\0"
    "handle_prevChannelButton\0handle_muteButton\0"
    "handle_dlTextButton\0handle_scanListSelect\0"
    "TerminateProcess\0updateTimeDisplay\0"
    "channel_timeOut\0doStart\0newDevice\0"
    "handle_serviceSelect\0QModelIndex\0"
    "setPresetService\0muteButton_timeOut\0"
    "scheduler_timeOut\0handle_configButton\0"
    "handle_scheduleButton\0handle_httpButton\0"
    "handle_onTop\0handle_autoBrowser\0"
    "handle_transmitterTags\0color_contentButton\0"
    "color_detailButton\0color_resetButton\0"
    "color_scanButton\0color_presetButton\0"
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
    "show_spectrum\0show_tii\0show_tii_spectrum\0"
    "show_snr\0show_null\0showIQ\0show_correlation\0"
    "QVector<int>\0show_stdDev\0showPeakLevel\0"
    "handle_muteTimeSetting\0handle_switchDelaySetting\0"
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
    "handle_loadTable\0handle_fontSelect\0"
    "handle_dcRemovalSelector\0"
    "handle_decoderSelector"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RadioInterface[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
     131,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  669,    2, 0x06 /* Public */,
       3,    1,  672,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    3,  675,    2, 0x0a /* Public */,
       5,    2,  682,    2, 0x0a /* Public */,
       6,    1,  687,    2, 0x0a /* Public */,
       7,    2,  690,    2, 0x0a /* Public */,
       8,    2,  695,    2, 0x0a /* Public */,
       9,    2,  700,    2, 0x0a /* Public */,
      10,    1,  705,    2, 0x0a /* Public */,
      11,    1,  708,    2, 0x0a /* Public */,
      12,    1,  711,    2, 0x0a /* Public */,
      13,    1,  714,    2, 0x0a /* Public */,
      14,    1,  717,    2, 0x0a /* Public */,
      15,    1,  720,    2, 0x0a /* Public */,
      16,    5,  723,    2, 0x0a /* Public */,
      17,    1,  734,    2, 0x0a /* Public */,
      18,    2,  737,    2, 0x0a /* Public */,
      19,    0,  742,    2, 0x0a /* Public */,
      20,    4,  743,    2, 0x0a /* Public */,
      21,    1,  752,    2, 0x0a /* Public */,
      22,    1,  755,    2, 0x0a /* Public */,
      23,    0,  758,    2, 0x0a /* Public */,
      24,    1,  759,    2, 0x0a /* Public */,
      25,    0,  762,    2, 0x0a /* Public */,
      26,    1,  763,    2, 0x0a /* Public */,
      29,    9,  766,    2, 0x0a /* Public */,
      30,    2,  785,    2, 0x0a /* Public */,
      31,    2,  790,    2, 0x0a /* Public */,
      32,    1,  795,    2, 0x0a /* Public */,
      33,    4,  798,    2, 0x0a /* Public */,
      34,    0,  807,    2, 0x0a /* Public */,
      35,    1,  808,    2, 0x0a /* Public */,
      37,    1,  811,    2, 0x0a /* Public */,
      38,    2,  814,    2, 0x0a /* Public */,
      39,    1,  819,    2, 0x0a /* Public */,
      40,    0,  822,    2, 0x0a /* Public */,
      41,    1,  823,    2, 0x0a /* Public */,
      42,    0,  826,    2, 0x0a /* Public */,
      43,    0,  827,    2, 0x08 /* Private */,
      44,    0,  828,    2, 0x08 /* Private */,
      45,    0,  829,    2, 0x08 /* Private */,
      46,    0,  830,    2, 0x08 /* Private */,
      47,    0,  831,    2, 0x08 /* Private */,
      48,    0,  832,    2, 0x08 /* Private */,
      49,    0,  833,    2, 0x08 /* Private */,
      50,    0,  834,    2, 0x08 /* Private */,
      51,    0,  835,    2, 0x08 /* Private */,
      52,    1,  836,    2, 0x08 /* Private */,
      53,    0,  839,    2, 0x08 /* Private */,
      54,    0,  840,    2, 0x08 /* Private */,
      55,    0,  841,    2, 0x08 /* Private */,
      56,    0,  842,    2, 0x08 /* Private */,
      57,    0,  843,    2, 0x08 /* Private */,
      58,    0,  844,    2, 0x08 /* Private */,
      59,    0,  845,    2, 0x08 /* Private */,
      60,    1,  846,    2, 0x08 /* Private */,
      61,    0,  849,    2, 0x08 /* Private */,
      62,    0,  850,    2, 0x08 /* Private */,
      63,    0,  851,    2, 0x08 /* Private */,
      64,    0,  852,    2, 0x08 /* Private */,
      65,    1,  853,    2, 0x08 /* Private */,
      66,    0,  856,    2, 0x08 /* Private */,
      67,    0,  857,    2, 0x08 /* Private */,
      68,    0,  858,    2, 0x08 /* Private */,
      69,    1,  859,    2, 0x08 /* Private */,
      70,    1,  862,    2, 0x08 /* Private */,
      71,    1,  865,    2, 0x08 /* Private */,
      73,    0,  868,    2, 0x08 /* Private */,
      74,    0,  869,    2, 0x08 /* Private */,
      75,    1,  870,    2, 0x08 /* Private */,
      76,    0,  873,    2, 0x08 /* Private */,
      77,    0,  874,    2, 0x08 /* Private */,
      78,    0,  875,    2, 0x08 /* Private */,
      79,    1,  876,    2, 0x08 /* Private */,
      80,    1,  879,    2, 0x08 /* Private */,
      81,    1,  882,    2, 0x08 /* Private */,
      82,    0,  885,    2, 0x08 /* Private */,
      83,    0,  886,    2, 0x08 /* Private */,
      84,    0,  887,    2, 0x08 /* Private */,
      85,    0,  888,    2, 0x08 /* Private */,
      86,    0,  889,    2, 0x08 /* Private */,
      87,    0,  890,    2, 0x08 /* Private */,
      88,    0,  891,    2, 0x08 /* Private */,
      89,    0,  892,    2, 0x08 /* Private */,
      90,    0,  893,    2, 0x08 /* Private */,
      91,    0,  894,    2, 0x08 /* Private */,
      92,    0,  895,    2, 0x08 /* Private */,
      93,    0,  896,    2, 0x08 /* Private */,
      94,    0,  897,    2, 0x08 /* Private */,
      95,    0,  898,    2, 0x08 /* Private */,
      96,    0,  899,    2, 0x08 /* Private */,
      97,    0,  900,    2, 0x08 /* Private */,
      98,    0,  901,    2, 0x08 /* Private */,
      99,    0,  902,    2, 0x08 /* Private */,
     100,    0,  903,    2, 0x08 /* Private */,
     101,    0,  904,    2, 0x08 /* Private */,
     102,    0,  905,    2, 0x08 /* Private */,
     103,    0,  906,    2, 0x08 /* Private */,
     104,    0,  907,    2, 0x08 /* Private */,
     105,    0,  908,    2, 0x08 /* Private */,
     106,    1,  909,    2, 0x0a /* Public */,
     107,    2,  912,    2, 0x0a /* Public */,
     108,    0,  917,    2, 0x0a /* Public */,
     109,    1,  918,    2, 0x0a /* Public */,
     110,    1,  921,    2, 0x0a /* Public */,
     111,    1,  924,    2, 0x0a /* Public */,
     112,    3,  927,    2, 0x0a /* Public */,
     114,    1,  934,    2, 0x0a /* Public */,
     115,    2,  937,    2, 0x0a /* Public */,
     116,    1,  942,    2, 0x08 /* Private */,
     117,    1,  945,    2, 0x08 /* Private */,
     118,    0,  948,    2, 0x08 /* Private */,
     119,    0,  949,    2, 0x08 /* Private */,
     120,    0,  950,    2, 0x08 /* Private */,
     121,    1,  951,    2, 0x08 /* Private */,
     122,    1,  954,    2, 0x08 /* Private */,
     123,    0,  957,    2, 0x08 /* Private */,
     124,    0,  958,    2, 0x08 /* Private */,
     125,    1,  959,    2, 0x08 /* Private */,
     126,    1,  962,    2, 0x08 /* Private */,
     127,    0,  965,    2, 0x08 /* Private */,
     128,    0,  966,    2, 0x08 /* Private */,
     129,    1,  967,    2, 0x08 /* Private */,
     130,    1,  970,    2, 0x08 /* Private */,
     131,    1,  973,    2, 0x08 /* Private */,
     132,    1,  976,    2, 0x08 /* Private */,
     133,    0,  979,    2, 0x08 /* Private */,
     134,    0,  980,    2, 0x08 /* Private */,
     135,    0,  981,    2, 0x08 /* Private */,
     136,    1,  982,    2, 0x08 /* Private */,
     137,    1,  985,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float,    2,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Float,    2,    2,
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
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    2,    2,    2,    2,
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
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    2,    2,
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
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
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
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 72,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
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
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 113,    2,    2,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,    2,    2,
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
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,

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
        case 2: _t->show_quality((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 3: _t->show_rsCorrections((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->show_clock_error((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->show_Corrector((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 6: _t->add_to_ensemble((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->name_of_ensemble((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 8: _t->show_frameErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->show_rsErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->show_aacErrors((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->show_ficSuccess((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->set_synced((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->show_label((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->handle_motObject((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5]))); break;
        case 15: _t->sendDatagram((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->handle_tdcdata((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 17: _t->changeinConfiguration(); break;
        case 18: _t->newAudio((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4]))); break;
        case 19: _t->setStereo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 20: _t->set_streamSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->no_signal_found(); break;
        case 22: _t->show_mothandling((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->set_sync_lost(); break;
        case 24: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 25: _t->clockTime((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8])),(*reinterpret_cast< int(*)>(_a[9]))); break;
        case 26: _t->start_announcement((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 27: _t->stop_announcement((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 28: _t->newFrame((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->set_epgData((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 30: _t->epgTimer_timeOut(); break;
        case 31: _t->switchVisibility((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        case 32: _t->nrServices((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 33: _t->handle_presetSelect((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 34: _t->handle_contentSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 35: _t->http_terminate(); break;
        case 36: _t->show_channel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->handle_iqSelector(); break;
        case 38: _t->handle_timeTable(); break;
        case 39: _t->handle_contentButton(); break;
        case 40: _t->handle_detailButton(); break;
        case 41: _t->handle_resetButton(); break;
        case 42: _t->handle_scanButton(); break;
        case 43: _t->handle_etiHandler(); break;
        case 44: _t->handle_snrButton(); break;
        case 45: _t->handle_spectrumButton(); break;
        case 46: _t->handle_devicewidgetButton(); break;
        case 47: _t->handle_clearScan_Selector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 48: _t->handle_scanListButton(); break;
        case 49: _t->handle_presetButton(); break;
        case 50: _t->handle_sourcedumpButton(); break;
        case 51: _t->handle_framedumpButton(); break;
        case 52: _t->handle_audiodumpButton(); break;
        case 53: _t->handle_prevServiceButton(); break;
        case 54: _t->handle_nextServiceButton(); break;
        case 55: _t->handle_channelSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 56: _t->handle_nextChannelButton(); break;
        case 57: _t->handle_prevChannelButton(); break;
        case 58: _t->handle_muteButton(); break;
        case 59: _t->handle_dlTextButton(); break;
        case 60: _t->handle_scanListSelect((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 61: _t->TerminateProcess(); break;
        case 62: _t->updateTimeDisplay(); break;
        case 63: _t->channel_timeOut(); break;
        case 64: _t->doStart((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 65: _t->newDevice((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 66: _t->handle_serviceSelect((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 67: _t->setPresetService(); break;
        case 68: _t->muteButton_timeOut(); break;
        case 69: _t->scheduler_timeOut((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 70: _t->handle_configButton(); break;
        case 71: _t->handle_scheduleButton(); break;
        case 72: _t->handle_httpButton(); break;
        case 73: _t->handle_onTop((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 74: _t->handle_autoBrowser((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 75: _t->handle_transmitterTags((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 76: _t->color_contentButton(); break;
        case 77: _t->color_detailButton(); break;
        case 78: _t->color_resetButton(); break;
        case 79: _t->color_scanButton(); break;
        case 80: _t->color_presetButton(); break;
        case 81: _t->color_spectrumButton(); break;
        case 82: _t->color_snrButton(); break;
        case 83: _t->color_devicewidgetButton(); break;
        case 84: _t->color_scanListButton(); break;
        case 85: _t->color_sourcedumpButton(); break;
        case 86: _t->color_muteButton(); break;
        case 87: _t->color_prevChannelButton(); break;
        case 88: _t->color_nextChannelButton(); break;
        case 89: _t->color_prevServiceButton(); break;
        case 90: _t->color_nextServiceButton(); break;
        case 91: _t->color_dlTextButton(); break;
        case 92: _t->color_scheduleButton(); break;
        case 93: _t->color_configButton(); break;
        case 94: _t->color_httpButton(); break;
        case 95: _t->color_set_coordinatesButton(); break;
        case 96: _t->color_loadTableButton(); break;
        case 97: _t->color_skinButton(); break;
        case 98: _t->color_fontButton(); break;
        case 99: _t->color_portSelector(); break;
        case 100: _t->show_spectrum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 101: _t->show_tii((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 102: _t->show_tii_spectrum(); break;
        case 103: _t->show_snr((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 104: _t->show_null((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 105: _t->showIQ((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 106: _t->show_correlation((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QVector<int>(*)>(_a[3]))); break;
        case 107: _t->show_stdDev((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 108: _t->showPeakLevel((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 109: _t->handle_muteTimeSetting((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 110: _t->handle_switchDelaySetting((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 111: _t->handle_orderAlfabetical(); break;
        case 112: _t->handle_orderServiceIds(); break;
        case 113: _t->handle_ordersubChannelIds(); break;
        case 114: _t->handle_scanmodeSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 115: _t->handle_saveServiceSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 116: _t->handle_skipList_button(); break;
        case 117: _t->handle_skipFile_button(); break;
        case 118: _t->handle_tii_detectorMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 119: _t->handle_LoggerButton((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 120: _t->handle_set_coordinatesButton(); break;
        case 121: _t->handle_portSelector(); break;
        case 122: _t->handle_epgSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 123: _t->handle_transmSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 124: _t->handle_eti_activeSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 125: _t->handle_saveSlides((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 126: _t->handle_skinSelector(); break;
        case 127: _t->handle_loadTable(); break;
        case 128: _t->handle_fontSelect(); break;
        case 129: _t->handle_dcRemovalSelector((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 130: _t->handle_decoderSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
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
        case 106:
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
        if (_id < 131)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 131;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 131)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 131;
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
