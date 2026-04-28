/**
 * ic_common.js
 **/

poststr = "";
UI_PATH = "/config/";

Get_UI_CONTENTBOX_ID = function()
{
  //Muss als Funktion abgefragt werden, weil zur Zeit des Ladens
  //die Boxen noch nicht vollständig existieren.
  return ( document.getElementById('infobox') ? 'infobox' : 'centerbox'  );
};

ResetPostString = function()
{
  poststr = "";
};

AddProfileValues = function(prefix)
{
  var i = 1;

  var elem;
  while (elem = document.getElementById(prefix + i))
  {
    AddParam(elem);
    i++;
  }
};

AddSeparateSettings = function(prefix, pnr)
{
  var i = 1,
  elem;
  while (elem = document.getElementById(prefix + pnr + '_' + i))
  {
    // add ALL parameters (do NOT use IsDirty()) because
    // AddSeparateSettings is also used while modifiying
    // device link paramaters which requires all parameters
    // to be present.
    AddParam(elem);
    i++;
  }
};

SubmitProfile = function(pnr, pname)
{
  ShowLoadingBox('Bitte warten, das Profil \"' +pname+ '\" wird gespeichert...');
  ResetPostString();
  document.getElementById('profile').value = pnr;
  AddParam(document.getElementById('profile'));
  AddProfileValues('global_');
  AddSeparateSettings('separate_', pnr);
  AddSeparateSettings('subset_', pnr);
  SendRequest('ic_paramset.cgi');
};

AddParam = function(elem)
{
  if (elem !== null)
  {
    if (elem.name !== "")
    {
      if (poststr !== "") poststr += "&";

      if (elem.type == "checkbox")
      {
        if (elem.checked) { elem.value = "true";  }
        else              { elem.value = "false"; }
      }
       
      var arrId = elem.id.split("_");
      var type = arrId[1];
          
      // falls es sich um Kanalparameter handelt (die ID ist länger)
      if (arrId.length == 6) {
        type = arrId[1] + "_" + arrId[2] + "_" + arrId[3]; 
      } 
         
      if (elem.value == "99999999" ) 
      {  
          var sec  = document.getElementById("sec_"  + prefix[elem.name + type]);
          var min = document.getElementById("min_"  + prefix[elem.name + type]);
          var hour = document.getElementById("hour_"  + prefix[elem.name + type]);
     
          free_options = (hour.value * 3600) + (min.value * 60) + (sec.value * 1);  
          // elem.name = "SHORT_ON_TIME|LONG_ON_TIME" usw 
          poststr += elem.name + "=" + parseFloat(free_options);
        } 
        else if (elem.value == "99999998" )
        {
          var percent = document.getElementById("percent_" + prefix[elem.name + type]);  
          
          free_options = percent.value / 100 ;
          poststr += elem.name + "=" + parseFloat(free_options);
        }
        else if (elem.value == "99999997" )
        {
          var temp = document.getElementById("temp_" + prefix[elem.name + type]);  
          
          // falls Fahrenheit eingestellt, dann Wert umrechnen
          if ( temp.value >= 31 ) free_options = ((temp.value - 32) / 9 * 5);
          else free_options = temp.value ;
          
          poststr += elem.name + "=" + parseFloat(free_options);
        }
        else if (elem.value == "99999996" ) // Expertenmodus HM-Sen-EP
        {
          // Kanal aus poststr herausfiltern
          type = poststr.split("=");
          ad = type[3];
          type = ad.split(":");
          ad = type[1];
          type = ad.split("&");
          ch = type[0];
          // Der Kanal steht jetzt in ch zur Verfuegung  
          poststr += "SEQUENCE_PULSE_1="    +  $F('free_CHANNEL_' + ch + '_1') ;
          poststr += "&SEQUENCE_PULSE_2="    +  $F('free_CHANNEL_' + ch + '_2') ;    
          poststr += "&SEQUENCE_PULSE_3="    +  $F('free_CHANNEL_' + ch + '_3') ;    
          poststr += "&SEQUENCE_PULSE_4="    +  $F('free_CHANNEL_' + ch + '_4') ;    
          poststr += "&SEQUENCE_PULSE_5="    +  $F('free_CHANNEL_' + ch + '_5') ;    
          poststr += "&SEQUENCE_TOLERANCE="  +  $F('free_CHANNEL_' + ch + '_6') ;
        }
        else if (elem.value == "99999995" )
        {
          poststr += "SEQUENCE_PULSE_1=0.496" ; 
          poststr += "&SEQUENCE_PULSE_2=0.496" ;
          poststr += "&SEQUENCE_PULSE_3=0.496" ;
          poststr += "&SEQUENCE_PULSE_4=0.496" ;
          poststr += "&SEQUENCE_PULSE_5=0.496" ;
          poststr += "&SEQUENCE_TOLERANCE=0.496" ;
        }
        else if (elem.value == "99999994" )
        {
          poststr += "SEQUENCE_PULSE_1=0.496" ;
          poststr += "&SEQUENCE_PULSE_2=0.496" ;
          poststr += "&SEQUENCE_PULSE_3=0.496" ;
          poststr += "&SEQUENCE_PULSE_4=0" ;
          poststr += "&SEQUENCE_PULSE_5=0" ;
          poststr += "&SEQUENCE_TOLERANCE=0.496" ; 
        }
        else if (elem.value == "99999993" )
        {
          poststr += "SEQUENCE_PULSE_1=0.496" ;
          poststr += "&SEQUENCE_PULSE_2=0" ;
          poststr += "&SEQUENCE_PULSE_3=0" ;
          poststr += "&SEQUENCE_PULSE_4=0" ; 
          poststr += "&SEQUENCE_PULSE_5=0" ; 
          poststr += "&SEQUENCE_TOLERANCE=0.496" ; 
        }
        else if (elem.value == "99999992" )
        {
          poststr += "SEQUENCE_PULSE_1=0" ;
          poststr += "&SEQUENCE_PULSE_2=0" ;
          poststr += "&SEQUENCE_PULSE_3=0" ;
          poststr += "&SEQUENCE_PULSE_4=0" ; 
          poststr += "&SEQUENCE_PULSE_5=0" ;
          poststr += "&SEQUENCE_TOLERANCE=0.496" ; 
        }
        else if (elem.value == "99999990" )
        {
          var freeVal = document.getElementById("val_" + prefix[elem.name + type]);
          free_options = freeVal.value;
          poststr += elem.name + "=" + parseFloat(free_options);
        }
      
        //else   poststr += elem.name + "=" + elem.value;
        else poststr += elem.name + "=" + encodeURIComponent(elem.value);  
    }
  }
};

