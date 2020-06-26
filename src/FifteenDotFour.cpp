#include "FifteenDotFour.h"
#include <software_stack/ti15_4stack/macTask.h>
#include <software_stack/ti15_4stack/mac/rom/rom_jt_154.h>
#include <advanced_config.h>
#include <xdc/runtime/System.h>
#include <software_stack/ti15_4stack/stack_user_api/api_mac/api_mac.h>
#include "ti_154stack_config.h"
//#include <buffer.h>

#define DEFAULT_KEY_SOURCE {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33}

FifteenDotFour *_this;
extern "C" ApiMac_sAddrExt_t ApiMac_extAddr;
static uint8_t defaultChannelMask[APIMAC_154G_CHANNEL_BITMAP_SIZ] = CONFIG_CHANNEL_MASK;

extern "C" {
static ApiMac_callbacks_t Sensor_macCallbacks =
    {
      /*! Associate Indicated callback */
      NULL,
      /*! Associate Confirmation callback */
      FifteenDotFour::associateCnfCB,
      /*! Disassociate Indication callback */
      NULL,
      /*! Disassociate Confirmation callback */
      NULL,
      /*! Beacon Notify Indication callback */
      FifteenDotFour::beaconNotifyIndCb,
      /*! Orphan Indication callback */
      NULL,
      /*! Scan Confirmation callback */
      FifteenDotFour::scanCnfCB,
      /*! Start Confirmation callback */
      NULL,
      /*! Sync Loss Indication callback */
      NULL,
      /*! Poll Confirm callback */
      FifteenDotFour::pollCnfCb,
      /*! Comm Status Indication callback */
      NULL,
      /*! Poll Indication Callback */
      NULL,
      /*! Data Confirmation callback */
      FifteenDotFour::dataCnfCB,
      /*! Data Indication callback */
      FifteenDotFour::dataIndCB,
      /*! Purge Confirm callback */
      NULL,
      /*! WiSUN Async Indication callback */
      NULL,
      /*! WiSUN Async Confirmation callback */
      NULL,
      /*! Unprocessed message callback */
      NULL
    };
}

FifteenDotFour::FifteenDotFour(void)
{
    /*
     * Assign global class ptr to this instance.
     * We need a pointer to the class instance to be able
     * to access the class properties in the C callbacks.
     */
    _this = this;
    /*
     * RX Buffer tracking --
     */
    memset(rx_buffer, 0, RX154_MAX_BUFF_SIZE);
    this->rx_readCurrentIndex = 0;
    this->rx_fillLevel = 0;
}

/*
 * @brief   How many bytes are available in the RX Buffer? Calculated by subtracting
 *          the (head - tail) pointers if the result is positive. If the write pointer
 *          is before the read in the circular buffer, take the length of the first and
 *          last chunk and add them together for total available bytes.
 *
 * @return  Length of bytes occupied in buffer
 */
uint8_t FifteenDotFour::available(void)
{
    int fillBytes = 0;   // head - tail
    if (_this != NULL) {
        if (rx_fillLevel - rx_readCurrentIndex > 0) {
            fillBytes = rx_fillLevel - rx_readCurrentIndex; // [0 1 2 3 4 5 6 7 ... 255]
        } else {                                        //      ^write    ^read
            fillBytes = RX154_MAX_BUFF_SIZE - rx_readCurrentIndex + 1 + rx_fillLevel;
        }
    }
    return fillBytes;
}

/*
 * @brief   If there are bytes available in the read buffer, default extract one
 *          byte and move read pointer over by one byte (one index).
 *
 * @return  Current indexed byte in rx buffer
 */
uint8_t FifteenDotFour::read(void)
{
    int readByte;
    if(available()) {
        readByte = rx_buffer[rx_readCurrentIndex];
        /* Increment read index, mod for overflow */
        rx_readCurrentIndex = (rx_readCurrentIndex + 1) % RX154_MAX_BUFF_SIZE;
        /* Check for buffer vacancy */
        checkBufferFull(); //   write in front of read, bufsize-1 to be stored, no overlap.
        return readByte;
    } else{
        return (uint8_t)-1;
    }
}

/*
 * @breif   A method to check if the head and tail pointers are at that the same
 *          position in the array. Assign this boolean to the private variable
 *          bufferFull to be used to vacancy checking.
 *
 */
void FifteenDotFour::checkBufferFull(void)
{
    bufferFull = (rx_readCurrentIndex == rx_fillLevel) ? true : false;
}


