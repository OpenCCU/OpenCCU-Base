/**
 * ic_deviceparameters.js
 **/

CloseDeviceParameters = function()
{
  WebUI.goBack();
};

ToggleChannelView = function()
{
  var ChannelTable = jQuery("#id_channel_parameters_table");
  var ToggleButton = jQuery("#ToggleButtonChannelView:first-child");

  if (!ToggleButton.attr("name")){
    ToggleButton.attr("name","btnOpen");
  }

  if(ToggleButton.attr("name") == "btnOpen" )
  {
    ChannelTable.css({"display": "none","visibility":"hidden"});
    ToggleButton.attr("name","btnClose");
    ToggleButton.html(translateKey("deviceAndChannelParamsBtnOpenParamList"));
  }
  else
  {
    ChannelTable.css({"display": "", "visibility":"visible"});
    ToggleButton.attr("name","btnOpen");
    ToggleButton.html(translateKey("deviceAndChannelParamsBtnCloseParamList"));
  }
};


isDutyCycleOK4DevUpdate = function() {
  var iface = homematic("Interface.listBidcosInterfaces", {"interface": "BidCos-RF"});
  var dcVal = 0,
    dcWarningLevel = 80,
    dcOK = true;

  if(iface === null || iface.length === 0) {
    iface = homematic("Interface.listBidcosInterfaces", {"interface": "HmIP-RF"});
  }

  if(iface !== null) {
    jQuery.each(iface, function (index, iFace) {
      if (iFace.type == "CCU2") {
        dcVal = (typeof iFace.dutyCycle != "undefined") ? parseInt(iFace.dutyCycle) : 0;
        if (dcVal >= dcWarningLevel) {
          dcOK = false;
        }
        return false; //leave each loop
      }
    });
  }
  return dcOK;
};

FirmwareUpdate = function(devType) {
  var isHmIPWired = false;
  if (devType) {
   isHmIPWired =  isDevTypeHmIPW(devType);
  }

  if ((isHmIPWired) || isDutyCycleOK4DevUpdate()) {
    ResetPostString();
    AddParam($('global_sid'));
    poststr += "&iface=" + $F('global_iface');
    poststr += "&address=" + $F('global_address');
    poststr += "&cmd=firmware_update";
    SendRequest('ic_ifacecmd.cgi');
    ProgressBar = new ProgressBarMsgBox(translateKey("performingFirmwareUpdate"), 1);
    ProgressBar.show();
    ProgressBar.StartKnightRiderLight();
  } else {
    MessageBox.show(translateKey("dialogHint"), translateKey("hintDevFwUpdateDCHigh"), "", 400, 120);
  }
};



isHeatingControl = function() {
  if (window.P1_tom || window.P2_tom || window.P3_tom || window.P4_tom || window.P5_tom || window.P6_tom) {
    return true;
  }
  return false;
};

isHeatingControlAddress = function(deviceAddress) {
  if (P1_tom.address == deviceAddress || P2_tom.address == deviceAddress || P3_tom.address == deviceAddress || P4_tom.address == deviceAddress || P5_tom.address == deviceAddress || P6_tom.address == deviceAddress) {
    return true;
  }
  return false;
};

isHeatingControlDirty = function() {
  if (P1_tom.tom_isDirty() || P2_tom.tom_isDirty() || P3_tom.tom_isDirty() || P4_tom.tom_isDirty() || P5_tom.tom_isDirty() || P6_tom.tom_isDirty()) {
    return true;
  }
  return false;
};