Get_ReGa_Path = function(path)
{
  //WebUI läuft von ise aus (Session-Parameter ist "sid")
  //der Pfad fängt nicht mit dem vom ise WebUI aus nötigen Prefix an
  if ( ($('global_sid').name == "sid") && (path.substr(0, UI_PATH.length ) != UI_PATH))
  {
    //Prefix anfügen:
    path = UI_PATH + path;
  }

  return path;
};

SendRequest = function(scriptname, html_container_id, callback)
{
  scriptname = Get_ReGa_Path(scriptname);
  var params = poststr;

  if (params === "") { params += "?AvoidBrowserCache=" + Math.random(); }
  else               { params += "&AvoidBrowserCache=" + Math.random(); }

  if ((html_container_id) && (html_container_id !== "")) { id = html_container_id; }
  else                                                   { id = Get_UI_CONTENTBOX_ID(); }

  var _callback_ = callback; 
  
  var opt =
  {
    method:'get',
    parameters:params,
    evalScripts:true,
    onComplete: function() { if (_callback_) { _callback_(); } }
  };

  new Ajax.Updater(id, scriptname, opt);
};

ShowLoadingBox = function(msg)
{
  
  $(Get_UI_CONTENTBOX_ID()).innerHTML = "<table><tr><td><img style=\"margin: 4px;\" src=\"/ise/img/loading.gif\"/></td>" +
    "<td><span>"+msg.escapeHTML()+"</span></td></tr></table>";

  ShowElement(Get_UI_CONTENTBOX_ID());
};

ShowLoadingBoxTimeout = function(msg, msec)
{
  ShowLoadingBox(msg);
    window.setTimeout("HideLoadingBox()",msec);
};

ShowInfoMsgBox = function(msg)
{
  $(Get_UI_CONTENTBOX_ID()).innerHTML = "<table><tr><td><span>" + msg + "</span></td></tr></table>";
  ShowElement(Get_UI_CONTENTBOX_ID());
};

ShowInfoMsgBoxTimeout = function(msg, msec)
{
  ShowInfoMsgBox(msg);
  window.setTimeout("HideLoadingBox()",msec);
};

HideLoadingBox = function()
{
  HideElement(Get_UI_CONTENTBOX_ID());
};

ShowElement = function(id)
{
  $(id).style.visibility = "visible";
  $(id).show();
};

HideElement = function(id)
{
  $(id).style.visibility = "hidden";
  $(id).hide();
};

ToggleVisibility = function(id)
{
  if ($(id).visible()) HideElement(id);
  else                 ShowElement(id);
};

EnableComponent = function(id, b)
{
  $(id).enabled = b;
};

EnableAllSeparateComponents = function(pcount)
{
  var elem;
  var i, j;
  
  for (i=0; i<=pcount; i++)
  {
    j = 1;
    elem = document.getElementById('separate_' + i + '_' + j);
    
    while (elem)
    {
      elem.enabled = true;
      ++j;
      elem = document.getElementById('separate_' + i + '_' + j);
    }
  }
};