uint8_t FifteenDotFour::read(uint8_t* buf, size_t size)
{
    /* If no bytes available. return immediately */
    if(!available()){
        return 0;
    }
    /* Length of current buffer */
    int fillBytes = 0;
    if (rx_fillLevel - rx_readCurrentIndex > 0) {
        /* Length of occupied buffer  */
        fillBytes = rx_fillLevel - rx_readCurrentIndex;
        /* Does the user want to read more bytes than the length of occupied memory? */
        if (fillBytes > size) {
            fillBytes = size;
        }
        memcpy(buf, &rx_buffer[rx_readCurrentIndex], fillBytes);    // copy to local buffer by reference
        rx_readCurrentIndex = (rx_readCurrentIndex + fillBytes) % RX154_MAX_BUFF_SIZE; // move read pointer mod length
        return fillBytes;
    }
    /* Copy last and first chunks independently, move pointers accordingly */
    int bytesCopyAtEnd = (RX154_MAX_BUFF_SIZE - rx_readCurrentIndex);
    memcpy(buf, &rx_buffer[rx_readCurrentIndex], bytesCopyAtEnd);
    /* Copy the remaining bytes at the front of the circular buffer) */
    int remainingBytes = size - bytesCopyAtEnd;
    memcpy(buf, rx_buffer, remainingBytes);
    /* Move read pointer accordingly to amount of bytes read */
    rx_readCurrentIndex = remainingBytes;       // if 3 bytes were read, read_ptr will now be at index 3
    /* How to handle edge case where read pointer crosses write pointer? */
    return bytesCopyAtEnd + remainingBytes;
}

void FifteenDotFour::flush(void)
{
    memset(rx_buffer, 0, RX154_MAX_BUFF_SIZE);
    rx_fillLevel = 0;
    rx_readCurrentIndex = 0;
}

void FifteenDotFour::begin(bool autoJoin)
{
//    Task_disable();
//    _macTaskId = macTaskInit(macUser0Cfg);
//    Task_enable();
    initializePollClock();
    if(autoJoin) {
        this->revents |= JOIN_EVENT;
    }

    /* Get the Primary IEEE Address */
    memcpy(ApiMac_extAddr, (uint8_t *)(FCFG1_BASE + EXTADDR_OFFSET), (APIMAC_SADDR_EXT_LEN));

    /* Initialize MAC without frequency hopping */
    sem = (Semaphore_Handle)ApiMac_init(macTaskId, false);

    ApiMac_registerCallbacks(&Sensor_macCallbacks);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyCurrentDescriptorId,
                           (uint8_t)CONFIG_PHY_ID);

    ApiMac_mlmeSetReqUint8(ApiMac_attribute_channelPage,
                           (uint8_t)CONFIG_CHANNEL_PAGE);
    /* Set the transmit power */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyTransmitPowerSigned,
                           (uint8_t)CONFIG_TRANSMIT_POWER);
    /* Set Min BE */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_backoffExponent,
                              (uint8_t)CONFIG_MIN_BE);
    /* Set Max BE */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxBackoffExponent,
                              (uint8_t)CONFIG_MAX_BE);
    /* Set MAC MAX CSMA Backoffs */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxCsmaBackoffs,
                              (uint8_t)CONFIG_MAC_MAX_CSMA_BACKOFFS);
    /* Set MAC MAX Frame Retries */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_maxFrameRetries,
                              (uint8_t)CONFIG_MAX_RETRIES);
}

void FifteenDotFour::connect(void)
{
    revents |= JOIN_EVENT;
}