SaveDeviceParameters = function()
{
  var actions = 0,
  parentChannelAddress;

  //Geräteparameter speichern
  if (AreParametersDirty('DEVICE'))
  {
    SetParameters ($F('global_iface'), $F('global_address'), 'DEVICE');
    actions++;
  }

  //Kanalparameter speichern
  var ch_count = $F('global_channel_count');
  var channel_address;
  var startChannel = (($F('global_iface')!= "HmIP-RF") && ($F('global_iface')!= "HmIP-Wired") && ($F('global_iface') != "VirtualDevices"))? 1 : 0;

  for (var i=startChannel; i<ch_count; i++) //(Kanal 0 ist bei BidCos-RF / Wired der Maintenance-Kanal, der nicht in seinen Parametern verändert werden kann.)
  {
    channel_address = $F('global_channel_address_' +(i+1) );

    if (AreParametersDirty('CHANNEL_' +i) )
    {
      SetParameters ($F('global_iface'), channel_address, 'CHANNEL_'+i);
      actions++;
    }

    if($('chInternalPeers_' + i)) {
      // Alle Adressen der internen Links
      var _internalLinks = $('chInternalPeers_' + i).innerHTML,
      internalLinks = _internalLinks.split(" ");
  
      for (var intKey = 1; intKey <= internalLinks.length; intKey++) {
        var selectelem;
        try { 
          selectelem = $('receiver_' + i + '_' + intKey + '_profiles');
          if (selectelem) throw "internalKey";
        } catch (result) { 
          if (result == 'internalKey') {
          
            var pnr = selectelem.options[selectelem.selectedIndex].value;
            if (AreParametersDirty('receiver_' + i + '_' + intKey + '_' + pnr) || IsInternalKeyDirty('receiver_' + i + '_' + intKey + '_profiles'))
            {
              SetInternalKey ($F('global_iface'), channel_address, 'receiver_' + i + '_' + intKey, internalLinks[intKey - 1]);
              actions++;
            }
          }
        } 
      }    
    }

    //Timeout-Module?
    if (window.tom)
    {
      if ((tom.iface == $F('global_iface')) && (tom.address == channel_address) && (tom.tom_isDirty()))
      {
        SetParameters ($F('global_iface'), channel_address, 'TIMEOUTMANAGER');
        actions++;
      }
      //-----
    }
  }

  try {
    //Timeout-Module
    if (window.tom)
    {
      var deviceAddress = $F('global_address');
      if ((tom.iface == $F('global_iface')) && (tom.address ==  deviceAddress ) && (tom.tom_isDirty()))
      {
        SetParameters ($F('global_iface'), deviceAddress, 'TIMEOUTMANAGER');
        actions++;
      }
      //-----
    }

    //Timeout-Module for the new wallmounted heating control?
    if (isHeatingControl()) {
      var deviceAddress = $F('global_address');
      if (($F('global_iface') != "HmIP-RF") && ($F('global_iface') != "VirtualDevices")) {
        if ((P1_tom.iface == $F('global_iface')) && isHeatingControlAddress(deviceAddress) && isHeatingControlDirty()) {
          SetParameters($F('global_iface'), deviceAddress, 'TIMEOUTMANAGER');
          actions++;
        }
      } else {
        var HMIpTomAddress = P1_tom.address;
        if ((P1_tom.iface == $F('global_iface')) && isHeatingControlDirty()) {
          SetParameters($F('global_iface'), HMIpTomAddress, 'TIMEOUTMANAGER');
          actions++;
        }
      }
    }

    //
    if (window.P1_tomHmIP) {
      var HMIpTomAddress = P1_tomHmIP.address;
      SetParameters ($F('global_iface'), HMIpTomAddress, 'TIMEOUTMANAGER_HMIP_OnOff');
      actions++;
    }
  } catch (e) {}
  //-----

  if (actions === 0)
  {
    CheckConfigPending($F('global_iface'), $F('global_address'), '', $F('global_redirect_url'), true);
  }
  else 
  {
    //ProgressBar = new ProgressBarMsgBox("Übertrage Geräte-/Kanaleinstellungen an Komponenten...", actions);
    ProgressBar = new ProgressBarMsgBox(translateKey("transferConfigData"), actions);
    ProgressBar.OnFinish = function ()
    {
      CheckConfigPending($F('global_iface'), $F('global_address'), '', $F('global_redirect_url'), true);
    };
    ProgressBar.show();
    ProgressBar.StartKnightRiderLight();
  }
};

AreParametersDirty = function(special_input_id)
{
  var i = 1;
  var inputelem = $('separate_' + special_input_id + '_' + i);
  
  while (inputelem)
  {
    if (IsDirty(inputelem)) return true;

    //Integer und Floats werden über ein zweites input-element eingestellt. das hidden-input ist relevant,
    //jedoch ist es immer "clean" aufgrund seines Typs. Deshalb muss das dazugehörende input-element noch 
    //geprüft werden (wenn es eines gibt).
    if (IsDirty($('separate_' + special_input_id + '_' + i + '_temp'))) return true;

    i++;
    inputelem = $('separate_' + special_input_id + '_' + i);
  }

  return false;
};

IsInternalKeyDirty = function(special_input_id)
{
  if (IsDirty($(special_input_id ))) return true;
  else return false;
};

