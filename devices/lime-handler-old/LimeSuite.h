/**
 * @file lime/LimeSuite.h
 *
 * @brief LMS API library
 *
 * Copyright (C) 2016 Lime Microsystems
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LMS7_API_H
#define LMS7_API_H

#include <cstdint>
#include <cstdlib>
#include "LMS7002M_parameters.h"

#ifdef __cplusplus
extern "C" {
#else
/* stdbool.h is not applicable for C++ programs, as the language inherently
 * provides the bool type.
 *
 * Users of Visual Studio 2012 and earlier will need to supply a stdbool.h
 * implementation, as it is not included with the toolchain. Visual Studio 2013
 * onward supplies this header.
 */
#include <stdbool.h>
#endif

#if defined _WIN32 || defined __CYGWIN__
#   include <windows.h>
#   define CALL_CONV __cdecl
#   ifdef __GNUC__
#       define API_EXPORT __attribute__ ((dllexport))
#   else
#       define API_EXPORT __declspec(dllexport)
#   endif
#elif defined _DOXYGEN_ONLY_
    /** Marks an API routine to be made visible to the dynamic loader.
     *  This is OS and/or compiler-specific. */
#   define API_EXPORT
    /** Specifies calling convention, if necessary.
     *  This is OS and/or compiler-specific. */
#   define CALL_CONV
#else
#   define API_EXPORT __attribute__ ((visibility ("default")))
#   define CALL_CONV
#endif

///Floating point data type
typedef double float_type;

///convenience constant for good return code
static const int LMS_SUCCESS = 0;

/**
 * @defgroup FN_INIT    Initialization/deinitialization
 *
 * The functions in this section provide the ability to query available devices,
 * initialize them, and deinitialize them.
 * @{
 */

///LMS Device handle
typedef void lms_device_t;

///Convenience type for fixed length LMS Device information string
typedef char lms_info_str_t[256];

/**
 * Obtain a list of LMS devices attached to the system
 *
 * @param[out]  dev_list    List of available devices
 *
 * @return      number of devices in the list on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetDeviceList(lms_info_str_t *dev_list);

/**
 * Opens device specified by the provided ::lms_info_str_t string
 * This function should be used to open a device based upon the results of
 * LMS_GetDeviceList()
 *
 * @pre device should be initialized to NULL
 *
 * @param[out]  device      Updated with device handle on success
 * @param[in]   info        Device information string. If NULL, the first
 *                          available device will be opened.
 * @param[in]   args        additional arguments. Can be NULL.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Open(lms_device_t **device, const lms_info_str_t info,
                                                         void* args);

/**
 * Close device
 *
 * @post     device is deallocated and may no longer be used.
 *
 * @param    device  Device handle previously obtained by LMS_Open().
 *
 * @return   0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Close(lms_device_t *device);

/** @} (End FN_INIT) */

/**
 * @defgroup FN_HIGH_LVL    High-level control functions
 *
 * The functions in this section provide the ability to easily configure the
 * device for operation. They modify multiple internal device settings. 
 *
 * @{
 */

static const bool LMS_CH_TX = true;   ///<Convenience constants for TX selection
static const bool LMS_CH_RX = false;  ///<Convenience constants for RX selection

/** Convenience type for fixed length name string*/
typedef char lms_name_t[16];

/**Structure used to represent the allowed value range of various parameters*/
typedef struct
{
    float_type min;     ///<Minimum allowed value
    float_type max;     ///<Minimum allowed value
    float_type step;    ///<Minimum value step
}lms_range_t;

/**Enumeration of LMS7 TEST signal types*/
typedef enum
{
    LMS_TESTSIG_NONE=0,     ///<Disable test signals. Return to normal operation
    LMS_TESTSIG_NCODIV8,    ///<Test signal from NCO half scale
    LMS_TESTSIG_NCODIV4,    ///<Test signal from NCO half scale
    LMS_TESTSIG_NCODIV8F,   ///<Test signal from NCO full scale
    LMS_TESTSIG_NCODIV4F,   ///<Test signal from NCO full scale
    LMS_TESTSIG_DC          ///<DC test signal
}lms_testsig_t;

