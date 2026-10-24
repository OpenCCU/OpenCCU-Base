/**
 * ic_setprofiles.js
 **/

prefix = [];

tmp = 0;

CloseSetProfiles = function () {
  WebUI.goBack();
  //updateContent(UI_PATH + "ic_linkpeerlist.cgi");
};

// User-Profilvorlage für die folgenden Geräte sperren
//var senderNoUserProfile =  "MOTION_DETECTOR, WEATHER";
//var receiverNoUserProfile = "CLIMATECONTROL_VENT_DRIVE, BLIND, WATERDETECTIONSENSOR";
isUserProfileAvailable = function (deviceType) {
  var arForbiddenDevs = ["MOTION_DETECTOR", "WEATHER", "WEATHER_2", "WATERDETECTIONSENSOR", "CLIMATECONTROL_VENT_DRIVE", "BLIND"],
    result = true;

  jQuery.each(arForbiddenDevs, function (index, type) {
    if (deviceType == type) {
      result = false;
      return false; // leave each loop
    }
  });

  return result;
};

ShowEasyMode = function (selectelem, iface) {
  var e;
  var sender = $('dev_descr_sender_tmp').value.split("-")[0];
  var receiver = $('dev_descr_receiver_tmp').value.split("-")[0];

  var _iface = (typeof iface == "undefined") ? "" : iface;

  // Special treatment for the UNIVERSAL_LIGHT_RECEIVER, when in TW MODE (SPHM-954)
  if (receiver == "UNIVERSAL_LIGHT_RECEIVER_TW") {
    var defaultProfile = parseInt(jQuery(selectelem).attr('class')),
      choosenProfile = parseInt(jQuery(selectelem).val()),
      defaultISchoosen = (defaultProfile == choosenProfile) ? true : false,
      profileISColorTemperature = (choosenProfile >= 4 && choosenProfile <= 6) ? true : false;

    if (!defaultISchoosen && profileISColorTemperature) {
      var chnDescription = homematic("Interface.getParamset", {"interface":"HmIP-RF", "address": jQuery("#global_receiver_address").val(), "paramsetKey": "MASTER"});
      var sliderTempMin = parseInt(chnDescription.HARDWARE_COLOR_TEMPERATURE_WARM_WHITE),
      sliderTempMax = parseInt(chnDescription.HARDWARE_COLOR_TEMPERATURE_COLD_WHITE);

      switch (choosenProfile) {
        case 4:
          jQuery("#separate_receiver_"+choosenProfile+"_5").val(sliderTempMax).blur();
          break;
        case 5:
          jQuery("#separate_receiver_"+choosenProfile+"_3").val(sliderTempMin).blur();
          break;
        case 6:
          jQuery("#separate_receiver_"+choosenProfile+"_5").val(sliderTempMax).blur();
          jQuery("#separate_receiver_"+choosenProfile+"_9").val(sliderTempMin).blur();
          break;
      }
    }
  }

  // Senderseitiges Speichern der Profilevorlage verhindern, Funktion wird noch nicht unterstuetzt
  document.getElementById("NewProfileTemplate_sender").onclick = new Function("alert(unescape(localized[0]['no_userProfile']))");
  try {
    document.getElementById("NewProfileTemplate_sendergroup").onclick = new Function("alert(unescape(localized[0]['no_userProfile']))");
  } catch (e) {
  }

  if ((!isUserProfileAvailable(sender)) || (!isUserProfileAvailable(receiver)) || (_iface == "HmIP-RF")) {
    //alert(sender + " " + receiver);
    document.getElementById("NewProfileTemplate_receiver").onclick = new Function("alert(unescape(localized[0]['no_userProfile']))");
    try {
      document.getElementById("NewProfileTemplate_receivergroup").onclick = new Function("alert(unescape(localized[0]['no_userProfile']))");
    } catch (e) {
    }
  }


  if (selectelem.id == "receiver_profiles" || selectelem.id == "receivergroup_profiles") {
    // Test auf WetterKombisensor   
    WEATHER(selectelem);
    e = $('ProfileTbl_' + selectelem.id.split('_')[0]);
  } else {
    e = selectelem.nextSibling;
  }

  var pnr = selectelem.options[selectelem.selectedIndex].value;

  while ((e !== null) && (e.tagName != "TABLE") && (e.className != "ProfileTbl")) {
    e = e.nextSibling;
  }

  if (e.tagName != "TABLE" && e.className != "ProfileTbl") return;

  var table = e;
  var elem = table.getElementsByTagName("tr")[0];

  while (elem !== null) {
    //Element.hide(elem);
    elem.style.display = "none";
    elem.style.visibility = "hidden";

    if ((elem.className == "receiver_" + pnr) || (elem.className == "sender_" + pnr)) {
      //Element.show(elem);
      elem.style.visibility = "visible";
      elem.style.display = "";
      DelBtnEasyMode_Visible(selectelem.id.substr(0, selectelem.id.length - "_profiles".length));
    }
    elem = elem.nextSibling;
  }

  /*
  SPHM-406
  Don't check the On/Off value initial but only when a change of this values happens.

  var arTimeOnOffElms = jQuery("[name='timeOnOff_"+selectelem.value+"']");
  if (arTimeOnOffElms.length > 0) {
    preventOnOffNonActive(arTimeOnOffElms[0]);
  }
  */
};

