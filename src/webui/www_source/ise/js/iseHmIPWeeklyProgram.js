/**
 * Created by grobelnik on 01.08.2016.
 */


iseHmIPWeeklyProgram = Class.create();

iseHmIPWeeklyProgram.prototype = {
  initialize: function (id, opts, callback) {
    var self = this;
    conInfo("opts", opts);
    virtChnCounterWP = 0;
    //conInfo(opts);
    this.callback = callback;
    this.opts = opts;
    this.id = id;

    this.fwMajor = opts.fwMajor;
    this.fwMinor = opts.fwMinor;
    this.fwPatch = opts.fwPatch;

    this.devLabel = opts.deviceLabel;
    this.iface = this.opts.chInterface;
    this.chAddress = this.opts.chnAddress;

    this.device = this.getDevice(this.opts.deviceID);

    this.wiegandInterface = (this.isDeviceType("HmIP-FWI")) ? true : false;
    this.deviceIsHmIPWKP = this.isDeviceType("HmIP-WKP");
    this.deviceIsHmIP_MOD_WD_VK = this.isDeviceType("HmIP-MOD-WD-VK");
    this.deviceIsHmIP_RGBW = this.isDeviceType("HmIP-RGBW");
    this.deviceIsHmIP_DALI = this.isDeviceType("HmIP-DRG-DALI");
    this.deviceIsHmIP_FLC = (this.isDeviceType("HmIP-FLC") || (this.isDeviceType("HmIP-FDC")));

    this.isWGS = (this.device.deviceType.id.includes("HmIP-WGS")) ? true : false;
    this.isWGT = (this.device.deviceType.id.includes("HmIP-WGT")) ? true : false;

    this.expert = (! this.opts.userEasyLinkMode || this.deviceIsHmIP_FLC) ? true : false;


    this.relevantChn = this.getRelevantChannels();

    // The HmIP-BSL consists of SWITCH and DIMMER channels. For the weekly program we are currently using only the SWITCH channels.
    if (this.isDeviceType("HmIP-BSL")) {
      if (this.fwMajor < 2) {
        this.relevantChn = (this.expert) ? [4, 5, 6] : [4];
      } else {
        this.relevantChn = (this.expert) ? [4, 5, 6, 8, 9, 10, 12, 13, 14] : [4, 8, 12];
      }
    }

    if (this.deviceIsHmIPWKP) {
      this.relevantChn = [1, 3, 5, 7, 9, 11, 13, 15];
    }

    if (this.deviceIsHmIP_MOD_WD_VK) {
      this.relevantChn = [2];
    }

    if ((this.isDeviceType("HmIP-SMO230")) || (this.isDeviceType("HmIP-SMO230-A")) || (this.isDeviceType("HmIPW-SMO230")) || (this.isDeviceType("HmIPW-SMO230-A"))) {
      this.relevantChn =  [10, 11, 12];
    }

    if (this.isWGS) {
      this.relevantChn = (this.expert) ? [7, 9, 10, 11] : [7, 9];
    }

    if (this.isWGT) {
      var firstVirtSwitchChn = this.device.channels[4];
      if (firstVirtSwitchChn.channelMode == "modeBWTH") {
        this.relevantChn = [2];
      } else {
        this.relevantChn = (this.expert) ? [2, 4, 5, 6] : [2, 4];
      }
    }

    if (this.deviceIsHmIP_RGBW) {
      var oChannel = DeviceList.getChannelByAddress(this.chAddress),
      oMaintChannel = DeviceList.getChannelByAddress(this.chAddress.split(":")[0] + ":0"), // The maintenance channel stores the deviceMode
      deviceMode = parseInt(homematic("Interface.getMetadata", {"objectId": oMaintChannel.id, "dataId": "deviceMode"}));

      if (oChannel.channelType == "UNIVERSAL_LIGHT_WEEK_PROFILE") {
        switch (deviceMode) {
          case 0:
          case 1:
            // RGB/RGBW Mode
            this.relevantChn = [1];
            break;
          case 2:
            // Tunable White Mode
            this.relevantChn = [1, 2];
            break;
          default:
            // PWM Mode - all channels visible
            this.relevantChn = [1, 2, 3, 4];
        }
      }
    }

    if (this.deviceIsHmIP_DALI) {

      /*
       Das erste angeschlossene DAlI Gerät belegt Kanal 1, das nächste DALI Gerät bekommt Kanal 2 usw.
       Es können 32 DAlI Geräte angeschlossen werden, d. h. Kanal 1 - 32
       Angezeigt werden nur die Kanäle, welche auch benutzt werden. Damit kein XML-RPC Aufruf für ungenutzte Kanäle gemacht wird,
       wird die Variable lastUsedDaliChn eingesetzt.
       Die Kanäle 33 - 48 sind Gruppenkanäle und werden immer angezeigt.

       The first DAlI device connected occupies channel 1, the next DALI device gets channel 2, etc.
       32 DAlI devices can be connected, i.e. channel 1 - 32.
       Only those channels are displayed which are actually used. So that no XML-RPC call is made for unused channels,
       the variable lastUsedDaliChn is used.
       Channels 33 - 48 are group channels and are always displayed.
      */

      var self = this,
        oDevice = DeviceList.getDeviceByAddress(this.chAddress.split(":")[0]),
        lastUsedDaliChn = false,
        maxCap;

      this.relevantChn = [];

      if (typeof oDevice != "undefined") {
        jQuery.each(oDevice.channels, function (index, chn) {

          if (chn.channelType == "UNIVERSAL_LIGHT_RECEIVER") {
            if (index < 33) {
              if (!lastUsedDaliChn) {
                maxCap = parseInt(homematic("Interface.getMetadata", {"objectId": chn.id, "dataId": "maxCap"}));
              }
              if ((!isNaN(maxCap)) && (maxCap < 5)) {
                self.relevantChn.push(index);
              } else {
                lastUsedDaliChn = true;
              }
            } else {
              self.relevantChn.push(index);
            }
          }
        });
      }
    }

    this.anchor = jQuery("#anchor_"+this.id);
    this.anchor.html(this.getMainHtml());

    this.initChannelState();

    jQuery("#weekprg_"+this.id).show();
    window.setTimeout(function() {delete virtChnCounterWP;},5000);
    this.initBtnEvents();
  },

  initBtnEvents: function() {
    var that = this;
    jQuery("#setChannelMode_"+this.id).click(function(){
      var self = this;
      jQuery(this).toggleClass("ControlBtnOn");
      window.setTimeout(function(){jQuery(self).toggleClass("ControlBtnOn");},500);
      that.getModeDialog();
      window.setTimeout(function(){
        that.modeElm = jQuery("#wpChannelMode_" + that.id);
        that.chnElems = jQuery("[name='wpChannelSel_"+that.id+"']");
      },500);

    });
  },

  getModeDialog: function() {
    var that = this;
    var sOutput = this.getDialogHtml();
    var dlg = new YesNoDialog(translateKey("dialogSetWPModeTitle"), sOutput, function(result) {
      var selectedMode = that.modeElm.val(),
        selectedCh = 0;
      if (result == YesNoDialog.RESULT_YES) {
        jQuery.each(that.chnElems, function(index,elm){
          if (jQuery(elm).is(":checked")) {
            selectedCh += parseInt(jQuery(elm).val());
          }
        });
        that.selectedCh = selectedCh;
        conInfo("iface: " + that.iface + " - address: " + that.chAddress);
        conInfo("selectedMode: " + selectedMode + " - selectedCh: " + selectedCh);

        if (typeof that.callback == "undefined" ) {
          homematic("Interface.putParamset", {
            'interface': that.iface,
            'address': that.chAddress,
            'paramsetKey': 'VALUES',
            'set':
              [
                {name: 'WEEK_PROGRAM_TARGET_CHANNEL_LOCK', type: 'string', value: selectedMode},
                {name: 'WEEK_PROGRAM_TARGET_CHANNEL_LOCKS', type: 'int', value: selectedCh}
              ]
          }, function (result) {
            conInfo(result);
          });
        }
      }

      if (that.callback) {that.callback(result);}
    },"html");

    if(that.deviceIsHmIP_DALI) {
      dlg.setWidth(700);
    }

  },


  initChannelState: function() {
    var self = this,
      binChannelState = this.getBinChannelState(),
      chState,
      relevantChn = [];

    if (! this.expert) {
      relevantChn = (!this.wiegandInterface) ? this.relevantChn : [1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12];
    } else {
      relevantChn = this.relevantChn;
    }
    jQuery.each(relevantChn, function (index, value) {
      //debugger;
      chState = (binChannelState[index]) ? binChannelState[index] : "1";
      if (! self.deviceIsHmIP_DALI) {
        if (chState == "0") {
          jQuery("#" + self.id + "_bit" + index + "0").attr("checked", true); // Auto
        } else {
          jQuery("#" + self.id + "_bit" + index + "1").attr("checked", true);  // Manu
        }
      } else {
        if (chState == "0") {
          jQuery("#" + self.id + "_bit" + (value - 1) + "0").attr("checked", true); // Auto
        } else {
          jQuery("#" + self.id + "_bit" + (value - 1) + "1").attr("checked", true); // Manu
        }
      }
    });
  },

  getMainHtml: function() {
    var self = this,
    html = "";

    var valCheckBox,
    tmpVal;

    function getHTML(selectedChannels) {
      var html = "",  relevantChn;

      if (selectedChannels) {
        relevantChn = selectedChannels;
        html += "<div><hr></div>";
      } else {
        relevantChn = self.relevantChn;
      }

      html += "<table>";
      html += "<thead>";

      // channel number
      html += "<tr>";
      if (!self.deviceIsHmIPWKP) {
        html += "<td></td>";
      } else {
        html += "<td>" + translateKey('lblUser') + "</td>";
      }

      if (!self.deviceIsHmIP_MOD_WD_VK) {
        jQuery.each(relevantChn, function (index, val) {
          if (!self.deviceIsHmIPWKP) {
            html += "<td>" + val + "</td>";
          } else {
            html += "<td>" + (index + 1) + "</td>";
          }
        });
      }
      html += "</tr>";
      html += "</thead>";

      html += "<tbody>";
      // row auto
      html += "<tr>";
      html += "<td>" + translateKey("stringTableClimateControlRTTransceiverAutoMode") + "</td>";
      var _tmpIndex;
      jQuery.each(relevantChn, function (index, val) {

        if (self.wiegandInterface) {
          // index 0 - 7 = 3 - 10 - index 9 - 10 = 0 - 2
          if (index <= 7) {
            _tmpIndex = index + 3;
          } else {
            _tmpIndex = index - 8;
          }
          html += "<td>";
          html += "<input id='" + self.id + "_bit" + _tmpIndex + "0'  type='radio' name='" + self.id + "_bit" + _tmpIndex + "' value=0 disabled='disabled'>";
          html += "</td>";
        } else if (self.deviceIsHmIP_DALI) {
           html += "<td>";
          html += "<input id='" + self.id + "_bit" + (val-1) + "0'  type='radio' name='" + self.id + "_bit" + (val-1) + "' value=0 disabled='disabled'>";
          html += "</td>";
        } else {
          html += "<td>";
          html += "<input id='" + self.id + "_bit" + index + "0'  type='radio' name='" + self.id + "_bit" + index + "' value=0 disabled='disabled'>";
          html += "</td>";
        }

      });
      html += "</tr>";

      // row manu
      html += "<tr>";
      html += "<td>" + translateKey("stringTableClimateControlRTTransceiverManuMode") + "</td>";
      jQuery.each(relevantChn, function (index, val) {
        if (self.wiegandInterface) {
          //******************
          if (self.expert) {
            // index 0 - 7 = 3 - 10 - index 9 - 10 = 0 - 2
            if (index <= 7) {
              _tmpIndex = index + 3;
            } else {
              _tmpIndex = index - 8;
            }
            valCheckBox = Math.pow(2, _tmpIndex);
          } else {
            if (index <= 7) {
              _tmpIndex = index + 3;
              valCheckBox = Math.pow(2, _tmpIndex);
            } else {
              _tmpIndex = index - 8;
              valCheckBox = 1;
            }
          }
          html += "<td>";
          html += "<input id='" + self.id + "_bit" + _tmpIndex + "1'  type='radio' name='" + self.id + "_bit" + _tmpIndex + "' value=" + valCheckBox + " disabled='disabled'>";
          html += "</td>";
          //******************
        } else if (self.deviceIsHmIP_DALI) {
          valCheckBox = Math.pow(2, (val-1));
          html += "<td>";
          html += "<input id='" + self.id + "_bit" + (val-1) + "1'  type='radio' name='" + self.id + "_bit" + (val-1) + "' value=" + valCheckBox + " disabled='disabled'>";
          html += "</td>";
        } else {
          if (self.expert) {
            valCheckBox = Math.pow(2, index);
          } else {
            if (index == 0) {
              valCheckBox = 1;
              tmpVal = 1;
            } else {
              valCheckBox = tmpVal << 3;
              tmpVal = valCheckBox;
            }
          }
          html += "<td>";
          html += "<input id='" + self.id + "_bit" + index + "1'  type='radio' name='" + self.id + "_bit" + index + "' value=" + valCheckBox + " disabled='disabled'>";
          html += "</td>";
        }
      });
      html += "</tr>";
      html += "</tbody>";
      html += "</table>";

      return html;

    };

    if (! this.deviceIsHmIP_DALI) {
      html += getHTML();
    } else {
    // DALI - Create one row for the normal channels and one row for the group channels.
    var self = this,
      oDevice = DeviceList.getDeviceByAddress(this.chAddress.split(":")[0]),
      lastUsedDaliChn = false,
      maxCap;

      var chnInUse = [];

      jQuery.each(oDevice.channels, function(index, chn) {

        if (chn.channelType == "UNIVERSAL_LIGHT_RECEIVER") {
          if (index < 33) {
            if (! lastUsedDaliChn) {
              maxCap = parseInt(homematic("Interface.getMetadata", {"objectId": chn.id, "dataId": "maxCap"}));
            }
            if ((!isNaN(maxCap)) && (maxCap < 5)) {
              chnInUse.push(index);
            } else {
              lastUsedDaliChn = true;
            }
          }
        }
      });

      html += getHTML(chnInUse);
      // DALI group channels
      chnInUse = [];
      for (var i = 33; i <= 48; i++) {
        chnInUse.push(i);
      }
      html += getHTML(chnInUse);
    }
    return html;
  },

  getDialogHtml: function() {
    var self = this,
      newTR = false,
      html = "";

    var valCheckBox,
    tmpVal;

    html += "<table align='center'>";
      html += "<tr>";
          html += "<td>"+translateKey("lblMode")+": </td>";
          html += "<td>";
          html += "<select id='wpChannelMode_"+self.id+"'>";
            html += "<option value='MANU_MODE'>"+translateKey("stringTableClimateControlRTTransceiverManuMode")+"</option>";
            //html += "<option value='AUTO_MODE_WITH_RESET'>AUTO_WITH_RESET</option>";
            html += "<option value='AUTO_MODE_WITHOUT_RESET'>"+translateKey("stringTableClimateControlRTTransceiverAutoMode")+"</option>";
          html += "</select>";
        html+= "</td>";
      html += "</tr>";

        if (! self.deviceIsHmIP_MOD_WD_VK) {
          html += "<tr>";
        } else {
          html += "<tr class='hidden'>";
        }
        if (! this.deviceIsHmIPWKP) {
          html += "<td>" + translateKey("btnChooseChannel") + ": </td>";
        } else {
          html += "<td>" + translateKey("lblUser") + ": </td>";
        }
        html += "<td>";
        jQuery.each(this.relevantChn, function (index, val) {
          if (!self.wiegandInterface) {
            if (self.expert) {
              valCheckBox = Math.pow(2, index);
            } else {
              if (index == 0) {
                valCheckBox = 1;
                tmpVal = 1;
              } else {
                valCheckBox = tmpVal << 3;
                tmpVal = valCheckBox;
              }
            }
          } else {
            // Wiegand
            if (self.expert) {
              if (index <= 7) {
                tmpVal = index + 3;
              } else {
                tmpVal = index - 8;
              }
              valCheckBox = Math.pow(2, tmpVal);
            } else {
              if (index <= 7) {
                tmpVal = index + 3;
                valCheckBox = Math.pow(2, tmpVal);
              } else {
                tmpVal = index - 8;
                valCheckBox = 1;
              }
            }
          }
          if (! self.deviceIsHmIP_MOD_WD_VK) {
            html += "<input name='wpChannelSel_" + self.id + "' value='" + valCheckBox + "' type='checkbox'>";
          } else {
            html += "<input name='wpChannelSel_" + self.id + "' value='" + valCheckBox + "' type='checkbox' checked>";
          }
          if (!self.deviceIsHmIPWKP) {
            html += "<label for='wpChannelSel_" + self.id + "'>" + val + "</label>";
          } else {
            html += "<label for='wpChannelSel_" + self.id + "'>" + (index + 1) + "</label>";
          }

          if (newTR == false) {
            if ((self.deviceIsHmIP_DALI) && (self.relevantChn[(index + 1)] - 32 > 0)) {
              html += "</td>";
              html += "</tr>";
              html += "<tr><td>"+translateKey('btnSettingsGroups')+"</td><td>";
              newTR = true;
            }
          }
        });

        html += "</td>";
        html += "</tr>";

    html += "</table>";
    return html;
  },

  getDevice: function(id) {
    var device = DeviceList.getDevice(this.opts.deviceID);
    if (typeof device != "object") {
      device = homematic("Device.get", {"id": id});
    }
    return device;
  },

  getRelevantChannels: function() {
    var self = this,
    result = [],
    virtualChID = "_VIRTUAL_RECEIVER",
    AccessReceiverID = "ACCESS_RECEIVER", // HmIP-DLD
    AccessTransceiverID = "ACCESS_TRANSCEIVER", // HmIP-FWI (Wiegand Iface)
    DoorLockTransmitterID = "DOOR_LOCK_STATE_TRANSMITTER", // HmIP-DLD
    OpticalSignalID = "OPTICAL_SIGNAL_RECEIVER", // HmIPW-WRC6
    UniversalLightReceiver = "UNIVERSAL_LIGHT_RECEIVER", // HmIP-RGBW
    PermissionTranseiverID = "PERMISSION_TRANSCEIVER", // HmIP-FLC/FLD
    SwitchTranseiverID = "SWITCH_TRANSCEIVER", // HmIP-FLC/FLD
    expertChn;

    jQuery.each(this.device.channels, function(index,chn) {
      if (
        (chn.channelType.indexOf(virtualChID) !== -1)
        || (chn.channelType.indexOf(AccessReceiverID) !== -1)
        || (chn.channelType.indexOf(AccessTransceiverID) !== -1)
        || (chn.channelType.indexOf(DoorLockTransmitterID) !== -1)
        || (chn.channelType.indexOf(OpticalSignalID) !== -1)
        || (chn.channelType.indexOf(UniversalLightReceiver) !== -1)
        || (chn.channelType.indexOf(PermissionTranseiverID) !== -1)
        || (chn.channelType.indexOf(SwitchTranseiverID) !== -1)
      ) {
        if (self.expert) {
          result.push(index);
        } else {
          expertChn = self.getOnlyExpertChannels(chn.channelType, index);
           if (expertChn) {
             result.push(expertChn);
           }
        }
      }
    });
    return result;
  },

  getOnlyExpertChannels: function(channelType, channelNr) {
    var result = null, self=this;

      if (
        channelType == "DIMMER_VIRTUAL_RECEIVER" ||
        channelType == "SWITCH_VIRTUAL_RECEIVER" ||
        channelType == "WATER_SWITCH_VIRTUAL_RECEIVER" ||
        channelType == "BLIND_VIRTUAL_RECEIVER" ||
        channelType == "SHUTTER_VIRTUAL_RECEIVER" ||
        channelType == "ACOUSTIC_SIGNAL_VIRTUAL_RECEIVER" ||
        channelType == "SERVO_VIRTUAL_RECEIVER"
      ) {
        virtChnCounterWP = (virtChnCounterWP >= 3) ? 0 : virtChnCounterWP;
        virtChnCounterWP++;
        if (virtChnCounterWP == 1) {
          return channelNr;
        } else {
          return null;
        }
      }  else if (
        channelType == "ACCESS_RECEIVER"
        || channelType == "ACCESS_TRANSCEIVER"
        || channelType == "DOOR_LOCK_STATE_TRANSMITTER"
        || channelType == "OPTICAL_SIGNAL_RECEIVER"
        || channelType == "UNIVERSAL_LIGHT_RECEIVER"
        || channelType == "PERMISSION_TRANSCEIVER"
        || channelType == "SWITCH_TRANSCEIVER")
        {
          return channelNr;
        }

    return result;
  },

  // Checks if the device type is of a particular kind
  // This is useful for the treatment of special cases (e.g. the HmIP-BSL which is a DIMMER_WEEKLY_PROFILE but must be treated as a SWITCH_WEEKLY_PROFILE
  isDeviceType: function(devType) {
    return (this.devLabel == devType) ? true : false;
  },

  getBinChannelState: function() {
    var missingZero = "",
    tmp = "",
    bVal = this.opts.channelLocks.toString(2);

    jQuery.each(this.relevantChn, function(index, value) {
      missingZero += "0";
    });

    bVal = missingZero.substr(bVal.length)+bVal;
    bVal = this.reverseString(bVal);
    if (! this.expert) {

      if (! this.wiegandInterface) {
        for (var x = 0; x < bVal.length; x += 3) {
          tmp += bVal[x];
        }
        bVal = tmp;
      } else {
        // 3 virtual switch actor channels _ Bit 1, 2, 3
        // 8 access control channels - Bit 4 - 11
        for (var loop = 0; loop <= 10; loop++) {
          tmp += (bVal.charAt(loop) != "") ? bVal.charAt(loop) : "0";
        }
        bVal = tmp;
      }
    }
    return bVal;
  },

  reverseString: function (str) {
    return str.split("").reverse().join("");
  },

  getConfigString: function() {
    var arMode = ["MANU_MODE", "AUTO_MODE_WITH_RESET", "AUTO_MODE_WITHOUT_RESET"];
    return "WPTCLS="+this.selectedCh+",WPTCL="+arMode.indexOf(this.modeElm.val());
  }
};
