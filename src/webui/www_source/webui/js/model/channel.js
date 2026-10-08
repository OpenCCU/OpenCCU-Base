/**
 * channel.js
 **/

/**
 * Kanal
 **/
Channel = Class.create({
  /**
   * Konstruktor
   **/

  initialize: function(device, data)
  {
    var self = this;
    var devAddress = data["address"].split(":")[0],
    devMode, arBlindFW;

    // Set the master parameter UNIVERSAL_LIGHT_MAX_CAPABILITIES of the HmIP-DRG-DALI Gateway as meta data
    // Channels 1 - 32 = connected dali devices. Here we need only set the meta data for those channels where a device is connected.
    // The first channel with no connected device has the UNIVERSAL_LIGHT_MAX_CAPABILITIES set to 5. All following channels don't have a device connected, so we can skip the rest.
    //
    // For the group channels 33 - 48 we need all UNIVERSAL_LIGHT_MAX_CAPABILITIES.
    if ((device.typeName == "HmIP-DRG-DALI")) {
      if (data.channelType == "UNIVERSAL_LIGHT_RECEIVER") {
        var maxCap = data.daliMaxCapabilities;
        if ((typeof maxCap == "undefined") || (maxCap == "undefined")) {
          var maxCap = homematic("Interface.getMasterValue", {
            "interface": "HmIP-RF",
            "address": data.address,
            "valueKey": "UNIVERSAL_LIGHT_MAX_CAPABILITIES"
          });
          homematic("Interface.setMetadata", {
            "objectId": data.id, "dataId": "maxCap", "value": maxCap
          });
        }
        this.daliMaxCapabilities = maxCap; // currently used for determining the target channels of the weekly program.
      }
    }

    if (data.channelType == "CLIMATECONTROL_FLOOR_TRANSCEIVER") {
      homematic("Interface.getMetadata", {
        "interface" : "HmIP-RF",
        "objectId" : data["id"],
        "dataId" :  "chnActive"
      }, function(result) {
        self.isActive = (result == "true") ? true : false;
        //homematic('Interface.suppressServiceMessages', {'interface':'HmIP-RF','channelAddress': data.address, 'parameterId': '', 'suppress': !self.isActive});
      });
    }

    this.updateEvent = new eQ3.Event();

    var chType = data["channelType"];

    // This adds the attribute virtChCounter to the virtual channels
    if (chType.indexOf("_VIRTUAL_RECEIVER") != -1) {
      if (typeof virtChCounter == "undefined") {
        virtChCounter = 1;
        this.virtChCounter = virtChCounter;
      } else {
        if (virtChCounter < 3) {
          virtChCounter++;
          this.virtChCounter = virtChCounter;
        } else {
          delete virtChCounter;
        }
      }
    } else {
      delete virtChCounter;
    }

    if ((device.deviceType.description.indexOf("HmIPW-DRBL4") != -1)
      || (device.deviceType.description.indexOf("HmIP-DRBLI4") != -1)
      || (device.deviceType.description.indexOf("HmIP-BBL") != -1)
      || (device.deviceType.description.indexOf("HmIP-BBL-2") != -1)
      || (device.deviceType.description.indexOf("HmIP-BBL-I") != -1)
      || (device.deviceType.description.indexOf("HmIP-FBL") != -1)

    ) {
      if (chType.indexOf("BLIND_TRANSMITTER") != -1 || chType.indexOf("BLIND_VIRTUAL_RECEIVER") != -1) {
        if (typeof devToConfigure != "undefined" || typeof blindChAddress == "undefined" || blindChAddress != devAddress) {
          blindChAddress = devAddress;
          var devDescr = homematic("Interface.getDeviceDescription", {"interface": "HmIP-RF", "address":devAddress});
          if (devDescr) {
            blindFw = devDescr.firmware;
            arBlindFW = blindFw.split(".");
            fwGTE16 = (arBlindFW[0] > 1 || ((arBlindFW[0] = 1) && (arBlindFW[1] >= 6)));
            window.setTimeout(function() {delete blindChAddress; delete blindFw; delete fwGTE16;},10000);
          }
        }


        if (typeof fwGTE16 != "undefined" && fwGTE16) {
          homematic("Interface.getMetadata_crRFD", {"interface": "HmIP-RF", "objectId": data["address"], "dataId": "channelMode"}, function(result) {
            if (result == "") {
              self.changedMultiMode = "shutter";
              data["channelType"] = data["channelType"].replace("BLIND", "SHUTTER");

              homematic("Interface.setMetadata_crRFD", {
                "interface" : "HmIP-RF",
                "objectId" : data["address"],
                "dataId" :  "channelMode",
                "value" : self.changedMultiMode
              });

              homematic("Interface.setMetadata", {
                "objectId" : data["address"],
                "dataId" : "channelMode",
                "value" : self.changedMultiMode
              });

            } else {
              self.changedMultiMode = result;
              if (result == "shutter") {
                data["channelType"] = data["channelType"].replace("BLIND", "SHUTTER");
              }
            }
            self.update(device, data);
          });
        } else {
          // Fw. < 1.6 - The default channel type is always BLIND
          // Check if meta data already available. If not create them.
          homematic("Interface.getMetadata_crRFD", {"interface": "HmIP-RF", "objectId": data["address"], "dataId": "channelMode"}, function(result) {
            if (result == "") {
              devMode = "blind";

              homematic("Interface.setMetadata_crRFD", {
               "interface" : "HmIP-RF",
               "objectId" : data["address"],
               "dataId" :  "channelMode",
               "value" : devMode
              });

              homematic("Interface.setMetadata", {
               "objectId" : data["address"],
               "dataId" : "channelMode",
               "value" : devMode
              });
            }
          });
        }
      }
    } else {
      this.changedMultiMode = "";
    }

    if ((device.deviceType.description.includes("HmIP-WGT")) && (chType == "SWITCH_VIRTUAL_RECEIVER")) {
       homematic("Interface.getMetadata", {
        "interface" : "HmIP-RF",
        "objectId" : data["id"],
        "dataId" :  "channelMode"
      }, function(result) {
        self.channelMode = result;
      });
    }

    window.setTimeout(function() {delete virtChCounter;},15000); // Fallback to ensure this global var is being deleted after not in use anymore.

    this.update(device, data);
  },

  /**
   * Aktualisiert die Kanaldaten
   **/
  update: function(device, data)
  {
    var deviceType = device.deviceType,
     chnMultiMode;

    if ((device) && (data))
    {
      this.id = data["id"];
      this.name = data["name"];
      this.nameExtention = "";
      this.address = data["address"];       
      this.deviceId = data["deviceId"];  
      this.device = device;
      this.deviceType = deviceType;
      this.virtChannelType = ""; // for HmIPW-DRBLx
      this.typeName = deviceType.name;
      this.typeDescription = deviceType.description;
      this.channelType = data["channelType"];
      this.partnerId = data["partnerId"];  
      this.groupId = (this.id < this.partnerId) ? this.id + "_" + this.partnerId : this.partnerId + "_" +this.id;
      this.index = data["index"];
      this.category = translateKey(Channel.getCategoryName(data["category"]));
      this.mode = translateKey(Channel.getModeName(data["mode"]));
      this.isAesAvailable = data["isAesAvailable"];  
      this.isLogged = data["isLogged"];      
      this.isVisible = data["isVisible"];      
      this.isReadyConfig = data["isReady"];  
      this.isVirtual = data["isVirtual"];
      this.isLogable = data["isLogable"];      
      this.isReadable = data["isReadable"];      
      this.isWritable = data["isWritable"];      
      this.isEventable = data["isEventable"];
      this.isActive = data["isChannelActive"];

      if (typeof data["mode_multi_mode"] != "undefined") {
        this.multiMode = data["mode_multi_mode"];
      } else {
        if (this.changedMultiMode != "") {
          this.multiMode = this.changedMultiMode;
        }
      }

      if ((ConfigData.isPresent) && (this.channelType == "MULTI_MODE_INPUT_TRANSMITTER")) {
        if (! isNaN(this.multiMode)) {
          data.multiMode = this.multiMode;
          homematic("Interface.setMetadata", {"objectId": data.id, "dataId": "channelMode", "value": this.multiMode});
        }
      }

      this.isUsable = (this.isWritable && data["isUsable"]);
      this.thumbnailHTML = deviceType.getThumbnailHTML(this.index);
      this.imageHTML = deviceType.getImageHTML(this.index);
    }

    //this.m_extendName();
    this.extendName();

    this.rooms = this.m_getRooms();
    this.subsections = this.m_getSubsections();
    
    
  },

  extendName: function() {
    var self = this;
    if (ConfigData.isPresent) {
      this.m_extendName();
    } else {
      window.setTimeout(function() {self.extendName();},1000);
    }
  },

  m_extendName: function() {
    var ext = getExtendedDescription({"deviceType":this.typeName,"channelType": this.channelType,"channelID": this.id, "channelAddress": this.address,"channelIndex" : this.index, "isVisible" : this.isVisible, "multiMode" : this.multiMode});
    if (ext.length > 0) {
      this.nameExtention += "<br/>" + ext;
    }
    this.m_setWiredBlind();
  },

  /**
   * Ermittelt alls Räume, in denen der Kanal definiert ist
   **/
  m_getRooms: function()
  {
    var result = [];
    var rooms  = RoomList.list();
    var id     = this.id;
    
    rooms.each(function(room) {
      if (room.contains(id)) { result.push(room); }
    });
    
    return result;
  },
  
  /**
   * Ermittelt alls Gewerke, in denen der Kanal definiert ist
   **/
  m_getSubsections: function()
  {
    var result       = [];
    var subsections  = SubsectionList.list();
    var id           = this.id;
    
    subsections.each(function(subsection) {
      if (subsection.contains(id)) { result.push(subsection); }
    });
    
    return result;
  },

  m_setWiredBlind: function() {
    if (((this.typeDescription.indexOf("HmIPW-") != -1) && (this.channelType.indexOf("BLIND_") != -1))
      || (this.typeDescription.toLocaleLowerCase() == "hmip-drbli4")
      || (this.typeDescription.toLocaleLowerCase() == "hmip-bbl-2")
      ) {

      if (this.channelType.indexOf("BLIND_WEEK_PROFILE") == -1) {
        var curType = "";

        curType = (this.multiMode == "shutter") ? this.channelType.replace("BLIND", "SHUTTER") : this.channelType;

        this.virtChannelType = curType;

        var ext = getExtendedDescription({
          "deviceType": this.typeName,
          "channelType": this.virtChannelType,
          "channelID": this.id,
          "channelAddress": this.address,
          "channelIndex": this.index,
          "isVisible": this.isVisible
        });
        if (ext.length > 0) {
          this.nameExtention = "<br/>" + ext;
        }
      }
    }
  },

  getVirtChannelType : function() {
    return this.virtChannelType;
  },

  changeChannelDescription: function() {
    this.m_setWiredBlind();
  },

  /**
   * Legt den Namen des Kanals fest
   **/
  setName: function(name)
  {
    if (this.name != name)
    {
      var result = homematic("Channel.setName", {id: this.id, name: name});
      if (typeof(result) == "string")
      {
        this.name = result;
        this.updateEvent.fire(this, {reason: "setName", isUsable: this.name}); 
      }
    }
    
    return this.name;
  },
  
  /**
   * Legt fest, ob der Kanal für normale Anwender sichtbar ist
   **/
  setVisibility: function(isVisible)
  {
    if (this.isVisible != isVisible)
    {
      var result = homematic("Channel.setVisibility", {id: this.id, isVisible: isVisible});
      if (typeof(result) == "boolean")
      {
        this.isVisible = result;
        this.updateEvent.fire(this, {reason: "setVisibility", isUsable: this.isVisible}); 
      }
    }
    
    return this.isVisible;
  },
  
  /**
   * Legt fest, ob der Kanal für normale Anwender bedienbar ist
   **/
  setUsability: function(isUsable)
  {
    if (this.isUsable != isUsable)
    {
      var result = homematic("Channel.setUsability", {id: this.id, isUsable: isUsable});
      if (typeof(result) == "boolean")
      {
        this.isUsable = result;
        this.updateEvent.fire(this, {reason: "setUsability", isUsable: this.isUsable}); 
      }
    }
    
    return this.isUsable;
  },
  
  /**
   * Legt fest, ob der Kanal protokolliert wird
   **/
  setLogging: function(isLogged)
  {
    if (this.isLogged != isLogged)
    {
      var result = homematic("Channel.setLogging", {id: this.id, isLogged: isLogged});
      if (typeof(result) == "boolean")
      {
        this.isLogged = result;
        this.updateEvent.fire(this, {reason: "setLogging", isLogged: this.isLogged}); 
      }
    }
    
    return this.isLogged;
  },

  getMultiMode: function(mode) {
    return this.changedMultiMode;
  },

  setMultiMode: function(mode) {
    this.changedMultiMode = mode;
    this.updateEvent.fire(this, {reason: "setMultiMode", changedMultiMode: this.changedMultiMode});
  },

  getChannelMode: function() {
    return this.channelMode;
  },

  setChannelMode: function(mode) {
    this.channelMode = mode;
    this.updateEvent.fire(this, {reason: "setChannelMode", channelMode: this.channelMode});
  },

  /**
   * Legt den Übertragungsmodus des Kanals fest.
   **/
  setMode: function(mode)
  {

    if (this.mode != mode)
    {
      var result = homematic("Channel.setMode", {id: this.id, mode: Channel.getMode(mode)});
      if (typeof(result) == "string")
      {
        this.mode = translateKey(Channel.getModeName(result));
        this.updateEvent.fire(this, {reason: "setMode", mode: this.mode});
      }
    }
    return this.mode;
  },

  getName: function()
  {
    return this.name;
  },

  /**
   * Liefert die Kanalgruppe
   **/
  getChannelGroup: function()
  {
    return DeviceList.getChannelGroup(this.groupId);
  },
  
  addToRoom: function(room)
  {
    if (!this.rooms.ex_contains(room))
    {
      this.rooms.push(room);
      this.rooms.ex_sortBy("name");
      this.updateEvent.fire(this, {reason: "addToRoom", room: room}); 
    }
  },
  
  removeFromRoom: function(room)
  {
    if (this.rooms.ex_contains(room))
    {
      this.rooms = this.rooms.without(room);
      this.rooms.ex_sortBy("name");
      this.updateEvent.fire(this, {reason: "removeFromRoom", room: room}); 
    }
  },
  
  /**
   * Fügt dem Kanal ein Gewerk hinzu.
   * Diese Methode dient lediglich zur Aktualisierung des Datenmodells.
   * Um den Kanal einem Gewerk hinzuzufügen, sollte 
   *   Subsection.addChannel(channel, true)
   * verwendet werden.
   **/
  addToSubsection: function(subsection)
  {
    if (!this.subsections.ex_contains(subsection))
    {
      this.subsections.push(subsection);
      this.subsections.ex_sortBy("name");
      this.updateEvent.fire(this, {reason: "addToSubsection", subsection: subsection}); 
    }
  },
  
  /**
   * Entfernt den Kanal aus einem Gewerk.
   * Diese Methode dient lediglich zur Aktualisierung des Datenmodells.
   * Um den Kanal aus dem Gewerk zu löschen, sollte 
   *   Subsection.removeChannel(channel, true)
   * verwendet werden.
   **/
  removeFromSubsection: function(subsection)
  {
    if (this.subsections.ex_contains(subsection))
    {
      this.subsections = this.subsections.without(subsection);
      this.subsections.ex_sortBy("name");
      this.updateEvent.fire(this, {reason: "removeFroSubsections", subsection: subsection}); 
    }
   },
  
  
  /**
   * Liefert eine Liste sämtlicher Programme (Ids), die den Kanal verwenden
   **/
  listProgramIds: function()
  {
    return homematic("Channel.listProgramIds", {"id": this.id});
  },

  /**
   * Liefert eine Liste sämtlicher Programme (Ids), die den Kanal verwenden
   **/
  hasProgramIds: function()
  {
    return homematic("Channel.hasProgramIds", {"id": this.id});
  },

  /**
   * Ermittelt, ob der Kanal den Funktionstest unterstützt.
   **/
  supportsComTest: function()
  {
    return this.isEventable;
  }
  
});
 