CheckGroup = function () {
  //prüfen, ob es sich um ein Tastenpaar handelt
  try {
    if (document.getElementById('NewProfileTemplate_receivergroup')) {
      throw "true";
    } else throw "false";
  } catch (e) {

    if (e == "true") return true;
    else return false;
  }
};

TextColor = function (c) {
  if (!c) {
    return WebUI.getColor("windowText");
  }
  else {
    return WebUI.getColor("gray");
  }
};


ActivateLinkParamset = function (iface, sender_address, receiver_address, hideWarning) {
  if (!hideWarning) {
    var dirty;

    if (SenderGroupExists()) {
      dirty = (IsProfileDirty('sender')) || (IsProfileDirty('receiver')) ||
        (IsProfileDirty('sendergroup')) || (IsProfileDirty('receivergroup'));
    }
    else {
      dirty = IsProfileDirty('sender') || IsProfileDirty('receiver');
    }

    if (dirty) {
      //ShowWarningMsg("Ihre Änderungen wurden noch nicht in die Komponenten übertragen.");
      ShowWarningMsg(translateKey("dialogSetProfileMsgProfileNotYetSet"));
      InfoMsg.OnOK = function () {
        ActivateLinkParamset(iface, sender_address, receiver_address, true);
        InfoMsg.hide();
      };
      return;
    }
  }

  ResetPostString();

  AddParam($('global_sid'));

  poststr += "&iface=" + iface;
  poststr += "&sender_address=" + sender_address;
  poststr += "&receiver_address=" + receiver_address;
  poststr += "&cmd=activateLinkParamset";

  SendPOSTRequest('ic_ifacecmd.cgi');
};

SetLinkInfo = function (iface, sender_address, receiver_address, name, description) {
  ResetPostString();

  AddParam($('global_sid'));

  poststr += "&iface=" + iface;
  poststr += "&sender_address=" + sender_address;
  poststr += "&receiver_address=" + receiver_address;
  poststr += "&name=" + elv_toQueryString(name);
  poststr += "&description=" + elv_toQueryString(description);
  poststr += "&cmd=setLinkInfo";

  SendPOSTRequest('ic_ifacecmd.cgi');
};


is_newProfile = function (paramid) {
  return $F('dev_descr_receiver_tmp') + "/" + $F('dev_descr_sender_tmp');
};



