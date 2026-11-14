#!/bin/tclsh
source ../once.tcl
sourceOnce cgi.tcl
sourceOnce session.tcl
sourceOnce common.tcl
loadOnce tclrpc.so

set sid [session_urlsid]
set urlsid "$sidname=$sid"

array set interfaces ""
array set interface_descriptions ""

proc read_interfaces {} {
    global interfaces interface_descriptions INTERFACES_FILE env
    set retval 1
    if { [ info exist env(BIDCOS_SERVICE) ] } {
        set interfaces(default) "$env(BIDCOS_SERVICE)"
        set interface_descriptions(default) "Default BidCoS Interface"
    } else {
        set fd -1
        catch {set fd [open $INTERFACES_FILE r]}
        if { $fd >=0 } {
            set contents [read $fd]
            while { [regexp -indices {</ipc[^>]*>} $contents range] } {
                set section [string range $contents 0 [lindex $range 1]]
                set contents [string range $contents [expr [lindex $range 1] + 1] end]
                if { 
                        [regexp {<name[^>]*>([^<]+)</name} $section dummy name] &&
                        [regexp {<url[^>]*>([^<]+)</url} $section dummy url] &&
                        [regexp {<info[^>]*>([^<]+)</info} $section dummy description ]
                } {
                    array set interfaces [list $name $url]
                    array set interface_descriptions [list $name $description]
                }
            }
            close $fd
        } else {
            puts "Could not open interface file"
            set retval 0
        }
    }
    return $retval
}

proc list_devices {} {
    global env interfaces urlsid
    
    set FLAG_INTERNAL 2
    
    set dev_pending 0
    foreach iface [array names interfaces] {
#        catch {
            set url $interfaces($iface)
            set bidcos_iface_count 1
                set bidcos_iface_count [llength [xmlrpc $url listBidcosInterfaces]]
            set devlist [xmlrpc $url listDevices [list bool 0]]
            foreach dev $devlist {
                array_clear dev_descr
                array set dev_descr $dev
                if { $dev_descr(FLAGS) & $FLAG_INTERNAL } continue
                if { "$dev_descr(PARENT)" == "" } {
                    # device object
                    if { $dev_pending } {
                        puts "arDevices\[arDevices.length\] = deviceData;"
                        set dev_pending 0
                    }
                    set dev_pending 1
                    array_clear dev_metadata
                    set name "$dev_descr(TYPE) $dev_descr(ADDRESS)"
                    catch { 
                        array set dev_metadata [xmlrpc $url getAllMetadata [list string $dev_descr(ADDRESS)]] 
                        set name $dev_metadata(NAME)
                    }
                    puts "deviceData = new Object();"
                    puts "deviceData\['id'\] = '$dev_descr(ADDRESS)';"
                    puts "deviceData\['name'\] = '$name';"
                    puts "deviceData\['type'\] = '$dev_descr(TYPE)';"
                    puts "deviceData\['pic'\] = '';"
                    puts "deviceData\['desc'\] = DEV_getDescription('$dev_descr(TYPE)');"
                    puts "deviceData\['sn'\] = '$dev_descr(ADDRESS)';"
                    puts "deviceData\['flags'\] = $dev_descr(FLAGS);"
                    puts "deviceData\['iface'\] = '$iface';"
                    if { $dev_descr(INTERFACE) != "" } {
                        set roaming [expr $dev_descr(ROAMING)?"/*":""]
                        puts "deviceData\['bidcos_iface'\] = '$dev_descr(INTERFACE)$roaming';"
                        puts "deviceData\['bidcos_iface_change'\] = [expr $bidcos_iface_count > 1 ? true : false];"
                    } else {
                        puts "deviceData\['bidcos_iface'\] = 'Intern';"
                        puts "deviceData\['bidcos_iface_change'\] = false;"
                    }
                    puts "deviceData\['loaded'\] = false;"
                    puts "deviceData\['gm'\] = 0;"
                    puts "deviceData\['func'\] = '';"
                    puts "deviceData\['room'\] = '';"
                    puts "deviceData\['trans'\] = '';"
                    puts "deviceData\['commtestdp'\] = false;"
                    puts "deviceData\['testrunning'\] = false;"
                    puts "deviceData\['fltOpts'\] = new Object();"
                    puts "deviceData\['fltOpts'\]\['iCat'\] = 0;"
                    puts "deviceData\['fltOpts'\]\['iTrans'\] = 0;"
                    array_clear channels_aes
                    array_clear channels_rooms
                    array_clear channels_funcs
                } else {
                    # channel object
                    if { $dev_descr(AES_ACTIVE) } {
                        set aes_mode "Gesichert"
                    } else {
                        set aes_mode "Standard"
                    }
                    if { ![info exists channels_aes($aes_mode)] } {
                        puts "if( deviceData\['trans']\ != '' ) deviceData\['trans'\] += ' ';"
                        puts "deviceData\['trans'\] += '$aes_mode';"
                        set channels_aes($aes_mode) 1
                    }
                }
            }
 #       }
    }
    if { $dev_pending } {
        puts "arDevices\[arDevices.length\] = deviceData;"
    }
}
      
