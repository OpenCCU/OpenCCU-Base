/**
 * device.js
 **/
 
/**
 * HomeMatic Gerät.
 **/
Device = Class.create({

  /**
   * Konstruktor
   **/
  initialize: function(data)
  {
    this.m_updateChannelHandler = this.m_onUpdateChannel.bind(this);
    
    this.update(data);
  },
  
  /**
   * Aktualisiert die Gerätedaten
   **/
  update: function(data)
  {
    if (data)
    {
      var deviceType = DeviceTypeList.getDeviceType(data["type"]);
    
      this.id  = data["id"];
      this.name = data["name"];
      this.address = data["address"];

      // For debugging only
      this.rfAddress = "";
      if (showRFAddress) {
        var self = this;
        homematic("Device.getRFAddressByAddress", {"address": this.address}, function(result) {
          self.rfAddress = "<br/><br/>0x" + parseInt(result.split(":")[1]).toString(16);
        });
      }
      // End debugging


      this.interfaceName = data["interface"];
      this.isReadyConfig = data["isReady"];
      this.deviceType = deviceType;
      this.typeName = deviceType.name;
      this.typeDescription = deviceType.description;
      this.isDeletable = deviceType.isDeletable();
      this.isOperateGroupOnly = (data["operateGroupOnly"] == "true") ? true : false;
      this.deviceInputCheck = false;

      if (this.typeName.indexOf("-WGS") != -1) {
        this.setPrevPic("HmIP-WGS");
      }
      this.thumbnailHTML = deviceType.getThumbnailHTML();
      this.imageHTML = deviceType.getImageHTML();

      this.channels = new Array();
      this.groups = new Array();
      this.singles = new Array();
      data["channels"].each(function(data) {
        var channel = DeviceList.updateChannel(this, data);
        channel.updateEvent.add(this.m_updateChannelHandler);
        this.channels.push(channel);
      
        if (channel.partnerId == Channel.INVALID_ID)
        {
          this.singles.push(channel);
        }
        else
        {
          var group = DeviceList.updateChannelGroup(this, channel);
          this.groups.ex_pushUnique(group);
        }
      }, this);
    }
    
    this.isWritable  = false;
    this.isVisible   = false;
    this.isUsable    = false;
    this.isLogged    = false;
    this.isLogable   = false;
    this.categories  = [];
    this.modes       = [];
    this.rooms       = [];
    this.subsections = [];
    
    this.channels.each(function(channel) {
      this.isWritable |= channel.isWritable;
      this.isVisible  |= channel.isVisible;
      this.isUsable   |= channel.isUsable;
      this.isLogged   |= channel.isLogged;
      this.isLogable  |= channel.isLogable; 
      this.categories.push(channel.category);
      this.modes.push(channel.mode);
      this.rooms       = this.rooms.concat(channel.rooms);
      this.subsections = this.subsections.concat(channel.subsections);
    }, this);
    
    this.categories  = this.categories.uniq();
    this.modes       = this.modes.uniq();
    this.rooms       = this.rooms.uniq().ex_sortBy("name");
    this.subsections = this.subsections.uniq().ex_sortBy("name");
  },

  /**
   * Ändert das Default Vorschaubild eines Gerätes
   * @param pic
   */
  setPrevPic: function(dev) {
    if (dev == "HmIP-WGS") {
      var chnDescription = homematic("Interface.getParamset", {
        "interface": "HmIP-RF",
        "address": this.address + ":0",
        "paramsetKey": "MASTER"
      });
      var mode = parseInt(chnDescription["DEVICE_INPUT_LAYOUT_MODE"]);

      if (mode == 0) {
        DEV_PATHS[this.typeName][50] = "/config/img/devices/50/239_hmip-wgs-f_thumb_0.png";
        DEV_PATHS[this.typeName][250] = "/config/img/devices/250/239_hmip-wgs-f_0.png";
      } else if (mode == 1) {
        DEV_PATHS[this.typeName][50] = "/config/img/devices/50/239_hmip-wgs-f_thumb_v-2.png";
        DEV_PATHS[this.typeName][250] = "/config/img/devices/250/239_hmip-wgs-f_v-2.png";
      } else if (mode == 2) {
        DEV_PATHS[this.typeName][50] = "/config/img/devices/50/239_hmip-wgs-f_thumb_h-2.png";
        DEV_PATHS[this.typeName][250] = "/config/img/devices/250/239_hmip-wgs-f_h-2.png";
      } else if (mode == 3) {
        DEV_PATHS[this.typeName][50] = "/config/img/devices/50/239_hmip-wgs-f_thumb_1-4.png";
        DEV_PATHS[this.typeName][250] = "/config/img/devices/250/239_hmip-wgs-f_1-4.png";
      }
    }
  },

  /**
   * Legt den Namen des Geräts fest.
   **/
  setName: function(name)
  {
    if (name != this.name)
    {
      var result = homematic("Device.setName", {id: this.id, name: name});
      if (typeof(result) == "string")
      {
        this.name = result;
      }
    }
    
    return this.name;
  },

  setUsable: function(isUsable)
  {
  },
  
  setVisible: function(isVisible)
  {
  },
  
  setLogging: function(isLogged)
  {
  },  

  setDeviceInputCheck: function() {
    this.deviceInputCheck = true;
  },

  getDeviceInputCheck: function() {
    return this.deviceInputCheck;
  },

  m_onUpdateChannel: function(channel, eventArgs)
  {
    this.update();
  },
  
  addSubsection: function(subsection)
  {
    if (!this.subsections.ex_contains(subsection))
    {
      this.subsections.push(subsection);
      this.subsections.ex_sortBy("name");
    }
  },
  
  removeSubsection: function(subsection)
  {
    if (this.subsections.ex_contains(subsection))
    {
      this.subsections = this.subsections.without(subsection);
    }
  },

  getName: function()
  {
    return this.name;
  },

  /**
   * Liefert den HTML-Code des Geräte-Vorschau-Bildes
   **/
  getThumbnailHTML: function()
  {
    if (this.thumbnailHTML === null)
    {
      this.thumbnailHTML = this.deviceType.getThumbnailHTML();
    }
    
    return this.thumbnailHTML;
  },
  
  /**
   * Liefert den HTML-Code des Gerätebildes
   **/
  getImageHTML: function()
  {
    if (this.imageHTML === null)
    {
      this.imageHTML = this.deviceType.getImageHTML();
    }
    
    return this.imageHTML;
  },
  
  /**
   * Liefert die Ids der Programme, die mindestens einen Kanal des Geräts verwenden
   **/
  listProgramIds: function()
  {
    return homematic("Device.listProgramIds", {id: this.id});
  },
  
  /**
   * Ermittelt, ob das Gerät direkte Verknüpfungen oder Programme besitzt.
   **/
  hasLinksOrPrograms: function(callback)
  {
    if (this.interfaceName != "HmIP-RF") {
      homematic("Device.hasLinksOrPrograms", {id: this.id}, callback);
    } else {
      var self = this;
      var result = false;

      var arLinkPeers = homematic("Interface.getLinkPeers", {'interface': this.interfaceName, 'address': this.address});
      jQuery.each(arLinkPeers, function(index,val) {
        var linkPeerAddress = val.split(":")[0];
        if (self.address != linkPeerAddress) {
          result = true;
          return; // leave the each loop
        }
      });

      // No links available, check if the device is used within programs
      if (!result) {
        result = homematic("Device.hasPrograms", {'id': this.id});
      }

      if (callback) {
        callback(result);
      }
    }
  },
  
  /**
   * Asynchron. Versucht ein Gerät zu löschen.
   **/
  remove: function(flags, callback)
  {
    var _this_     = this;
    var _callback_ = callback;
    
    homematic("Interface.deleteDevice", {
      "interface": this.interfaceName,
      address:     this.address,
      flags:       flags
    }, function(result, error) {
      if (result === true)
      {
        // DeviceList.removeDevice(_this_);
      }
      if (_callback_) { _callback_(result, error); }
    });
  }
  
});