void FifteenDotFour::process(void)
{
    if(revents & JOIN_EVENT) {
        revents &= ~JOIN_EVENT;

        ApiMac_mlmeScanReq_t scanReq;

        /*Turn receiver ON for SCAN */
        ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, true);

        /* set common parameters for all scans */
        memset(&scanReq, 0, sizeof(ApiMac_mlmeScanReq_t));
        /* set scan channels from channel mask*/
        memcpy(scanReq.scanChannels, defaultChannelMask,
               APIMAC_154G_CHANNEL_BITMAP_SIZ);
        scanReq.scanType = ApiMac_scantype_active;
        if(CONFIG_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
        {
            scanReq.scanDuration = CONFIG_SCAN_DURATION;
        } else {
            scanReq.scanDuration = CONFIG_MAC_BEACON_ORDER;
        }

        scanReq.maxResults = 0;/* Expecting beacon notifications */
        scanReq.permitJoining = false;
        scanReq.linkQuality = CONFIG_LINKQUALITY;
        scanReq.percentFilter = CONFIG_PERCENTFILTER;
        scanReq.channelPage = CONFIG_CHANNEL_PAGE;
        scanReq.phyID = CONFIG_PHY_ID;
        /* using no security for scan request command */
        memset(&scanReq.sec, 0, sizeof(ApiMac_sec_t));
        /* send scan Req */
        ApiMac_mlmeScanReq(&scanReq);
    }

    if(revents & ACCOCIATE_EVENT) {
        revents &= ~ACCOCIATE_EVENT;
        ApiMac_mlmeAssociateReq_t assocReq;
        memset(&assocReq, 0, sizeof(ApiMac_mlmeAssociateReq_t));
        assocReq.coordPanId = getPanID();
        assocReq.logicalChannel = getChannel();
        assocReq.coordAddress.addrMode = ApiMac_addrType_short;
        assocReq.coordAddress.addr.shortAddr = getCoordShortAddr();
        assocReq.channelPage = CONFIG_CHANNEL_PAGE;
        assocReq.phyID = CONFIG_PHY_ID;
        assocReq.sec.securityLevel = ApiMac_secLevel_none;
        assocReq.capabilityInformation.allocAddr = true;
        assocReq.capabilityInformation.ffd = false;
        assocReq.capabilityInformation.panCoord = false;
        assocReq.capabilityInformation.rxOnWhenIdle = CONFIG_RX_ON_IDLE;
        ApiMac_mlmeAssociateReq(&assocReq);
    }

    if(revents & IDLE_EVENT) {
        revents &= ~IDLE_EVENT;
        ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, false);
        /* Start polling timer to poll collector for data */
        setPollClock(POLLING_INTERVAL);
    }

    if(revents & POLL_EVENT) {
        revents &= ~POLL_EVENT;
        revents |= IDLE_EVENT;
        ApiMac_mlmePollReq_t pollReq;
        memset(&pollReq, 0, sizeof(ApiMac_mlmePollReq_t));
        pollReq.coordPanId = getPanID();
        pollReq.coordAddress.addrMode = ApiMac_addrType_short;
        pollReq.coordAddress.addr.shortAddr = getCoordShortAddr();
        ApiMac_mlmePollReq(&pollReq);
    }

    /*
     * PollRequest was successful. Collector has data to send;
     * data indication callback
     */
    if(revents & RECV_EVENT) {
        // what to do here?
        revents &= ~RECV_EVENT;
    }

    /*
     * Sending Mesage Event
     */
//    if(revents & SEND_EVENT) {
//        /* changing state */
//        revents &= ~RECV_EVENT
//        memset(&sensor, 0, sizeof(Smsgs_sensorMsg_t));
//
//        /* fill in the config settings -> move these else where
//         * put in some sort of messageInit() method. TODO: extend this to sensor readings
//         * (have to update bitmask for the configSettings like sensor.c)
//         * */
//        configSettings.frameControl |= Smsgs_dataFields_msgStats;
//        configSettings.frameControl |= Smsgs_dataFields_configSettings;
//
//        /* copy to sensors internal structure */
//        _this->sensor.frameControl = _this->configSettings.frameControl;
//        if(sensor.frameControl & Smsgs_dataFields_msgStats)
//        {
//            memcpy(&sensor.msgStats, &Sensor_msgStats,
//                   sizeof(Smsgs_msgStatsField_t));
//        }
//    }


    ApiMac_processIncoming();
}

/*
 * @brief
 *
 * @params a pointer to the poll confirmation struct
 */
void FifteenDotFour::pollCnfCb(ApiMac_mlmePollCnf_t *pData) {
    if(pData->status == ApiMac_status_success)
    {
        _this->revents = RECV_EVENT;
    }
    else if(pData->status == ApiMac_status_noAck)
    {
        _this->revents = IDLE_EVENT;
    }
}
/*
 * MAC API C callbacks - sending data (did it send correctly?
 *
 */
void FifteenDotFour::dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf)
{
    // data sending state ->
//    if (CONFIC_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
//    {
        if(pDataCnf->status == ApiMac_status_noAck)
        {
            //_this->updateDataFailures()
            // how to handle max data failures or not -> switch to a polling state
            _this->revents = POLL_EVENT;
        }
        else if (pDataCnf->status == ApiMac_status_success)
        {
            //-this->resetDataFailures
            _this->revents = IDLE_EVENT;
        }
//    }
}