/**
 * Configure LMS chip with settings that make it ready for operation.
 *
 * @note This configuration differs from default LMS chip configuration which is
 * described in chip datasheet. In order to load default chip configuration use
 * LMS_Reset().
 *
 * @param[in]   device  Device handle previously obtained by LMS_Open().
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Init(lms_device_t *device);

/**
 * Obtain number of RX or TX channels. Use this to determine the maximum
 * channel index (specifying channel index is required by most API functions).
 * The maximum channel index is N-1, where N is number returned by this function
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param dir_tx    Select RX or TX
 *
 * @return          Number of channels on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetNumChannels(lms_device_t *device, bool dir_tx);

/**
 * Enable or disable specified RX or TX channel. Some API functions will fail
 * when performed on disabled channel.
 *
 * @param[in]   device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param       enabled     true(1) to enable, false(0) to disable.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_EnableChannel(lms_device_t *device, bool dir_tx,
                                           size_t chan, bool enabled);

/**
 * Set sampling rate for all RX/TX channels. Sample rate is in complex samples
 * (1 sample = I + Q). The function sets sampling rate that is used for data
 * exchange with the host. It also allows to specify higher sampling rate to be
 * used in RF by setting oversampling ratio. Valid oversampling values are 1, 2,
 * 4, 8, 16, 32 or 0 (use device default oversampling value).
 *
 * @param[in]   device      Device handle previously obtained by LMS_Open().
 * @param       rate        sampling rate in Hz to set
 * @param       oversample  RF oversampling ratio.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetSampleRate(lms_device_t *device, float_type rate,
                                           size_t oversample);

/**
 * Get the sampling rate of the specified RX or TX channel.
 * The function obtains the sample rate used in data interface with the host and
 * the RF sample rate used by DAC/ADC.
 *
 * @param[in]   device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param[out]  host_Hz     sampling rate used for data exchange with the host
 * @param[out]  rf_Hz       RF sampling rate in Hz
 *
 * @return       0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetSampleRate(lms_device_t *device, bool dir_tx,
                           size_t chan, float_type *host_Hz, float_type *rf_Hz);
/**
 * Get the range of supported sampling rates.
 *
 * @param      device   Device handle previously obtained by LMS_Open().
 * @param      dir_tx   Select RX or TX
 * @param[out] range    Allowed sample rate range in Hz.
 *
 * @return              0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetSampleRateRange(lms_device_t *device, bool dir_tx,
                                                lms_range_t *range);

/**
 * Set RF center frequency in Hz.
 *
 * @note channels A and B in LMS7 chip share the same clock so ability to set
 * different frequencies for channels A and B is very limited. This function 
 * will attempt to achieve different requested frequencies using NCO when 
 * possible, however often changing frequency for one (A or B) channel will
 * result in frequency being changed for both (A and B) channels.
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   frequency   Desired RF center frequency in Hz
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetLOFrequency(lms_device_t *device, bool dir_tx,
                                            size_t chan, float_type frequency);

/**
 * Obtain the current RF center frequency in Hz.
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param[out]  frequency   Current RF center frequency in Hz
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetLOFrequency(lms_device_t *device, bool dir_tx,
                                            size_t chan, float_type *frequency);

/**
 * Obtain the supported RF center frequency range in Hz.
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param[out]  range       Supported RF center frequency in Hz
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetLOFrequencyRange(lms_device_t *device, bool dir_tx,
                                                 lms_range_t *range);

///Enumeration of RF ports
enum
{
    LMS_PATH_NONE = 0, ///<No active path (RX or TX)
    LMS_PATH_LNAH = 1, ///<RX LNA_H port
    LMS_PATH_LNAL = 2, ///<RX LNA_L port
    LMS_PATH_LNAW = 3, ///<RX LNA_W port
    LMS_PATH_TX1 = 1,  ///<TX port 1
    LMS_PATH_TX2 = 2,   ///<TX port 2
    LMS_PATH_AUTO = 255, ///<Automatically select port (if supported)
};

/**
 * Obtain antenna list with names. First item in the list is the name of antenna
 * index 0.
 *
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param       dir_tx  Select RX or TX
 * @param       chan    channel index
 * @param[out]  list    List of antenna names (can be NULL)
 *
 * @return      number of available antennae, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetAntennaList(lms_device_t *dev, bool dir_tx,
                                            size_t chan, lms_name_t *list);

/**
 * Select the antenna for the specified RX or TX channel.
 *
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param       dir_tx  Select RX or TX
 * @param       chan    channel index
 * @param       index   Index of antenna to select
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetAntenna(lms_device_t *dev, bool dir_tx,
                                        size_t chan, size_t index);

/**
 * Obtain currently selected antenna of the the specified RX or TX channel.
 *
 * @param dev       Device handle previously obtained by LMS_Open().
 * @param dir_tx    Select RX or TX
 * @param chan      channel index
 *
 * @return      Index of selected antenna on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetAntenna(lms_device_t *dev, bool dir_tx,
                                        size_t chan);

/**
 * Obtains recommended bandwidth (lower and upper frequency) for the specified
 * antenna port. The ports can be used outside this range.
 *
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param       dir_tx  Select RX or TX
 * @param       chan    channel index
 * @param       index   Antenna index
 * @param[out]  range   Antenna bandwidth
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetAntennaBW(lms_device_t *dev, bool dir_tx,
                                 size_t chan, size_t index, lms_range_t *range);

/**
 * Set the combined gain value
 * This function computes and sets the optimal gain values of various amplifiers
 * that are present in the device based on desired normalized gain value.
 *
 * @note actual gain depends on LO frequency and analog LPF configuration and
 * resulting output signal level may be different when those values are changed
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   gain        Desired gain, range [0, 1.0], where 1.0 represents the
 *                      maximum gain
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetNormalizedGain(lms_device_t *device, bool dir_tx,
                                               size_t chan,float_type gain);

/**
 * Set the combined gain value in dB
 * This function computes and sets the optimal gain values of various amplifiers
 * that are present in the device based on desired  gain value in dB.
 *
 * @note actual gain depends on LO frequency and analog LPF configuration and
 * resulting output signal levle may be different when those values are changed
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   gain        Desired gain, range [0, 73]
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetGaindB(lms_device_t *device, bool dir_tx,
                                        size_t chan, unsigned gain);

/**
 * Obtain the current combined gain value
 *
 * @note actual gain depends on LO frequency and analog LPF configuration and
 * resulting output signal level may be different when those values are changed
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param[out]  gain        Current gain, range [0, 1.0], where 1.0 represents
 *                          the maximum gain
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetNormalizedGain(lms_device_t *device, bool dir_tx,
                                                size_t chan, float_type *gain);
/**
 * Obtain the current combined gain value in dB
 *
 * @note actual gain depends on LO frequency and analog LPF configuration and
 * resulting output signal level may be different when those values are changed
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param[out]  gain        Current gain
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetGaindB(lms_device_t *device, bool dir_tx,
                                                size_t chan, unsigned *gain);

/**
 * Configure analog LPF of the LMS chip for the desired RF bandwidth.
 * This function automatically enables LPF.
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   bandwidth   LPF bandwidth in Hz
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetLPFBW(lms_device_t *device, bool dir_tx,
                                             size_t chan, float_type bandwidth);

/**
 * Get the currently configured analog LPF RF bandwidth.
 * @note readback from board is currently not supported, only returns last set
 * value cached by software.
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       chan        Channel index
 * @param[out]  bandwidth   Current LPF bandwidth in Hz
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetLPFBW(lms_device_t *device, bool dir_tx,
                                            size_t chan, float_type *bandwidth);

/**
 * Get the RF bandwidth setting range supported by the analog LPF of LMS chip.
 *
 * @param       device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param[out]  range       Supported RF bandwidth range in Hz
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetLPFBWRange(lms_device_t *device, bool dir_tx,
                                            lms_range_t *range);

/**
 * Disables or enables the analog LPF of LMS chip without reconfiguring it.
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   enable      true(1) to enable, false(0) to disable
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetLPF(lms_device_t *device, bool dir_tx,
                                    size_t chan, bool enable);

/**
 * Set up digital LPF using LMS chip GFIRS. This is a convenience function to
 * quickly configure GFIRS as LPF with desired RF bandwidth.
 *
 * @pre sampling rate must be set
 *
 * @param  device      Device handle previously obtained by LMS_Open().
 * @param  dir_tx      Select RX or TX
 * @param  chan        channel index
 * @param  enabled     Disable (false) or enable (true) GFIRS.
 * @param  bandwidth   LPF bandwidth in Hz. Has no effect if enabled is false.
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetGFIRLPF(lms_device_t *device, bool dir_tx,
                               size_t chan, bool enabled, float_type bandwidth);

/**
 * Perform the automatic calibration of specified RX/TX channel. The automatic
 * calibration must be run after device configuration is finished because
 * calibration values are dependant on various configuration settings.
 *
 * @pre Device should be configured
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        channel index
 * @param   bw          bandwidth
 * @param   flags       additional calibration flags (normally should be 0)
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Calibrate(lms_device_t *device, bool dir_tx,
                                        size_t chan, double bw, unsigned flags);

/**
 * Load LMS chip configuration from a file
 *
 * @note this only loads LMS chip configuration, in oder for streaming to work
 * properly FPGA has also to be configured. Use LMS_SetSampleRate() to configure
 * LMS and FPGA for streaming.
 *
 * @param   device      Device handle
 * @param   filename    path to file
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_LoadConfig(lms_device_t *device, const char *filename);

/**
 * Save LMS chip configuration to a file
 *
 * @param   device      Device handle
 * @param   filename    path to file with LMS chip configuration
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SaveConfig(lms_device_t *device, const char *filename);

/**
 * Apply the specified test signal
 *
 * @param   device  Device handle previously obtained by LMS_Open().
 * @param   dir_tx  Select RX or TX.
 * @param   chan    Channel index.
 * @param   sig     Test signal. LMS_TESTSIG_NONE disables test signal.
 * @param   dc_i    DC I value for LMS_TESTSIG_DC mode. Ignored in other modes.
 * @param   dc_q    DC Q value for LMS_TESTSIG_DC mode. Ignored in other modes.
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetTestSignal(lms_device_t *device, bool dir_tx,
                    size_t chan, lms_testsig_t sig, int16_t dc_i, int16_t dc_q);
/**
 * Get the currently active test signal
 *
 * @param   device      Device handle previously obtained by LMS_Open().
 * @param   dir_tx      Select RX or TX
 * @param   chan        Channel index
 * @param   sig         Currently active test signal
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetTestSignal(lms_device_t *device, bool dir_tx,
                                               size_t chan, lms_testsig_t *sig);

/**
 * Read LMS7 chip internal temperature sensor
 *
 * @param   dev         Device handle previously obtained by LMS_Open().
 * @param   ind         chip index
 * @param   temp        temperature value
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetChipTemperature(lms_device_t *dev, size_t ind,
                                                float_type *temp);

/**
 * @defgroup FN_ADVANCED    Advanced control functions
 *
 * The functions in this section provides some additional control compared to
 * High-Level functions. They are labeled advanced because they require better
 * understanding of hardware and provide functionality that may conflict with
 * other High-Level functions.
 * @{
 */

