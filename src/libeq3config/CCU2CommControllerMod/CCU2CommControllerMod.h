/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2COMMCONTROLLERMOD_H_
#define _CCU2COMMCONTROLLERMOD_H_

#include <string>
#include <CCU2CoprocessorCommandMod.h>
#include <pthread.h>
#include <cstdint>

namespace HM2Mod {

//Forward declarations (HM2 namespace)
class CCU2PortWrapperMod;
class CCU2SerialFrameMod;

/** \brief This class is responsible for communication with the ccu 2 coprocessor.
* \details There are three types of communication with the ccu 2 coprocessor.
* Firstable coprocessor system commands, secondable coprocessor commands concerning bidcos and
* thirdly bidcos telegrams. Each of them is handled seperately and protected with an own mutex.
* Nevertheless there is only one receive thread to parse and handle incoming messages.
*/
class CCU2CommControllerMod
{
public:
	/** \brief Constructor.*/
	CCU2CommControllerMod();
	/** \brief Destructor.*/
	virtual ~CCU2CommControllerMod();

	/** \brief Initialization of the serial port connection.
	* \details Starts the coprocessor app-mode if needed.
	* \param dev Device file path.[in]
	* \param access Path of access file. [in]
	* \param reset Path of reset file. [in]
	* \coprocessorSerialNr Serial number of coprocessor. [out]
	*/
	//virtual bool init(const std::string& dev, const std::string& access, const std::string& reset, const bool csmacaEnabled);

	/** \brief Checks if the serial port connection is established.
	* \return True if connection over serial port is established.
	*/
	bool isDeviceOpen() const;

	/**\brief Starts the receive thread.
	*\return True if receiver (receive thread) has been started successfully.
	*/
	bool startReceiver();

	/**\brief Stops the receive thread.
	* \return True on success.
	*/
	bool stopReceiver();

	/**\brief Sends bidcos command to coprocessor.
	* \details These commands are only for the coprocessor.
	* \param bidcosCommand BidcosCommand (command type)
	* \param param Command data. Content highly depends on command.
	* \param responseData (Payload)-Data from response. Existence and content depends on command.
	* \return True, if the command sent successfully and a response with status OK came in.
	*/
	bool sendBidcosRequest(const BidcosCommand bidcosCommand, const std::string& param, std::string* responseData = NULL);

	/** \brief Reads serial number from coprocessor.
	 * \return Serial number or an empty string on error.*/
	std::string readSerialNumber();

	/*! Setzt die UTC Zeit f�r das Interface.
	* \param utcSeconds Anzahl Sekunden seit 01.01.1970 00:00 Uhr (UTC)
	* \param offsetMinutes Offset in Minuten entsprechend der Zeitzone.*/
	bool setInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes);
	bool setDataRate100k();
	bool setDataRate10k();
	bool getDutyCycle(int *dutyCycle);
	bool isDualCoprocessor();

	bool startCoprocessorApp(bool wait = true);
	bool startCoprocessorBootloader(bool wait = true);

    /** \brief Sets info LED of HomeMatic RF-Lan Gateway (rfd internal type: HMLGW2)
     * param state 0: off; 1: on; 2: blink slow (1 second); 3: blink fast (500ms)
     */
	virtual bool setRFLGWInfoLED(const unsigned int state);

	void setInterfaceSerial(const std::string& ifSerial);

	/** \brief Sends co-processor system commands.
	* \param systemCommand System command type.
	* \param pResponseValue Response string if the command-response gives one back, or NULL.
	* \return True on success.
	*/
	bool sendSystemCommand(const SystemCommand systemCommand, const std::string& cmdData, std::string* pResponseValue = NULL);
	bool sendHmipCommonCommand(const HmipCommonCommand hmipCommonCommand, const std::string& cmdData, std::string* pResponseValue = NULL, bool shortTimeout = false);
	bool sendHmipTrxAdapterCommand(const HmipTrxAdapterCommand hmipCommonCommand, const std::string& cmdData, std::string* pResponseValue = NULL);
	bool sendLowLevelMacCommand(const LowLevelMacCommand lowLevelMacCommand, std::string* pResponseValue = NULL);
protected:

	/** \brief Internal state used when waiting for a response.*/
	enum ResponseStatus
	{
		RESPONSE_WAIT,//! Waiting for response
		RESPONSE_OK,//! Response successfully received.
		RESPONSE_FAIL,//! No response received.
		RESPONSE_FAIL_BUSY, //!Failure because coprocessor is busy right now.
		RESPONSE_IDLE,//! Not waiting for an response.
		RESPONSE_TIMEOUT//! Response was not received until timeout.
	};

	/** \brief Determines receive thread state.*/
	enum InterfaceState
	{
		IFSTATE_INACTIVE = 0, //reveive thread will terminate
		IFSTATE_INIT = 1, //receive thread will not terminate but bidcos commands won't be handled
		IFSTATE_REINIT = 2,
		IFSTATE_ACTIVE = 3 //receive thread will not terminate and bicods commands will be handled
	};

	enum CoprocessorState {
		COPROCESSOR_STATE_UNDEFINED = 0,
		COPROCESSOR_STATE_BOOTLOADER = 1,
		COPROCESSOR_STATE_APPLICATION = 2,
		COPROCESSOR_STATE_DUAL_COPRO_APP = 3,
	};
	/** \brief Holds information regarding a request.*/
	struct RequestInfo {
		CCU2CoprocessorCommandMod requestCmd;
		CCU2CoprocessorCommandMod responseCmd;
		ResponseStatus responseStatus;
		RequestInfo();
	};


	enum CoprocessorType {
		COPROCESSOR_TYPE_UNDEFINED,
		COPROCESSOR_TYPE_HMIP,
		COPROCESSOR_TYPE_HM
	};

	/** \brief Responsible for communication over serial port.*/
	CCU2PortWrapperMod* pPortWrapper;

	/**\brief Current state of interface. Impact on receive thread.*/
	volatile InterfaceState interfaceState;

	/** \brief Receive thread. Responsible for reading and handling incoming messages.*/
	pthread_t receiveThread;
	/** \brief PThread attributes for 'receive thread'. */
	pthread_attr_t receiveThreadAttributes;

	/** \brief Thread to start the coprocessor application asynchronously.
	* \details Must be done in own thread because it get's triggered by receive thread
	* which habe to receive the answer and must not be blocked. */
	pthread_t startCoprocessorAppThread;
	/** \brief Thread attributes for startCoprocessorAppThread.*/
	pthread_attr_t startCoprocessorAppThreadAttributes;

	/** \brief Mutex for coprocessorState.*/
	pthread_mutex_t mutexCoprocessorState;
	/** \brief Wait condition to wait until corprocessor successfully started the app.*/
	pthread_cond_t waitConditionStartCoprocessorApp;


	/** \brief Mutex for system-type commands to coprocessor. */
	pthread_mutex_t mutexSystemCommandRequest;
	/** \brief Mutex for hmip-common-type commands to coprocessor. */
	pthread_mutex_t mutexHmipCommonCommandRequest;
	/** \brief Mutex for hmip-trx-adapter-type commands to coprocessor. */
	pthread_mutex_t mutexHmipTrxAdapterCommandRequest;
	/** \brief Mutex for bidcos-type request to coprocessor.*/
	pthread_mutex_t mutexBidcosRequest;
	/** \brief Mutex for bidcos- / homemeatic telegrams.*/
	pthread_mutex_t mutexBidcosTelegramRequest;
	/** \brief Mutex for low level mac frames (dual copro).*/
	pthread_mutex_t mutexLowLevelMacRequest;


	/** \brief Wait condition for sytem commands.*/
	pthread_cond_t waitConditionSystemCommandRequest;
	/** \brief Wait condition for hmip common commands.*/
	pthread_cond_t waitConditionHmipCommonCommandRequest;
	/** \brief Wait condition for hmip trx adapter commands.*/
	pthread_cond_t waitConditionHmipTrxAdapterCommandRequest;
	/** \brief Wait condition for bidcos requests.*/
	pthread_cond_t waitConditionBidcosRequest;
	/** \brief Wait condition for bidcos/homematic telegrams. */
	pthread_cond_t waitConditionBidcosTelegramRequest;
	/** \brief Wait condition for low level mac frames (dual copro).*/
	pthread_cond_t waitConditionLowLevelMacRequest;