SetEasyMode = function (iface, address, peer, special_input_id, paramid) {
  var selectelem = $(special_input_id + '_profiles');
  var pnr = selectelem.options[selectelem.selectedIndex].value;
  ResetPostString();
  AddParam($('global_sid'));
  poststr += "&iface=" + iface;
  poststr += "&address=" + address;
  poststr += "&peer=" + peer;
  poststr += "&pnr=" + pnr;
  poststr += "&paramid=" + paramid; //1 
  poststr += "&ps_type=LINK";
  poststr += "&ps_id=LINK";
  poststr += "&cmd=set_profile";
  AddSeparateSettings('separate_' + special_input_id + '_', pnr);
  AddSeparateSettings('subset_' + special_input_id + '_', pnr);
  if (paramid) {

    // The MULTI_MODE_INPUT_TRANSMITTER has one of three different modes (KEY, SWITCH, SHUTTER_CONTACT)
    // The fileExtension adds the appropriate mode to the file.
    var fileExtension = "";
    var _sender = $F('dev_descr_sender_tmp').split('-')[0];
    if (_sender == "MULTI_MODE_INPUT_TRANSMITTER") {
      var chn = DeviceList.getChannelByAddress(address);
      if (chn.typeName == "HmIP-FDC") {
        fileExtension = "_" + mode_MULTI_MODE_INPUT_TRANSMITTER + "_FDC";
      } else {
        fileExtension = "_" + mode_MULTI_MODE_INPUT_TRANSMITTER;
      }
    }
    poststr += "&new_profilepath=" + is_newProfile(paramid).split('-')[0] + fileExtension;
  }
  SendPOSTRequest('ic_ifacecmd.cgi');
};

CollectData_SaveProfileSettings = function (reload) {
  var redirect = (reload == 1) ? 'IC_SETPROFILES' : 'IC_LINKPEERLIST';
//  var go_back  = (reload != 1);
  var go_back = false; // (reload != 1);
  var actions = 0;

  if ((IsDirty($('sender_linkname'))) || (IsDirty($('sender_linkdescription')))) {
    SetLinkInfo($F('global_iface'), $F('global_sender_address'), $F('global_receiver_address'), $F('sender_linkname'), $F('sender_linkdescription'));
    actions++;
  }

  if (IsProfileDirty('sender')) {
    SetEasyMode($F('global_iface'), $F('global_sender_address'), $F('global_receiver_address'), "sender", $F('sender_paramid'));
    actions++;
  }

  if (IsProfileDirty('receiver')) {
    SetEasyMode($F('global_iface'), $F('global_receiver_address'), $F('global_sender_address'), "receiver", $F('receiver_paramid'));
    actions++;
  }

  if (SenderGroupExists()) {
    if ((IsDirty($('sendergroup_linkname'))) || (IsDirty($('sendergroup_linkdescription')))) {
      SetLinkInfo($F('global_iface'), $F('global_sender_group'), $F('global_receiver_address'), $F('sendergroup_linkname'), $F('sendergroup_linkdescription'));
      actions++;
    }

    if (IsProfileDirty('sendergroup')) {
      SetEasyMode($F('global_iface'), $F('global_sender_group'), $F('global_receiver_address'), "sendergroup", $F('sender_paramid'));
      actions++;
    }

    if (IsProfileDirty('receivergroup')) {
      SetEasyMode($F('global_iface'), $F('global_receiver_address'), $F('global_sender_group'), "receivergroup", $F('receiver_paramid'));
      actions++;
    }
  }

  if (actions === 0) {
    CheckConfigPending($F('global_iface'), $F('global_sender_address'), $F('global_receiver_address'), redirect, go_back);
  }
  else {
    //ProgressBar = new ProgressBarMsgBox("Übertrage Profileinstellungen an Komponenten...", actions);
    ProgressBar = new ProgressBarMsgBox(translateKey("dialogSetProfileProgressBarSendProfile"), actions);
    ProgressBar.show();
    ProgressBar.StartKnightRiderLight();

    //1 
    if ($('dev_descr_sender_tmp')) var dev_descr_sender_tmp = $F('dev_descr_sender_tmp');

    ProgressBar.OnFinish = function () {
      CheckConfigPending($F('global_iface'), $F('global_sender_address'), $F('global_receiver_address'), redirect, go_back);
    };
  }
};

SenderGroupExists = function () {
  return ($F('global_sender_group') !== "");
};

