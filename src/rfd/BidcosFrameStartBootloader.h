/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#ifndef BIDCOSFRAMESTRATBOOTLOADER_H_
#define BIDCOSFRAMESTRATBOOTLOADER_H_

#include "BidcosFrame.h"

class BidcosFrameStartBootloader : public BidcosFrame
{
public:
    BidcosFrameStartBootloader();
    virtual ~BidcosFrameStartBootloader();
protected:
    //! Überprüft op der Sysinfoframe mit Seriennummer vom Gerät gesendet wurde
    bool CheckReceiveComplete(uint64_t* wait_time_ms);
};

#endif /* BIDCOSFRAMESTRATBOOTLOADER_H_ */