//special_input_id: DEVICE|CHANNEL_<0..n>|TIMEOUTMANAGER
SetParameters = function(iface, address, special_input_id)
{
  var command,
    tomIsSet = false;
  
  try {
    
      if ($('separate_CHANNEL_1_1').name == "TEAM" && special_input_id != "DEVICE")
      throw "TEAM";
      else throw "noTEAM";
    } catch (e) {
      if (e == "TEAM") { command = "&cmd=set_team"; }
      else                     { command = "&cmd=set_profile"; }
    }

    ResetPostString();
    AddParam($('global_sid'));
    poststr += "&iface="   + iface;
    poststr += "&address=" + address;
    poststr += "&peer=MASTER";
    poststr += "&ps_type=MASTER";
    poststr += "&paramid=";
    poststr += "&pnr=";
    poststr += command;  


    if (typeof oChnMultiModeTransmitter != "undefined") {
      var relevantMultiModeTransmitter =  oChnMultiModeTransmitter[address.toString()];

      if (typeof relevantMultiModeTransmitter != "undefined") {


        homematic("Interface.setMetadata_crRFD", {
          'interface': iface,
          'objectId' : address.toString(),
          'dataId' : relevantMultiModeTransmitter.dataId,
          'value': relevantMultiModeTransmitter.mode});

        homematic("Interface.setMetadata", {
          "objectId": relevantMultiModeTransmitter.channelId,
          "dataId": relevantMultiModeTransmitter.dataId,
          "value": relevantMultiModeTransmitter.mode
        }, function (result) {
          var oChannel = DeviceList.getChannelByAddress(address.toString());
          oChannel.setMultiMode(relevantMultiModeTransmitter.mode);
          delete oChnMultiModeTransmitter[address.toString()];
        });
      }
    }

    // After setting the config parameters of a dali device, we have to store the UNIVERSAL_LIGHT_MAX_CAPABILITIES of each group channel as mata data.
   // Otherwise ReGa can't determne the correct value on the page Status/Control, especially when changing a group.
    var channel = DeviceList.getChannelByAddress(address);
    if ((typeof channel !== "undefined") && (channel.typeName == "HmIP-DRG-DALI") && (typeof MetaDaliGroupHasBeenSet == "undefined")) {
      // After 5 seconds we can determine the new UNIVERSAL_LIGHT_MAX_CAPABILITIES of the DALI group channels and store them as meta data
      window.setTimeout(function() {
        var devAddress = channel.device.address,
        grpChannel;

        for (var loop = 33; loop <= 48; loop++) {
         var maxCap = homematic("Interface.getMasterValue", {
           "interface": "HmIP-RF",
           "address": devAddress + ":" + loop,
           "valueKey": "UNIVERSAL_LIGHT_MAX_CAPABILITIES"
         });
         grpChannel = DeviceList.getChannelByAddress(devAddress + ":" + loop);
         homematic("Interface.setMetadata", {"objectId": grpChannel.id , "dataId" : "maxCap", "value": maxCap});
         DeviceList.channels[grpChannel.id].daliMaxCapabilities = maxCap;
        }
        window.setTimeout(function() {delete MetaDaliGroupHasBeenSet;},5000);
      },5000);
      MetaDaliGroupHasBeenSet = true;
    }



    if ((special_input_id == "TIMEOUTMANAGER") && (typeof tom == "object") && (tom.iface == iface) && (tom.address == address) && (tom.tom_isDirty()))
    {
      poststr += tom.tom_getPostStr();
      tomIsSet = true;
    }

    if((special_input_id == "TIMEOUTMANAGER") && (typeof P1_tom == "object") && (P1_tom.iface == iface) && (P1_tom.address == address) && (P1_tom.tom_isDirty())) {
      poststr += P1_tom.tom_getPostStr();
      tomIsSet = true;
    }

    if((special_input_id == "TIMEOUTMANAGER") && (typeof P2_tom == "object") && (P2_tom.iface == iface) && (P2_tom.address == address) && (P2_tom.tom_isDirty())) {
      poststr += P2_tom.tom_getPostStr();
      tomIsSet = true;
    }

    if((special_input_id == "TIMEOUTMANAGER") && (typeof P3_tom == "object") && (P3_tom.iface == iface) && (P3_tom.address == address) && (P3_tom.tom_isDirty())) {
      poststr += P3_tom.tom_getPostStr();
      tomIsSet = true;
    }

    if((special_input_id == "TIMEOUTMANAGER") && (typeof P4_tom == "object") && (P4_tom.iface == iface) && (P4_tom.address == address) && (P4_tom.tom_isDirty())) {
      poststr += P4_tom.tom_getPostStr();
      tomIsSet = true;
    }
    if((special_input_id == "TIMEOUTMANAGER") && (typeof P5_tom == "object") && (P5_tom.iface == iface) && (P5_tom.address == address) && (P5_tom.tom_isDirty())) {
      poststr += P5_tom.tom_getPostStr();
      tomIsSet = true;
    }
    if((special_input_id == "TIMEOUTMANAGER") && (typeof P6_tom == "object") && (P6_tom.iface == iface) && (P6_tom.address == address) && (P6_tom.tom_isDirty())) {
      poststr += P6_tom.tom_getPostStr();
      tomIsSet = true;
    }

    if((special_input_id == "TIMEOUTMANAGER_HMIP_OnOff") && (typeof P1_tomHmIP == "object") && (P1_tomHmIP.iface == iface) && (P1_tomHmIP.address == address) && (P1_tomHmIP.tom_isDirty())) {
      poststr += P1_tomHmIP.tom_getPostStr();
      tomIsSet = true;
    }

    if(!tomIsSet) {
      AddSeparateSettings('separate_' + special_input_id, '');
    }
 
   try 
   {
    var device = DeviceList.getDeviceByAddress(address);   
    if (!device)
    {
      var channel = DeviceList.getChannelByAddress(address);
      if (channel) { device = channel.device; }
    }
   } catch (e){}

  /*
   var device = DeviceList.getDeviceByAddress(address);
  if (!device)
  {
    var channel = DeviceList.getChannelByAddress(address);
    if (channel) { device = channel.device; }
  }
  */

  // This prevents an non-existing string for MAIN_/SUB_TEXT for e. g.  the HmIPW-WGD(-PL)
  if ((typeof channel !== "undefined") && ((channel.channelType == "DISPLAY_INPUT_TRANSMITTER") || (channel.channelType == "DISPLAY_THERMOSTAT_INPUT_TRANSMITTER") || (channel.channelType == "ENERGIE_METER_TRANSMITTER"))) {
    arPostStr = poststr.split("&");
    jQuery.each(arPostStr, function (index, val) {
      if (val == "MAIN_TEXT=" || val == "SUB_TEXT=" || val == "METER_OBIS_SEARCH_STRING=") {
        // %24 = $
        poststr = poststr.replace(val, val + "%24%24%24%24%24");
      }
    });
  }

  // Here we change the value of the config param COND_TX_THRESHOLD_LO/HI from a user input of e.g. 1013.5 hPa to 101350
  if ((typeof channel !== "undefined") && (channel.typeName == "ELV-SH-CAP") && (channel.channelType == "COND_SWITCH_TRANSMITTER") && (channel.index == 3)) {
    var arPostStr = poststr.split("&"), arValue, val100;
    jQuery.each(arPostStr, function (index, val) {
      if (val.includes("COND_TX_THRESHOLD_LO=") || val.includes("COND_TX_THRESHOLD_HI=")) {
        arValue = val.split("=");
        val100 = parseFloat(arValue[1]) * 100;
        poststr = poststr.replace(val, arValue[0] + "=" + val100.toString());
      }
    });
  }



  SendRequest('ic_ifacecmd.cgi', null, function() {
    if (device)
    {
      DeviceList.beginUpdateDevice(device.id);
    }
  });

};