initComponents = function(pcount)
{
  var elem;
  var i, j;
  
  for (i=0; i<=pcount; i++)
  {
    j    = 1;
    elem = document.getElementById('use_separate_' + i + '_' + j);
    
    while (elem)
    {
      elem.checked = elem.defaultChecked;
      EnableComponent(elem.id.substr(4), elem.defaultChecked);

      ++j;
      elem = document.getElementById('use_separate_' + i + '_' + j);
    }
  }
};

SwitchActiveProfile = function(pnr)
{
  var i = 0;
  var elem = document.getElementById('caption_profile' + i);
  
  while (elem)
  {
    elem.innerHTML = elem.innerHTML.replace(/ \(aktiv\)/, '');
    elem.style.backgroundColor = WebUI.getColor("profile");
  
    if (i == pnr)
    {
      elem.innerHTML += ' (aktiv)';
      elem.style.backgroundColor = WebUI.getColor("profileActive");
    }
  
    ++i;
    elem = document.getElementById('caption_profile' + i);
  }
  
  ShowElement('profile'+pnr);
};

ShowDeviceDetails = function()
{
  var select  = $('linkabledevices');
  var otext   = select.options[select.selectedIndex].text;
  var len     = select.length - 1;
  var idx     = otext.search(/\=\=/);
  var devname = otext.substr(idx+2);
  
  if (idx == -1) return;
  
  var elemId = "";
  var i;

  //Opera
  try       { elemId = $('radio_channels_'+devname).id; }
  catch (e) { elemId = ""; }

  for (i = 0; i<=len; i++) 
  {
    HideElement('radio_channels_' + i);

    //Internet Explorer:
    /*
    try {
      if (elemId == "" && document.getElementById)
      {
        var elem = document.getElementById('radio_channels_' + i);
        if (elem.name && elem.name == 'radio_channels_' + devname) elemId = 'radio_channels_' + i; 
      }
    }
    catch (e) { elemId = ""; }
    */
  }

  //Firefox:
  try
  {
    if ((elemId === "") && (document.getElementsByName)) { elemId = document.getElementsByName('radio_channels_'+devname)[0].id; }
  }
  catch (e) { elemId = ""; }

  try 
  { 
    if ((elemId === "") && (document.getElementsByTagName))
    {
      var divtags = document.getElementsByTagName("div");
      len = divtags.length - 1;
      
      for (i = 0; i <= len; i++) 
      {
        var elem = divtags[i];
        if (((elem.name) && (elem.name == 'radio_channels_' + devname)) || ((elem.className) && (elem.className == 'radio_channels_' + devname)))
        {
          elemId = elem.id;
          break;
        }
      }
    }
  }
  catch (e) { elemId = ""; }

  ShowElement(elemId);

  /* Beim Firefox gibt es kein elem.name und beim Internet Explorer gibt es kein elem.id */
  /* Opera kann weder document.getElementsByName, noch document.getElementById. Stattdessen geht $(name). */
};

AddPeer2Channel = function(iface, device, radioid)
{
  var elem = document.getElementsByName(radioid);
  var inputelem = null;

  for (var i=0; i<elem.length; i++)
  {
    if (elem[i].checked)
    {
      inputelem = elem[i];
      break;
    }
  }
      
  if (inputelem === null)
  {
    alert("Bitte wählen Sie einen Kanal aus.");
    return;
  }

  var peer = inputelem.value;
  
  AddLinkPeer (iface, device, peer);
};

SwitchDeviceInstallMode = function(iface, b)
{
  ShowLoadingBox('Bitte warten, der Installationsmodus wird gesetzt...');
  ResetPostString();
  
  document.getElementById('global_iface').value = iface;

  if (b) document.getElementById('global_cmd').value = 'enterinstallmode';
  else   document.getElementById('global_cmd').value = 'leaveinstallmode';
  
  AddParam(document.getElementById('global_sid'));
  AddParam(document.getElementById('global_cmd'));
  AddParam(document.getElementById('global_iface'));

  SendRequest('ic_newdev.cgi');
};

AddDeviceBySN = function(iface, sn)
{
  if (sn.length != 10)
  {
    alert('Bitte geben Sie eine gültige Seriennummer ein.');
  }
  else
  {
    ShowLoadingBox('Bitte warten, das Gerät mit der Seriennummer '+sn+' wird angelernt...');
    ResetPostString();
  
    document.getElementById('global_iface' ).value = iface;
    document.getElementById('global_cmd'   ).value = 'installviaserial';
    document.getElementById('global_serial').value = sn;
  
    AddParam(document.getElementById('global_sid'));
    AddParam(document.getElementById('global_cmd'));
    AddParam(document.getElementById('global_iface'));
    AddParam(document.getElementById('global_serial'));

    SendRequest('ic_newdev.cgi');
  }  
};