///Enumeration of LMS7 GFIRS
typedef enum
{
    LMS_GFIR1 = 0,
    LMS_GFIR2,
    LMS_GFIR3
}lms_gfir_t;

///Number of NCO frequency/phase offset values
static const int LMS_NCO_VAL_COUNT = 16;

/**
 * Set sampling rate for all RX or TX channels. Sample rate is in complex
 * samples (1 sample = I + Q). The function sets sampling rate that is used for
 * data exchange with the host. It also allows to specify higher sampling rate
 * to be used in RF by setting oversampling ratio. Valid oversampling values are
 * 1, 2, 4, 8, 16, 32 or 0 (use device default oversampling value).
 *
 * @note RX and TX rates sampling are closely tied in LMS7 chip. Changing RX or
 * TX will often result in change of both (RX and TX). RX/TX ratio can only be
 * power of 2 and is also limited by other factors. Use LMS_GetSampleRate() to
 * obtain actual sample rate values. The function returns success if it is able
 * to achieve  desired sample rate and oversampling for the specified direction
 * (RX or TX) ignoring possible value changes in other direction channels.
 *
 * @param[in]   device      Device handle previously obtained by LMS_Open().
 * @param       dir_tx      Select RX or TX
 * @param       rate        Sampling rate in Hz to set
 * @param       oversample  RF oversampling ratio.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetSampleRateDir(lms_device_t *device, bool dir_tx,
                                            float_type rate, size_t oversample);

/**
 * Configure NCO to operate in FCW mode. Configures NCO with up to 16
 * frequencies that can be quickly switched between.
 * Automatically starts NCO with frequency at index 0
 * Use LMS_SetNCOindex() to switch between NCO frequencies.
 *
 * @param       device     Device handle previously obtained by LMS_Open().
 * @param       dir_tx     Select RX or TX
 * @param       chan       Channel index
 * @param[in]   freq       List of NCO frequencies. Values cannot be negative.
 *                         Must be at least ::LMS_NCO_VAL_COUNT length;
 * @param       pho        NCO phase offset in deg
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetNCOFrequency(lms_device_t *device, bool dir_tx,
                     size_t chan, const float_type *freq, float_type pho);

/**
 * Get the current NCO FCW mode configuration.
 *
 * @param       device     Device handle previously obtained by LMS_Open().
 * @param       dir_tx     Select RX or TX
 * @param       chan       Channel index
 * @param[out]  freq       List of NCO frequencies. Must be at least
 *                         ::LMS_NCO_VAL_COUNT length;
 * @param[out]  pho        Phase offset in deg
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetNCOFrequency(lms_device_t *device, bool dir_tx,
                          size_t chan, float_type *freq, float_type *pho);

/**
 * Configure NCO to operate in PHO mode. Configures NCO with up to 16
 * phase offsets that can be quickly switched between.
 * Automatically starts NCO with phase at index 0
 * Use LMS_SetNCOindex() to switch between NCO phases.
 *
 * @param       device     Device handle previously obtained by LMS_Open().
 * @param       dir_tx     Select RX or TX
 * @param       chan       Channel index
 * @param[in]   phases     List of NCO phases. Values cannot be negative.
 *                         Must be at least ::LMS_NCO_VAL_COUNT length;
 * @param       fcw        NCO frequency in Hz
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetNCOPhase(lms_device_t *device, bool dir_tx,
                   size_t chan, const float_type *phases, float_type fcw);

/**
 * Get the current NCO PHO mode configuration.
 *
 * @param       device    Device handle previously obtained by LMS_Open().
 * @param       dir_tx    Select RX or TX
 * @param       chan      channel index
 * @param[out]  phases    List of configured NCO phases
 *                        Must be at least ::LMS_NCO_VAL_COUNT length;
 * @param[out]  fcw       Current NCO frequency
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetNCOPhase(lms_device_t *device, bool dir_tx,
                            size_t chan, float_type *phases, float_type *fcw);

/**
 * Switches between configured list of NCO frequencies/phase offsets. Also
 * Allows to switch CMIX mode to either downconvert or upconvert.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param dir_tx    Select RX or TX
 * @param chan      channel index
 * @param index     NCO frequency/phase index to activate or (-1) to disable NCO
 * @param downconv  true(1) CMIX downconvert, false(0) CMIX upconvert
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetNCOIndex(lms_device_t *device, bool dir_tx,
                                    size_t chan, int index, bool downconv);

/**
 * Get the currently active NCO frequency/phase offset index
 *
 * @param       device    Device handle previously obtained by LMS_Open().
 * @param       dir_tx    Select RX or TX
 * @param       chan      Channel index
 *
 * @return Current NCO frequency/phase index on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetNCOIndex(lms_device_t *device, bool dir_tx,
                                        size_t chan);

/**
 * Configure LMS GFIR using specified filter coefficients. Maximum number of
 * coefficients is 40 for GFIR1 and GFIR2, and 120 for GFIR3.
 *
 * @param       device       Device handle previously obtained by LMS_Open().
 * @param       dir_tx    Select RX or TX
 * @param       chan      Channel index
 * @param       filt      GFIR to configure
 * @param[in]   coef      Array of filter coefficients. Coeff range [-1.0, 1.0].
 * @param       count     number of filter coefficients.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetGFIRCoeff(lms_device_t * device, bool dir_tx,
             size_t chan, lms_gfir_t filt, const float_type* coef,size_t count);

/**
 * Get currently set GFIR coefficients.
 *
 * @param       device       Device handle previously obtained by LMS_Open().
 * @param       dir_tx    Select RX or TX
 * @param       chan      Channel index
 * @param       filt      GFIR to configure
 * @param[out]  coef      Current GFIR coefficients. Array must be big enough to
 *                        hold 40 (GFIR1, GFIR2) or 120 (GFIR3) values.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetGFIRCoeff(lms_device_t * device, bool dir_tx,
                                size_t chan, lms_gfir_t filt, float_type* coef);

/**
 * Enables or disables specified GFIR.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param dir_tx    Select RX or TX
 * @param chan      Channel index
 * @param filt      GFIR to configure
 * @param enabled   true(1) enable, false(0) disable.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetGFIR(lms_device_t * device, bool dir_tx,
                                    size_t chan, lms_gfir_t filt, bool enabled);



/**
 * Enables or disable caching of LMS7 and FPGA register values.
 * 
 * @deprecated calibration cache has been removed from LimeSuite. Use
 * LMS_EnableCache() to enable caching of register values
 * 
 * @param   dev         Device handle previously obtained by LMS_Open().
 * @param   enable      true to enable cache
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_EnableCalibCache(lms_device_t *dev, bool enable);

/**
 * Enables or disable caching of LMS7 and FPGA register values.
 * 
 * @param   dev         Device handle previously obtained by LMS_Open().
 * @param   enable      true to enable cache
 *
 * @return 0 on success, (-1) on failure
 */