SendInternalKeyPress = function(iface, sender, receiver, longKeyPress)
{
  var simLongKeyPress = (longKeyPress) ? 1 : 0;
  ResetPostString();
  AddParam($('global_sid'));
  poststr += "&iface=" + iface;
  poststr += "&sender=" + sender;
  poststr += "&receiver=" + receiver;
  poststr += "&longKeyPress=" + simLongKeyPress;
  poststr += "&cmd=SendInternalKeyPress";
  SendRequest('ic_ifacecmd.cgi');
};

SetInternalKey = function(iface, address, special_input_id, peerAddress)
{
  var mainChannel = address;
 
  var ch = address.split(':')[1];
  var selectelem = $(special_input_id + '_profiles');
  var pnr = selectelem.options[selectelem.selectedIndex].value;
  var actorType = $('chType_'+ch).innerHTML;
  var paramid = $('chParamID_'+ch).innerHTML;

  ResetPostString();
  AddParam($('global_sid'));
  poststr += "&iface="   + iface;
  poststr += "&address=" + mainChannel; 
  poststr += "&peer="    + peerAddress;
  poststr += "&pnr="     + pnr;
  poststr += "&paramid=" + paramid;  
  poststr += "&ps_type=receiver";
  poststr += "&ps_id=receiver" ;
  poststr += "&internalKey=true";
  poststr += "&new_profilepath=" + actorType + "/" + actorType;
  poststr += "&cmd=set_profile";
  AddSeparateSettings('separate_' + special_input_id + '_', pnr);
  AddSeparateSettings('subset_'   + special_input_id + '_', pnr);
  
  //alert(poststr);
  SendRequest('ic_ifacecmd.cgi');
};

ShowInternalKeyProfile = function(selectelem, channel, counter)
{
  var e;
  var pnr = selectelem.options[selectelem.selectedIndex].value;
  e = $('internalKey_' + counter + "_" + channel);

  while ((e !== null) && (e.tagName != "TABLE") && (e.className != "ProfileTbl"))
  {
    e = e.nextSibling;
  }

  if (e.tagName != "TABLE" && e.className != "ProfileTbl") return;
  
  var table = e;
  var elem  = table.getElementsByTagName("tr")[0];

  while (elem !== null)
  {
    //Element.hide(elem);
    elem.style.display = "none";
    elem.style.visibility = "hidden";
    if ((elem.className == "receiver_"+counter + "_" + channel + "_" + pnr) )
    {
      elem.style.visibility = "visible";
      elem.style.display = "";
    }
    elem = elem.nextSibling;
  }
};


