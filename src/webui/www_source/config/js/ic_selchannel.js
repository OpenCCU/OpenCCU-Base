/**
 * ic_selchannel.js
 **/
 
//======================================================================
// Globale Variablen für diese Datei
SORT_DESC   = false;
IGNORE_CASE = true;
SORTED_COL  = -1;
SORT_ASC_SRC  = "/ise/img/arrow_up.gif";
SORT_DESC_SRC = "/ise/img/arrow_down.gif";
//======================================================================

//------------------------------------------------------------------------
//Klasse SelChannelBox für das PopUp Kanalauswahl:
/*
SelChannelBox = Class.create();

SelChannelBox.prototype = Object.extend(new MsgBox(), {
  initialize: function(w, h) {

  this.init(w, h);
  }
});
*/
//------------------------------------------------------------------------

CloseSelChannel = function()
{
  updateContent(UI_PATH + "ic_linkpeerlist.cgi");
};

Sort = function(tableid, colNr)
{
  HideFilterControls();

  if (($F('global_realchannels') === 0) && ($F('global_virtualchannels') === 0)) return;

  try 
  {
    SORTED_COL = document.getElementsByClassName('sorted')[0].cellIndex;
  }
  catch (e) {
    //alle unsorted: einen auf sorted stellen
    $('tr_caption_colnames').getElementsByTagName("td")[colNr     ].className = "sorted";
  }
      
  if (SORTED_COL != colNr && SORTED_COL != -1)
  {
    //Highlighting ändern
    $('tr_caption_colnames').getElementsByTagName("td")[SORTED_COL].className = "unsorted";
    $('tr_caption_colnames').getElementsByTagName("td")[colNr     ].className = "sorted";
  }

  SetSortingOrder(colNr);
  
  //Sortierungssymbol zuordnen:
  var img = $('tr_caption_colnames').getElementsByTagName("td")[colNr].getElementsByTagName("img")[0];
  SORT_DESC ? img.src=SORT_DESC_SRC : img.src=SORT_ASC_SRC;
  //-----

  SortTable(tableid, colNr);

  SORTED_COL = colNr;
};

//Wenn b_order_desc nicht gesetzt, dann wird die Sortierreihenfolge gewechselt,
//es sei denn, die Spalte wird zum ersten mal sortiert. Bei der ersten Sortierung
//wird standardmäßig aufsteigend sortiert.
SetSortingOrder = function(colNr, b_order_desc)
{
  if (b_order_desc)
  {
    SORT_DESC = b_order_desc;
  }
  else
  {
    if  (SORTED_COL == -1 || SORTED_COL != colNr) SORT_DESC = false; //init und default bei Spaltenwechsel
    else                                          SORT_DESC = !SORT_DESC; //Wechsel der Sortierreihenfolge
  }
};

SortTable = function(tableid, colNr)
{
  var tr_list = $('chnListBody').getElementsByTagName("tr");

  var valueList = new Array();
  var valueMap  = new Object();
  var value;
  var i;

  for (i = 0; i < tr_list.length; i++)
  {
    value = tr_list[i].cells[colNr].innerHTML.replace(/<[^>]+>/g,"");
    
    if (typeof(valueMap[value]) == "undefined")
    {
      valueMap[value] = new Array(); //Liste nimmt Tabellenzeile gleichen values auf.
      valueList.push(value); //In dieser Liste ist value nur einmal drin.
    }
    
    valueMap[value].push(tr_list[i]); //Liste gleicher values nimmt Zeile auf
  }

  // ColNr 4 = Sort by serial number
  if (colNr == 4) {
      valueList.sort(function (a, b) {
        var ar1 = a.split(":"),
          ar2 = b.split(":");
        if (ar1[0] == ar2[0]) {
          return (!SORT_DESC) ? (parseInt(ar1[1]) - parseInt(ar2[1])) : (parseInt(ar2[1]) - parseInt(ar1[1]));
        }
      });
  } else {
    valueList.sort(compareStrings_globalsettings); //Sort list
  }

  var rowList;
  var headerlen = tr_list[0].rowIndex;
  var k = 0;

  for (i = 0; i<valueList.length; i++) //Zeilen eines gleich bleibenden values nacheinander einbauen
  {
    rowList = valueMap[valueList[i]];
    
    for (var j = 0; j < rowList.length; j++) //Für jede Zeile mit gleichem value (Array)
    {
      if (k+headerlen != rowList[j].rowIndex) swapRows(tableid, k + headerlen, rowList[j].rowIndex);
      k++;
    }
  }
};