PrepareDeleteDeviceForm = function(iface, sn, devname, devtype)
{
  var deviceType = DeviceTypeList.getDeviceType(devtype);
  var device = {
    address: sn,
    interfaceName: iface,
    name: devname,
    thumbnailHTML: deviceType.getThumbnailHTML(),
    hasLinksOrPrograms: function(callback)
    {
      if (callback) { callback.defer(false); }
      return false;
    },
    remove: function(flags, callback)
    {
      homematic("Interface.deleteDevice", {
        "interface": this.interfaceName,
        "address"  : this.address,
        "flags"    : flags
      }, callback);
    }
  };
  
  new DeleteDeviceDialog(device, function(isDeleted) {
    if (isDeleted)
    {
      window.setTimeout("WebUI.reload()", 2500);
    }
  });
  
};

PrepareDeleteDeviceForm_old = function(iface, sn, devname, devtype)
{  
  ResetPostString();
  
  AddParam(document.getElementById('global_sid'));
  poststr += "&cmd=DeleteDeviceForm";
  poststr += "&iface="   + iface;
  poststr += "&address=" + sn;
  poststr += "&devname=" + devname;
  poststr += "&devtype=" + devtype;

  SendRequest('ic_ifacecmd.cgi');

  ProgressBar = new ProgressBarMsgBox("Verknüpfungen und Programme werden abgefragt...", 1);
  ProgressBar.show();
    ProgressBar.StartKnightRiderLight();
};

DeleteDeviceForm = function(iface, sn, devname, devtype, devimg, linkcount)
{
  DeleteDeviceFrm = new DelDevMsgBox(640, 480, iface, sn, devname, devtype, devimg, linkcount);
  DeleteDeviceFrm.LoadFromFile("ic_deldev.htm");
  DeleteDeviceFrm.show();
};

DeleteDevice = function(iface, sn)
{
  if (confirm("Möchten Sie das Gerät mit der Seriennummer \'"+sn+"\' wirklich löschen?"))
  {
    ShowLoadingBox('Bitte warten, das Gerät mit der Seriennummer '+sn+' wird abgelernt...');
    ResetPostString();

    document.getElementById('global_iface' ).value = iface;
    document.getElementById('global_cmd'   ).value = 'deletedevice';
    document.getElementById('global_device').value = sn;
  
    AddParam(document.getElementById('global_sid'));
    AddParam(document.getElementById('global_cmd'));
    AddParam(document.getElementById('global_iface'));
    AddParam(document.getElementById('global_device'));

    SendRequest('ic_devices.cgi');
  }  
};

DeleteDeviceAndReset = function(iface, sn)
{
  if (confirm("Möchten Sie das Gerät mit der Seriennummer \'"+sn+"\' wirklich löschen und auf Werkseinstellungen zurücksetzen?"))
  {
    ShowLoadingBox('Bitte warten, das Gerät mit der Seriennummer '+sn+' wird abgelernt...');
    ResetPostString();

    document.getElementById('global_iface' ).value = iface;
    document.getElementById('global_cmd'   ).value = 'deletedevice';
    document.getElementById('global_device').value = sn;
  
    AddParam(document.getElementById('global_sid'));
    AddParam(document.getElementById('global_cmd'));
    AddParam(document.getElementById('global_iface'));
    AddParam(document.getElementById('global_device'));
    AddParam(document.getElementById('global_reset'));

    SendRequest('ic_devices.cgi');
  }  
};

AddLinkPeer = function(iface, device, peer)
{
  ShowLoadingBox('Bitte warten, der Verknüpfungspartner \'' +peer+ '\' wird hinzugefügt...');
  ResetPostString();

  document.getElementById('global_iface'  ).value = iface;
  document.getElementById('global_cmd'    ).value = 'newlinkpeer';
  document.getElementById('global_device' ).value = device;
  document.getElementById('global_peer'   ).value = peer;
  
  AddParam(document.getElementById('global_sid'));
  AddParam(document.getElementById('global_iface'));
  AddParam(document.getElementById('global_cmd'));
  AddParam(document.getElementById('global_device'));
  AddParam(document.getElementById('global_peer'));

  SendRequest('ic_linkpeers.cgi');
};

DeleteLinkPeer = function(iface, device, peer)
{
  if (confirm("Möchten Sie diesen Verknüpfungspartner \'" +peer+ "\' wirklich löschen?"))
  {
    ShowLoadingBox('Bitte warten, der Verknüpfungspartner \'' +peer+ '\' wird gelöscht...');
    ResetPostString();

    document.getElementById('global_iface'  ).value = iface;
    document.getElementById('global_cmd'    ).value = 'deletelinkpeer';
    document.getElementById('global_device' ).value = device;
    document.getElementById('global_peer'   ).value = peer;
  
    AddParam(document.getElementById('global_sid'));
    AddParam(document.getElementById('global_iface'));
    AddParam(document.getElementById('global_cmd'));
    AddParam(document.getElementById('global_device'));
    AddParam(document.getElementById('global_peer'));

    SendRequest('ic_linkpeers.cgi');
  }  
};