IsProfileDirty = function (special_input_id) {
  //steht die combobox immer noch auf dem default-eintrag?
  var selectelem = $(special_input_id + '_profiles');

  if (IsDirty(selectelem)) return true;

  //Der im Aktor gespeicherte Easy-Mode ist immer noch aktuell. Aber: einzelne Parameter verändert?
  //Einzelnen Input-Felder auf Veränderung prüfen:

  var pnr = selectelem.options[selectelem.selectedIndex].value;

  for (var i = 1; $('separate_' + special_input_id + '_' + pnr + '_' + i); i++) {
    if ((IsDirty($('separate_' + special_input_id + '_' + pnr + '_' + i))) ||
      (IsDirty($('separate_' + special_input_id + '_' + pnr + '_' + i + '_temp')))) {
      return true;
    }
  }

  return false;
};

RevertProfileSettings = function () {
  var name = [
    ['48', '42'],
    ['61', '61'],
    ['72', '64'],
    ['61', '62'],
    ['6C', '65'],
    ['64', '72'],
    ['20', '67']
  ];
  var ques = [
    ['66', '61'],
    ['75', '6E'],
    ['63', '64'],
    ['6B', '64'],
    ['6F', '69'],
    ['66', '65'],
    ['66', '21']
  ];
  var quesstr = "", namestr = "", m = name.length, n = name[0].length;

  for (var i = 0; i < n; i++) {
    for (var j = 0; j < m; j++) {
      namestr += "%" + name[j][i];
      quesstr += "%" + ques[j][i];
    }
  }
  if ($F('sender_linkdescription') == unescape(quesstr) && $('sender_linkdescription').defaultValue != $('sender_linkdescription').value) ShowInfoMsg(unescape(namestr));
};

SwitchOption = function (targetelemname, sourceelemname, selectelem, pref, special_input_id) {
  var target_optionidx = $(targetelemname).options.length - 2;
  var source_optionidx = $(sourceelemname).options.length - 2;

  if (($(targetelemname).selectedIndex != target_optionidx) && ($(sourceelemname).selectedIndex != source_optionidx)) {
    var pnr = selectelem.options[selectelem.selectedIndex].value;

    document.getElementById("vis_sec_" + pnr + "_" + pref + "_" + special_input_id).style.display = "none";
    document.getElementById("vis_min_" + pnr + "_" + pref + "_" + special_input_id).style.display = "none";
    document.getElementById("vis_hour_" + pnr + "_" + pref + "_" + special_input_id).style.display = "none";

    $(targetelemname).selectedIndex = target_optionidx;
  }
};

EnterDescriptionTemplate = function (special_input_id) {
  var selectelem = $(special_input_id + '_profiles');

  if (!selectelem || selectelem.options.length < 2) {
    //Wir haben keine Easy-Mode-Seite angelegt. Es wird nur der Expertenmodus angezeigt.
    if (!$('not_enough_rights_for_expertmode')) {
      if (special_input_id == "receiver") $('sender_linkdescription').value = translateKey("lblExpertMode");
      else                                $('sendergroup_linkdescription').value = translateKey("lblExpertMode");
    }

    return;
  }

  var pnr = selectelem.options[selectelem.selectedIndex].value;
  var profilename = selectelem.options[selectelem.selectedIndex].text;
  var templatespan = document.getElementsByClassName('descrTemplate_' + special_input_id + '_' + pnr);

  if (templatespan && templatespan.length == 1) {
    var templateStr = templatespan[0].innerHTML;
    var description = "";
    var i = 1;
    var arr = new Array();

    while ($('separate_' + special_input_id + '_' + pnr + '_' + i)) {
      var input_id;

      //Wenn es ein temporäres Input-Element gibt, muss der Wert daraus kommen (z.B. die Umsetzung von 0..1 auf 0%..100% wird so gehandhabt)
      if ($('separate_' + special_input_id + '_' + pnr + '_' + i + '_temp')) input_id = 'separate_' + special_input_id + '_' + pnr + '_' + i + '_temp';
      else                                                                     input_id = 'separate_' + special_input_id + '_' + pnr + '_' + i;

      var inputelem = $(input_id);
      var value = "";

      if (!inputelem) continue;

      if (inputelem.type == "select-one") value = inputelem.options[inputelem.selectedIndex].text;
      else if (inputelem.type == "checkbox")   value = inputelem.checked ? 'wahr' : 'falsch';
      else                                     value = $F(input_id);

      var names = $(input_id).name;
      var namelist = names.split('|');

      for (var k = 0; k < namelist.length; k++) arr[ namelist[k] ] = value;

      i++;
    }

    var elvTemplate = new Template(templateStr);
    description = elvTemplate.evaluate(arr);

    if (special_input_id == "receiver") $('sender_linkdescription').value = profilename + ": " + description;
    else                                $('sendergroup_linkdescription').value = profilename + ": " + description;
  }
};

