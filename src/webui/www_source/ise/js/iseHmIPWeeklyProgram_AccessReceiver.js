/**
 * Created by grobelnik on 04.12.2020.
 */


iseHmIPWeeklyProgramAccessReceiver = Class.create();

iseHmIPWeeklyProgramAccessReceiver.prototype = {
  initialize: function (id, opts, callback) {
    var self = this;

    virtChnCounterWP = 0;
    //conInfo(opts);
    this.callback = callback;
    this.opts = opts;
    this.id = id;
    this.devLabel = opts.deviceLabel;
    this.iface = this.opts.chInterface;

    this.doorLockStateTransmitterID = "DOOR_LOCK_STATE_TRANSMITTER";
    this.accessReceiverID  = "ACCESS_RECEIVER";
    this.permissionTransceiverID  = "PERMISSION_TRANSCEIVER";
    this.doorlockTransceiverID  = "DOOR_LOCK_TRANSCEIVER";
    this.autoRelockTransceiverID  = "AUTO_RELOCK_TRANSCEIVER";

    this.expert = (! this.opts.userEasyLinkMode) ? true : false;

    this.chAddress = this.opts.chnAddress;

    this.device = this.getDevice(this.opts.deviceID);
    this.relevantChn = this.getRelevantChannels();

    this.isDLP = (this.devLabel.indexOf("HmIP-DLP") == -1) ? false : true;

    // This can be used for later devices that require special treatment.
    /*if (this.isDeviceType("HmIP-XXX")) {
      this.relevantChn = (this.expert) ? [4, 5, 6] : [4];
    }*/

    this.anchor = jQuery("#anchor_"+this.id);
    if (! this.isDLP) {
      this.anchor.html(this.getMainHtml(this.doorLockStateTransmitterID) + this.getMainHtml(this.accessReceiverID));
    } else {
      this.anchor.html(this.getMainHtml(this.permissionTransceiverID) + this.getMainHtml(this.doorlockTransceiverID) + this.getMainHtml(this.autoRelockTransceiverID));
    }
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
    var sOutput = this.getDialogHtml(this.doorLockStateTransmitterID) + this.getDialogHtml(this.accessReceiverID);

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

   dlg.btnTextNo(translateKey("btnCancel"));
   dlg.btnTextYes(translateKey("btnOk"));
  },


  initChannelState: function() {
    var self = this,
      binChannelState = this.getBinChannelState(),
      chState, bit;

    jQuery.each(this.relevantChn, function(index, value) {
      chState = (binChannelState[index]) ? binChannelState[index] : "0";
      bit = (! self.isDLP) ? index + 1 : index;

      if (chState == "1") {
        jQuery("#"+self.id+"_bit"+bit+"1").attr("checked",true);
      } else {
        jQuery("#"+self.id+"_bit"+bit+"0").attr("checked",true);
      }

    });
  },

  getMainHtml: function(chnType) {
    var self = this,
    html = "",
    valCheckBox,
    tmpVal;

    var chType = this.getChannelOfType(chnType);

    if (chnType == this.accessReceiverID || chnType == this.permissionTransceiverID) {
      html += "<hr>";
      html += "<div style='color:white;'><u>"+translateKey('optionDoorLockUser')+"</u></div>";
    } else if (chnType == this.doorLockStateTransmitterID || chnType == this.doorlockTransceiverID) {
      html += "<div style='color:white;'><u>"+translateKey('optionDoorLockAction')+"</u></div>";
    } else if (chnType == this.autoRelockTransceiverID) {
      html += "<div style='color:white;'><u>"+translateKey('optionAutoRelock')+"</u></div>";
    }

    html += "<table style='width: 100%'>";
      html += "<thead>";
        // channel number
        html += "<tr>";
        if (chnType == this.accessReceiverID || chnType == this.permissionTransceiverID) {
          html += "<td style='text-align: left'>"+translateKey('lblUser')+"</td>";
          jQuery.each(chType, function (index, val) {
            if (chnType == self.accessReceiverID) {
              html += "<td>" + (parseInt(val) - 1) + "</td>";
            } else {
              html += "<td>" + (parseInt(val)) + "</td>";
            }
          });
        }
        html += "</tr>";
      html += "</thead>";

      html += "<tbody>";
        // row auto
        html += "<tr>";
          html += "<td style='text-align: left; width: 1%; white-space: nowrap;'>"+translateKey("stringTableClimateControlRTTransceiverAutoMode")+"</td>";
          jQuery.each(chType, function(index,val){
            html += "<td style='text-align: left;'>";
            if (! self.isDLP) {
              html += "<input id='" + self.id + "_bit" + val + "0'  type='radio' name='" + self.id + "_bit" + val + "' value=0 disabled='disabled'>";
            } else {
              tmpVal = parseInt(val) - 4;
              html += "<input id='" + self.id + "_bit" + tmpVal + "0'  type='radio' name='" + self.id + "_bit" + tmpVal + "' value=0 disabled='disabled'>";
            }
            html += "</td>";
          });
        html += "</tr>";

        // row manu
        html += "<tr>";
          html += "<td style='text-align: left; width: 1%; white-space: nowrap'>"+translateKey("stringTableClimateControlRTTransceiverManuMode")+"</td>";
          // This works if only one doorLockStateTransmitter channel is available.
          // For new devices with more of this channels this must be reworked.
          jQuery.each(chType, function(index,val) {
            if (chnType == self.doorLockStateTransmitterID || chnType == self.doorlockTransceiverID || chnType == self.permissionTransceiverID) {
              valCheckBox = (chnType == self.doorLockStateTransmitterID) ? 1 : 256; // DLD : DLP
            } else if (chnType == self.accessReceiverID || chnType == self.permissionTransceiverID ) {
              if (chnType == self.accessReceiverID) {
                valCheckBox = Math.pow(2, (index + 1));
              } else {
                valCheckBox = Math.pow(2, (index));
              }
            } else if (chnType == self.autoRelockTransceiverID) {
              valCheckBox = 512;
            }
            html += "<td style='text-align: left;'>";
            if (! self.isDLP) {
              html += "<input id='" + self.id + "_bit" + val + "1'  type='radio' name='" + self.id + "_bit" + val + "' value=" + valCheckBox + " disabled='disabled'>";
            } else {
              tmpVal = parseInt(val) - 4;
              html += "<input id='" + self.id + "_bit" + tmpVal + "1'  type='radio' name='" + self.id + "_bit" + tmpVal + "' value=" + valCheckBox + " disabled='disabled'>";
            }
            html += "</td>";
          });
        html += "</tr>";
      html += "</tbody>";
    html += "</table>";
    return html;
  },

  getDialogHtml: function(chnType) {

    var self = this,
    html = "";

    var valCheckBox,
    tmpVal;

    if (chnType == this.doorLockStateTransmitterID) {
      html += "<table align='center'>";
      html += "<tr>";
      html += "<td>" + translateKey("lblMode") + ": </td>";
      html += "<td>";
      html += "<select id='wpChannelMode_" + self.id + "'>";
      html += "<option value='MANU_MODE'>" + translateKey("stringTableClimateControlRTTransceiverManuMode") + "</option>";
      //html += "<option value='AUTO_MODE_WITH_RESET'>AUTO_WITH_RESET</option>";
      html += "<option value='AUTO_MODE_WITHOUT_RESET'>" + translateKey("stringTableClimateControlRTTransceiverAutoMode") + "</option>";
      html += "</select>";
      html += "<img src='/ise/img/help.png' style='cursor: pointer; width:18px; height:18px; position:relative; top:2px' onclick=showParamHelp(translateKey('helpWeeklyProgramDlg'),450,100)>";
      html += "</td>";
      html += "</tr>";
    }

    html += "<tr>";
    if (chnType == this.doorLockStateTransmitterID) {
      html += "<td>" + translateKey("lblDoorLock") + ": </td>";
    } else if (chnType == this.accessReceiverID) {
      html += "<td>" + translateKey("lblUser") + ": </td>";
    }
    html += "<td>";
    if (chnType == this.doorLockStateTransmitterID) {
      html += "<input name='wpChannelSel_" + self.id + "' value='1' type='checkbox'>";
    } else {
      jQuery.each(this.relevantChn, function (index, val) {
        if (index > 0) {
          valCheckBox = Math.pow(2, index);
          html += "<input name='wpChannelSel_" + self.id + "' value='" + valCheckBox + "' type='checkbox'>";
          html += "<label for='wpChannelSel_" + self.id + "'>" + (parseInt(val) - 1) + "</label>";
        }
      });
    }

    html += "</td>";
    html += "</tr>";

    if (chnType == this.accessReceiverID) {
      html += "</table>";
    }

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
    AccessReceiverID = "ACCESS_RECEIVER", // HmIP-DLD :2 - :9 = User access
    DoorLockTransmitterID = "DOOR_LOCK_STATE_TRANSMITTER", // HmIP-DLD :1 = Device behaviour
    PermissionTransceiver = "PERMISSION_TRANSCEIVER", // HmIP-DLP :4 - :11 = User access
    DoorLockTransceiver = "DOOR_LOCK_TRANSCEIVER", // HmIP-DLP :12 = Device behaviour
    AutoRelockTransceiver = "AUTO_RELOCK_TRANSCEIVER"; // HmIP-DLP :13 = Auto Relock

    jQuery.each(this.device.channels, function(index,chn) {
      if (
        (chn.channelType.indexOf(AccessReceiverID) !== -1)
        || (chn.channelType.indexOf(DoorLockTransmitterID) !== -1)
        || (chn.channelType.indexOf(PermissionTransceiver) !== -1)
        || (chn.channelType.indexOf(DoorLockTransceiver) !== -1)
        || (chn.channelType.indexOf(AutoRelockTransceiver) !== -1)
      ) {
          result.push(index);
      }
    });
    return result;
  },

  // The chType should be ACCESS_RECEIVER or DOOR_LOCK_STATE_TRANSMITTER
  getChannelOfType: function(chType) {
    var result = [];
    jQuery.each(this.device.channels, function(index,chn) {
      if (chn.channelType.indexOf(chType) != -1) {
        result.push(index);
      }
    });
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