/*!
 * @brief      MAC Data Indication callback.
 *
 * @param      pDataInd - pointer to the data indication information
 */

void FifteenDotFour::dataIndCB(ApiMac_mcpsDataInd_t *pDataInd)
{
//    Smsgs_cmdIds_t cmdId;
    if(pDataInd != NULL && pDataInd->msdu.p != NULL && pDataInd->msdu.len > 0)
    {
       /* Save command for local use */
//       cmdId = (Smsgs_cmdIds_t)*(pDataInd->msdu.p);
       /* First, flush data in buffer */
       // _this->flush();                          // continue adding to buffer, do not flush
       /* Save signal strength */
       _this->signalStrength = pDataInd->rssi;
       /* Copy data from MAC frame to APP frame */
       memcpy(_this->rx_buffer, pDataInd->msdu.p, pDataInd->msdu.len);
       /* Updating write pointer */
       _this->rx_fillLevel += pDataInd->msdu.len;
    }
//    System_printf("Command ID=", cmdId); // printing to console inside of CCS
    _this->revents = IDLE_EVENT;
    // RSSI -> save this in a private variable for the user.
    // validate frame ?
    // if you get another, we put stuff in buffer 1, now fill buffer 2
    // buffer <queue> a solution, down the line
}

void FifteenDotFour::beaconNotifyIndCb(ApiMac_mlmeBeaconNotifyInd_t *pData)
{
    if(pData->beaconType == ApiMac_beaconType_normal) {
        if(APIMAC_SFS_ASSOCIATION_PERMIT(pData->panDesc.superframeSpec))
        {
            /* Check for beacon order match */
            if(_this->checkBeaconOrder(pData->panDesc.superframeSpec) == true)
            {
                /* Sensor with 0xFFFF as panID can join any network */
                if(0xFFFF == _this->getPanID())
                {
                    _this->setPanID(pData->panDesc.coordPanId);
                    _this->setPanIdMatch(true);
                }

                if (pData->panDesc.coordPanId == _this->getPanID())
                {
                    ApiMac_mlmeSetReqBool(ApiMac_attribute_autoRequest, true);
                    _this->setCoordShortAddr(pData->panDesc.coordAddress.addr.shortAddr);
                    _this->setChannel(pData->panDesc.logicalChannel);
                    _this->setPanIdMatch(true);
                }
            }
        }
    }
 }

void FifteenDotFour::scanCnfCB(ApiMac_mlmeScanCnf_t *pData)
{
    if(pData->status == ApiMac_status_success) {
        if(_this->getPanIdMatch()) {
            /* go to the association state */
            _this->revents |= ACCOCIATE_EVENT;
        }
    }
}

void FifteenDotFour::associateCnfCB(ApiMac_mlmeAssociateCnf_t *pAssocCnf)
{
    _this->revents |= IDLE_EVENT;
    _this->_connected = true;
}

bool FifteenDotFour::checkBeaconOrder(uint16_t superframeSpec)
{
    if(CONFIG_MAC_BEACON_ORDER == JDLLC_BEACON_ORDER_NON_BEACON)
    {
        if(APIMAC_SFS_BEACON_ORDER(superframeSpec) == CONFIG_MAC_BEACON_ORDER)
        {
            return (true);
        }
    }
    else if((APIMAC_SFS_BEACON_ORDER(superframeSpec) <= CONFIG_MAC_BEACON_ORDER))
    {
        return (true);
    }

    return (false);
}

/* Data poll timer related functions */
void FifteenDotFour::initializePollClock(void)
{
    pollClkHandle = Timer_construct(&pollClkStruct,
                                         processPollTimeoutCallback,
                                         POLL_TIMEOUT_VALUE,
                                         0,
                                         false,
                                         (UArg)this);
}

void FifteenDotFour::setPollClock(uint32_t pollTime)
{
    /* Stop the Reading timer */
    if(Timer_isActive(&pollClkStruct) == true)
    {
        Timer_stop(&pollClkStruct);
    }

    /* Setup timer */
    if(pollTime > 0)
    {
        Timer_setTimeout(pollClkHandle, pollTime);
        Timer_start(&pollClkStruct);
    }
}

void FifteenDotFour::processPollTimeoutCallback(UArg instance)
{
    FifteenDotFour *node = static_cast<FifteenDotFour*>((void *)instance);

    /* Wake up the application thread when it waits for clock event */

    node->revents |= POLL_EVENT;
    Semaphore_post(node->sem);
}