API_EXPORT int CALL_CONV LMS_EnableCache(lms_device_t *dev, bool enable);

/** @} (End FN_ADVANCED) */

/** @} (End FN_HIGH_LVL) */

/**
 * @defgroup FN_LOW_LVL    Low-Level control functions
 * The functions in this section provide a low access to device such as modifying
 * device internal register or clock frequency. Low-Level functions can be used 
 * to configure device entirely, however a more practical use is to fine-tune
 * device settings after configuring it with /ref FN_HIGH_LVL.    
 * @{
 */

/**
 * Send Reset signal to LMS chip. This initializes LMS chip with default
 * configuration as described in LMS chip datasheet.
 *
 * @param device  Device handle previously obtained by LMS_Open().
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Reset(lms_device_t *device);

/**
 * Read device LMS chip register
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param address   Register address
 * @param val       Current register value
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_ReadLMSReg(lms_device_t *device, uint32_t address,
                                        uint16_t *val);

/**
 * Write device LMS chip register
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param address   Register address
 * @param val       Value to write
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_WriteLMSReg(lms_device_t *device, uint32_t address,
                                        uint16_t val);

/**
 * Read device parameter. Parameter defines specific bits in device register.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param param     Parameter.
 * @param val       Current parameter value.
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_ReadParam(lms_device_t *device,
                                     struct LMS7Parameter param, uint16_t *val);

/**
 * Write device parameter. Parameter defines specific bits in device register.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param param     Parameter.
 * @param val       Parameter value to write
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_WriteParam(lms_device_t *device,
                                      struct LMS7Parameter param, uint16_t val);

/**
 * Read device FPGA register
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param address   Register address
 * @param val       Current register value
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_ReadFPGAReg(lms_device_t *device, uint32_t address,
                                        uint16_t *val);

/**
 * Write device FPGA register
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param address   Register address
 * @param val       Value to write
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_WriteFPGAReg(lms_device_t *device, uint32_t address,
                                        uint16_t val);

/**
 * @defgroup BOARD_PARAM  Board parameter
 *
 * @{
 */
