#ifndef _HS485COMMMESSAGELGW_H_
#define _HS485COMMMESSAGELGW_H_

#include <HS485CommMessage.h>
#include <HMWLGWCommand.h>
#include <HMWLGWTransportFrame.h>

class HMWLGWTransportFrame;

class HS485CommMessageLGW : public HS485CommMessage
{

	friend class HS485ControllerLGW;

public:

	virtual ~HS485CommMessageLGW();
protected:
	/** \brief Default constructor.*/
	HS485CommMessageLGW();
	HS485CommMessageLGW(HMWLGWTransportFrame& transportFrame);
	HMWLGWCommand hmwlgwCommand;

public:
	virtual inline HS485CommMessage* GetParent(){return (HS485CommMessage*)parent;};
	virtual void SetFrame(const StructuredFrame& frame);
    virtual HS485Frame ExtractFrame();
	virtual inline HS485CommMessage* GetResponse(unsigned int index=0){return (HS485CommMessage*)CommMessage::GetResponse(index);};
	virtual void SetTimeout(int to);
	virtual bool TransformToSimulationMessage();

	virtual bool MatchType(unsigned long type);
	virtual uint32_t GetSenderAddress();
	virtual uint32_t GetReceiverAddress();
	virtual void SetSenderAddress(uint32_t address);
	virtual void SetReceiverAddress(uint32_t address);
	virtual void SetCtrl(int flags);
	virtual int GetCtrl();

	/**\brief Prepares raw data to send.
	* \details Overrides CommMessage::PrepareRawData().
	* \return Raw data ready to send to LGW*/
	virtual std::string PrepareRawData();

	/** \brief Translates and sets command.
	* \details Overrides CommMessageCommand.
	* Translates command types of HS485CommMessageCCU1 to HMWLGW commands.
	* The reason is, that some other clases, like HS485Channel set the command type directly
	* and other command types would break those implementations.
	* \param cmd Command type of HS485CommMessageCCU1.*/
	//virtual void SetCommand(int cmd);

	/** \brief Sets command HMWLGWCommandType.*/
	virtual void SetHMWLGWCommand(const HMWLGWCommandType cmdType);
	/** \brief Sets HMWLGWCommand command data.*/
	virtual void SetHMWLGWCommandData(const std::string& commandData);

	/** \brief Returns the data (user data/payload) of this command.
	 * \return User data / payload / command data.*/
	virtual std::string getCommandData();

	virtual HMWLGWCommandType GetHMWLGWCommand();

protected:
	virtual int MapIndex(int index);
    virtual unsigned long GetType();
	virtual bool ProcessResponse(CommMessage* m, t_state* new_state);
	virtual bool ProcessEEPRomResponse(CommMessage* m, t_state* new_state);
	virtual void SetType(unsigned long type);
	friend class HS485CommMessageDecoder;

};



#endif