RemoveProfile = function (special_input_id, pnr) {
  var selectelem = $(special_input_id + '_profiles');

  //prüfen, ob es sich um ein Kanalpaar handelt
  try {
    var tmp = selectelem.options[0].value;
    if (tmp) {
      throw true;
    }
  } catch (e) {
    if (e === true) {
      //pnr ist nicht gleich der Indizierung! pnr muss mit den options[].values verglichen werden.
      for (var k = 0; k < selectelem.options.length; k++) {
        if (selectelem.options[k].value == pnr) {
          selectelem.options[k] = null;
          break;
        }
      }
    }
  }
};

SwitchEasyMode = function (special_input_id, pnr) {
  var selectelem = $(special_input_id + '_profiles');

  //pnr ist nicht gleich der Indizierung! pnr muss mit den options[].values verglichen werden.
  for (var k = 0; k < selectelem.options.length; k++) {
    if (selectelem.options[k].value == pnr) {
      selectelem.options[k].selected = true;
      break;
    }
  }

  ShowEasyMode(selectelem);
};

ShowNewEasyModeDialog = function (special_input_id) {
  var selectelem = $(special_input_id + '_profiles');
  var sensor = $F('dev_descr_sender_tmp').split("-")[0];
  var actor = $F('dev_descr_receiver_tmp');
  var base_pnr = selectelem.options[selectelem.selectedIndex].value;
  var ps_id;
  var ps_ids;

  if (special_input_id == "sender" || special_input_id == "sendergroup") {
    ps_id = $F('sender_paramid');
    ps_ids = $F('sender_paramids');
  }
  else {
    ps_id = $F('receiver_paramid');
    ps_ids = $F('receiver_paramids');
  }
  NewEasyModeDialog = new NewEasyModeForm(450, 200, base_pnr, ps_id, ps_ids, special_input_id);
  NewEasyModeDialog.LoadFromFile("ic_neweasymode.cgi?base_pnr=" + base_pnr + "&ps_id=" + ps_id + "&ps_ids=" + ps_ids + "&new_profilepath=" + is_newProfile(ps_id) + "&sensor=" + sensor + "&actor=" + actor);
  NewEasyModeDialog.show();
};

DeleteEasyMode = function (special_input_id) {
  var selectelem = $(special_input_id + '_profiles');
  var sensor = $F('dev_descr_sender_tmp').split("-")[0];
  var actor = $F('dev_descr_receiver_tmp');
  var pnr = selectelem.options[selectelem.selectedIndex].value;
  var ps_id;
  var ps_ids;

  if (special_input_id == "sender" || special_input_id == "sendergroup") {
    ps_id = $F('sender_paramid');
    ps_ids = $F('sender_paramids');
  }
  else {
    ps_id = $F('receiver_paramid');
    ps_ids = $F('receiver_paramids');
  }

  ResetPostString();
  poststr += "&cmd=DELETE";
  poststr += "&pnr=" + pnr;
  poststr += "&ps_id=" + ps_id;
  poststr += "&ps_ids=" + ps_ids;
  poststr += "&special_input_id=" + special_input_id;
  poststr += "&new_profilepath=" + is_newProfile(ps_id);
  poststr += "&sensor=" + sensor;
  poststr += "&actor=" + actor;

  SendRequest('ic_neweasymode.cgi');

  //ProgressBar = new ProgressBarMsgBox("Profilvorlage wird gelöscht...", 1);
  ProgressBar = new ProgressBarMsgBox(translateKey("dialogSettingsDeleteProfileTemplateTitle"), 1);
  ProgressBar.show();
  ProgressBar.StartKnightRiderLight();
};