///Runtime VCTCXO DAC trim value. Does not persist over power-cycle
#define BOARD_PARAM_DAC     0
///The value of board temperature sensor (if present), read-only.
#define BOARD_PARAM_TEMP    1  
/** @} (End BOARD_PARAM) */

/**
 * Read custom parameter from board
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param id        Parameter identifier (\ref BOARD_PARAM)
 * @param val       Current register value
 * @param units     [optional] measurement units of parameter if available
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_ReadCustomBoardParam(lms_device_t *device,
                                 uint8_t id, float_type *val, lms_name_t units);

/**
 * Write custom parameter from board
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param id        Parameter identifier (\ref BOARD_PARAM)
 * @param val       Value to write
 * @param units     [optional] measurement units of parameter if available
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_WriteCustomBoardParam(lms_device_t *device,
                            uint8_t id, float_type val, const lms_name_t units);

/**
 * @defgroup LMS_CLOCK_ID   Clock definitions
 *
 * Clock definitions for accessing specific internal clocks
 * @{
 */
///Chip reference clock
#define LMS_CLOCK_REF    0x0000  
///RX LO clock
#define LMS_CLOCK_SXR    0x0001  
///TX LO clock
#define LMS_CLOCK_SXT    0x0002 
///CGEN clock
#define LMS_CLOCK_CGEN   0x0003  
///RXTSP reference clock (read-only)
#define LMS_CLOCK_RXTSP  0x0004 
///TXTSP reference clock (read-only)
#define LMS_CLOCK_TXTSP  0x0005 
/** 
 * @brief External reference clock (write-only)
 * 
 * Set to positive value to enable usage of external reference clock of the
 * specified frequency. Set to 0 or negative value to disable usage of external
 * reference clock (if switching reference clock source is supported by HW)
 */ 