	/** \brief Request info struct holding system command request-information.*/
	RequestInfo systemCommandRequestInfo;
	/** \brief Request info struct holding bidcos command request-information.*/
	RequestInfo bidcosRequestInfo;
	RequestInfo hmipCommonRequestInfo;
	RequestInfo hmipTrxAdapterRequestInfo;
	/** \brief Request info struct holding bidcos/homematic telegram request-information.*/
	RequestInfo bidcosTelegramRequestInfo;
	/**\brief Request info for low level mac frames to dualcopro*/
	RequestInfo lowLevelMacRequestInfo;

	/* \brief Mutex attributes.*/
	pthread_mutexattr_t mutexAttr;

	/**\brief Constant: Timeout for request.*/
	static const uint32_t defaultResponseTimeout;
	static const uint32_t shortResponseTimeout;

	/**\brief Current coprocessor state (UNDEFINED, BOOTLOADER, APPLICATION).*/
	CoprocessorState coprocessorState;

	/**\brief Target coprocessor state. true =  Either bidcosApp or DualCoproApp. false = bootloader.*/
	bool startApp;
	/**\brief Serial number of the interface, for logging purposes.*/
	std::string interfaceSerial;

	/**
	 * \brief Type of coprocessor. (UNDEFINED/HMIP/HM)
	 */
	CoprocessorType coprocessorType;


	/** \brief Sends co-processor bidcos commands.
	* \param requestCmd Bidcos command type.
	* \param requestInfo Struct that holds information about the request.
	* \param mutex Threads mutex
	* \param waitCondition Threads wait condition
	* \param done True, if request was successful.
	* \return Response co-processor command.
	*/
	CCU2CoprocessorCommandMod sendBidcosRequest(CCU2CoprocessorCommandMod requestCmd, RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition, bool& done);

	/** \brief Waits for a response of given request.
	* \param timeout Maximum wait time.
	* \param requestInfo Struct that holds information about the request.
	* \param mutex Mutex.
	* \param waitCondition Wait condition.
	*/
	void waitForCoProcessorResponse(const int64_t& timeout, RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition);

	/** \brief Function executed by the receive thread.
	* \param params Must be this pointer.
	* \return NULL
	*/
	static void* receiveThreadFunction(void* params);

	/** \brief Handles incoming serial frames.
	* \details This method is called by receiver thread if a serial frame has been received completely.
	* \param serialFrame Serial frame to handle.
	*/
	void handleIncomingSerialFrame(const CCU2SerialFrameMod& serialFrame);

	/** \brief Handles co-processor commands of type event.
	* \details Called by handleIncomingSerialFrame.
	* \param copCmd Coprocessor command.
	*/
	void handleIncomingEvent(const CCU2CoprocessorCommandMod& copCmd);

	/** \brief Handles co-processor commands recognized as response of a request.
	* \details Called by handleIncomingSerialFrame.
	* \param copCmd Co-processor command which was encapsulated in received serial frame.
	* \param serialFrame Received serial frame.
	*/
	void handleIncomingResponse(const CCU2CoprocessorCommandMod& copCmd, const CCU2SerialFrameMod& serialFrame);

	/** \brief Checks if a co-processor command (event) is an additional bidcos-type response, that came in as event.
	* \return True if it's a response.
	*/
	bool isBidcosEventABidcosResponse(const CCU2CoprocessorCommandMod& eventCmd);

	/** \brief Checks if a co-processor command (event) is an additional bidcos/homematic-telegram-type response, that came in as event.
	* \return True if it's a response.
	*/
	bool isBidcosEventABidcosTelegramResponse(const CCU2CoprocessorCommandMod& eventCmd);

	/** \brief Helper method, that safely!!! locks a mutex. */
	void checkedLock(pthread_mutex_t* mutex);

	void handleIdentifyEvent(const CCU2CoprocessorCommandMod& copCmd);
	void updateCoprocessorState(const std::string& identifyString);

	static void* startCoprocessorAppThreadFunction(void* params);

	bool readFirmwareVersion(std::string& bootloaderVersion, std::string& firmwareVersion);

	bool setCSMACAEnabled(const bool enabled);

	bool init(bool skipStartApp = false);

	bool performIdentify();

	
};

}

#endif