GoToStartPage = function()
{
  ResetPostString();
  AddParam(document.getElementById('global_sid'));
  SendRequest('ic_start.cgi');
};

SubmitCentralSetting = function(pnr)
{
  ShowLoadingBox('Bitte warten, Einstellung wird gespeichert...');
  ResetPostString();
  AddProfileValues('global_');
  AddSeparateSettings('separate_', pnr);
  SendRequest('ic_central.cgi');
};

CheckNetworkSettings = function()
{
  var a = isIPAddress(document.getElementsByName('IP')[0].value);
  var b = isIPAddress(document.getElementsByName('NETMASK')[0].value);
  var c = isIPAddress(document.getElementsByName('GATEWAY')[0].value);

  if (!a) alert("Die IP-Adresse ist ungültig");
  if (!b) alert("Die Netzmaske ist ungültig");
  if (!c) alert("Die Gateway-Adresse ist ungültig");
  
  return ( a && b && c );
};

isIPAddress = function( strIPAddress )
{
  var regExp = new RegExp ( "^([0-9]{1,3})[\.]{1,}([0-9]{1,3})[\.]{1,}([0-9]{1,3})[\.]{1,}([0-9]{1,3})$" );
  var aParts = regExp.exec ( strIPAddress );

  if ((aParts === null) || (aParts.length != 5)) return false;

  for ( var nLoopCnt = 1 ; nLoopCnt < 5 ; nLoopCnt++ )
  {
    if ((aParts [ nLoopCnt ] < 0) || (aParts [ nLoopCnt ] > 255 )) { return false; }
  }

  return true ;
};

SimulateShortKeyPress = function()
{
  SimulateKeyPress(false);
};

SimulateLongKeyPress = function()
{
  SimulateKeyPress(true);
};

SimulateKeyPress = function(longpress)
{
  ShowLoadingBox('Bitte warten, der Tastendruck wird ausgelöst...');
  ResetPostString();
  AddParam(document.getElementById('global_1'));//sid
  AddParam(document.getElementById('global_2'));//peer
  AddParam(document.getElementById('global_3'));//peer
  AddParam(document.getElementById('global_5'));//iface
  AddParam(document.getElementById('global_6'));//device
  poststr += "&longpress" + "=" +longpress;
  poststr += "&cmd" + "=" +"simulate"; 
  SendRequest('ic_paramset.cgi');
};

SetExpertMode = function(b)
{
  ShowLoadingBox('Bitte warten, der Expertenmodus wird gesetzt...');
  ResetPostString();
  
  AddParam(document.getElementById('global_sid'));

  if (b) document.getElementById('expert').value = 'true';
  else   document.getElementById('expert').value = 'false';

  AddParam(document.getElementById('expert'));
  
  SendRequest('ic_start.cgi');
};

SetCurrentTime = function()
{
  var jetzt = new Date();

  var hh = jetzt.getHours();
  var mm = jetzt.getMinutes();
  var DD = jetzt.getDate();
  var MM = jetzt.getMonth()+1;
  var YY = jetzt.getFullYear();

  if (hh < 10) hh = "0" + hh;
  if (mm < 10) mm = "0" + mm;
  if (DD < 10) DD = "0" + DD;
  if (MM < 10) MM = "0" + MM;
  if (YY < 10) YY = "0" + YY;

  document.getElementById('separate_3_1').value = hh;
  document.getElementById('separate_3_2').value = mm;
  document.getElementById('separate_3_3').value = DD;
  document.getElementById('separate_3_4').value = MM;
  document.getElementById('separate_3_5').value = YY;
};

RebootCentral = function()
{
  if (confirm("Möchten Sie die Zentrale wirklich neu starten?"))
  {
    ShowLoadingBox('Bitte warten, Zentrale startet neu...');
    ResetPostString();
    AddProfileValues('global_');
    poststr += "&cmd=reboot";
    SendRequest('ic_central.cgi');
  }  
};

set_value = function(input_id, id, type)
{
  var elem = document.getElementById(input_id);
  
  ShowLoadingBox('Bitte warten, Wert \''+id+'\' wird gesetzt');
  ResetPostString();
  document.getElementById('profile').value = "9999";
  AddParam(document.getElementById('profile'));
  AddProfileValues('global_');//sid
  if (elem.type == "checkbox")
  {
    if (elem.checked) elem.value = "true";
    else              elem.value = "false";
  }
  poststr += "&CH_VAL_VALUE=" +elem.value+ "&CH_VAL_ID=" +id+ "&CH_VAL_TYPE=" +type;
  SendRequest('ic_paramset.cgi');
};

