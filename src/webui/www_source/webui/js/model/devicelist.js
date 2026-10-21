/**
 * devicelist.js
 * Geräteliste.
 **/

/**
 * Geräteliste.
 **/
DeviceList = Singleton.create({

  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.m_addRoomHandler = this.m_onAddRoom.bind(this);
    this.m_removeRoomHandler = this.m_onRemoveRoom.bind(this);
    this.m_addChannelToRoomHandler      = this.m_onAddChannelToRoom.bind(this);
    this.m_removeChannelFromRoomHandler = this.m_onRemoveChannelFromRoom.bind(this);
    
    this.m_addSubsectionHandler = this.m_onAddSubsection.bind(this);
    this.m_removeSubsectionHandler = this.m_onRemoveSubsection.bind(this);
    this.m_addChannelToSubsectionHandler      = this.m_onAddChannelToSubsection.bind(this);
    this.m_removeChannelFromSubsectionHandler = this.m_onRemoveChannelFromSubsection.bind(this);

    this.addEvent    = new eQ3.Event();
    this.removeEvent = new eQ3.Event();
    
    this.devices  = {};
    this.channels = {};
    this.groups   = {};
  },
  
  /**
   * Aktualisiert ein Gerät
   **/
  updateDevice: function(data, callback)
  {
    var device;
    
    if (data !== null)
    {
      var id = data["id"];
      device = this.devices[id];
      data["interface_displayname"] = this.loadIfDisplayName(data);

      if (typeof(device) != "undefined") 
      {
        device.update(data); 
      }
      else
      {
        device =  new Device(data); 
        this.devices[id] = device;
        this.addEvent.fire(this, {device: device});
      }
    }
    
    if (callback) { callback(); }
    return device;
  },
  
  /**
   * Aktualisiert einen Kanal
   **/
  updateChannel: function(device, data)
  {
    var channel;
    
    if (data !== null)
    {
      var id  = data["id"];
      channel = this.channels[id];
      
      if (typeof(channel) != "undefined") 
      { 
        channel.update(device, data); 
      }
      else
      { 
        channel = new Channel(device, data); 
        this.channels[id] = channel; 
      }
    }
    
    return channel;
  },
  
  /**
   * Aktualisiert eine Kanalgruppe
   **/
  updateChannelGroup: function(device, channel)
  {
    var id = channel.groupId;
    var group  = this.groups[id];
      
    if (typeof(group) != "undefined") 
    {
      group.update(device, channel); 
    }
    else
    {
      group = new ChannelGroup(device, channel);
      this.groups[id] = group;
    }
    
    return group;
  },
  
  /**
   * Entfernt ein Gerät aus dem Datenmodell
   **/
  removeDevice: function(device)
  {
    // alert("removeDevice: " + device.id);
  
    delete this.devices[device.id];
    device.groups.each(function(group) { delete this.groups[group.id]; }, this);
    device.channels.each(function(channel) { delete this.channels[channel.id]; }, this);
    
    this.removeEvent.fire(this, {device: device});
  },

  /**
   * Liefert die Liste aller Geräte.
   **/
  listDevices: function()
  {
    return Object.values(this.devices);
  },
  
  /**
   * Liefert die Liste aller Kanalgruppen.
   **/
  listChannelGroups: function()
  {
    return Object.values(this.groups);
  },
  
  /**
   * Liefert die Liste aller Kanäle.
   **/
  listChannels: function()
  {
    return Object.values(this.channels);
  },
  
  /**
   * Liefert die Liste aller Kanäle ohne Gruppen-Partner.
   **/
  listSingleChannels: function()
  {
    var singles = new Array();
    for (var id in m_devices) { singles = singles.concat(m_devices[id].SingleChannels()); }
    return singles;
  },
  
  loadBidCosGateways: function()
  {
    this.BidCosGateways=[];	   
    this.allBidCosDevsList = homematic("Interface.listDevices", {"interface": "BidCos-RF"});
    var rfGateways = homematic("BidCoS_RF.getConfigurationRF");
    if (rfGateways && rfGateways.interfaces) {
      for (var i = 0, len = rfGateways.interfaces.length; i < len; i++) {
        var gateway = rfGateways.interfaces[i];
        if (gateway.type === globalLGWTypes.HMLGW2) {
          this.BidCosGateways.push(
           new BidcosRfPage.Gateway()
            .setGatewayClass("RF")
            .setType(gateway.type)
            .setUserName((gateway.userName.length > 0) ? gateway.userName : gateway.serialNumber)
            .setAddress(gateway.serialNumber)
            .setKey(gateway.encryptionKey)
            .setIP(gateway.ipAddress)
          );
        }
      }
      this.BidCosGateways.sort((a, b) => a.m_userName.localeCompare(b.m_userName, undefined, { numeric: true, sensitivity: 'base' }));
      for (var i = 0, len = this.BidCosGateways.length; i < len; i++) {
        DeviceListPage.INTERFACES.push({id:this.BidCosGateways[i].m_userName, name:this.BidCosGateways[i].m_userName});
      }
    }
  },
  
  loadIfDisplayName: function(data) 
  {
    if (typeof this.BidCosGateways === 'undefined') { this.loadBidCosGateways(); }
    var dispname = data["interface"];
    if ( this.allBidCosDevsList && this.BidCosGateways.length > 0 && data["interface"] === "BidCos-RF") {
      var idx = this.allBidCosDevsList.findIndex(({ address }) => address === data["address"]);
      if (idx > -1) {
        var _device= this.allBidCosDevsList[idx];
        var gwIdx = this.BidCosGateways.findIndex(({ m_address }) => m_address === _device["interface"]);
        if ( gwIdx > -1 ) {
          dispname = (this.BidCosGateways[gwIdx].m_userName.length > 0) ?  this.BidCosGateways[gwIdx].m_userName : _device["interface"]; 
        }
      }
    }
    return dispname;
  },
  updateInterfaceDisplayName:function(addr,gwName) {
    var _this_ = this;
    for (var id in _this_.devices) {
      var device = _this_.devices[id];
      if (device.address === addr) {
        device.updateInterfaceDisplayName(gwName);
      }
    }
  },
  /**
   * Lädt die Geräteliste erneut
   **/
  reload: function(loader)
  {
    eQ3.HomeMatic.Event.subscribe("delete", function(type, data) {
      var address = data;
      var device = DeviceList.getDeviceByAddress(address);
      
      if (device) { DeviceList.removeDevice(device); }
    });
    RoomList.addEvent.add(this.m_addRoomHandler);
    RoomList.removeEvent.add(this.m_removeRoomHandler);
    SubsectionList.addEvent.add(this.m_addSubsectionHandler);
    SubsectionList.removeEvent.add(this.m_removeSubsectionHandler);
  
    var _loader_ = loader;
    var _this_   = this;
    var _ids_    = [];
    var current = 0;

    var typeCCU1 = "HM-CCU-1";
    
    var loadNext = function()
    {
      if (current < _ids_.length)
      {
        _this_.beginUpdateDevice(_ids_[current], loadNext);
        current++;
        _loader_.reportLoadingState(current / _ids_.length);
      }
      else
      {
        _loader_.ready();
      }
    };
    
    this.devices  = {};
    this.channels = {};
    this.groups   = {};

    var self=this;
    homematic("Device.listAllDetail", null, function(deviceList) {
      // alert("after listAllDetail devices: " + deviceList);
      jQuery.each(deviceList, function (index, data) {
      // console.dir (data);
        if (data !== null && (data["type"] != typeCCU1))
        {
          var id = data["id"];
          var device = self.devices[id];
          data["interface_displayname"] =  self.loadIfDisplayName(data);

          if (typeof(device) != "undefined")
          {
            device.update(data);
          }
          else
          {
            device =  new Device(data);
            self.devices[id] = device;
            self.addEvent.fire(this, {device: device});
          }
        }
      });
      loader.ready();
    });
/**
    homematic("Device.listAll", null, function(ids) {
      if (ids !== null)
      {
        _ids_ = ids;
        loadNext();
      }
      else
      {
        loader.ready();
      }
    });
  */
  },
  
  /**
   * Startet die Akualisierung eines Geräts.
   **/
  beginUpdateDevice:function(id, callback)
  {
    var _this_ = this;
    var _callback_ = callback;
    
    homematic("Device.get", {"id": id}, function(data) { _this_.updateDevice(data, _callback_); });
  },
  
  /**
   * Liefert ein Gerät anhand seiner Id.
   **/
  getDevice: function(id)
  {
    return this.devices[id];
  },
  
  /**
   * Liefert ein Gerät anhand seiner Seriennummer.
   **/
  getDeviceByAddress: function(address)
  {
    for (var id in this.devices)
    {
      var device = this.devices[id];
      if (device.address == address)
      {
        return device;
      }
    }
    
    return undefined;
  },

  // Returns an array of devices matching typeName - typeName is something like 'HmIP-WGS-A"
  getDevicesByTypeName: function(typeName) {
    var arDevices = [];
    for (var id in this.devices)
    {
      var device = this.devices[id];
      if (device.typeName == typeName)
      {
        arDevices.push(device);
      }
    }
    return arDevices;
  },


  /**
   * Liefert eine Kanalgruppe anhand ihrer Id
   **/
  getChannelGroup: function(id)
  {
    return this.groups[id];
  },
  
  /**
   * Liefert einen Kanal anhand seiner Id.
   **/
  getChannel: function(id)
  {
    return this.channels[id];
  },
  
  /**
   * Liefert einen Kanal anhand seiner Seriennummer.
   **/
  getChannelByAddress: function(address)
  {
    for (var id in this.channels)
    {
      var channel = this.channels[id];
      if (channel.address == address)
      {
        return channel;
      }
    }
    
    return undefined;  
  },
  
  /**
   * Event-Handler. 
   * Wird aufgerufen, sobald ein Raum hinzugefügt wurde.
   **/
  m_onAddRoom: function(roomList, eventArgs)
  {
    var room = eventArgs.room;
    room.addChannelEvent.add(this.m_addChannelToRoomHandler);
    room.removeChannelEvent.add(this.m_removeChannelFromRoomHandler);
    
    room.channelIds.each(function(id) {
      var channel = DeviceList.getChannel(id);
      if (channel) { channel.addToRoom(room); }
    });
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Raum gelöscht wurde.
   **/
  m_onRemoveRoom: function(roomList, eventArgs)
  {
    var room = eventArgs.room;
    room.addChannelEvent.add(this.m_addChannelToRoomHandler);
    room.removeChannelEvent.add(this.m_removeChannelFromRoomHandler);
    
    room.channelIds.each(function(id) {
      var channel = DeviceList.getChannel(id);
      if (channel) { channel.removeFromRoom(room); }
    });
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Kanal einem Raum hinzugefügr wurde.
   **/
  m_onAddChannelToRoom: function(room, eventArgs)
  {
    var channel = this.channels[eventArgs.channelId];
    if (channel) { channel.addToRoom(room); } 
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Kanal aus einem Raum entfernt wurde.
   **/
  m_onRemoveChannelFromRoom: function(room, eventArgs)
  {
    var channel = this.channels[eventArgs.channelId];
    if (channel) { channel.removeFromRoom(room); }
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein neues Gewerk hinzugefügt wurde.
   **/
  m_onAddSubsection: function(subsectionList, eventArgs)
  {
    var subsection = eventArgs.subsection;
    subsection.addChannelEvent.add(this.m_addChannelToSubsectionHandler);
    subsection.removeChannelEvent.add(this.m_removeChannelFromSubsectionHandler);
    
    subsection.channelIds.each(function(id) {
      var channel = DeviceList.getChannel(id);
      if (channel) { channel.addToSubsection(subsection); }
    });
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Gewerk gelöscht wurde.
   **/
  m_onRemoveSubsection: function(subsectionList, eventArgs)
  {
    var subsection = eventArgs.subsection;
    subsection.addChannelEvent.add(this.m_addChannelToSubsectionHandler);
    subsection.removeChannelEvent.add(this.m_removeChannelFromSubsectionHandler);
    
    subsection.channelIds.each(function(id) {
      var channel = DeviceList.getChannel(id);
      if (channel) { channel.removeFromSubsection(subsection); }
    });  
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Kanal einem Gewerk hinzugefügt wurde.
   **/
  m_onAddChannelToSubsection: function(subsection, eventArgs)
  {
    var channel = this.channels[eventArgs.channelId];
    if (channel) { channel.addToSubsection(subsection); } 
  },
  
  /**
   * Event-Handler.
   * Wird aufgerufen, sobald ein Kanal aus einem Gewerk entfernt wurde.
   **/
  m_onRemoveChannelFromSubsection: function(subsection, eventArgs)
  {
    var channel = this.channels[eventArgs.channelId];
    if (channel) { channel.removeFromSubsection(subsection); }
  }

});