/*##############*/
/*# Konstanten #*/
/*##############*/

Channel.CATEGORY =
{
  NONE:     "lblChannelNotLinkable",
  SENDER:   "lblSender",
  RECEIVER: "lblReceiver"

};

Channel.MODE =
{
  DEFAULT: "lblStandard",
  AES    : "lblSecured"
};

Channel.INVALID_ID = "";   

/*########################*/
/*# Statische Funktionen #*/
/*########################*/

/**
 * Liefert den Übertragungsmodus anhand seines Namens.
 **/
Channel.getMode = function(modeName)
{
  switch (modeName)
  {
    //case translateKey(Channel.MODE.DEFAULT): return "MODE_DEFAULT";
    //case translateKey(Channel.MODE.AES)    : return "MODE_AES";
    case "Standard" : return "MODE_DEFAULT";
    case "Gesichert"    : return "MODE_AES";
    default: return "MODE_DEFAULT";
  }
};

/**
 * Liefert den Namen eines Übertragungs-Modus
 **/
Channel.getModeName = function(mode)
{
  switch (mode)
  {
    case "MODE_DEFAULT":  return Channel.MODE.DEFAULT;
    case "MODE_AES":      return Channel.MODE.AES;
    default:              return Channel.MODE.DEFAULT;
  }
};

/**
 * Liefert den Namen einer Kategorie.
 **/
Channel.getCategoryName = function(category)
{
  switch (category)
  {
    case "CATEGORY_NONE":      return Channel.CATEGORY.NONE; 
    case "CATEGORY_SENDER":    return Channel.CATEGORY.SENDER; 
    case "CATEGORY_RECEIVER":  return Channel.CATEGORY.RECEIVER; 
    default:                   return Channel.CATEGORY.NONE;
  }
};