RemoveLink = function(iface, sender_address, receiver_address, sender_type, redirect_url)
{
  var questionRemoveLink = ((iface == "HmIP-RF") && (sender_type != "HmIP-SMI55") && (sender_type != "HmIP-SMI55-A") && (sender_address.split(":")[0] == receiver_address.split(":")[0])) ? translateKey('dialogQuestionRemoveInternalLink') : translateKey('dialogQuestionRemoveLink');

  new YesNoDialog(translateKey('dialogSafetyCheck'), questionRemoveLink, function(result) {
    if (result == YesNoDialog.RESULT_YES)
    {
      ResetPostString();

      AddParam(document.getElementById('global_sid'));

      poststr += "&cmd=removeLink";
      poststr += "&iface="            +iface;
      poststr += "&sender_address="   +sender_address;
      poststr += "&receiver_address=" +receiver_address;

      if ((redirect_url) && (redirect_url !== "")) { poststr += "&redirect_url=" + redirect_url; }
      else                                         { poststr += "&redirect_url=IC_LINKPEERLIST"; }

      SendRequest('ic_ifacecmd.cgi');
    }
  }, "html");
};

getInnerDimensions = function()
{
  var x, y;

  if (self.innerHeight) // all except Explorer
  {
    x = self.innerWidth;
    y = self.innerHeight;
  }
  else if (document.documentElement && document.documentElement.clientHeight)
  // Explorer 6 Strict Mode
  {
    x = document.documentElement.clientWidth;
    y = document.documentElement.clientHeight;
  }
  else if (document.body) // other Explorers
  {
    x = document.body.clientWidth;
    y = document.body.clientHeight;
  }

    return {width: x, height: y};
};

getPageOffsets = function()
{
  var x,y;

  if (self.pageYOffset) // all except Explorer
  {
    x = self.pageXOffset;
    y = self.pageYOffset;
  }
  else if (document.documentElement && document.documentElement.scrollTop)
  // Explorer 6 Strict
  {
    x = document.documentElement.scrollLeft;
    y = document.documentElement.scrollTop;
  }
  else if (document.body) // all other Explorers
  {
    x = document.body.scrollLeft;
    y = document.body.scrollTop;
  }

    return {xOffset: x, yOffset: y};
};

/*
Wenn nur ein Gerät angezeigt werden soll, kann man sender_address, oder receiver_address leer lassen ( '' )
*/
CheckConfigPending = function(iface, sender_address, receiver_address, redirect_url, goBack)
{
  ResetPostString();
  
  AddParam($('global_sid'));

  poststr += "&go_back="          + goBack;
  poststr += "&redirect_url="     +redirect_url;
  poststr += "&iface="            +iface;
  poststr += "&sender_address="   +sender_address;
  poststr += "&receiver_address=" +receiver_address;
  poststr += "&cmd=ShowConfigPendingMsg";

  SendRequest('ic_ifacecmd.cgi');
};


OpenSetProfiles = function(iface, sender_address, receiver_address)
{
  exists_timearr = "";
  exists_percarr = "";
  exists_tmparr = "";
  time_tmp = "";
  perc_tmp = "";
  temp_tmp = "";

  ResetPostString();

  poststr += "&iface="            +iface;
  poststr += "&sender_address="   +sender_address;
  poststr += "&receiver_address=" +receiver_address;
  
  updateContent(UI_PATH + 'ic_setprofiles.cgi', poststr);
};


IsDirty = function(inputelem)
{

  var result;

  if (inputelem === null) return false;

  if (inputelem.type == "select-one")
  {
    result = inputelem.options[inputelem.selectedIndex].defaultSelected != inputelem.options[inputelem.selectedIndex].selected;
    if (result) {conInfo("IsDirty - type: " + inputelem.type + " - " + inputelem.id + " - " + inputelem.name + " - isDirty: " + result);}
    return result;
  }
  else if (inputelem.type == "checkbox" || inputelem.type == "radio")
  {
    result = inputelem.checked != inputelem.defaultChecked;
    if (result) {conInfo("IsDirty - type: " + inputelem.type + " - " + inputelem.id + " - " + inputelem.name + " - isDirty: " + result);}
    return result;
  }
  else if (inputelem.type == "text" || inputelem.type == "textarea" || inputelem.type == "password")
  {
    result = inputelem.defaultValue != inputelem.value;
    if (result) {conInfo("IsDirty - type: " + inputelem.type + " - " + inputelem.id + " - " + inputelem.name + " - isDirty: " + result);}
    return result;
  }
  else if (inputelem.type == "hidden")
  {
    return false;
  }
  else
  {
    conInfo("IsDirty - default" + " - " + inputelem.id + " - " + inputelem.name + " - isDirty: fix true");
    return true;
  }
};