#define LMS_CLOCK_EXTREF 0x0006  

/** @} (End LMS_CLOCK_ID) */

/**
 * Get frequency of the specified clock.
 *
 * @param   dev     Device handle previously obtained by LMS_Open().
 * @param   clk_id  Clock identifier (\ref LMS_CLOCK_ID)
 * @param   freq    Clock frequency in Hz
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetClockFreq(lms_device_t *dev, size_t clk_id,
                                         float_type *freq);

/**
 * Set frequency of the specified clock
 *
 * @param   dev     Device handle previously obtained by LMS_Open().
 * @param   clk_id  Clock identifier (\ref LMS_CLOCK_ID)
 * @param   freq    Clock frequency in Hz. Pass zero or negative value to only
 *                  perform tune (if supported) without recalculating values
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetClockFreq(lms_device_t *dev, size_t clk_id,
                                         float_type freq);

/**
 * Write value to VCTCXO trim DAC. Used to adjust/calibrate reference clock
 * generated by voltage controlled oscillator. Value is written to non-volatile
 * storage.
 * @note calling this functions switches clock source to VCTCXO
 *
 * @param   dev         Device handle previously obtained by LMS_Open().
 * @param   val         Value to write to VCTCXO trim DAC 
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_VCTCXOWrite(lms_device_t * dev, uint16_t val);

/**
 * Read VCTCXO trim DAC value from non-volatile storage. Returned value is value
 * that is loaded on power-on and may different from current runtime value. 
 *
 * @param[in]   dev     Device handle previously obtained by LMS_Open().
 * @param[out]  val     VCTCXO trim DAC value
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_VCTCXORead(lms_device_t * dev, uint16_t *val);

/**
 * Synchronizes register values between API cache and chip
 *
 * @param   dev         Device handle previously obtained by LMS_Open().
 * @param   toChip      if true copies values from API cache to chip.
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Synchronize(lms_device_t *dev, bool toChip);

/**
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param[in]   buffer  read values (8 GPIO values per byte, LSB first)
 * @param       len     number of bytes to read
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GPIORead(lms_device_t *dev, uint8_t* buffer, size_t len);

/**
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param[out]  buffer  values to write (8 GPIO values per byte, LSB first)
 * @param       len     number of bytes to write
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GPIOWrite(lms_device_t *dev, const uint8_t* buffer, size_t len);

/**
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param[out]  buffer  GPIO direction configuration(8 GPIO per byte, LSB first; 0 input, 1 output)
 * @param       len     number of bytes to read
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GPIODirRead(lms_device_t *dev, uint8_t* buffer, size_t len);

/**
 * @param       dev     Device handle previously obtained by LMS_Open().
 * @param[in]   buffer  GPIO direction configuration(8 GPIO per byte, LSB first; 0 input, 1 output)
 * @param       len     number of bytes to write
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GPIODirWrite(lms_device_t *dev, const uint8_t* buffer, size_t len);

/** @} (End FN_LOW_LVL) */