compareStrings_globalsettings = function(x, y)
{
  return compareStrings(x, y, SORT_DESC, IGNORE_CASE);
};

//b_order_desc == true:  Sortierreihenfolge absteigend
//             == false:                    aufsteigend
//b_ignore_case == true: Groß-/Kleinschreibung nicht beachten (Muster == muster)
//              == false:                      beachten       (Muster != muster)
compareStrings = function(x, y, b_order_desc, b_ignore_case)
{
  var higherindex;
  var lowerindex;
  var equalindex = 0;
  var xx = x;
  var yy = y;

  if (b_order_desc)
  {
    //Sortierung absteigend
    higherindex = -1;
    lowerindex  =  1;
  }
  else
  {
    //Sortierung aufsteigend
    higherindex =  1;
    lowerindex  = -1;
  }
  
  if (b_ignore_case)
  {
    //Groß-/Kleinschreibung ignorieren
    xx = xx.toLowerCase();
    yy = yy.toLowerCase();
  }
  
  if      (xx < yy) return lowerindex;
  else if (xx > yy) return higherindex;
  else              return equalindex;
};

swapRows = function(tableid, i, j)
{
  var table      = document.getElementById(tableid);
  var minNode    = table.rows[Math.min(i, j)];
  var maxNode    = table.rows[Math.max(i, j)];
  var parentNode = minNode.parentNode;
  
  if ((i - j) * (i - j) == 1)
  {
    parentNode.removeChild(maxNode);
    parentNode.insertBefore(maxNode, minNode);
  }
  else
  {
    var nextNode = minNode.nextSibling;
    
    parentNode.removeChild(minNode);
    parentNode.replaceChild(minNode, maxNode);
    parentNode.insertBefore(maxNode, nextNode);
  }
};

AddFilter = function(colNr)
{
  HideElement('id_filtercontrol_' + colNr);

  if (($F('global_realchannels') == 0) && ($F('global_virtualchannels') == 0)) { return; }

  var input = document.getElementsByName('input_filtercontrol_' +colNr);
  var patternlist = "";
  var i;

  if ((input == null) || (input[0] == null)) { return; }

  if (input[0].type == "text")
  {
    patternlist = input[0].value;
  }
  else if (input[0].type == "checkbox") 
  {
    for (i = 0; i < input.length; i++)
    {
      if (input[i].checked) patternlist += input[i].value + '|';
    }
  
    if (patternlist.length !== "") { patternlist = patternlist.substring(0, patternlist.length-1); }
  }
  else if (input[0].type== "radio") 
  {
    for (i = 0; i < input.length; i++)
    {
      if (input[i].checked)
      {
        patternlist = input[i].value;
        break;
      }
    }
  }
    
  addFilterColumn($('chnListBody').getElementsByTagName("tr"), colNr, patternlist);
  
  if (patternlist !== "") { $('id_filtertd_'+ colNr).className = "filtered"; }
  else                    { $('id_filtertd_'+ colNr).className = "unfiltered"; }
};

ResetFilter = function()
{
  //var table  = document.getElementById('ChnListTbl');
  //var len = table.getElementsByTagName("tbody")[0].getElementsByTagName("tr")[0].getElementsByTagName("td").length;
  var i=0;
  
  while ($('id_filtercontrol_'+i) !== null)
  {
    HideElement('id_filtercontrol_' + i);

    var input = document.getElementsByName('input_filtercontrol_' +i);

    if ((input !== null) && (input[0] !== null) && typeof (input[0]) != "undefined")
    {
      if (input[0].type == "text")
      {
        input[0].value = "";
      }
      else if ((input[0].type == "checkbox") || (input[0].type == "radio")) 
      {
        for (var j=0; j<input.length; j++) input[j].checked = false;
      }
  
      $('id_filtertd_'+ i).className = "unfiltered";
    }
    
    i++;
  }
};

ResetTable = function()
{
  if (($F('global_realchannels') === 0) && ($F('global_virtualchannels') === 0)) return;
  
  var tr_list  = $('chnListBody').getElementsByTagName("tr");
  for (var i=0; i<tr_list.length; i++) 
  {
    tr_list[i].style.display = '';
    //wenn Verknuepfungspartner vorhanden ist, trifft die 2. Bedingung nicht zu (kein Bild vorhanden)
		if ((tr_list[i].className != "virtual_key_hidden") && (tr_list[i].cells.length > 1)) 
    {
      tr_list[i].cells[2].childNodes[0].style.display = '';  //um die Bilder wieder einzublenden
    }
  }
};

