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
      this.interfaceDisplayName = data["interface_displayname"];
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
   * Startet die Aktualisierung der Werte des Gerätestatus
   **/
  updateStatus:function(deviceStatus, rssiListHmRF)
  {
    // catch DUTY_CYCLE or DUTYCYCLE
    if (typeof(deviceStatus.DUTY_CYCLE) !== "undefined")
      this.DUTY_CYCLE = !!+deviceStatus.DUTY_CYCLE;
    if (typeof(deviceStatus.DUTYCYCLE) !== "undefined")
      this.DUTY_CYCLE = !!+deviceStatus.DUTYCYCLE;

    // catch OPERATING_VOLTAGE
    if (typeof(deviceStatus.OPERATING_VOLTAGE) !== "undefined")
      this.OPERATING_VOLTAGE = deviceStatus.OPERATING_VOLTAGE;

    // catch SABOTAGE
    if (typeof(deviceStatus.SABOTAGE) !== "undefined")
      this.SABOTAGE = !!+deviceStatus.SABOTAGE;

    // catch UNREACH
    if (typeof(deviceStatus.UNREACH) !== "undefined")
      this.UNREACH = !!+deviceStatus.UNREACH;

    // catch ERROR_OVERHEAT
    if (typeof(deviceStatus.ERROR_OVERHEAT) !== "undefined")
      this.ERROR_OVERHEAT = !!+deviceStatus.ERROR_OVERHEAT;

    // catch LOWBAT or LOW_BAT
    if (typeof(deviceStatus.LOWBAT) !== "undefined")
      this.LOWBAT = !!+deviceStatus.LOWBAT;
    if (typeof(deviceStatus.LOW_BAT) !== "undefined")
      this.LOWBAT = !!+deviceStatus.LOW_BAT;

    // catch CONFIG_PENDING
    if (typeof(deviceStatus.CONFIG_PENDING) !== "undefined")
      this.CONFIG_PENDING = !!+deviceStatus.CONFIG_PENDING;

    // catch UPDATE_PENDING
    if (typeof(deviceStatus.UPDATE_PENDING) !== "undefined")
      this.UPDATE_PENDING = !!+deviceStatus.UPDATE_PENDING;

    // catch RSSI_DEVICE and RSSI_PEER
    // in case of a BidCos-RF device we take the RSSI
    // values fron the supplied rssiListHmRF
    if (this.interfaceName === 'BidCos-RF')
    {
      if (rssiListHmRF !== null)
      {
        var address = this.address;
        var rssiInfo = rssiListHmRF.find(function(x) { return x.name === address });

        if (rssiInfo !== null && typeof(rssiInfo) !== 'undefined' &&
            rssiInfo.partner !== null && typeof(rssiInfo.partner) !== 'undefined')
        {
          var rssiData = rssiInfo.partner[0].rssiData;
          this.RSSI_DEVICE = rssiData[0];
          this.RSSI_PEER = rssiData[1];
          if(this.RSSI_DEVICE === 65536)
            this.RSSI_DEVICE = -65535;
          if(this.RSSI_PEER === 65536)
            this.RSSI_PEER = -65535;
        }
      }
    }
    else
    {
      if (typeof(deviceStatus.RSSI_DEVICE) !== "undefined")
        this.RSSI_DEVICE = deviceStatus.RSSI_DEVICE;

      if (typeof(deviceStatus.RSSI_PEER) !== "undefined")
        this.RSSI_PEER = deviceStatus.RSSI_PEER;
    }

    // update/calculate the web colors for displaying
    // the RSSI values like in devconfig
    if (this.RSSI_DEVICE !== null || this.RSSI_PEER !== null)
      this.updateRssiColors();

    // now we update the table cell with a pregenerated HTML
    var cell = document.getElementById('DeviceStatus' + this.id);
    if (cell !== null && typeof(cell) !== 'undefined')
    {
      var html = '';

      if (this.enabledServiceMsg === false)
        html += '<span style="background-color: #FFFF00;">NO&nbsp;SERVICEMSG</span><br/>';

      if (this.CONFIG_PENDING === true)
        html += '<span style="background-color: #FFFF00;">CONFIG</span><br/>';

      if (this.UPDATE_PENDING === true)
        html += '<span style="background-color: #FFFF00;">UPDATE</span><br/>';

      if (this.LOWBAT === true)
        html += '<span style="background-color: #FFFF00;">LOWBAT</span><br/>';

      if (this.UNREACH === true)
        html += '<span style="background-color: #FFFF00;">UNREACH</span><br/>';

      if (this.ERROR_OVERHEAT === true)
        html += '<span style="background-color: #FF0000;">OVERHEAT</span><br/>';

      if (this.SABOTAGE === true)
        html += '<span style="background-color: #FF0000;">SABOTAGE</span><br/>';

      if (this.OPERATING_VOLTAGE && parseFloat(this.OPERATING_VOLTAGE) > 0)
        html += parseFloat(this.OPERATING_VOLTAGE).toFixed(1) + '&nbsp;V<br/>';

      if (this.RSSI_DEVICE && this.RSSI_DEVICE > -65535)
        html += this.RSSI_DEVICE + '&nbsp;dBm&nbsp;<span style="background-color: ' + this.rssiDeviceColor + '">&darr;</span><br/>';

      if (this.RSSI_PEER && this.RSSI_PEER > -65535)
        html += this.RSSI_PEER + '&nbsp;dBm&nbsp;<span style="background-color: ' + this.rssiPeerColor + '">&uarr;</span><br/>';

      if (this.DUTY_CYCLE === true)
        html += '<span style="background-color: #FFFF00;">DUTYCYCLE</span><br/>';

      cell.innerHTML = html;
    }
  },

  /**
   * Startet die Aktualisierung der RSSI Werte
   **/
  updateRssiColors:function()
  {
    var getColorCode = function(rssi)
    {
      var RSSI_BAD = -120.0;
      var RSSI_MEDIUM = -100.0;
      var RSSI_GOOD = -20.0;

      var rssi_color = function(lower_bound, upper_bound, rssi)
      {
        var result = 256 * (rssi - lower_bound) / (upper_bound - lower_bound);
        if (result < 0)   result = 0;
        if (result > 255) result = 255;
        return Math.round(result);
      };

      if (rssi > -65535)
      {
        var red = rssi_color(RSSI_GOOD, RSSI_MEDIUM, rssi);
        var green = rssi_color(RSSI_BAD, RSSI_MEDIUM, rssi);
        return "#" + ('00'+(red).toString(16)).substr(-2)
                   + ('00'+(green).toString(16)).substr(-2)
                   + "00";
      }
      else
        return "#000000";
    };

    this.rssiDeviceColor = getColorCode(this.RSSI_DEVICE);
    this.rssiPeerColor = getColorCode(this.RSSI_PEER);
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

  updateInterfaceDisplayName: function(interfaceDisplayName) {
    this.interfaceDisplayName = interfaceDisplayName;
    var devInterfaceDisplayNameCell = document.getElementById('DeviceInterfaceDisplayName' + this.id);
    if (devInterfaceDisplayNameCell !== null && typeof(devInterfaceDisplayNameCell) !== 'undefined') {
      devInterfaceDisplayNameCell.innerHTML = String(interfaceDisplayName).escapeHTML();
    }
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