/**
 * @defgroup FN_STREAM    Sample Streaming functions
 * The functions in this section provides support for sending and receiving
 * IQ data samples.
 * @{
 */

/**Metadata structure used in sample transfers*/
typedef struct
{
    /**
     * Timestamp is a value of HW counter with a tick based on sample rate.
     * In RX: time when the first sample in the returned buffer was received
     * In TX: time when the first sample in the submitted buffer should be send
     */
    uint64_t timestamp;

    /**In TX: wait for the specified HW timestamp before broadcasting data over
     * the air
     * In RX: not used/ignored
     */
    bool waitForTimestamp;

    /**In TX: send samples to HW even if packet is not completely filled (end TX burst).
     * in RX: not used/ignored
     */
    bool flushPartialPacket;

}lms_stream_meta_t;

/**Stream structure*/
typedef struct
{
    /**
     * Stream handle. Should not be modified manually.
     * Assigned by LMS_SetupStream().*/
    size_t handle;

    //! Indicates whether stream is TX (true) or RX (false)
    bool isTx;

    //! Channel number. Starts at 0.
    uint32_t channel;

    //! FIFO size (in samples) used by stream.
    uint32_t fifoSize;

    /**
     * Parameter for controlling configuration bias toward low latency or high
     * data throughput range [0,1.0].
     * 0 - lowest latency, usually results in lower throughput
     * 1 - higher throughput, usually results in higher latency
     */
    float throughputVsLatency;

    //! Data output format
    enum
    {
        LMS_FMT_F32=0,    ///<32-bit floating point
        LMS_FMT_I16,      ///<16-bit integers
        LMS_FMT_I12       ///<12-bit integers stored in 16-bit variables
    }dataFmt;
}lms_stream_t;

/**Streaming status structure*/
typedef struct
{
    ///Indicates whether the stream is currently active
    bool active;
    ///Number of samples in FIFO buffer
    uint32_t fifoFilledCount;
    ///Size of FIFO buffer
    uint32_t fifoSize;
    ///FIFO underrun count
    uint32_t underrun;
    ///FIFO overrun count
    uint32_t overrun;
    ///Number of dropped packets by HW
    uint32_t droppedPackets;
    ///Currently not used
    float_type sampleRate;
    ///Combined data rate of all stream of the same direction (TX or RX)
    float_type linkRate;
    ///Current HW timestamp
    uint64_t timestamp;

} lms_stream_status_t;

/**
 * Create new stream based on parameters passed in configuration structure.
 * The structure is initialized with stream handle.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param stream    Stream configuration .See the ::lms_stream_t description.
 *
 * @return      0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SetupStream(lms_device_t *device, lms_stream_t *stream);

/**
 * Deallocate memory used by stream.
 *
 * @param dev       Device handle previously obtained by LMS_Open().
 * @param stream    Stream structure previously initialized with LMS_SetupStream().
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_DestroyStream(lms_device_t *dev, lms_stream_t *stream);

/**
 * Start stream
 *
 * @param stream Stream structure previously initialized with LMS_SetupStream().
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_StartStream(lms_stream_t *stream);

/**
 * Stop stream
 *
 * @param stream Stream structure previously initialized with LMS_SetupStream().
 *
 * @return 0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_StopStream(lms_stream_t *stream);

/**
 * Read samples from the FIFO of the specified stream.
 * Sample buffer must be big enough to hold requested number of samples.
 *
 * @param stream        structure previously initialized with LMS_SetupStream().
 * @param samples       sample buffer.
 * @param sample_count  Number of samples to read
 * @param meta          Metadata. See the ::lms_stream_meta_t description.
 * @param timeout_ms    how long to wait for data before timing out.
 *
 * @return number of samples received on success, (-1) on failure
 */
 API_EXPORT int CALL_CONV LMS_RecvStream(lms_stream_t *stream, void *samples,
             size_t sample_count, lms_stream_meta_t *meta, unsigned timeout_ms);