ResetFilterAndTable = function()
{
  ResetFilter();
  ResetTable();
};

HideFilterControls = function()
{
  var i=0;
  
  while ( $('id_filtercontrol_'+i) !== null)
  {
    HideElement('id_filtercontrol_' + i);
    i++;
  }
};

ShowFilterControl = function(colNr)
{
  var visible = $('id_filtercontrol_' +colNr).style.visibility == "visible";

  HideFilterControls();

  if (! visible) 
  {
    ShowElement('id_filtercontrol_' + colNr);
    jQuery('#id_filtercontrol_' +colNr).draggable();

    var input = document.getElementsByName('input_filtercontrol_' +colNr)[0];
    if ((input !== null) && (input.type == "text"))
    {
      input.focus();
      input.select();
    }
  }
};

filterTable = function()
{
  var i=0;

  ResetTable();//Zeige alle Zeilen

  while ( $('id_filtercontrol_'+i) != null)
  {
    AddFilter(i); //Filter setzen in Zeile i
    jQuery('#id_filtercontrol_' + i).css({"left": 0, "top" : 0 });
    i++;
  }
  SizeTable();

};

filterCheckEnterEsc = function(keyCode, filterNr)
{
  switch (keyCode) {
    case 13:
      // Set the filter -- press ok
      filterTable();
      break;
    case 27:
      jQuery("#inputTextFilter_" + filterNr).val("");
      filterTable();
      break;
  }
};

//Filtert eine Tabelle und lässt nur die Zeilen sichtbar,
//  die mit einem Muster (Liste) übereinstimmen. Die 
//  Tabelle muss ein <tbody> - Tag haben.
//tableid: html-id des table-Tags
//colNr: Spalte, die nach pattern gefiltert werden soll 
//patternlist: pattern1|pattern2|pattern3|...|patternN
addFilterColumn = function(tr_list, colNr, patternlist)
{
  var patterns = patternlist.toLowerCase().split("|");

  if (patternlist === "") return;

  for (var i=0; i<tr_list.length; i++)
  {
    var text = tr_list[i].cells[colNr].innerHTML.replace(/<[^>]+>/g,"");
    text = text.toLowerCase();

    for (var j=0; j<patterns.length; j++)
    {
      var pattern = patterns[j].toLowerCase();
      
      if (text.indexOf(pattern) < 0)
      {
        tr_list[i].style.display = 'none';
        tr_list[i].cells[2].childNodes[0].style.display = 'none';  //um die Bilder im IE auszublenden
        break;
      }
    }
  }
};

ToggleVirtualKeys = function()
{
  var i;
  var tr;
  
  //if ( $('ToggleVirtualKeys').firstChild.nodeValue == "Virtuelle Kanäle ausblenden" )
  if ( $('ToggleVirtualKeys').firstChild.nodeValue == translateKey("footerBtnVirtualChannelsHide") )
  {
    tr = $A(document.getElementsByClassName('virtual_key_visible'));

    for (i = 0; i < tr.length; i++) 
    {
      tr[i].className = "virtual_key_hidden";
      tr[i].cells[2].childNodes[0].style.display = 'none';  //um die Bilder im IE auszublenden
    }

    //$('ToggleVirtualKeys').firstChild.nodeValue  = "Virtuelle Kanäle einblenden";
    $('ToggleVirtualKeys').firstChild.nodeValue  = translateKey("footerBtnVirtualChannelsShow");
  }
  else
  {
    tr = $A(document.getElementsByClassName('virtual_key_hidden'));

    for (i = 0; i < tr.length; i++)
    {
      tr[i].className = "virtual_key_visible";
      if (tr[i].style.display != 'none')
      {
        tr[i].cells[2].childNodes[0].style.display = '';  //um die Bilder im IE wieder einzublenden
      }
    }
    
    //$('ToggleVirtualKeys').firstChild.nodeValue  = "Virtuelle Kanäle ausblenden";
    $('ToggleVirtualKeys').firstChild.nodeValue  = translateKey("footerBtnVirtualChannelsHide");
  }

  SizeTable();
};