ConvTime = function(u_value)
{
  //wird in SetInputValue für User-Profilvorlagen benötigt
  var Userwert = "";
  var hour = parseInt(u_value / 3600);
  var min  = parseInt((u_value % 3600) / 60);
  var sec  = parseInt((u_value % 3600) % 60);
  var msec = u_value - parseInt(u_value); 
  
  if (hour > 0) {
    Userwert = hour + "h ";
    if (min > 0) Userwert = Userwert.concat(min + "min ");
    if (sec > 0) Userwert = Userwert.concat(sec + "s");
  } 
  else if (min > 0) {
    Userwert = min + "min ";
    if (sec > 0) Userwert = Userwert.concat(sec + "s");
  }
  else if (sec > 0) {
    Userwert = sec + "s ";
    if (msec > 0) Userwert = sec + msec + "s";
  }
  else if (msec > 0) {
    Userwert = msec + "s";
  } 
  else Userwert = "0s";

  
  return Userwert ;
};

sort_num = function(a,b) {
  return a - b;
};

sort_opt = function(elem) {
  
  arrVal = new Array();
  arrTexts = new Array();
  arrTexts[0] = new Object();
  
  for(i = 0; i < elem.length; i++) {
    arrVal[i] = parseFloat(elem.options[i].value); 
    arrTexts[0][arrVal[i].toString(10)] = elem.options[i].text;
  }
  
  arrVal.sort(sort_num);
  
  for(i = 0; i < elem.length; i++) {
    elem.options[i].value = arrVal[i];
    elem.options[i].text  = arrTexts[0][arrVal[i].toString(10)];
  }
};

SetInputValue = function(html_inputelem_id, value)
{
  var inputelem = $(html_inputelem_id);
  var Userwert;
  var i;
  
  if (inputelem === null) return;

  if (inputelem.type == "select-one")
  {
    var selectelem = inputelem;
    var  no_entry = true;
      
    //AG  
    
    for (i = 0; i < selectelem.options.length; i++)
    {  
      if (selectelem.options[i].value == value) {  
        no_entry = false;
      } 
    }  
  
    // falls in der Profilvorlage kein entsprechender Wert vorhanden ist,
    // weil eine beutzerdef. Wert eingegeben wurde, der nicht als Auswahl vorhanden ist,
    // wird hier ein neuer Eintrag mit dem entsprechenden Wert erzeugt.
    // Die Werte müssen entsprechend konvertiert werden, 90 Sek. werden z. B. zu 1min 30sec usw.
    
    if (no_entry === true) 
    {
        perc = selectelem.options[1].text.search(/%/);
        h = selectelem.options[1].text.search(/min/);
        m = selectelem.options[1].text.search(/s/);
        s = selectelem.options[1].text.search(/h/);
      
        if (h != -1 || m != -1 || s != -1) Userwert = ConvTime(value);   //es handelt sich um einen Zeitwert
        if (perc != -1) {Userwert = parseInt(value * 100) + "%";}    //es handelt sich um einen Prozentwert
          
        new_option = new Option(Userwert,value,true,true);  //Userwert = angezeigter Wert, value = zu übertragener Wert  
        selectelem.options[selectelem.length] = new_option;  // hier wird der neue Eintrag hinzugefügt
        
      //  Optionen neu sortieren , die beiden nächsten Zeilen sortieren jeweils  wunderbar in Firefox. Im IE gehts mal wieder nicht
      //  $A(selectelem.options).sort(function(a,b) {return (parseFloat(a.value) < parseFloat(b.value)) ? -1 : 1;}).each(function(o,i){selectelem.options[i] = o});
      //  Array.prototype.sort.call(selectelem.options,function(a,b){return parseFloat(a.value) < parseFloat(b.value) ? -1 : parseFloat(a.value) > parseFloat(b.value) ? 1 : 0;});
        
      //  Dieser Aufwand ist wegen IE noetig
        sort_opt(selectelem);
    }
    //End AG 
    
    for (i = 0; i < selectelem.options.length; i++)
    {
      
      selectelem.options[i].selected        = (value == selectelem.options[i].value);
      selectelem.options[i].defaultSelected = (value == selectelem.options[i].value);
    }
  }
  else if (inputelem.type == "checkbox" || inputelem.type == "radio")
  {
    inputelem.checked        = value;
    inputelem.defaultChecked = value;
  }
  else if (inputelem.type == "text" || inputelem.type == "textarea" || inputelem.type == "password" || inputelem.type == "hidden")
  {
    inputelem.value        = value;
    inputelem.defaultValue = value;
  }
};

DetermineParameterValue = function(iface, address, ps_id, param_id, html_inputelem_id)
{
  ResetPostString();
  
  AddParam(document.getElementById('global_sid'));
  poststr += "&cmd=determineParameter";
  poststr += "&iface="             + iface;
  poststr += "&address="           + address;
  poststr += "&ps_id="             + ps_id;
  poststr += "&param_id="          + param_id;
  poststr += "&html_inputelem_id=" + html_inputelem_id;

  SendRequest('ic_ifacecmd.cgi');

  //ProgressBar = new ProgressBarMsgBox("Parameter wird festgesetzt...", 1);
  ProgressBar = new ProgressBarMsgBox(translateKey("dialogDetermineParameterTitle"), 1);
  ProgressBar.show();
    ProgressBar.StartKnightRiderLight();
};