proc put_page {} {

puts {
<script type="text/javascript">

  ResetGAC();

  setPath("<span onclick='loadSystemConfig()'>Einstellungen</span> &gt; Ger&auml;te");
  arSubIds = new Array();
  arImgIds = new Array(); // +/-
  var deviceData = null;
  arDevices = new Array(); // Objects der Geräte
  arDevPosis = new Array(); // Position der Geräte-Tabellenreihe
  arSortChns = new Array();
  arIeBugCells = new Array(); // Zellen-IDs IE-Darstellungsfehler (kein rechter Rand)
  fltD = new iseFilter('fltD');
  bEIP = false; // Expand In Progress
  bNecc = false; // to reload page in neccecary
  bNameFilter = false;
  bPassFilterName = false;
</script>

<div id="dummy" style="display:none"></div>
<div style="width:100%;height:100%;overflow-y:scroll;">
<div id="noDevs" style="padding: 10px;display:none;">Es wurden noch keine Ger&auml;te angelernt.</div>
<table id="tbrDevices" class="tblList" cellpadding="0" width="95%" style="empty-cells:show;display:none">
<colgroup>
  <col style="width:3%;"/> <!-- Plus/Minus Device-->
  <col style="width:3%;"/> <!-- Plus/Minus Group -->
  <col style="width:3%;"/> <!-- Leer vor GroupMember -->
  <col style="width:36%;"/> <!-- Name -->
  <col style="width:10%;"/> <!-- Typenbezeichnung -->
  <col style="width:6%;"/> <!-- Bild -->
  <col style="width:10%;"/> <!-- Bezeichnung -->
  <col style="width:8%;"/> <!-- Seriennummer -->
  <col style="width:8%;"/> <!-- Interface / Kategorie -->
  <col style="width:8%;"/> <!-- Uebertragungsmodus -->
  <col style="width:5%;"/> <!-- Aktion -->
</colgroup>
<tbody id="tblDevices">
<tr>
  <th id="tblListFold"></th>
  
  <th class="Sort" colspan='3' style="font-size:10pt;height:40px;"><b>Name</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Typen-<br />Bezeichnung</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Bild</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Bezeichnung</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Serien-<br />nummer</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Interface /<br />Kategorie</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>&Uuml;bertragungs-<br />modus</b></th>
  <th class="Sort" style="font-size:10pt;height:40px;"><b>Aktion</b></th>
</tr>
<tr style="height:auto;">
  <td></td>
  <td colspan='3' style="background-color:#999999;color:White;">
    <div id="btnFilterName" class="FilterBtn">Filter</div>
    <div class="FilterSubMenu" id="btnFilterNameSub" style="display:none">
        <input id="ftName" type="text" size="15" style="margin:5px;" />
        <div class='FilterSetButton' onclick="bNameFilter=true;setSFilter(fltD, 'ftName', FIL_NAME);updateTable();$('btnFilterNameSub').hide();">Setzen</div>
        <div class='FilterSetButton' onclick='$("btnFilterNameSub").hide();'>Schliessen</div>
    </div>
  </td>
  <td style="background-color:#999999;color:White;">
    <div id="btnFilterType" class="FilterBtn">Filter</div>
    <div class="FilterSubMenu" id="btnFilterTypeSub" style="display:none">
        <input id="ftType" type="text" size="15" style="margin:5px;" />
        <div class="FilterSetButton" onclick="setSFilter(fltD, 'ftType', FIL_TYPE);updateTable();$('btnFilterTypeSub').hide();" style="margin-left:5px;">Setzen</div>
        <div class='FilterSetButton' onclick="$('btnFilterTypeSub').hide();">Schliessen</div>
    </div>
  </td>
  <td style="background-color:#999999;"></td>
  <td style="background-color:#999999;color:White;">
    <div id="btnFilterDesc" class="FilterBtn">Filter</div>
    <div class="FilterSubMenu" id="btnFilterDescSub" style="display:none">
        <input id="ftDesc" type="text" size="15" style="margin:5px;" />
        <div class="FilterSetButton" onclick="setSFilter(fltD, 'ftDesc', FIL_DESC);updateTable();$('btnFilterDescSub').hide();" style="margin-left:5px;">Setzen</div>
        <div class='FilterSetButton' onclick="$('btnFilterDescSub').hide();">Schliessen</div>
    </div>
  </td>
  <td style="background-color:#999999;color:White;">
    <div id="btnFilterSN" class="FilterBtn">Filter</div>
    <div class="FilterSubMenu" id="btnFilterSNSub" style="display:none">
        <input id="ftSN" type="text" size="15" style="margin:5px;" />
        <div class="FilterSetButton" onclick="setSFilter(fltD, 'ftSN', FIL_SN);updateTable();$('btnFilterSNSub').hide();" style="margin-left:5px;">Setzen</div>
        <div class='FilterSetButton' onclick="$('btnFilterSNSub').hide();">Schliessen</div>
    </div>
  </td>
  
  <td style="background-color:#999999;"></td>
  <td style="background-color:#999999;"></td>
  <td style="background-color:#999999;"></td>
</tr>

<!-- END OF FILTER -->
}

puts { <script type="text/javascript"> }

list_devices

puts {
var dev_sort_column="name";
function sort_devices(a, b)
{
  if(a[dev_sort_column] < b[dev_sort_column])return -1;
  if(a[dev_sort_column] > b[dev_sort_column])return 1;
  return 0;
}

arDevices.sort(sort_devices);
var FLAG_DONTDELETE = (1<<3);
for (var i = 0; i < arDevices.length; i++) {
  if (arDevices[i] != null) {
    // Geraet hinzufuegen
    var tr = Builder.node('tr', {id: "tr" + arDevices[i]['id']});
    var td = Builder.node('td');
    var sStyle = "";
    arImgIds[arImgIds.length] = "img" + arDevices[i]['id'];
    var img = Builder.node('img', {src: "/ise/img/plus.png", id:"img" + arDevices[i]['id'], alt:"plus", style: sStyle, onclick: "expandDevice(" + i + ")"});
    td.appendChild(img);
    tr.appendChild(td);
    
    
    if (NAV_IE) {
      td = Builder.node('td', {className: 'WhiteBkg',  colSpan:3}, 
          [Builder.node('div', {id:'tdn' + arDevices[i]['id'], style: "height:100%;width:100%;cursor:pointer", onclick: "textEdit(id,'"+arDevices[i]['id']+";NAME', false, 0, 0)"}, arDevices[i]['name']) ]); tr.appendChild(td);
    }
    else {
      td = Builder.node('td', {colSpan:3, className: 'WhiteBkg', style: "cursor:pointer", id:'tdn' + arDevices[i]['id'], onclick: "textEdit(id, '"+arDevices[i]['id']+";NAME', false, 0, 0)"}, arDevices[i]['name']); tr.appendChild(td);
    }
    td = Builder.node('td', {className: 'GrayBkg'}, strCut(arDevices[i]['type'], true)); tr.appendChild(td);
    td = Builder.node('td', {className: 'WhiteBkg'}, [
      Builder.node('div', {id: 'picDiv' + arDevices[i]['id'], style: "position:relative; width:50px; height:55px;",
                          onmouseover:"picDivShow(jg_250, '"+arDevices[i]['type']+"', 250, '-1', this);", onmouseout:"picDivHide(jg_250);"})
      ]);
    Element.setStyle(td, {textAlign: 'left', backgroundColor: 'white'});
    tr.appendChild(td);
    td = Builder.node('td', {className: 'GrayBkg'}, spaceCut(arDevices[i]['desc'])); tr.appendChild(td);
    td = Builder.node('td', {className: 'GrayBkg'}, strCut(arDevices[i]['sn'])); tr.appendChild(td);
    
    if (arDevices[i]['bidcos_iface_change']) {
        var ifaceId = "iface" + arDevices[i]['id'];
        var onClick = "changeBidcosIface('"+ arDevices[i]['id'] + "','" + ifaceId + "')";
        td = Builder.node('td', {className: 'WhiteBkg'}, 
            [Builder.node('div', {id: ifaceId, style: "height:100%;width:100%;cursor:pointer", onclick: onClick}, arDevices[i]['bidcos_iface']) ]);
    } else {
        td = Builder.node('td', {className: 'GrayBkg'}, arDevices[i]['bidcos_iface']);
    }
    tr.appendChild(td);
    
    td = Builder.node('td', {className: 'GrayBkg'}, spaceCut(arDevices[i]['trans'])); tr.appendChild(td);
    
    td = Builder.node('td', {className: 'WhiteBkg'}, ''); 
    writeDeviceAction(td, false, true, !(arDevices[i]['flags']&FLAG_DONTDELETE), arDevices[i], false);
    tr.appendChild(td);
    $("tblDevices").appendChild(tr);
    var jg_0 = new jsGraphics('picDiv' + arDevices[i]['id']);
    InitGD(jg_0, 50);
    Draw(jg_0, arDevices[i]['type'], 50, '-1');
    
    arDevPosis[arDevPosis.length] = i+3;
  }
}

if (NAV_IE) 
{
  var tr = Builder.node('tr');
  Element.setStyle(tr, {backgroundColor: '#003366'});
  for (var i = 0; i < 15; i++)
  {
    var td = Builder.node('td', {}, ".");
    Element.setStyle(td, {backgroundColor: '#003366', color: '#003366'});
    tr.appendChild(td);
  }
  $("tblDevices").appendChild(tr);
}

if (arDevices.length > 0)
  show("tbrDevices");
else
  show("noDevs");
document.body.style.cursor = "auto";
</script>
</tbody>
</table>
</div>

<script type="text/javascript">
  // Build filter object
  var mouseOpts = {
    onTopOver: function(divToShow) {
      selectFilters(fltD, divToShow);
    }
  };
  //new iseSubMenuControl("btnFilterIface", "btnFilterIfaceSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  //new iseSubMenuControl("btnFilterMode", "btnFilterModeSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  new iseSubMenuControl("btnFilterName", "btnFilterNameSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  new iseSubMenuControl("btnFilterType", "btnFilterTypeSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  new iseSubMenuControl("btnFilterDesc", "btnFilterDescSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  new iseSubMenuControl("btnFilterSN", "btnFilterSNSub", "tblDevices", {d: 22}, mouseOpts, false, true);
  
  // new deviceLoader( <%Write(iCount); %>, false );
  var iDevicesExpanded = 0;
  
  
  
  /* * * * * * * Expand / Collapse  * * * * * * * * * * * * * * * * * * * */
  
  expandDevice = function(arIdx, bAutoExpandGroups) {    
    // Ausklappen
    var id = arDevices[arIdx]['id'];
    bPassFilterName = fltD.objPassFilter(arDevices[arIdx]);
    if ($("img" + id).alt == "plus") {
      if (!bEIP)
      {
        $("img" + id).src = "/ise/img/minus.png";
        $("img" + id).alt = "minus";
        arDevices[arIdx]['expanded'] = true;
        if (arDevices[arIdx]['loaded']) 
        { 
          for (var i = 0; i < arDevices[arIdx]['chns'].length; i++)
          {
            if ((fltD.objPassFilter(arDevices[arIdx]['chns'][i])) || (bPassFilterName))
            {
              show(arDevices[arIdx]['chns'][i]['trid']);
              // show picture for row
              show('picDiv' + arDevices[arIdx]['chns'][i]['id']);
            }
          }
          
          for (var i = 0; i < arDevices[arIdx]['chnsgr'].length; i++)
          {
            if ( fltD.objPassFilter(arDevices[arIdx]['chnsgr'][i][0]) ||
                 fltD.objPassFilter(arDevices[arIdx]['chnsgr'][i][1]))
            {
              show(arDevices[arIdx]['chnsgr'][i]['trid']);
              // show picture for row
              show('picDiv' + arDevices[arIdx]['chnsgr'][i][0]['id'] + arDevices[arIdx]['chnsgr'][i][1]['id']);
              if (bAutoExpandGroups)
              {
                expandGroup(arDevices[arIdx]['id'],
                   arDevices[arIdx]['chnsgr'][i][0]['id'],
                   arDevices[arIdx]['chnsgr'][i][1]['id']);
              }
            }
          }
          iDevicesExpanded++;
        }
        else {
          document.body.style.cursor = "wait";              
          var pb = "";
          pb += "devid=" + id + "&";
          pb += "arIdx=" + arIdx + "&";
          if (bAutoExpandGroups)
            pb += "autoExpGr=1&";
          else 
            pb += "autoExpGr=0&";
          pb += "insIdx=" + arDevPosis[arIdx] + "&";
          pb += "iface=" + arDevices[arIdx]['iface'] + "&";
          var opts = {
            evalScripts: true, 
            postBody: pb,
            sendXML: false,
            asynchronous: false,
            onComplete: function()
            {
              arDevices[arIdx]['loaded'] = true;
              bEIP = false;
            } 
          };
          new Ajax.Updater("dummy", "/configapp/deviceLoader.cgi?sid=" + SessionId, opts);
          bEIP = true;
          iDevicesExpanded++;
        }
      }
      else {
        var s = "false";
        if (bAutoExpandGroups)
          s = "true";
        window.setTimeout("expandDevice(" + arIdx+ ", "+s+")", 500);
      }
    }
    // Einklappen
    else
    {
      $("img" + id).src = "/ise/img/plus.png";
      $("img" + id).alt = "plus";
      arDevices[arIdx]['expanded'] = false;
      if (arDevices[arIdx]['chns'])
      {
        for (var i = 0; i < arDevices[arIdx]['chns'].length; i++)
        {
          hide(""+arDevices[arIdx]['id']+ arDevices[arIdx]['chns'][i]['id']);
          // arDevices[arIdx]['chns'][i]['id'] -> id des Channels
          // hide picture for row
          hide('picDiv' + arDevices[arIdx]['chns'][i]['id']);
        }
      }
      if (arDevices[arIdx]['chnsgr'])
      {
        for (var i = 0; i < arDevices[arIdx]['chnsgr'].length; i++)
        {
          // arDevices[arIdx]['chnsgr'][i][0]['id'] -> id des Channels in der channel group 0
          // hide picture for row
          hide('picDiv' + arDevices[arIdx]['chnsgr'][i][0]['id'] + arDevices[arIdx]['chnsgr'][i][1]['id']);
          collapseGroup(arDevices[arIdx]['id'], arDevices[arIdx]['chnsgr'][i][0]['id'], arDevices[arIdx]['chnsgr'][i][1]['id'], false);
        }
        iDevicesExpanded--;
      }
    }
    refreshFooterButton();
  }
  
  
  expandGroup = function(devId, id1, id2)
  {
    if ($("img" + id1 + id2).alt == "plus")
    {
      $("img" + id1 + id2).src = "/ise/img/minus.png";
      $("img" + id1 + id2).alt = "minus";
      show("" + devId + id1 + id2 + "1");
      show("" + devId + id1 + id2 + "2");
      // show channel picture pictures
      show('picDiv' + id1)
      show('picDiv' + id2);
    }
    // Einklappen
    else
    {
      collapseGroup(devId, id1, id2, true);
    }
  }
  
  collapseGroup = function(devId, id1, id2, subsOnly)
  {
    if($("img" + id1 + id2))
    {
      $("img" + id1 + id2).src = "/ise/img/plus.png";
      $("img" + id1 + id2).alt = "plus";
    }
    if (!subsOnly)
      hide("" + devId + id1 + id2);
      hide("" + devId + id1 + id2 + "1");
      hide("" + devId + id1 + id2 + "2");
      // hide channel pictures
      hide('picDiv' + id1);
      hide('picDiv' + id2);
    }
  
  collapseAll = function()
  {
    var pb = "integer iNewOnly = 0;";
    if (iDevicesExpanded == 0)
    {
       for (var i = 0; i < arDevices.length; i++)
       {
         expandDevice( i, true);
       }
    }
    else
    {
      for (var i = 0; i < arSubIds.length; i++)
      {
        $(arSubIds[i]).hide();
      }
      for (var i = 0; i < arImgIds.length; i++)
      {
        $(arImgIds[i]).src = "/ise/img/plus.png";
        $(arImgIds[i]).alt = "plus";
      }
      for (var i = 0; i < arDevices.length; i++)
      {
        arDevices[i]['expanded'] = false;
        if(arDevices[i]['chns'])
        {     
          for(var x = 0; x < arDevices[i]['chns'].length; x++)
          {
            hide('picDiv'+arDevices[i]['chns'][x]['id']);
          }
        }
        if(arDevices[i]['chnsgr'])
        {
          for(var x = 0; x < arDevices[i]['chnsgr'].length; x++)
          {
            collapseGroup(arDevices[i]['id'],arDevices[i]['chnsgr'][x][0]['id'],arDevices[i]['chnsgr'][x][1]['id'],false);
            hide('picDiv'+arDevices[i]['chnsgr'][x][0]['id']+arDevices[i]['chnsgr'][x][1]['id']);
          }
        }
      }
      iDevicesExpanded = 0;
    }
    refreshFooterButton();
  }
  
  refreshFooterButton = function()
  {
    if (iDevicesExpanded == 0) {
      $("btnStructure").innerHTML = "Baumstruktur<br />&ouml;ffnen";
    }
    else {
      $("btnStructure").innerHTML = "Baumstruktur schlie&szlig;en";
    }
  }
  
  specialReloadPage = function() 
  {
    if(bNecc)
    {
      reloadPage();
    } 
  }
  
  updateTable = function() {
    var bDoOpenStructure = false;
    colorFilterBtns(fltD);
    for (var i = 0; i < arDevices.length; i++)
    {
      var iShown = 0;
          
      bPassFilterName = fltD.objPassFilter(arDevices[i]);    
      if (arDevices[i]['chns'])
      {  // Prüfen ob Kanäle eingelesen wurden
        for (var x = 0; x < arDevices[i]['chns'].length; x++)
        {
          if( $(arDevices[i]['chns'][x]['trid']) )// prüfen ob Reihe überhaupt vorhanden ist
          { 
            if ((!fltD.objPassFilter(arDevices[i]['chns'][x])) && (!bPassFilterName))
            {
              hide(arDevices[i]['chns'][x]['trid']);
              hide("picDiv" + arDevices[i]['chns'][x]['id']);
            }
            else
            {
              iShown++;
              if (arDevices[i]['expanded'])
              {
                show(arDevices[i]['chns'][x]['trid']);
                show("picDiv" + arDevices[i]['chns'][x]['id']);
              }
            }
          }
        }
        if (arDevices[i]['chnsgr']) {
          for (var x = 0; x < arDevices[i]['chnsgr'].length; x++) {
            for (var y = 0; y < 2; y++) {
              var bShowGrTr = false;
              if ($(arDevices[i]['chnsgr'][x][y]['trid'])) { // prüfen ob Reihe überhaupt vorhanden ist
                if (!fltD.objPassFilter(arDevices[i]['chnsgr'][x][y])) {
                  hide(arDevices[i]['chnsgr'][x][y]['trid']);
                  hide("picDiv" + arDevices[i]['chnsgr'][x][y]['id']);
                }
                else {
                  iShown++;
                  if (arDevices[i]['expanded']) {
                    show(arDevices[i]['chnsgr'][x][y]['trid']);
                    show("picDiv" + arDevices[i]['chnsgr'][x][y]['id']);
                    bShowGrTr= true;
                  }
                }
              }
              if (!bShowGrTr) {
                hide("" + arDevices[i]['id'] + arDevices[i]['chnsgr'][x][0]['id'] + arDevices[i]['chnsgr'][x][1]['id']);
                hide("picDiv" + arDevices[i]['chnsgr'][x][0]['id'] + arDevices[i]['chnsgr'][x][1]['id']);
              }
              else {
                show("" + arDevices[i]['id'] + arDevices[i]['chnsgr'][x][0]['id'] + arDevices[i]['chnsgr'][x][1]['id']);
                show("picDiv" + arDevices[i]['chnsgr'][x][0]['id'] + arDevices[i]['chnsgr'][x][1]['id']);
              }
            }
          }
        }
        if (iShown == 0) {
          hide("tr" + arDevices[i]['id']);          
          hide("picDiv" + arDevices[i]['id']);
        }
        else {
          show("tr" + arDevices[i]['id']);          
          show("picDiv" + arDevices[i]['id']);          
        }
      }
      else
      {
        bDoOpenStructure = true;
      }      
    }
    if( bDoOpenStructure ) collapseAll();
  }
  
  editRooms = function() {
    dlgPopup = new iseMessageBox(ID_ROOMS);
    PopupClose = function() {
      dlgPopup.hide();
      iseRooms.buildRoomOverlay();
    }
    dlgPopup.ShowPopup();
  }
  
  editFuncs = function() {
    dlgPopup = new iseMessageBox(ID_FUNCTIONS);
    PopupClose = function() {
      dlgPopup.hide();
      iseFunctions.buildFuncOverlay();
    }
    dlgPopup.ShowPopup();
  }
  
  restoreDevs = function() {
    for (var i = 0; i <arDevices.length; i++) {
      show("tr" + arDevices[i]['id']);
      show("picDiv" + arDevices[i]['id']);
    }
  }
  var s = "";
  s += "<table style='background-color:transparent;' cellspacing='8'>";
  s += "<tr>";
    s += "<td style='text-align:center; vertical-align:middle;'><div id='btnClFl' class='FooterButton' onclick='restoreDevs();fltD.clearFilters();updateTable()' style='line-height:15px'>Filter<br />zur&uuml;cksetzen</div></td>";
    s += "<td style='text-align:center; vertical-align:middle;'><div id='btnStructure' class='FooterButton' style='line-height:15px;font-size:smaller;' onclick='collapseAll();'>Baumstruktur<br />&ouml;ffnen</div></td>";
  s += "</tr>";
  s += "</table>";
  setFooter(s);
  
  if (screen.availWidth > 1200) {
    $("btnClFl").setStyle({fontSize: 'smaller'});
    $("btnStructure").setStyle({fontSize: 'smaller'});
  }
  
  devSort = function () {
    updateContent("/pages/tabs/admin/views/devicechannels.htm");
  }
  
  NewDevSort = function (colName)
  {
    document.body.style.cursor = "wait";
    updateContent("/pages/tabs/admin/views/devicechannels.htm", null, "system.SetSessionVar('sessionLS', '" + colName + "');");
  }
</script>
}
}

cgi_eval {
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }
    read_interfaces
    
    http_head

    if {[session_requestisvalid 0] > 0} then {
        put_page
    }

}