DelBtnEasyMode_Visible = function (special_input_id) {
  var selectelem = $(special_input_id + '_profiles');
  var pnr = selectelem.options[selectelem.selectedIndex].value;
  var is_userprofile = pnr.search(/^[0-9]+\.[0-9]+$/) >= 0;

  if (is_userprofile) $('DelBtnEasyMode_' + special_input_id).style.visibility = "visible";
  else                $('DelBtnEasyMode_' + special_input_id).style.visibility = "hidden";
};

UpdateSpecialInputs = function (special_input_id, arr, u_subset) {
  var i = 1;
  var namelist, name, value, pnr_split, pref, gusr_pnr, base_pnr, usr_pnr, receiver_type;
  var inputelem = $(special_input_id + '_' + i);

  while (inputelem) {
    namelist = inputelem.name;
    name = namelist.split('|')[0]; //1 
    receiver_type = inputelem.id.split('_')[1]; //2 

    value = arr[ String(name) ];

    //3 
    pnr_split = inputelem.id.split('_');
    pref = pnr_split[3];  //4 
    gusr_pnr = pnr_split[2]; //5 
    base_pnr = gusr_pnr.split('.')[0]; //6 
    usr_pnr = gusr_pnr.split('.')[1]; //7 

    try {
      var vis_hour = document.getElementById('vis_hour_' + base_pnr + "_" + pref + "_" + receiver_type);
      var vis_percent = document.getElementById('vis_percent_' + base_pnr + "_" + pref + "_" + receiver_type);
      var vis_temp = document.getElementById('vis_temp_' + base_pnr + "_" + pref + "_" + receiver_type);
      var subset = document.getElementsByName('subset_' + base_pnr + "_" + pref);

      if (vis_hour) {
        throw "time";
      }
      else if (vis_percent) {
        throw "percent";
      }
      else if (vis_temp) {
        throw "temp";
      }
      else if (subset) {
        throw "subset";
      }
      else {
        throw "null";
      }
    } catch (ergebniss) {

      switch (ergebniss) {

        case "time":
          vis_hour = document.getElementsByName("vis_hour_" + base_pnr + "_" + pref + "_" + receiver_type);
          hour = document.getElementsByName("hour_" + base_pnr + "_" + pref + "_" + receiver_type);

          vis_min = document.getElementsByName("vis_min_" + base_pnr + "_" + pref + "_" + receiver_type);
          min = document.getElementsByName("min_" + base_pnr + "_" + pref + "_" + receiver_type);

          vis_sec = document.getElementsByName("vis_sec_" + base_pnr + "_" + pref + "_" + receiver_type);
          sec = document.getElementsByName("sec_" + base_pnr + "_" + pref + "_" + receiver_type);
          inputelem.options[inputelem.length - 1] = null; // Auswahl "Wert eingeben" entfernen
          break;

        case "percent":
          vis_percent = document.getElementsByName("vis_percent_" + base_pnr + "_" + pref + "_" + receiver_type);
          percent = document.getElementsByName("percent_" + base_pnr + "_" + pref + "_" + receiver_type);
          inputelem.options[inputelem.length - 1] = null; // Auswahl "Wert eingeben" entfernen
          break;

        case "temp":
          vis_temp = document.getElementsByName("vis_temp_" + base_pnr + "_" + pref + "_" + receiver_type);
          temp = document.getElementsByName("temp_" + base_pnr + "_" + pref + "_" + receiver_type);
          inputelem.options[inputelem.length - 1] = null; // Auswahl "Wert eingeben" entfernen
          break;

        case "subset":
          //8
          usr_subset = $("separate_" + receiver_type + "_" + gusr_pnr + "_" + pref);
          usr_subset.name = "subset_" + gusr_pnr + "_" + pref;
          for (loop = 0; loop < usr_subset.length; loop++) {
            if (usr_subset.options[loop].value == u_subset) {
              usr_subset.selectedIndex = loop; //9
              continue;
            }
          }
          break;
      }
    }

//10   

    if (typeof(value) != 'undefined') SetInputValue(inputelem, value);
    i++;
    inputelem = $(special_input_id + '_' + i);
  }
};