ProofAndSetValue = function(srcid, dstid, min, max, dstValueFactor, convInt2Float, event)
{
  var srcElm = $(srcid);
  var dstElm = $(dstid);

  // Falls das Tasten-Event nicht mit übergeben wurde ....
  /*
  var keyCode = 0,
    finalVal;

  if (event) {
    keyCode = event.keyCode;
  }
  */

  var ok = true;
  var fixedDecimalPoint = (jQuery(srcElm).attr("name") == "METER_CONSTANT_VOLUME") ? 3 : 2;

  if (! min) min = 0;
  if (! max) max = 100;
  if (! dstValueFactor) dstValueFactor = 0.01;//dstValue = value/100

  if (convInt2Float) {min = parseFloat(min).toFixed(fixedDecimalPoint); max = parseFloat(max).toFixed(fixedDecimalPoint);}

  var value = $F(srcid);

  //replace , by .
  if (value.indexOf(',') >= 0)
  {
    var tokens = value.split(",");

    value = "";
    if (tokens[0]) value += tokens[0];
    value += '.';
    if (tokens[1]) value += tokens[1];
    srcElm.value = value;
  }

  // Check if float is allowed
  try {
    if (min.toString().indexOf(".") == -1 && max.toString().indexOf(".") == -1) {
      min = parseInt(min);
      max = parseInt(max);
      value = (roundValue05(parseInt(value)));
    } else {
      min = parseFloat(parseFloat(min).toFixed(fixedDecimalPoint));
      max = parseFloat(parseFloat(max).toFixed(fixedDecimalPoint));
      value = parseFloat(parseFloat(value).toFixed(fixedDecimalPoint));

      if (value < min || isNaN(min)) {value = min;} else if (value > max) {value = max;}

      srcElm.value = parseFloat(value);
    }
  } catch(e) {conInfo(e);}

  if (typeof value == "undefined")
  {
    finalVal = min;
    ok = false;
  }
  else if (isNaN(value))
  {
    finalVal = min;
    ok = false;
  }
  else if (value < min)
  {
    finalVal = min;
    ok = false;
  }
  else if (value > max)
  {
    finalVal = max;
    ok = false;
  }

  if (ok)
  {
    srcElm.style.backgroundColor = "#fffffe";
    dstElm.value = value * dstValueFactor;
    if ((typeof _iface == "undefined") || (_iface != "BidCos-RF")) {
      srcElm.value = dstElm.value;
    }
    srcElm.setAttribute("valvalid", "true");
  }
  else
  {
    srcElm.setAttribute("valvalid", "false");
    srcElm.style.backgroundColor = "red";
    dstElm.value = finalVal * dstValueFactor;
    if ((typeof _iface == "undefined") || (_iface != "BidCos-RF")) {
      srcElm.value = dstElm.value;
    }
    window.setTimeout(function(){srcElm.style.backgroundColor = "white";},1000);
  }
};

elv_toQueryString = function(s)
{
  s = s.replace(/%/g, "%25");
  s = s.replace(/\+/g,"%2b");
  s = s.replace(/ /g, "%20");
  s = s.replace(/"/g, "%22");
  s = s.replace(/\?/g,"%3f");
  
  //Weicht von der tcl 'cgi_quote_url'-Funktion (cgi.tcl) ab:
  s = s.replace(/\&/g,"%26");

  return s;
};

/**
 * Convert a key string of HmIP devices with custom 32 digits to a Base16 string
 * @param valueString
 */
convertHmIPKeyBase32ToBase16 = function(valueString) {

  var HMIP_KEY_CHARS = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',
    'B', 'C', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'W', 'X', 'Y', 'Z' ];

  var buffer = new ArrayBuffer(16),
    keyValue = new Uint8Array(buffer),
    value = 0,
    counter = valueString.length - 1 ,
    bits = 0,
    byteCounter = keyValue.length - 1,
    keyString = "";

  while (counter >= 0) {
    for(var i= 0; i < HMIP_KEY_CHARS.length; i++) {
      if(HMIP_KEY_CHARS[i] == valueString.charAt(counter)) {
        value |= i << bits;
        //console.log(value +" - break");
        break;
      }
    }

    bits += 5;
    counter--;
    while (bits > 8 && byteCounter >= 0) {
      keyValue[byteCounter] = value & 0xff;
      value >>= 8;
      bits -= 8;
      byteCounter--;
    }
  }

  for(var i = 0; i < keyValue.length; i++)
  {
    if (keyValue[i] < 16) {
     keyString += "0";
    }
    keyString += keyValue[i].toString(16);
  }

  return keyString.toUpperCase();
};

