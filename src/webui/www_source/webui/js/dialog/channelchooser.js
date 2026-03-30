/**
 * channelchosser.js
 **/
 
/**
 * Einfache Kanalauswahl.
 **/

ChannelChooser = Singleton.create({

  SHOW_READABLE: 0x1,    // zeigt lesbare Kanäle an
  SHOW_WRITABLE: 0x2,    // zeigt schreibbare Kanäle an
  SHOW_EVENTABLE: 0x4,    // zeigt Kanäle mit Ereignisbehandlung an
  SHOW_ALL: 0x7,    // zeigt alle Kanäle an

  FAV_CHANNELS: 0,
  PRG_CONDITION: 1,
  PRG_ACTIVITY: 2,
    
  SORT_FN:  {
    NAME       : function(channels, reverse) { return channels.ex_sortBy("name", reverse); },
    DESCRIPTION: function(channels, reverse) { return channels.ex_sortBy("typeDescription", reverse); },
    //ADDRESS    : function(channels, reverse) { return channels.ex_sortBy("address", reverse); },

    // Sorting by serial number changed.
    ADDRESS    : function(channels, reverse) {
      var arSortedChannels = [];
      jQuery.each(channels.sort(function(a,b) {
          var ar1 = a.address.split(":"),
            ar2 = b.address.split(":");

          if (ar1[0] == ar2[0]) {
            return (parseInt(ar1[1]) - parseInt(ar2[1]));
          }
        }), function(index, ch) {
          arSortedChannels.push(ch);
      });
        return arSortedChannels;
    },

    ROOM_NAMES : function(channels, reverse) {
      channels.sort(function(a, b) { return Object.ex_compare(a.rooms.ex_joinItem("name"), b.rooms.ex_joinItem("name")); });
      return (reverse) ? channels.reverse() : channels;
    },
    FUNC_NAMES : function(channels, reverse) {
      channels.sort(function(a, b) { return Object.ex_compare(a.subsections.ex_joinItem("name"), b.subsections.ex_joinItem("name")); });
      return (reverse) ? channels.reverse() : channels;
    }
  },
    
  PREFIX: "ChannelChooser",                // Prefix für Ids der Tabellenzeilen
  WRAPPER_ID: "ChannelChooserWrapper",         // Id des Wrapper-Elements
  HIGHLIGHT_CLASS: "ChannelChooserCell_Highlight",  // Klasse für hervorgehobene Tabellenzellen
  
  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.HmIPIdentifier = "HmIP-RF";
    this.HmIPWIdentifier = "HmIP-RF";
    this.VirtualDevicesIdentifier = "VirtualDevices";
    this.template = TrimPath.parseTemplate(CHANNELCHOOSER_JST);
    this.arWGDScreenOrder = [];
    this.WGDStartChannelPerScreen = {};
    this.WGDChannelInUse = [];
    this.WGDdevice = "";
    this.arWGDTiles = [];
    this.noMoreDaliChannels = false;
  },

  sortByAddress: function(channels) {
    console.log("channels", channels);
  },

  /**
   * Wendet alle Filter auf einen Kanal an.
   **/
  match: function(channel)
  {  
    return ((hasUPL(UPL_ADMIN) | channel.isVisible)           &&
        ((this.showReadable  & channel.isReadable)      ||
         (this.showWritable  & channel.isWritable)      ||
         (this.showEventable & channel.isEventable))          &&
      (this.showVirtual | !channel.isVirtual)                 &&
      (this.NameFilter.match(channel.name))                   &&
      (this.DescriptionFilter.match(channel.typeDescription)) &&
      (this.AddressFilter.match(channel.address))             &&
      (this.RoomFilter.matchArray(channel.rooms))             &&
      (this.FuncFilter.matchArray(channel.subsections)));     
  },

  setMetaData: function(objectId, dataId, val) {
    homematic("Interface.setMetadata", {"objectId": objectId, "dataId": dataId, "value": val});
  },

  filterHmIPChannels4ProgramConditions: function(channel, arChannels) {
    var channelTypeName = channel.typeName.toLowerCase(),
      oDevice, oMaintChannel, deviceMode, endOfScreens = false;

    conInfo("filterHmIPChannels4ProgramConditions");
    // If the channel is visible and no KEY_TRANSCEIVER or *_WEEK_PROFILE then show the channel
    if (channel.isVisible
      && (channel.channelType != "KEY_TRANSCEIVER")
      && (channel.channelType != "UNIVERSAL_LIGHT_RECEIVER")
      && (channel.channelType != "DISPLAY_INPUT_TRANSMITTER")
      && (channel.channelType != "DISPLAY_LEVEL_INPUT_TRANSMITTER")
      && (channel.channelType != "DISPLAY_THERMOSTAT_INPUT_TRANSMITTER")
      && (channel.channelType != "MULTI_MODE_INPUT_TRANSMITTER")
      && (channel.channelType.indexOf("_WEEK_PROFILE") == -1)
    ) {
      arChannels.push(channel);
    }

    // If the channel is a KEY_TRANSCEIVER and the device type no HmIP-PS / PSM (-IT/-CH/-PE/-UK) / PDT /PCBS then show the channel
    // A key press of the internal button doesn't work for the above-named devices
    if ((channel.channelType == "KEY_TRANSCEIVER")
      && channel.isVisible
      && (channelTypeName != "hmip-ps")
      && (channelTypeName.indexOf("hmip-psm") == -1)
      && (channelTypeName != "hmip-pdt")
      && (channelTypeName != "hmip-pdt-uk")
      && (channelTypeName != "hmip-pcbs")
      && ((channelTypeName != "hmip-wgs") && (channelTypeName != "hmip-wgs-a")) // the wgs gets a special treatment further down
    ) {
      arChannels.push(channel);
    }

    if (channel.channelType == "UNIVERSAL_LIGHT_RECEIVER") {

      if (channelTypeName == "hmip-rgbw") {
        oMaintChannel = DeviceList.getChannelByAddress(channel.address.split(":")[0] + ":0"); // The maintenance channel stores the deviceMode
        deviceMode = parseInt(homematic("Interface.getMetadata", {
          "objectId": oMaintChannel.id,
          "dataId": "deviceMode"
        }));

        switch (deviceMode) {
          case 0:
          case 1:
            // RGB/RGBW Mode
            if (channel.index == 1) {
              arChannels.push(channel);
            }
            break;
          case 2:
            // Tunable White Mode
            if (channel.index == 1 || channel.index == 2) {
              arChannels.push(channel);
            }
            break;
          default:
            // PWM Mode - all channels visible
            arChannels.push(channel);
        }
      }

      if (channelTypeName == "hmip-drg-dali") {
        if ((channel.index != 0) && (channel.index <= 32)) {
          // Dali channels 1 - 32
          if (this.noMoreDaliChannels == false) {
            chnDescription = homematic("Interface.getParamset", {
              "interface": "HmIP-RF",
              "address": channel.address,
              "paramsetKey": "MASTER"
            });

            if (parseInt(chnDescription["DALI_ADDRESS"]) != 255) {
              arChannels.push(channel);
              // Store UNIVERSAL_LIGHT_MAX_CAPABILITIES as meta data
              // Because the user might have connected another DALI device to this channel we must set the meta data each time
              //this is not necessary anymore - this.setMetaData(channel.id, "maxCap", chnDescription["UNIVERSAL_LIGHT_MAX_CAPABILITIES"]);
            } else {
              this.noMoreDaliChannels = true;
            }
          }

        } else if ((channel.index == 0) || (channel.id >= 33)) {
          // Maintenance and group channels
          arChannels.push(channel);
        }
      }

      if ((channelTypeName == "hmip-lsc") && (channel.index == 1)) {
        arChannels.push(channel);
      }

    }

    if (((channel.channelType == "DISPLAY_INPUT_TRANSMITTER")
      || (channel.channelType == "DISPLAY_LEVEL_INPUT_TRANSMITTER")
      || (channel.channelType == "DISPLAY_THERMOSTAT_INPUT_TRANSMITTER")
      || (channel.channelType == "WEATHER_DISPLAY_RECEIVER")
    ) && ((channelTypeName.indexOf("-wgd") != -1))) {

      var wgdScreenOrder, screenEndID = "END", counter, chnDescription, curDevice, tilesA = [1, 3, 7], tilesB = [0, 1],loop,
        isWired = channelTypeName.indexOf("hmipw") != -1,
        self = this;

      if (channel.index == 41) {
        arChannels.push(channel);
      } else {
        curDevice = channel.device.address; // Do this only once per device
        if (this.WGDdevice != curDevice) {
          this.WGDdevice = channel.device.address;
          oDevice = DeviceList.getDeviceByAddress(channel.address.split(":")[0]); // The device stores the screen order
          wgdScreenOrder = homematic("Interface.getMetadata", {"objectId": oDevice.id, "dataId": "screenOrder"});
          this.arWGDScreenOrder = wgdScreenOrder.split(",");
          if (isWired) {
            this.WGDStartChannelPerScreen = (this.arWGDScreenOrder.length < 10) ? {0: 1, 1: 9, 2: 17, 3: 25, 4: 33} : {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42, 6: 44, 7: 46, 8: 48, 9: 50};
          } else {
            this.WGDStartChannelPerScreen = (this.arWGDScreenOrder.length < 11) ? {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42} : {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42, 6: 44, 7: 46, 8: 48, 9: 50, 10:52};
          }
          this.WGDChannelInUse = [];
          this.arWGDTiles = [];

          // Get number of tiles
          chnDescription = homematic("Interface.getParamset", {"interface": "HmIP-RF", "address": curDevice + ":0", "paramsetKey": "MASTER"});
          if (isWired) {
            loop = ((this.arWGDScreenOrder.length < 10)) ? 5 : 10;
          } else {
            loop = ((this.arWGDScreenOrder.length < 11)) ? 6 : 11;
          }

          for (var loopx = 1; loopx <= loop; loopx++) {
            this.arWGDTiles.push(chnDescription["SCREEN_LAYOUT_TILE_LAYOUT_" + loopx]);
          }

          jQuery.each(this.arWGDScreenOrder, function (index, screen) {
            if ((!endOfScreens) && (screen != screenEndID)) {
              if (isWired) {
                counter = (screen <= 4) ? tilesA[self.arWGDTiles[screen]] : tilesB[self.arWGDTiles[screen]];
              } else {
                counter = (screen <= 5) ? tilesA[self.arWGDTiles[screen]] : tilesB[self.arWGDTiles[screen]];
              }
              for (loop = self.WGDStartChannelPerScreen[screen]; loop <= (self.WGDStartChannelPerScreen[screen] + counter); loop++) {
                self.WGDChannelInUse.push(loop);
              }
            } else {endOfScreens = true;} // return false doesn't work because of a problem with the build-system
          });
        }

        if (this.WGDChannelInUse.indexOf(channel.index) != -1) {
          arChannels.push(channel);
        }
      }
    }

    if (channel.channelType == "MULTI_MODE_INPUT_TRANSMITTER") {
      if (channelTypeName == "elv-sh-bm-s") {
        var channelMode = parseInt(homematic("Interface.getMetadata", {
          "objectId": channel.id,
          "dataId": "channelMode"
        }));
        if (channelMode != 0) {
          arChannels.push(channel);
        }
      } else if ((channelTypeName == "hmip-flc") || (channelTypeName == "hmip-fdc")) {
        var channelMode = parseInt(homematic("Interface.getMasterValue", {
          "interface": this.HmIPIdentifier,
          "address": channel.address,
          "valueKey": "CHANNEL_OPERATION_MODE"
        }));

        if (channelMode != 0) {
          arChannels.push(channel);
        }

      } else {
        arChannels.push(channel);
      }
    }

    // Depending on the selected layout mode (1, 2 or 4 buttons) we have to filter some channels
    if ((channel.channelType == "KEY_TRANSCEIVER") && ((channelTypeName == "hmip-wgs") || (channelTypeName == "hmip-wgs-a"))) {
      var curDevice, chnDescription, mode;
      curDevice = channel.device.address;
      chnDescription = homematic("Interface.getParamset", {
        "interface": "HmIP-RF",
        "address": curDevice + ":0",
        "paramsetKey": "MASTER"
      });
      mode = parseInt(chnDescription["DEVICE_INPUT_LAYOUT_MODE"]);

      if (mode == 0) {
        if (channel.index == 1 || channel.index == 5) {
          arChannels.push(channel);
        }
      } else if (mode == 1 || mode == 2) {
        if (channel.index == 1 || channel.index == 2 || channel.index == 5) {
          arChannels.push(channel);
        }
      } else {
        arChannels.push(channel);
      }
    }
    return arChannels;
  },

  filterHmIPChannels4ProgramActivities: function(channel, arChannels) {
    conInfo("filterHmIPChannels4ProgramActivities");
    var channelTypeName = channel.typeName.toLowerCase(),
    oMaintChannel, deviceMode, endOfScreens = false;

    if (channel.isVisible) {

      // Channel 1 of the hmip-esi-ind has only 1 parameter (SELF_CALIBRATION) for the use in a program condition but it's not in use
      // Therefore we hide the channel
      if (channelTypeName == "hmip-esi-ind") {
        return;
      }

      if (channelTypeName == "hmip-wkp") {
        if ((channel.channelType == "MAINTENANCE") || ((channel.channelType == "ACCESS_TRANSCEIVER") && (channel.index % 2 != 0))) {
          arChannels.push(channel);
        }
      } else if (channel.channelType == "UNIVERSAL_LIGHT_RECEIVER") {
        if (channelTypeName == "hmip-rgbw") {
          oMaintChannel = DeviceList.getChannelByAddress(channel.address.split(":")[0] + ":0"); // The maintenance channel stores the deviceMode
          deviceMode = parseInt(homematic("Interface.getMetadata", {
            "objectId": oMaintChannel.id,
            "dataId": "deviceMode"
          }));

          switch (deviceMode) {
            case 0:
            case 1:
              // RGB/RGBW Mode
              if (channel.index == 1) {
                arChannels.push(channel);
              }
              break;
            case 2:
              // Tunable White Mode
              if (channel.index == 1 || channel.index == 2) {
                arChannels.push(channel);
              }
              break;
            default:
              // PWM Mode - all channels visible
              arChannels.push(channel);
          }
        }

        if (channelTypeName == "hmip-drg-dali") {
          if ((channel.index != 0) && (channel.index <= 32)) {
            // Dali channels 1 - 32
            if (this.noMoreDaliChannels == false) {
              chnDescription = homematic("Interface.getParamset", {
                "interface": "HmIP-RF",
                "address": channel.address,
                "paramsetKey": "MASTER"
              });

              if (parseInt(chnDescription["DALI_ADDRESS"]) != 255) {
                arChannels.push(channel);
                // Store UNIVERSAL_LIGHT_MAX_CAPABILITIES as meta data if not yet available
                // Because the user might have connected another DALI device to this channel we must set the meta data each time
                //this is not necessary anymore - this.setMetaData(channel.id, "maxCap", chnDescription["UNIVERSAL_LIGHT_MAX_CAPABILITIES"]);
              } else {
                this.noMoreDaliChannels = true;
              }
            }

          } else if ((channel.index == 0) || (channel.id >=33)) {
            // Maintenance and group channels
            arChannels.push(channel);
          }
        }

        if ((channelTypeName == "hmip-lsc") && (channel.index == 1)) {
          arChannels.push(channel);
        }


      } else if (((channel.channelType == "DISPLAY_INPUT_TRANSMITTER")
        || (channel.channelType == "DISPLAY_LEVEL_INPUT_TRANSMITTER")
        || (channel.channelType == "DISPLAY_THERMOSTAT_INPUT_TRANSMITTER")
        || (channel.channelType == "WEATHER_DISPLAY_RECEIVER")
      ) && ((channelTypeName.indexOf("-wgd") != -1))) {
        var  wgdScreenOrder, screenEndID = "END",  counter, chnDescription, curDevice, tilesA = [1,3,7], tilesB = [0,1], loop,
          isWired = channelTypeName.indexOf("hmipw") != -1,
          self = this;

        if (channel.index == 41) {
          arChannels.push(channel);
        } else {
          curDevice = channel.device.address; // Do this only once per device
          if (this.WGDdevice != curDevice) {
            this.WGDdevice = channel.device.address;
            oDevice = DeviceList.getDeviceByAddress(channel.address.split(":")[0]); // The device stores the screen order
            wgdScreenOrder = homematic("Interface.getMetadata", {"objectId": oDevice.id, "dataId": "screenOrder"});
            this.arWGDScreenOrder = wgdScreenOrder.split(",");
            if (isWired) {
              this.WGDStartChannelPerScreen = (this.arWGDScreenOrder.length < 10) ? {0: 1, 1: 9, 2: 17, 3: 25, 4: 33} : {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42, 6: 44, 7: 46, 8: 48, 9: 50};
            } else {
              this.WGDStartChannelPerScreen = (this.arWGDScreenOrder.length < 11) ? {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42} : {0: 1, 1: 9, 2: 17, 3: 25, 4: 33, 5: 42, 6: 44, 7: 46, 8: 48, 9: 50, 10:52};
            }
            this.WGDChannelInUse = [];
            this.arWGDTiles = [];

            // Get number of tiles
            chnDescription = homematic("Interface.getParamset", {"interface": "HmIP-RF", "address": curDevice + ":0", "paramsetKey": "MASTER"});
            if (isWired) {
              loop = ((this.arWGDScreenOrder.length < 10)) ? 5 : 10;
            } else {
              loop = ((this.arWGDScreenOrder.length < 11)) ? 6 : 11;
            }

            for(var loopx = 1; loopx <= loop; loopx++) {
              this.arWGDTiles.push(chnDescription["SCREEN_LAYOUT_TILE_LAYOUT_" + loopx]);
            }

            jQuery.each(this.arWGDScreenOrder, function(index,screen) {
              if ((! endOfScreens) && (screen != screenEndID)) {
                if (isWired) {
                  counter = (screen <= 4) ? tilesA[self.arWGDTiles[screen]] : tilesB[self.arWGDTiles[screen]];
                } else {
                  counter = (screen <= 5) ? tilesA[self.arWGDTiles[screen]] : tilesB[self.arWGDTiles[screen]];
                }

                for (loop = self.WGDStartChannelPerScreen[screen]; loop <= (self.WGDStartChannelPerScreen[screen] + counter); loop++) {
                  self.WGDChannelInUse.push(loop);
                }
              } else {endOfScreens = true;} // return false doesn't work because of a problem with the build-system
            });
          }

          if (this.WGDChannelInUse.indexOf(channel.index) != -1) {
            arChannels.push(channel);
          }
        }
      } else {
        if (
          channelTypeName != "hmip-esi" &&
          ((channel.channelType != "SHUTTER_TRANSMITTER") || ((channel.channelType == "SHUTTER_TRANSMITTER") && (channelTypeName != "hmip-m-td15")))
        ) {
         arChannels.push(channel);
        }
      }
    }



    return arChannels;
  },

  filterOsramLightify: function(channel, arChannels) {
    conInfo("filterOsramLightify");
    if (! isNonCCUDevice(channel)) {
      arChannels.push(channel);
    }
    return arChannels;
  },

  filterGateway: function(channel, arChannels) {
    conInfo("filterGateway");
    if (! isNonCCUGateway(channel)) {
      arChannels.push(channel);
    }
    return arChannels;
  },

  /**
   * Filtert eine Kanalliste
   **/
  filter: function(channels)
  {
    this.noMoreDaliChannels = false;
    var result = new Array();

    channels.each(function(channel) {
      var self = this;
      if (this.match(channel)) {

        if ((channel.device.interfaceName == this.HmIPIdentifier) || (channel.device.interfaceName == this.HmIPWIdentifier)) {
          if ((this.src == this.PRG_CONDITION) || (this.src == this.FAV_CHANNELS)) {
            this.filterHmIPChannels4ProgramConditions(channel, result);
          } else if (this.src == this.PRG_ACTIVITY) {
            this.filterHmIPChannels4ProgramActivities(channel, result);
          } else {
            result.push(channel);
          }
        } else if (channel.device.interfaceName == this.VirtualDevicesIdentifier) {
          if (this.src == this.PRG_CONDITION) {
            this.filterOsramLightify(channel, result);
          } else if (this.src == this.PRG_ACTIVITY) {
            this.filterGateway(channel, result);
          } else {
            result.push(channel);
          }
        } else {
          result.push(channel);
        }
      }
    }, this);
    this.WGDdevice = "";
    return result;
  },
    
  /**
   * Sortiert eine Kanalliste
   **/
  sort: function(channels)
  {
    var sort_fn = this.SORT_FN[this.sortId];
    
    if (typeof(sort_fn) != "undefined") { return sort_fn(channels, this.sortDescend); }
    else                                { return channels; }
  },
    
  /**
   * Schließt das Dialogfenster
   **/
  close: function(result)
  {
    picDivHide(jg_250);
    Layer.remove(this.layer);
    if (this.callback) { this.callback(result); }
  },
  
  /**
   * Startet die Aktualisierung der Anzeige
   **/
  beginUpdateView: function()
  {
    Element.setStyle(ChannelChooser.layer, {cursor: "wait"});
    window.setTimeout("ChannelChooser.updateView();", 1);
  },
   
  /**
   * Zeigt den Dialog an
   **/
  show: function(callback, options, src)
  {
    if (src) {
      this.src = src;
    } else {
      this.src = 0;
    }
    this.showVirtual   = false;
    this.sortId        = "NAME";
    this.sortDescend   = false;
    this.callback      = callback;
    this.showReadable  = (0 !== (options & this.SHOW_READABLE));
    this.showWritable  = (0 !== (options & this.SHOW_WRITABLE));
    this.showEventable = (0 !== (options & this.SHOW_EVENTABLE));
    

    this.channels = DeviceList.listChannels();
    
    var rooms    = RoomList.list().ex_sortBy("name");
    var funcs    = SubsectionList.list().ex_sortBy("name");
    this.NameFilter        = new StringFilter("ChannelChooser.NameFilter", this.beginUpdateView);
    this.DescriptionFilter = new StringFilter("ChannelChooser.DescriptionFilter", this.beginUpdateView);
    this.AddressFilter     = new StringFilter("ChannelChooser.AddressFilter", this.beginUpdateView);
    this.RoomFilter        = new ListFilter("ChannelChooser.RoomFilter", rooms, this.beginUpdateView);
    this.FuncFilter        = new ListFilter("ChannelChooser.FuncFilter", funcs, this.beginUpdateView);
    
    this.layer    = document.createElement("div");
    this.layer.id = this.WRAPPER_ID;
    Layer.add(this.layer);
      
    this.beginUpdateView();


  },

  /**
   * Wählt einen Kanal aus
   **/
  select: function(str_id)
  {
    var id = str_id.substring(this.PREFIX.length);
    this.close(id);
  },
  
  /**
   * Bricht den Dialog ab
   **/
  abort: function()
  {
    this.close();
  },
  
  /**
   * Setzt die Sortierreihenfolge
   **/
  sortBy: function(sortId)
  {
    if (this.sortId == sortId) { this.sortDescend = !this.sortDescend; }
    else                       { this.sortDescend = false; }
    this.sortId = sortId;
    
    this.beginUpdateView();
  },
  
  /**
   * Blendet virtuelle Kanäle ein bzw. aus
   **/
  toggleVirtualChannels: function()
  {
    this.showVirtual = !this.showVirtual;
    
    this.beginUpdateView();
  },
  
  /**
   * Setzt alle Filter zurück
   **/
  resetFilters: function()
  {
    this.NameFilter.reset();
    this.DescriptionFilter.reset();
    this.AddressFilter.reset();
    this.RoomFilter.reset();
    this.FuncFilter.reset();
    
    this.beginUpdateView();
  },
        
  /**
   * Callback. Aktualisiert die Anzeige.
   **/
  updateView: function()
  {
    Element.setStyle(this.layer, {"cursor": "default"});
    this.layer.innerHTML = this.template.process({
      PREFIX           : this.PREFIX,
      sortId           : this.sortId,
      sortDescend      : this.sortDescend,
      showVirtual      : this.showVirtual,
      nameFilter       : this.NameFilter,
      descriptionFilter: this.DescriptionFilter,
      addressFilter    : this.AddressFilter,
      roomFilter       : this.RoomFilter,
      funcFilter       : this.FuncFilter,
      channels         : this.sort(this.filter(this.channels))
      //channels         : this.filter(this.channels)
    });

    if (! userIsNoExpert) {
      jQuery(".j_expertChannel").show();
    }

    translateJSTemplate("#ChannelChooserDialog");
    translatePage(".j_rooms, .j_functions");

    // Add extended channel description
    jQuery(".j_extChnDescr").each(function(index) {
      try {
        var elmDescr = jQuery(this).text().split("_"),
        channelAddress = elmDescr[1],
        ch = DeviceList.getChannelByAddress(channelAddress),
        deviceType = ch.deviceType.id,
        j_descrElem = jQuery(this);

        if ((ch.channelType != "MULTI_MODE_INPUT_TRANSMITTER") && (deviceType != "HmIP-WKP")) {
          j_descrElem.html(getExtendedDescription({
            "deviceType": deviceType,
            "channelAddress": channelAddress,
            "channelIndex": channelAddress.split(":")[1]
          }));
        } else {
            if (deviceType == "HmIP-WKP") {
              // Channel 1 AND 2 = User 1, Channel 3 AND 4 = User 2 and so on - but we want to show only the first user channel
              var chn = parseInt(channelAddress.split(":")[1]),
                arUsrNr = ["","1","","2","","3","","4","","5","","6","","7","","8",""];

              if (chn != 0) {
                j_descrElem.html(translateKey("lblUser") + " " + arUsrNr[chn]);
              } else {
                j_descrElem.html("");
              }
            } else {
              // MULTI_MODE_INPUT_TRANSMITTER
              homematic("Interface.getMetadata", {"objectId": ch.id, "dataId": "channelMode"}, function (result) {
                result = (result == "null") ? 1 : result;
                j_descrElem.html(translateKey("chType_MULTI_MODE_INPUT_TRANSMITTER_" + result));
              });
            }
          }
      } catch(e) {
        conInfo(e);
      }
    });
  }
});