/**
 * Get stream operation status
 *
 * @param stream    structure previously initialized with LMS_SetupStream().
 * @param status    Stream status. See the ::lms_stream_status_t for description
 *
 * @return  0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetStreamStatus(lms_stream_t *stream, lms_stream_status_t* status);

/**
 * Write samples to the FIFO of the specified stream.
 *
 * @param stream        structure previously initialized with LMS_SetupStream().
 * @param samples       sample buffer.
 * @param sample_count  Number of samples to write
 * @param meta          Metadata. See the ::lms_stream_meta_t description.
 * @param timeout_ms    how long to wait for data before timing out.
 *
 * @return number of samples send on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_SendStream(lms_stream_t *stream,
                            const void *samples,size_t sample_count,
                            const lms_stream_meta_t *meta, unsigned timeout_ms);

/**
 * Uploads waveform to on board memory for later use
 * @param device        Device handle previously obtained by LMS_Open().
 * @param samples       multiple channel samples data
 * @param chCount       number of waveform channels
 * @param sample_count  number of samples in each channel
 * @param format        waveform data format
 * @return              0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_UploadWFM(lms_device_t *device, const void **samples,
                                uint8_t chCount, size_t sample_count, int format);

/**
 * Enables/Disables transmitting of uploaded waveform
 * @param device    Device handle previously obtained by LMS_Open().
 * @param chan      Channel index
 * @param active    Enable/Disable waveform playback
 * @return          0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_EnableTxWFM(lms_device_t *device, unsigned chan, bool active);

/** @} (End FN_STREAM) */

/**
 * @defgroup FN_VERSION   Version and update functions
 *
 * The functions in this section provides ability to check device version
 * and perform updates
 * @{
 */

/**
 * Get the list of supported programming modes.
 * 
 * @param device        Device handle previously obtained by LMS_Open().
 * @param[out]  list    list of programming modes (can be NULL).
 * 
 * @return      number of modes in the list, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_GetProgramModes(lms_device_t *device, lms_name_t *list);

/**
 * Callback from programming processes
 * @param bsent number of bytes transferred
 * @param btotal total number of bytes to send
 * @param progressMsg string describing current progress state
 * @return 0-continue programming, 1-abort operation
 */
typedef bool (*lms_prog_callback_t)(int bsent, int btotal, const char* progressMsg);

/**
 * Write binary firmware/bitsteam image to specified device component.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @param data      Pointer to memory containing firmware/bitsteam image
 * @param size      Size of firmware/bitsteam image in bytes.
 * @param mode      programming mode, use LMS_GetProgramModes to get list of modes 
 * @param callback  callback function for monitoring progress
 *
 * @return          0 on success, (-1) on failure
 */
API_EXPORT int CALL_CONV LMS_Program(lms_device_t *device, const char *data, 
                size_t size, const lms_name_t mode, lms_prog_callback_t callback);

/**Device information structure*/
typedef struct
{
    char deviceName[32];            ///<The display name of the device
    char expansionName[32];         ///<The display name of the expansion card
    char firmwareVersion[16];       ///<The firmware version as a string
    char hardwareVersion[16];       ///<The hardware version as a string
    char protocolVersion[16];       ///<The protocol version as a string
    uint64_t boardSerialNumber;     ///<A unique board serial number
    char gatewareVersion[16];       ///<Gateware version as a string
    char gatewareTargetBoard[32];   ///<Which board should use this gateware
}lms_dev_info_t;

/**
 * Get device serial number and version information
 *
 * @note This function returns pointer to internal data structure that gets
 * deallocated when device is closed. Do not attempt to read from it after
 * closing the device. If you need to keep using device info returned by this
 * function after closing the device, make a copy before closing the device.
 *
 * @param device    Device handle previously obtained by LMS_Open().
 * @return          pointer to device info structure ::lms_dev_info_t
 */
API_EXPORT const lms_dev_info_t* CALL_CONV LMS_GetDeviceInfo(lms_device_t *device);

/**
* @brief Returns API library version
*/
API_EXPORT const char* LMS_GetLibraryVersion();

/**
 * Get the error message detailing why the last error occurred.
 * 
 * @deprecated use LMS_RegisterLogHandler() to obtain error messages
 *
 * @return last error message.
 */
API_EXPORT const char * CALL_CONV LMS_GetLastErrorMessage(void);

/**
 * @defgroup LMS_LOG_LEVEL  Message logging level
 *
 * @{
 */
///An error message . An operation did not complete successfully.
#define LMS_LOG_ERROR    1  
///A warning message. An operation completed with an unexpected result.
#define LMS_LOG_WARNING  2  
///An informational message, usually denoting the successful completion of an operation
#define LMS_LOG_INFO     3  
 ///A debugging message.
#define LMS_LOG_DEBUG    4 
/** @} (End LMS_LOG_LEVEL) */

/**
 * Callback function for redirecting API messages
 * 
 * @param lvl   \ref LMS_LOG_LEVEL.
 * @param msg   string containing log message text.
 */
 typedef void (*LMS_LogHandler)(int lvl, const char *msg);
 
/*!
 * Register a new system log handler. Should be called to replace the default
 * stdio handler.
 * 
 * @param handler   function for handling API messages
 */
API_EXPORT void LMS_RegisterLogHandler(LMS_LogHandler handler);

/** @} (End FN_VERSION) */

#ifdef __cplusplus
} //extern "C"
#endif

#endif //LMS_SDR_INTERFACE_H