SizeTable = function()
{
  if (($F('global_realchannels') === 0) && ($F('global_virtualchannels') === 0)) return;

  if (window.navigator.userAgent.toUpperCase().indexOf("MSIE ") > -1) return;

  //Die Funktion SizeTable hat bei begrenzter Auflösung keinen Sinn.
    if (screen.availWidth < 1200) return;

  $('chnListBody').style.overflow = "";//um überhaupt sinnvoll Höhen bestimmen zu können
  $('chnListBody').style.height   = "";
    
  var dim = getInnerDimensions();
  
  var adjusted_tbody_height;
    
  if ( $('previous_step_wrapper') ) adjusted_tbody_height = dim.height -507; //(zusammengefasst aus vorigem Statement)
  else                              adjusted_tbody_height = dim.height -353; //(zusammengefasst aus vorigem Statement)

  if (adjusted_tbody_height < 130) adjusted_tbody_height = 130;

  var cur_tbody_height = Element.getHeight($('chnListBody'));
  
  if (adjusted_tbody_height < cur_tbody_height)
  {
    //$('chnListBody').style.height   = adjusted_tbody_height +"px"; // causes problems with IE11
    $('chnListBody').style.overflow = "";
  }
};

AddLink = function(iface, sender_address, sender_group, receiver_address, name, description, group_name, group_description, redirect_url)
{
  ResetPostString();

  var dev = DeviceList.getDeviceByAddress(receiver_address.split(":")[0]),
    specialVal = "";

  if (dev.deviceType.id == "HmIP-RGBW") {
    // Determine the device mode (RGBW, RGB, TW or PWM)
    var maintenanceChannel = DeviceList.getChannelByAddress(receiver_address.split(":")[0] + ":0");
    specialVal = homematic("Interface.getMetadata", {"objectId": maintenanceChannel.id, "dataId": "deviceMode"});
  }

  AddParam($('global_sid'));

  poststr += "&redirect_url="       +redirect_url;
  poststr += "&iface="              +iface;
  poststr += "&sender_address="     +sender_address;
  poststr += "&sender_group="       +sender_group;
  poststr += "&receiver_address="   +receiver_address;
  poststr += "&name="               +encodeURIComponent(name);
  poststr += "&description="        +description;
  poststr += "&group_name="         +encodeURIComponent(group_name);
  poststr += "&group_description="  +group_description;
  poststr += "&actorDeviceTypeId="  +dev.deviceType.id;
  poststr += "&specialVal="  +specialVal;
  poststr += "&cmd=addLink";

  //ProgressBar = new ProgressBarMsgBox("Verknüpfung wird erstellt...", 1);
  ProgressBar = new ProgressBarMsgBox(translateKey("progressBarCreateLinkTitle"), 1);
  ProgressBar.show();
    ProgressBar.StartKnightRiderLight();

  SendPOSTRequest('ic_ifacecmd.cgi');
};

ShowNewLinkSummary = function(iface, sender_address, receiver_address, name, description, group_name, group_description)
{
  ResetPostString();
  
  poststr += "&iface="             +iface;
  poststr += "&sender_address="    +sender_address;
  poststr += "&receiver_address="  +receiver_address;

  name = name.escapeHTML();
  poststr += "&name="              +name;
  
  description = description.escapeHTML();
  poststr += "&description="       +description;
  
  if (group_name)        
  {
    group_name = group_name.escapeHTML();
    poststr += "&group_name=" +group_name;
  }

  if (group_description)
  {
    group_description = group_description.escapeHTML();
    poststr += "&group_description=" +group_description;
  }

  updateContent(UI_PATH + "ic_selchannel.cgi", poststr);
};

Select2ndLinkPartner = function(iface, address, direction)
{
  ResetPostString();

  poststr += "&iface=" +iface;
  
  if (direction == 2) poststr += "&receiver_address=" +address;
  else                poststr += "&sender_address="   +address;
  
  updateContent(UI_PATH + "ic_selchannel.cgi", poststr);
};

CollectData_AddLink = function(goto_profiles)
{
  var nextPage = (goto_profiles==1 ? 'IC_SETPROFILES' : 'IC_LINKPEERLIST');
  
  AddLink(
  $F('global_iface'), 
  $F('global_sender_address'),
  $F('global_sender_group'),
  $F('global_receiver_address'),
  $F('input_name'),
  $F('input_description'), 
  $F('input_group_name'),
  $F('input_group_description'),
  nextPage);
};
