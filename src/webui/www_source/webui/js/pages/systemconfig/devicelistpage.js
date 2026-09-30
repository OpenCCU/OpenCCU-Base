/**
 * devielistpage.js
 **/

/**
 * Geräteliste.
 **/

if (PLATFORM == "Central") {

 DeviceListPage = Singleton.create(Page, {
  MAINMENU_ID: "MAINMENU_OPTIONS",
  TREE_COLLAPSED_FOOTER_HTML: "" +
    "<table border='0' cellspacing='8'>" +
      "<tr>" + 
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton'  onclick='WebUI.goBack();'>${footerBtnPageBack}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.resetFilters();'>${footerBtnResetFilter}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.expandTree();'>${footerBtnOpenTree}</div></td>" +
      "</tr>" +
    "</table>",
  TREE_EXPANDED_FOOTER_HTML: "" +
    "<table border='0' cellspacing='8'>" +
      "<tr>" + 
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton' style='width:auto;padding-left:5px;padding-right:5px;' onclick='WebUI.goBack();'>${footerBtnPageBack}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.resetFilters();'>${footerBtnResetFilter}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.collapseTree();'>${footerBtnCloseTree}</div></td>" +
      "</tr>" +
    "</table>",
  FLAT_FOOTER_HTML: "" +
    "<table border='0' cellspacing='8'>" +
      "<tr>" + 
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton' style='width:auto;padding-left:5px;padding-right:5px;' onclick='WebUI.goBack();'>${footerBtnPageBack}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.resetFilters();'>${footerBtnResetFilter}</div></td>" +
        "<td style='text-align:center; vertical-align: middle;'><div class='FooterButton CLASS04312' onclick='DeviceListPage.recoverTree();'>${footerBtnRestoreTree}</div></td>" +
      "</tr>" +
    "</table>",
  MODE:
  {
    TREE: 1,    // Anzeige als Baumstruktur
    FLAT: 2     // Anzeige als (flache) Kanalliste
  },
  UPDATE_DATA: true,
  SORT_FN:
  {
    NAME       : function(channels, reverse) { return channels.ex_sortBy("name", reverse); },
    TYPE_NAME  : function(channels, reverse) { return channels.ex_sortBy("typeName", reverse); },
    DESCRIPTION: function(channels, reverse) { return channels.ex_sortBy("typeDescription", reverse); },
    ADDRESS    : function(channels, reverse) { return channels.ex_sortBy("address", reverse); },
    CATEGORY   : function(channels, reverse) { return channels.ex_sortBy("category", reverse); },
    MODE       : function(channels, reverse) { return channels.ex_sortBy("mode", reverse); },
    ROOM_NAMES : function(channels, reverse) {
      channels.sort(function(a, b) { return Object.ex_compare(a.rooms.ex_joinItem("name"), b.rooms.ex_joinItem("name")); });
      return (reverse) ? channels.reverse() : channels;
    },
    FUNC_NAMES       : function(channels, reverse) {
      channels.sort(function(a, b) { return Object.ex_compare(a.subsections.ex_joinItem("name"), b.subsections.ex_joinItem("name")); });
      return (reverse) ? channels.reverse() : channels;
    }
  },
  PREFIX: "DeviceListPage",  
  HIGHLIGHT_CLASS: "DeviceListCell_Highlight",
  CATEGORIES:
  [
    {id: "CATEGORY_SENDER", name: translateKey("generalChannelConfigLblSender")}, // Sender
    {id: "CATEGORY_RECEIVER", name: translateKey("generalChannelConfigLblReceiver")}, // Empfänger
    {id: "CATEGORY_NOT_LINKABLE", name: translateKey("generalChannelConfigLblNone")} // nicht verknüpfbar
  ],
  MODES:
  [
    {id: "MODE_DEFAULT", name: translateKey("lblStandard") }, // Standard
    {id: "MODE_AES", name: translateKey("lblSecured") } // Gesichert
  ],
  INTERFACES:
  [
    {id: "INTERFACE_BIDCOS_RF", name: translateKey("BidCosRF-Filter")}, // BidCos-RF
    {id: "INTERFACE_BIDCOS_WIRED", name: translateKey("BidCosWired-Filter")}, // BidCos-Wired
    {id: "INTERFACE_HMIP_RF", name: translateKey("HmIPRF")}, // HmIP-RF
    {id: "INTERFACE_VIRTUAL_DEVICES", name: translateKey("VirtualDevices")},
    {id: "INTERFACE_CUXD", name: "CUxD"}, //
    {id: "INTERFACE_CCU_JACK", name: "CCU-Jack"}, //
    {id: "INTERFACE_SYSTEM", name: "System"} //
  ],
  
  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.m_onRemoveDevice = this.onRemoveDevice.bind(this);
    
    this.treeTemplate = TrimPath.parseTemplate(DEVICELIST_TREE_JST);
    this.flatTemplate = TrimPath.parseTemplate(DEVICELIST_FLAT_JST);
    
    this.channels = new Array(); 
    this.devices  = new Array();
    this.groups   = {};
  
    this.mode        = this.MODE.TREE;
    this.sortId      = "NAME";
    this.sortDescend = false;
    this.isExpanded  = false;

    this.virtChnCounter = 0;

    DeviceList.removeEvent.add(this.m_onRemoveDevice);
  },
    
  /**
   * Aktualisiert die zwischengespeicherten Daten eines Geräts
   **/
  updateDeviceData: function(device)
  {
    device.groups.each(function(group) {
      if (typeof(group._expanded) == "undefined") { group._expanded = false; }
    });
    
    // if (typeof(device._expanded) == "undefined") { device._expanded = false; }
    device._expanded = false;
    
    device.channels.each(function(channel) {
      channel._isVisible = true;
      channel.highlightChannel = ( ! userIsNoExpert && (channel.channelType.indexOf("_VIRTUAL_RECEIVER") != -1)) ? true : false;
    });
  },

  /**
   * Aktualisiert alle zwischengespreicherten Daten
   **/
  updateData: function()
  {
    this.devices  = DeviceList.listDevices().ex_sortBy("name");
    this.channels = DeviceList.listChannels();
    this.devices.each(this.updateDeviceData, this);
  },
  
  /**
   *
   **/
  sort: function(channels)
  {
    var sort_fn = this.SORT_FN[this.sortId];
    
    if (typeof(sort_fn) != "undefined") { return sort_fn(channels, this.sortDescend); }
    else                                { return channels; }
  },
  
  /**
   *
   **/
  matchChannel: function(channel)
  {
    return ((this.NameFilter.match(channel.name))                        &&
            (this.TypeNameFilter.match(channel.typeName))                &&
            (this.DescriptionFilter.match(translateKey(channel.typeDescription)))      &&
            (this.AddressFilter.match(channel.address))                  &&
            (this.CategoryFilter.matchString(channel.category))          &&
            (this.ModeFilter.matchStringArray(channel.mode)) &&
            (this.RoomFilter.matchArray(channel.rooms))                  &&
            (this.FuncFilter.matchArray(channel.subsections)));
  },
  
  /**
   *
   **/
  filterChannels: function(channels)
  {
    var result = new Array();
    
    channels.each(function(channel) {
      if (this.matchChannel(channel)) { result.push(channel); }
    }, this);
    
    return result;
  },
    
  matchDevice: function(device)
  {
    return ((this.NameFilter.match(device.name))                         &&
            (this.TypeNameFilter.match(device.typeName))                 &&
            (this.DescriptionFilter.match(translateKey(device.typeDescription)))       &&
            (this.AddressFilter.match(device.address))                   &&
            (this.InterfaceFilter.matchString(device.interfaceName))     &&
            (this.ModeFilter.matchStringArray(device.modes)) &&
            (this.RoomFilter.matchArray(device.rooms))                   &&
            (this.FuncFilter.matchArray(device.subsections)));
  },
  
  filterDevices: function(devices)
  {
    var result = new Array();

    devices.each(function(device) {
      if (this.matchDevice(device)) { result.push(device); }
    }, this);
    
    return result;
  },
    
  /**
   * Startet die Aktualisierung der Anzeige
   **/
  beginUpdateView: function(updateData)
  {
    var _updateData_ = updateData;
    if (typeof(_updateData_) == "undefined") { _updateData_ = false; }
    $("content").setStyle({cursor: "wait"});
    window.setTimeout("DeviceListPage.updateView(" + _updateData_ + ");", 1);
  },

  /**
   * Betreten der Geräteliste
   **/
  enter: function(options)
  {
    var self = this;
    this.content = $("content");
    this.content.innerHTML = "<img style=\"margin: 30px;\" alt=\"Loading...\" src=\"/ise/img/loading.gif\" />";
    this.m_visible = true;
    
    MainMenu.select(this.MAINMENU_ID);
    setPath("<span onclick='WebUI.enter(SystemConfigPage);'>"+translateKey('menuSettingsPage')+"</span> &gt; "+translateKey('submenuDevices'));
    setFooter("");
    this.userIsNoExpert = userIsNoExpert;

    this.mode        = this.MODE.TREE;
    this.sortId      = "NAME";
    this.sortDescend = false;
       
    var rooms = RoomList.list().ex_sortBy("name");
    var funcs = SubsectionList.list().ex_sortBy("name");

    this.NameFilter        = new StringFilter("DeviceListPage.NameFilter", this.beginUpdateView);
    this.TypeNameFilter    = new StringFilter("DeviceListPage.TypeNameFilter", this.beginUpdateView);
    this.DescriptionFilter = new StringFilter("DeviceListPage.DescriptionFilter", this.beginUpdateView);
    this.AddressFilter     = new StringFilter("DeviceListPage.AddressFilter", this.beginUpdateView);
    this.InterfaceFilter   = new ListFilter("DeviceListPage.InterfaceFilter", this.INTERFACES, this.beginUpdateView);
    this.CategoryFilter    = new ListFilter("DeviceListPage.CategoryFilter", this.CATEGORIES, this.beginUpdateView);
    this.ModeFilter        = new ListFilter("DeviceListPage.ModeFilter", this.MODES, this.beginUpdateView);
    this.RoomFilter        = new ListFilter("DeviceListPage.RoomFilter", rooms, this.beginUpdateView);
    this.FuncFilter        = new ListFilter("DeviceListPage.FuncFilter", funcs, this.beginUpdateView);

    window.setTimeout(function() {self.beginUpdateView(self.UPDATE_DATA);},50);
  },

  leave: function()
  {
    this.m_visible = false;
  },
  
  /**
   *  Prüft, ob es sich bei dem Kanal um einen der neuen virtellen Kanäle handelt (z. B. VIRTUAL_DIMMER, VIRTUAL_SWITCH, VIRTUAL_BLIND)
   *  Diese Kanäle sollen nur dann angezeigt werden, wenn der User den Expertenmodus aktiviert hat.
   *  Die virtuellen Fernbedienungen der CCU 'VIRTUAL_KEY' sind nicht betroffen
   **/
  showVirtualChannel: function(channel) {
    var deviceType = channel.deviceType.name.toUpperCase(),
    interfaceName = channel.device.interfaceName,
    hmIP_RF_Identifier = "HmIP-RF",
    channelNr = parseInt(channel.index);

    if (interfaceName != hmIP_RF_Identifier) {
      return (
        (!this.userIsNoExpert)
        || (channel.channelType == "VIRTUAL_KEY")
        || ((interfaceName != hmIP_RF_Identifier) && (channel.channelType.split("_")[0] != "VIRTUAL"))
        ) ? true : false;
    } else {
      if (this.userIsNoExpert) {
        if ((deviceType != "HMIP-MIOB") && (deviceType != "HMIP-WHS2")) {
          if (
            channel.channelType == "DIMMER_VIRTUAL_RECEIVER" ||
            channel.channelType == "SWITCH_VIRTUAL_RECEIVER" ||
            channel.channelType == "BLIND_VIRTUAL_RECEIVER" ||
            channel.channelType == "SHUTTER_VIRTUAL_RECEIVER" ||
            channel.channelType == "ACOUSTIC_SIGNAL_VIRTUAL_RECEIVER" ||
            channel.channelType == "SERVO_VIRTUAL_RECEIVER"
          ) {
            this.virtChnCounter = (this.virtChnCounter >= 3) ? 0 : this.virtChnCounter;
            this.virtChnCounter++;
            if (this.virtChnCounter == 1) {
              return true;
            } else {
              return false;
            }
          }
        } else {
          if (((deviceType == "HMIP-MIOB") || (deviceType == "HMIP-WHS2")) &&
            ((channelNr == 2) || (channelNr == 4) || (channelNr == 6) || (channelNr == 8))) {
            return false; // hide the virtual channels 2,4,6,8 - 3 and 7 are necessary for certain links
          }
        }
        return true;
      }
    }
    return true;
  },

  isHmIPMaintenanceChannel: function(channel)
  {
      return (channel.channelType == "MAINTENANCE") ? true : false;
  },

  isChannelVisible: function(channel)
  {
    // Don't show some channels
    var result = true;
    switch (channel.channelType) {
      case "ALARM_COND_SWITCH_TRANSMITTER":
      case "MAINTENANCE":
      case "WEEK_PROGRAM":
        result = false;
        break;
      default: result = true;
    }
    return result;
  },


  /**
   * Zeigt den Konfigurationsdialog für einen Kanal an.
   **/
  selectChannel: function(id)
  {
    var _this_  = this;
    var channel = DeviceList.getChannel(id);

    ChannelConfigDialog.show(channel, function(result) {
      if (result == ChannelConfigDialog.RESULT_OK)
      {
        _this_.beginUpdateView();
      }
    });
  },
  
  /**
   * Zeigt den Konfigurationsdialog für ein Gerät an.
   **/
  selectDevice: function(id)
  {
    var _this_ = this;
    var device = DeviceList.getDevice(id);
    
    DeviceConfigDialog.show(device, function(result) {
      if (result == DeviceConfigDialog.RESULT_OK)
      {
        _this_.beginUpdateView();
      }
    });
  },
  
  /**
   * 
   **/
  sortBy: function(sortId)
  {
    if (this.mode == this.MODE.FLAT)
    {
      if (this.sortId == sortId) { this.sortDescend = !this.sortDescend; }
      else                       { this.sortDescend = false; }
    }
    else
    {
      this.sortDescend = false;
      this.mode   = this.MODE.FLAT;
      this.resetFilters(false);
    }
    
    this.sortId = sortId;
    this.beginUpdateView();
  },
  
  
  /**
   * Filter zurücksetzen
   **/
  resetFilters: function(update)
  {
    var _update_ = update;
    if (typeof(_update_) == "undefined") { _update_ = true; }
    
    this.NameFilter.reset();
    this.TypeNameFilter.reset();
    this.DescriptionFilter.reset();
    this.AddressFilter.reset();
    this.InterfaceFilter.reset();
    this.CategoryFilter.reset();
    this.ModeFilter.reset();
    this.RoomFilter.reset();
    this.FuncFilter.reset();
    
    if (_update_ == true) { this.beginUpdateView(); }
  },

  expandTree: function()
  {
    this.mode = this.MODE.TREE;
    
    this.devices.each(function(device) {
      device._expanded = true;
   
      device.channels.each(function(channel) {
        channel._isVisible = false;
        if(this.showVirtualChannel(channel)) {
          channel._isVisible = true;
         }
      }, this);
      
      device.groups.each(function(group) {
        group._expanded = true;
      });
    }, this);
    
    this.isExpanded = true;
    this.beginUpdateView();
  },


  collapseTree: function()
  {
    this.mode = this.MODE.TREE;
    this.devices.each(function(device) {
      device._expanded = false;
      device.groups.each(function(group) {
        group._expanded = false;
      });
    });
    
    this.isExpanded = false;
    this.beginUpdateView();
  },

  expandDevice: function(event, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);
    
    var device = DeviceList.getDevice(id);

    device._expanded = true;
    
    $(this.PREFIX + id + "PLUS").hide();
    $(this.PREFIX + id + "MINUS").show();
    
    device.groups.each(function(group) {
      $(this.PREFIX + group.id).show();
      $(this.PREFIX + group.id + "Thumbnail").show();
      this.collapseGroup(evt, group.id);
    }, this);
    device.singles.each(function(channel) {
      channel._isVisible = false;
      if(this.showVirtualChannel(channel) && (this.isChannelVisible(channel)) ) {
        channel._isVisible = true;
        $(this.PREFIX + channel.id).show();
        $(this.PREFIX + channel.id + "Thumbnail").show();
      }
    }, this);
  },


  collapseDevice: function(event, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);

    var device = DeviceList.getDevice(id);
    device._expanded = false;
    
    $(this.PREFIX + id + "MINUS").hide();
    $(this.PREFIX + id + "PLUS").show();
    
    device.groups.each(function(group) {
      $(this.PREFIX + group.id).hide();
      $(this.PREFIX + group.id + "Thumbnail").hide();
      this.collapseGroup(evt, group.id);
    }, this);
    device.singles.each(function(channel) {
        try {
          $(this.PREFIX + channel.id).hide();
          $(this.PREFIX + channel.id + "Thumbnail").hide();
        } catch (e) {}
    }, this);
  },
  
  expandGroup: function(event, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);
    
    var group = DeviceList.getChannelGroup(id);
    group._expanded = true;
    
    $(this.PREFIX + id + "PLUS").hide();
    $(this.PREFIX + id + "MINUS").show();
    
    group.channels.each(function(channel) {
      $(this.PREFIX + channel.id).show();
      $(this.PREFIX + channel.id + "Thumbnail").show();
    }, this);
  },
  
  collapseGroup: function(event, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);

    var group = DeviceList.getChannelGroup(id);
    group._expanded = false;
    
    $(this.PREFIX + id + "MINUS").hide();
    $(this.PREFIX + id + "PLUS").show();
    
    group.channels.each(function(channel) {
      $(this.PREFIX + channel.id).hide();
      $(this.PREFIX + channel.id + "Thumbnail").hide();
    }, this);
  },
  
  /**
   * Baumstruktur wiederherstellen
   **/
  recoverTree: function()
  {
    this.mode = this.MODE.TREE;
    this.resetFilters();
  }, 
  
  /**
   * Gerät löschen
   **/
  deleteDevice: function(event, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);
  
    var device = DeviceList.getDevice(id);
    new DeleteDeviceDialog(device, function(isDeleted) {
      // if (isDeleted) { WebUI.reload(); }
    });
  },
  
  /**
   * Gerät, Kanal oder Kanalgruppe konfigurieren
   **/
  showConfiguration: function(event, typeId, id)
  {
    var evt = (event) ? event : window.event;
    try {
      Event.stop(evt);
    } catch(e) {}

    switch (typeId)
    {
      case "DEVICE": 
        var device  = DeviceList.getDevice(id);
        var iface   = device.interfaceName;
        var address = device.address;
        WebUI.enter(DeviceConfigPage, {'iface': iface, 'address': address, 'redirect_url': 'GO_BACK'});
        break;
      case "GROUP":
        var group   = DeviceList.getChannelGroup(id);
        var iface   = group.device.interfaceName;
        var address = group.channels[0].address;
        WebUI.enter(DeviceConfigPage, {'iface': iface, 'address': address, 'redirect_url': 'GO_BACK', 'with_group': 1});
        break;
      case "CHANNEL":
        var channel = DeviceList.getChannel(id);
        var iface   = channel.device.interfaceName;
        var address = channel.address;
        WebUI.enter(DeviceConfigPage, {'iface': iface, 'address': address, 'redirect_url': 'GO_BACK'});
        break;
      default:
        Debug.assert(false, "DeviceListPage.showConfiguration: invalid type id");
        break;
    }
    
  },
  
  /**
   * Direkte Verknüpfungen anzeigen
   **/
  showDirectLinks: function(event, typeId, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);
    
    switch (typeId)
    {
      case "DEVICE":
        var device = DeviceList.getDevice(id);
        WebUI.enter(LinkListPage, {
          iface  : device.interfaceName,
          channel: device.address
        });
        break;
      case "GROUP":
        var group = DeviceList.getChannelGroup(id);
        WebUI.enter(LinkListPage, {
          iface  : group.device.interfaceName,
          channel: group.channels[0].address,
          keypair: 1
        });
        break;
      case "CHANNEL":
        var channel = DeviceList.getChannel(id);
        WebUI.enter(LinkListPage, {
          iface  : channel.device.interfaceName,
          channel: channel.address
        });
        break;
      default:
        Debug.assert(false, "DeviceListPage.showDirectLinks: invalid type id");
        break;
    }
  },
  
  /**
   * Programme anzeigen
   **/
  showPrograms: function(event, typeId, id)
  {
    var evt = (event) ? event : window.event;
    Event.stop(evt);
    
    switch (typeId)
    {
      case "DEVICE":
        var device = DeviceList.getDevice(id);
        var ids    = device.listProgramIds();
        WebUI.enter(ProgramListPage, ids.join("\t"));
        break;
      case "GROUP":
        var group = DeviceList.getChannelGroup(id);
        var ids   = group.listProgramIds();        
        WebUI.enter(ProgramListPage, ids.join("\t"));
        break;
      case "CHANNEL":
        var channel = DeviceList.getChannel(id);
        var ids     = channel.listProgramIds();
        WebUI.enter(ProgramListPage, ids.join("\t"));
        break;
      default:
        Debug.assert(false, "DeviceListPage.showPrograms: invalid type id");
        break;
    }    
  },
  
  /**
   * Aktualisert die Anzeige
   **/
  updateView: function(updateData)
  {
    this.content.setStyle({"cursor": "default"});
    if (updateData === true) { this.updateData(); }

    if (this.mode == this.MODE.TREE)
    {
      this.content.innerHTML = this.treeTemplate.process({
        PREFIX           : this.PREFIX,
        nameFilter       : this.NameFilter,
        typeNameFilter   : this.TypeNameFilter,
        descriptionFilter: this.DescriptionFilter,
        addressFilter    : this.AddressFilter,
        interfaceFilter  : this.InterfaceFilter,
        modeFilter       : this.ModeFilter,
        funcFilter       : this.FuncFilter,
        roomFilter       : this.RoomFilter,
        devices          : this.filterDevices(this.devices)
      });
      if (this.isExpanded) { setFooter(this.TREE_EXPANDED_FOOTER_HTML); }
      else                 { setFooter(this.TREE_COLLAPSED_FOOTER_HTML); }
    }
    else
    {
      this.content.innerHTML = this.flatTemplate.process({
        PREFIX           : this.PREFIX,
        sortId           : this.sortId,
        sortDescend      : this.sortDescend,
        nameFilter       : this.NameFilter,
        typeNameFilter   : this.TypeNameFilter,
        descriptionFilter: this.DescriptionFilter,
        addressFilter    : this.AddressFilter,
        categoryFilter   : this.CategoryFilter,
        modeFilter       : this.ModeFilter,
        funcFilter       : this.FuncFilter,
        roomFilter       : this.RoomFilter,
        channels         : this.sort(this.filterChannels(this.channels))
      });
      setFooter(this.FLAT_FOOTER_HTML);
    }

    jQuery(".j_chMode").each(function(){
      var elm = jQuery(this);

        switch (elm.text()) {
          case "Gesichert":
            elm.html(translateKey("lblSecured"));
            break;
          case "Standard":
            elm.html(translateKey("lblStandard"));
        }
    });

    translateJSTemplate("#DeviceListTable");
    translatePage(".j_rooms, .j_functions"); // this translates the room name as well the function name within the main devicelist (Settings > Devices)
    jQuery("#DeviceListPage_RoomFilter").draggable();
    jQuery("#DeviceListPage_FuncFilter").draggable();
  },
  
  onRemoveDevice: function(whatEver)
  {
    if (this.m_visible)
    {
      WebUI.reload();
    }
  }
  
});

} else {

 DeviceListPage = new function()
 {
  //Geräteliste Konfigtool
  var m_menuId = "MAINMENU_OPTIONS_DEVICES";
  this.enter = function(options)
  {
     //MainMenu.select(m_menuId);
     updateContent('/configapp/devices.cgi');
  };

  this.leave = function()
  {
  };

  this.resize = function()
  {
  };

 }();

}
