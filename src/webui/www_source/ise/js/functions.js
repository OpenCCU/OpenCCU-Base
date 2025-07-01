/**
 * ise/functions.js
 **/

/**
 * @fileOverview Allgemeine Funktionen
 * @author ise, Änderungen durch Falk Werner (eQ-3)
 **/

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * *      Global Constants         * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * */

iseDOM_BuildLabel = "R1.00.0381.0100";
dbg = false;
rfr = true;

ID_ROOMS     = 101;  // Rooms collection ID.
ID_FUNCTIONS = 151;  // Functions collection ID.
ID_FAVORITES = 201;  // Favorites collection ID.
ID_LINKS     = 301;  // Links collection ID.
ID_TRANSFER_PARAMETERS = 1001;
ID_CHANNEL_LIST        = 1002;
ID_NEW_SYSVAR          = 1003;
ID_CREATE_SCRIPT       = 1005; 
ID_EDIT_SCRIPT         = 1006;
ID_CONTROL_TEST        = 1007;
ID_INSERT_VALUE        = 1008;
ID_INSERT_STRING       = 1009;
ID_SET_VALUE_RANGE     = 1010;
ID_DEL_SYS_VARIABLE    = 1011;
ID_SYS_VARS_SELECTION  = 1012;
ID_TIMEMODULE          = 1013;
ID_STATUSINFO          = 1014;
ID_STATUSINFO_WARNING  = 1015;
ID_USER_ACCOUNT_CONFIG_ADMIN = 1016;
ID_USER_ACCOUNT_CONFIG_USER  = 1017;
ID_AUTO_LOGIN_CONFIG     = 1018;
ID_PROGRAM_CHOOSER       = 1019;
ID_EDIT_SYSVAR           = 1021;
ID_CHOOSE_LED            = 1022;
ID_SET_OUCFM_MODE        = 1023;
ID_SET_STATUS_DISPLAY    = 1024;
ID_BIDCOS_INTERFACE = 1025;

nUA = navigator.userAgent;
NAV_IE = false;
NAV_MOZ = false;

if (nUA.toLowerCase().indexOf("msie") > -1) {
  NAV_IE = true;
}
if (nUA.toLowerCase().indexOf("gecko") > -1) {
  NAV_MOZ = true;
}
SCREEN_HEIGHT = 0; // wird in index.htm gesetzt
SCREEN_WIDTH  = 0; // wird in index.htm gesetzt

// Sortier-IDs
SORT_NAME = 1;
SORT_TYPE = 2;
SORT_DESC = 3;
SORT_SN = 4;
SORT_IFACE = 5;
SORT_IMODE = 6;
SORT_FUNC = 7;
SORT_ROOM = 8;

// Sysvar Types
SYSVAR_ALARM = 6;
SYSVAR_BOOL = 2;
SYSVAR_ENUM = 29;
SYSVAR_GENERIC = 0;
SYSVAR_STRING = 11;

// Filter-IDs
FIL_NAME     = 1;
FIL_TYPE     = 2;
FIL_DESC     = 3;
FIL_SN       = 4;
FIL_IFACE    = 5;
FIL_IFACE_S  = 6;
FIL_IFACE_R  = 7;
FIL_MODE     = 8;
FIL_MODE_AES = 9;
FIL_MODE_STD = 10;
FIL_FUNCS    = 11;
FIL_ROOMS    = 12;
FIL_UNIT     = 13;
FIL_CHN      = 14;
FIL_TIME     = 15;
FIL_DATE     = 16;
FIL_USERS    = 17;

TM_ONCE = 8; // ttCalOnce
TM_PERIODIC = 4; // ttCalPeriodic
TM_DAILY = 9; // ttCalDaily
TM_WEEKLY = 5; // ttCalWeekly
TM_MONTHLY = 6; // ttCalMonthly
TM_YEARLY = 7; // ttCalYearly
TM_MON = 1;
TM_TUE = 2;
TM_WED = 4;
TM_THU = 8;
TM_FRI = 16;
TM_SAT = 32;
TM_SUN = 64;
TM_WEEKEND = 96;
TM_WORKDAYS = (TM_FRI * 2) - 1;

CALL_STRCUT = 1;
CALL_SPACECUT = 2;

GROUPASSIGNMENT = null;

dlgResult = "";
sPreviousPage = "";
sPreviousPage2 = "";
sPreviousPageArgs = "";
sPreviousPageArgs2 = "";
sActPage = "";
sActPageArgs = "";
sPrevVal = "";
ul = 0;
var wndHelp;
bTxtEditMode = 0;
tmpNoOfChannels = [];
timeoutCounter = [];
deviceInputCheckedDevices = [];
doClearTempView = true;
bUpdateContentRunning = false;
bCheckForAllChannels = false;

/*#########################*/
/*# Erweiterte Funktionen #*/
/*#########################*/

setTime = function(time)
{
  if ($("maintime")) { $("maintime").innerHTML = time; }
};

setDate = function(date)
{
  if ($("maindate")) { $("maindate").innerHTML = date; }
};

setAlarmMessageCount = function (count) {

  if ($("msgAlarms")) {
    //$("msgAlarms").innerHTML = "${Alarmmeldungen} (" + count + ")";
    $("msgAlarms").innerHTML = translateKey('alarmMsg') + " (" + count + ")";

  }
  if (count === 0) {
    if ($("imgAlarms")) {
      $("imgAlarms").src = "/ise/img/dot/green.png";
    }
  }
  else {
    if ($("imgAlarms")) {
      $("imgAlarms").src = "/ise/img/dot/red.png";
    }
  }
};

setServiceMessageCount = function (count) {
  if ($("msgServices")) {
    $("msgServices").innerHTML = translateKey('serviceMsg') + " (" + count + ")";

  }
  if (count === 0) {
    if ($("imgServices")) {
      $("imgServices").src = "/ise/img/dot/green.png";
    }
  }
  else {
    if ($("imgServices")) {
      $("imgServices").src = "/ise/img/dot/yellow.png";
    }
  }
};

ReceiptAlarm = function(id,reload)
{
  var url = '/esp/functions.htm?sid='+SessionId;
  var pb = "";
  pb += 'string action = "ReceiptAlarm";';
  pb += 'string id = "'+id+'";';
  var opt =  {
    postBody: pb,
    onComplete: function(transport)
    {
      if( $("al"+id) )
      {
        hide( "al"+id );
      }
      else
      {
        if( typeof(reload) == "undefined" )
        {
          reloadPage();
        }
      }
    }
  };
  new Ajax.Request(url,opt);
};


/**
 * Aktualisiert Systemvariablen
 **/
updateSysVar = function(id, value)
{
  var PREFIX = "SYSVAR_";
  var element = $(PREFIX + id);
  
  if (element) 
  {
    element.innerHTML = "";
    element.appendChild(document.createTextNode(translateString(value)));
  }
};


/*setOldEnergyCounterVal = function(chn, value) {
  arrOldEnergyCounterVal[chn] = value;
};

getOldEnergyCounterVal = function(chn) {

  if (typeof arrOldEnergyCounterVal === "undefined") {
    arrOldEnergyCounterVal = [];
    arrOldEnergyCounterVal[chn] = 0;
  }

  return arrOldEnergyCounterVal[chn];
};*/

setValueOfEnergyCounters = function() {
  jQuery.each(arrEnergyCounter, function(index, counter){
    var id = counter.name.split("_")[1],
    chnVal = homematic("Channel.getValue", {"id": id});

    // Setze EnergyCounter-Variable
    homematic("SysVar.setFloat", {"name" : counter.name, "value" : chnVal});
  });
};

/**
 * Ermittelt die Systemvariablen f. d. Energiemessung
 */
updateSysVarEnergyCounter = function() {
 // conInfo("update SysVar EnergyCounter");

  if (typeof readEnergyCounter === "undefined" || readEnergyCounter == true) {
    var allSysVars = homematic("SysVar.getAll", {});

    arrEnergyCounter = [];

    jQuery.each(allSysVars, function(index, sysvar){
      if (sysvar.name.match(/EnergyCounter/) != null) {
        arrEnergyCounter.push(sysvar);
      }
    });
    readEnergyCounter = false; // wird beim Anlernen eines neuen Gerätes wieder auf true gesetzt
  }
  setValueOfEnergyCounters();
 };

/**
 * Markiert ein Gerät im Posteingang als fertig bzw. nicht fertig
 **/
setDeviceReadyConfig = function(id, isReady)
{
  var readyButton = $("readyBtn" + id);
  
  if (readyButton)
  {
    readyButton._isReady = isReady; 
  }
};

/**
 * Markiert einen Kanal im Posteingang als ferig bzw. nicht fertig
 **/
setChannelReadyConfig = function(id, isReady)
{
  var readyBox = $("inp" + id);

  if (readyBox)
  {
    readyBox.checked  = isReady;
    readyBox._isReady = isReady;
  }
};

/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * *   Navigation Functions        * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * */
 
LoadingHTML = function()
{
  return '<img style="margin: 30px;" src="/ise/img/loading.gif" alt=""/>';
};



updateContent = function(file, argsForUrl, codeToExec, bDontSaveUrl)
{
  if( !bUpdateContentRunning )
  {
    bUpdateContentRunning=true;
    iseRefr(false);
    /* <!-- ELV */
    if( !bDontSaveUrl )
    {
      if( sActPage != file )
      {
        conInfo("updateContent: Saving previous page ["+file+"].");
        sPreviousPage2 = sPreviousPage;
        sPreviousPageArgs2 = sPreviousPageArgs;
        sPreviousPage = sActPage;
        sPreviousPageArgs = sActPageArgs;
      }
    }
    sActPage = file;
    sActPageArgs = argsForUrl;
    /* ELV --> */
    if (dbg) { alert(file); }
    $("content").innerHTML = LoadingHTML();
    //sPreviousPage = sActPage;
    //sActPage = file;
    var pb = '';
    if( doClearTempView )
    {
      pb += 'system.SetSessionVar("sessionCTV", "true");';
    }
    else
    {
      pb += 'system.SetSessionVar("sessionCTV", "false");';
    }
    if(codeToExec)
    {
      pb += codeToExec;
    }
    
    iseInitUpdateArrays();
    
    /* <!-- ELV */
    var opts;
    if (file.substring(0, UI_PATH.length) == UI_PATH)
    {
      //method: 'get' ist für die ELV-CGIs notwendig.
      opts = {evalScripts: true, method: 'get', onComplete:function(){iseRefr(true);bUpdateContentRunning=false;}};
    }
    else
    {
      opts = {postBody: ReGa.encode(pb),evalScripts: true, onComplete:function(){iseRefr(true);bUpdateContentRunning=false;}};
    }
    /* ELV --> */
    
    var url = file + "?sid=" + SessionId;
    if (argsForUrl){ url += argsForUrl; }
    
    //console.info( "updateContent with "+pb );
    
    new Ajax.Updater("content", url, opts);
  }
};

clearUserTempViewIDs2Room = function()
{
  clearUserTempViewIDs2('/pages/tabs/admin/views/rooms.htm');
};

clearUserTempViewIDs2Function = function()
{
  clearUserTempViewIDs2('/pages/tabs/admin/views/functions.htm');
};

clearUserTempViewIDs2 = function(file, args)
{
  // $("content").innerHTML = LoadingHTML();
  sPreviousPage = sActPage;
  sActPage = file;
  var pb = '';
  pb += 'system.SetSessionVar("sessionCTV2", "true");';
  var opts = 
  {
    postBody: ReGa.encode(pb),
    evalScripts: true
  };  
  var url = file + "?sid=" + SessionId;
  if (args){ url += args; }
  // new Ajax.Updater("content", url, opts);
};

reloadSortedPage = function() {
  doClearTempView = false;
  updateContent(sActPage, sActPageArgs, "", true);
};

reloadPage = function() {
  doClearTempView = true;
  updateContent(sActPage, sActPageArgs);
};

loadStartPage = function(fid)
{
  var startPage = "/pages/tabs/startpage.htm";

  //wenn Konfigtool, dann eine andere Startseite anzeigen
  if (PLATFORM != "Central") {startPage = "/configapp/devices.cgi";}

  doClearTempView = true;
  if( typeof(fid) == "undefined" )
  {
    updateContent(startPage);
  }
  else
  {
    updateContent(startPage,"&fid="+fid);
  }
};

loadStartPageGuest = function() {
  doClearTempView = true;
  if( typeof fid == "undefined" )
  {
    updateContent("/pages/tabs/guest/startpageguest.htm");
  }
  else
  {
    updateContent("/pages/tabs/guest/startpageguest.htm","&fid="+fid);
  }
  
};

_loadHandling = function(fid) {
  doClearTempView = true;
  if (typeof fid == "undefined")
    updateContent("/pages/tabs/handling.htm");
  else 
    updateContent("/pages/tabs/handling.htm", "&fid="+fid);
};
  
loadDeviceConfig = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/deviceconfig.htm");
};

loadHelp = function()
{
  wndHelp = null;
  doClearTempView = true;
  updateContent("/config/help.cgi", "&from=internal");
};

loadStatus = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/control.htm");
};

loadAlarmMessages = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/statusviews/alarmMessages.htm");
};

loadServiceMessages = function() {
  var file = "serviceMessages.htm";
  if (PLATFORM != "Central") {file = "serviceMessages.cgi";}
  doClearTempView = true;
  updateContent("/pages/tabs/statusviews/" + file);
};

loadFavorites = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/favorites.htm");
};

loadFavViewer = function(id) {
  doClearTempView = true;
  updateContent("/pages/tabs/favViewer.htm","&id="+id);
};

loadFavOverview = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/user/favOverview.htm");
};

loadNewFav = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/user/newFav.htm");
};

loadLinkProg = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/linkprog.htm");
};

loadSystemConfig = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/systemconfig.htm");
};

loadSystemConfigUser = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/user/systemConfigUser.htm");
};

loadSystemVars = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/systemvars.htm");
};

loadPrograms = function()
{
  doClearTempView = true;
  updateContent("/pages/tabs/admin/views/programs.htm");
};

loadProgramList = function(filter)
{
  doClearTempView = true;
  if( typeof filter == "string" )
  {
    conInfo("loadProgramList: Filter = ["+filter+"]");
    updateContent("/pages/tabs/admin/views/programlist.htm", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
  }
  else
  {
    updateContent("/pages/tabs/admin/views/programlist.htm");
  }
};

loadHandlingRooms = function(roomId)
{
  doClearTempView = true; 
  if (typeof(roomId) == "undefined") { updateContent('/pages/tabs/control/rooms.htm'); }
  else                               { updateContent('/pages/tabs/control/rooms.htm', roomId); }
};

loadHandlingFunctions = function(funcId)
{
  doClearTempView = true; 
  if (typeof(funcId) == "undefined") { updateContent('/pages/tabs/control/functions.htm'); }
  else                               { updateContent('/pages/tabs/control/functions.htm', funcId); }
};

loadHandlingDevices = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/control/devices.htm');
};

loadHandlingPrograms = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/control/programs.htm');
};

ccuAdminFirstStartup = function() {
  doClearTempView = true;
  updateContent('/pages/tabs/admin/adminFirstStart.htm');
};

ccuUserFirstStartup = function() {
  doClearTempView = true;
  updateContent('/pages/tabs/user/userFirstStart.htm');
};

loadSysconfigUserAdmin = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/admin/userAdministration.htm');
};

loadSysconfigSysVars = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/admin/systemvars.htm');
};

loadStatusviewSysProtocol = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/control/systemProtocol.htm');
};


loadStatusviewSysVars = function()
{
  doClearTempView = true; 
  updateContent('/pages/tabs/control/sysvars.htm');
};


loadNewDevices = function(options) {
  doClearTempView = true;
  bCheckForAllChannels = true;
  //bCheckForAllChannels = false;
  /*
  if (options) {
    bCheckForAllChannels = (options.fromTeachIn == true) ? true : false;
  }
  */

  updateContent("/pages/tabs/admin/views/newdevices.htm");
};

loadDevicesToChange = function(SNNewDev, TypeNewDev, IDNewDev)
{
  doClearTempView = true;
  updateContent("/config/ic_seldevice.cgi", "&SNNewDev="+SNNewDev+"&TypeNewDev="+TypeNewDev+"&IDNewDev="+IDNewDev);
};

loadRoomList = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/views/rooms.htm");
};

loadFunctionList = function() {
  doClearTempView = true;
  updateContent("/pages/tabs/admin/views/functions.htm");
};

loadSystemControl = function() {
  doClearTempView = true;
  updateContent("/config/control_panel.cgi");
};
  
loadNewLinkPage = function()
{
  updateContent("/config/ic_selchannel.cgi");
};

setPath = function (path) {
  var s = "<span onclick='WebUI.enter(StartPage);'>" +translateKey('startPage')+"</span>";
  if ((path.length > 0) && $("PagePath")) {
    if ($("PagePath"))$("PagePath").innerHTML = s + " &gt; " + path;
  }
  else {
    if ($("PagePath"))$("PagePath").innerHTML = "<span id='PagePathSpan'>"+translateKey('startPage')+"</span>";
  }
  translatePage("#PagePath");
};



logout = function() {
  regaMonitor.stop();
  InterfaceMonitor.stop();
  // The second url-param has to be appended by a '?' instead of a '&'
  location.href = "/logout.htm?sid=" + SessionId+"?lang="+getLang();
};


/* * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * *     Misc Functions        * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * */

/* Funktion für Popup */
CreatePopup = function(id, type)
{
  dlgPopup = new iseMessageBox(id, type, false);
  PopupClose = function()
  {
    dlgPopup.hide();
  };
  dlgPopup.ShowPopup();
};

buildPropTable= function(id) {
  var opts = {
    evalScripts: true,
    onComplete: function(trans) {
      window.setTimeout(centerMessageBox, 200);
    }
  };
  var url = "/esp/system.htm?sid=" + SessionId + "&action=buildPropTable&id="+id;
  new Ajax.Updater("propTable", url, opts);
};

TestMe = function(id) {
  iseChannel.Test(id);
};

initCenterBox = function() {
  var centerBoxSel = jQuery("#centerbox");
  centerBoxSel.css({"overflow":"", "top":"50%"});
};

centerMessageBox = function() {
  var  dimCenterBox = $("centerbox").getDimensions();

  initCenterBox();

  if ($("messagebox")) {
    var msgbox = $("messagebox"),
    dimMsgBox = msgbox.getDimensions();
    msgbox.style.marginLeft = -(dimMsgBox.width / 2) + 'px';
    msgbox.style.left = '50%';
    msgbox.style.marginTop = -(dimMsgBox.height / 2) + 'px';

    /*if(dimCenterBox.height == 0) {
      msgbox.style.marginTop = -(dimMsgBox.height / 2) + 'px';
    } else {
      msgbox.style.marginTop = (dimMsgBox.height / 2) + 'px';
    }*/
  }
};

saveDlgResult = function(id) {
  dlgResult = id; 
};

checkTimeVal = function(val) {
  var sSplit = new Array(2);

  if (val.length != 5) { return false; }
  sSplit = val.split(':');
  if (sSplit.length != 2) { return false; }
  if (isNaN(sSplit[0]))   { return false; }
  if (isNaN(sSplit[1]))   { return false; }

  var iTmp = parseInt(sSplit[0],10);
  if ((iTmp < 0) || (iTmp > 23)) { return false; }
  iTmp = parseInt(sSplit[1],10);
  if ( (iTmp < 0) || (iTmp > 59) ) { return false; }
  
  return true;
};

setFooter = function(s)
{
   $("footer").innerHTML = s;
   translatePage("#footer");
};

SetFilter = function(id)
{
  return;
/*  
  if ($(id)) {
    var inplist = $(id).getElementsByTagName('input');
    for (var i = 0; i < inplist.length; i++) {
      var tmpId = inplist[i].id.substr(6);
      if (inplist[i].checked === true) 
        filterOptions.addFilter(tmpId);
      else 
        filterOptions.removeFilter(tmpId);
      // TODO: Channel-Liste neu laden (mit Filtern)
    }
  }
*/
};

 // Prüft anhand der filterOptions ob Checkboxen in Submenüs gesetzt werden sollen
selectFilters = function(divToShow) {
 if ($(divToShow)) {
    var inplist = $(divToShow).getElementsByTagName('input');
    for (var i = 0; i < inplist.length; i++) {
      var tmpId = inplist[i].id.substr(6);
      if (filterOptions.isFilterId(tmpId)) 
        inplist[i].checked = true;
      else 
        inplist[i].checked = false;
    }
  }
};

writeDevProg = function(tdParent, chnObj, bClosePopup) {
  var tblSub = Builder.node('table', {width: '100%'});
  var tbodySub = Builder.node('tbody');
  var trSub = Builder.node('tr');
  var tdSub = Builder.node('td');
  var divSub = Builder.node('div', {className: 'StdButton'}, translateKey('btnDirectLinks'));

  divSub = $(divSub);
  var iTmp = 0;
  if( chnObj['gm'] )
    iTmp = 1;
  divSub.onclick = function() 
  { 
    WebUI.enter(LinkListPage, {
      "iface"  : chnObj['deviface'],
      "channel": chnObj['sn'],
      "keypair": iTmp
    });
    if (bClosePopup) { PopupClose(); }
  };
  tdSub.appendChild(divSub);
  trSub.appendChild(tdSub);
  tbodySub.appendChild(trSub);
  trSub = Builder.node('tr');
  tdSub = Builder.node('td');
  divSub = Builder.node('div', {className: 'StdButton'}, translateKey('btnPrograms'));

  divSub.onclick = function() { loadProgramList(chnObj['enumprgfilter']); if(bClosePopup)PopupClose();  };
  tdSub.appendChild(divSub);
  trSub.appendChild(tdSub);
  tbodySub.appendChild(trSub);
  tblSub.appendChild(tbodySub);
  tdParent.appendChild(tblSub);
};

writeTestField = function(td, ids, tm, exist, running)
{
  var cn = "TestButtonDisabled";
  var sDate = '--:--:--';
  var sStyle = "OkButton";
  if( running )
  {
    sStyle = "OkButtonRunning";
  }
  else
  {
    sDate = tm;
    if( exist ) { cn = "TestButton colorGradient borderRadius2px"; }
  }

  var chid = ids.toString().split("\t").join("T");
  var div = Builder.node('div', {id:'TestButtonCH'+chid,className: cn}, translateKey('btnTest') /*'Test'*/);
  td.appendChild(div);

  if( exist && !running )
  {
    div.onclick = function()
    {
      iseChannels.Test(ids);
      $('TestButtonCH'+chid).className = 'TestButtonDisabled';
      $('OkButtonCH'+chid).className = 'OkButtonRunning';
      $('TestDateCH'+chid).innerHTML = '--:--:--';
    };
  }
  div = Builder.node('div', {id:'OkButtonCH'+chid,className: sStyle}, translateKey('btnOk') /*'OK'*/);
  td.appendChild(div);
  div = Builder.node('div', {id:'TestDateCH'+chid,className: 'TestDate'}, sDate);
  td.appendChild(div);
};

startUpdateTest = function(chid)
{
  setTimeout( "callbackStartUpdateTest("+chid+")", 5000 );
};

callbackStartUpdateTest = function(chid)
{
  var url = "/esp/system.htm?sid=" + SessionId;
  var pb = "string action = 'TestRunning';";
  pb += 'string id = "' + chid + '";';
  var opts = 
  {
    postBody: ReGa.encode(pb),
    onComplete: function(t) 
    {
      $("TestDateCH"+chid).innerHTML = t.responseText;
      if( t.responseText == "--:--:--" )
      {
        $("OkButtonCH"+chid).className = "OkButton";
      }
      else
      {
        $("OkButtonCH"+chid).className = "OkButtonRunning";
      }
    }
  };  
  new Ajax.Request(url, opts);
};

showInvisibleElems = function(devID) {
  var jBtnAddToGroupElm = jQuery("#btnAddToGroup" + devID),
    jBtnCreateNewGroupElm = jQuery("#btnCreateNewGroup" + devID),
    jActionElms = jQuery("[name='newDevAction" + devID + "']"),
    jBtnAutoRename = jQuery("#btnRename_" + devID);
  jActionElms.show();
  jBtnAddToGroupElm.show();
  jBtnCreateNewGroupElm.show();
  jBtnAutoRename.show();
};

showDeviceError = function(devID) {
  jQuery("#readyBtn" + devID).text(translateKey("stringTableError"));
};

allChannelsAvailable = function(devID) {
  var numberOfLoops = 60;
  conInfo("timeoutCounter " + devID + ": " + timeoutCounter[devID] );
  if (timeoutCounter[devID] < numberOfLoops) {
    timeoutCounter[devID]++;

    homematic("Device.get", {"id": devID}, function (device) {
      devID = devID.toString();
      var numberOfChannels = device.channels.length;
      conInfo("numberOfChannels: " + numberOfChannels);

      // Noch kein Kanal vorhanden
      if (numberOfChannels == 0) {
        window.setTimeout(function () {
          conInfo("No channels available yet. Check again");
          allChannelsAvailable(devID);
        }, 1000);
      } else {
        if (numberOfChannels != tmpNoOfChannels[devID]) {
          window.setTimeout(function () {
            conInfo("tmpNoOfChannels: " + tmpNoOfChannels[devID] + "- numberOfChannels: " + numberOfChannels + " Check again number of channels");
            tmpNoOfChannels[devID] = numberOfChannels;
            allChannelsAvailable(devID);
          }, 1000);
        } else {
          conInfo("All channels available - number of channels: " + numberOfChannels);
          var jReadyBtnElm = jQuery("#readyBtn" + devID),
            jWaitAnimationElm = jQuery("#waitForReadyBtn"+devID);
          jWaitAnimationElm.hide();
          jReadyBtnElm.addClass("StdButton CLASS04308 border1px");
          jReadyBtnElm.text(translateKey('btnReady'));
          jReadyBtnElm.bind("click", function () {
            arrayDeleteVal(deviceInputCheckedDevices, devID.toString());
            SetReadyConfigAndHideImgs(devID);
           });
          showInvisibleElems(devID);
          deviceInputCheckedDevices.push(devID);
        }
      }
    });
  } else {
    // Emergency - timeout reached :-(
    jQuery("#newDevActionDEL"+devID).show();
    showDeviceError(devID);
    if (sessionStorage) {
      sessionStorage.setItem("teachInFailure_" + devID, "true");
    }
    return;
  }
};

deviceAlreadyChecked = function(devID) {
  var result = false;
  jQuery.each(deviceInputCheckedDevices, function(index,deviceID){
    if (deviceID == devID) {
      result = true;
    }
  });
  return result;
};

writeReadyField = function(tdParent, obj) {
  var devAlreadyChecked = deviceAlreadyChecked(obj.id),
    devFailure = false;

  if (bCheckForAllChannels && ! devAlreadyChecked) {
    timeoutCounter[obj.id] = 0;
    tmpNoOfChannels[obj.id.toString()] = -1;
    allChannelsAvailable(obj.id);
  } else {
    if (sessionStorage) {
      if (sessionStorage.getItem("teachInFailure_" + obj.id) == "true") {
        devFailure = true;
      }
    }
  }

  var sn = obj['sn'];
  var objId = obj['id'];
  var tbl = Builder.node('table', {width: '100%', cellpadding: '0', id:obj['sn'] });
  var tr = Builder.node('tr');
  var td = Builder.node('td');
  var div;

  if ((bCheckForAllChannels && (! devAlreadyChecked)) || (devFailure)) {
    div = Builder.node('div', {
       id: 'readyBtn' + objId
    }, '' /*show waiting animation*/ );

    img = Builder.node('img', {
      id: 'waitForReadyBtn' + objId,
      src: '/ise/img/ajaxload_white.gif'
    }, '');

    div.appendChild(img);

  } else {
    div = Builder.node('div', {
      className: 'StdButton CLASS04308 border1px',
      id: 'readyBtn' + objId,
      onclick: "SetReadyConfigAndHideImgs(" + objId + ")"
    }, translateKey('btnReady') /*"Fertig"*/);
    window.setTimeout(function() {jQuery("#btnRename_" + objId).show();}, 100); // Show the auto rename btn
  }

  if( ! obj['readyconfig'] )
  {
    div._isReady = false;
  }
  else
  {
    div._isReady = true;
  }
  td.appendChild(div);
  tr.appendChild(td);
  tbl.appendChild(tr);
  tdParent.appendChild(tbl);
};

removeErrorFlag = function(devID) {
  if (sessionStorage && sessionStorage.getItem("teachInFailure_" + devID) != null) {
    sessionStorage.removeItem("teachInFailure_" + devID);
  }
};

writeDeviceAction = function(tdParent, includeChecks, bIsDev, bDelBtn, obj, bIsGroup) {
  var HmIPIfaceSelector = "HmIP-RF",
    devFailure = false;

  if (sessionStorage) {
    if (sessionStorage.getItem("teachInFailure_" + obj.id) == "true") {
      devFailure = true;
    }
  }

  var id = 0;
  var hidden = ((! bCheckForAllChannels || deviceAlreadyChecked(obj.id)) && (devFailure == false)) ? "" : "hidden";

  if (obj) { id = obj['id']; }
  var tbl = Builder.node('table', {
    width: '100%',
    cellpadding: '0'
  }, [
   Builder.node('colgroup', [
     Builder.node('col', {width: '40%'}),
     Builder.node('col', {width: '60%'})
   ])
  ]);
  var tbody = Builder.node('tbody');
  var tr = Builder.node('tr');
  var tdLeft = Builder.node('td');
  var tdRight = Builder.node('td');

  // buttons Löschen / Einstellen
  var tblSub = Builder.node('table', {width: '100%'});
  var tbodySub = Builder.node('tbody');
  var trSub;

  if (!devFailure) {
    trSub = Builder.node('tr', {id: "newDevActionDEL" + id, name: 'newDevAction' + id, className: hidden});
  } else {
    window.setTimeout(function() {showDeviceError(id);},500);
    trSub = Builder.node('tr', {id: "newDevActionDEL" + id, name: 'newDevAction' + id});
  }
  var tdSub = Builder.node('td');
  var s;

  if (bIsDev && bDelBtn) {
   s = "PrepareDeleteDeviceForm('"+ obj['iface']+"', '"+obj['sn']+"', '"+obj['name']+"', '"+obj['type']+"'), arrayDeleteVal(deviceInputCheckedDevices, "+obj['id']+"), removeErrorFlag("+obj['id']+");";

   var divSub = Builder.node('div', {
     className: 'StdButton',
     onclick: s}, translateKey('btnRemove'));
   tdSub.appendChild(divSub);
   trSub.appendChild(tdSub);
   tbodySub.appendChild(trSub);
  }
  trSub = Builder.node('tr', {name: 'newDevAction' + id, className: hidden});
  tdSub = Builder.node('td');
  
  s = "";
  if (
    (obj['type'] != "HmIPW-DRBL4")
    && (obj['type'] != "HmIP-DRBLI4")
    && (obj['type'] != "HmIP-RGBW")
    && (obj['type'] != "HmIP-WGD")
    && (obj['type'] != "HmIP-WGD-PL")
    && (obj['type'] != "HmIPW-WGD")
    && (obj['type'] != "HmIPW-WGD-PL")
    && (obj['type'] != "HmIP-ESI")
    && (obj['type'] != "HmIP-DRG-DALI")
    && (obj['type'] != "HmIP-FALMOT-C8")
    && (obj['type'] != "HmIP-FALMOT-C12")
  ) {

    if (bIsDev) {
      s = "WebUI.enter(DeviceConfigPage, {'iface': '" + obj['iface'] + "', 'address': '" + obj['sn'] + "', 'redirect_url':'GO_BACK'});";
    } else {
      if (bIsGroup) {
        s = "WebUI.enter(DeviceConfigPage, {'iface': '" + obj['deviface'] + "' ,'address': '" + obj['sn'] + "', 'redirect_url':'GO_BACK', 'with_group': 1});";
      } else {
        s = "WebUI.enter(DeviceConfigPage, {'iface': '" + obj['deviface'] + "' ,'address': '" + obj['sn'] + "', 'redirect_url':'GO_BACK'});";
      }
    }
  } else {
    s = "alert(translateKey('hintSetReady'));";
  }
  divSub = Builder.node('div', {className: 'StdButton', onclick: s + "devToConfigure=" + obj['id']}, translateKey('btnConfigure') /*'Einstellen'*/);

  tdSub.appendChild(divSub);
  trSub.appendChild(tdSub);
  tbodySub.appendChild(trSub);

  /* Geraetetausch */
  if (bIsDev && bDelBtn && (obj['iface'] != HmIPIfaceSelector)) {
    //divSub = Builder.node('div', {className: 'StdButton', onclick: 'loadDevicesToChange(\''+obj['sn']+'\',\''+obj['type']+'\')'}, translateKey('replaceDeviceBtn'));
    divSub = Builder.node('div', {className: 'StdButton', onclick: 'loadDevicesToChange(\''+obj['sn']+'\',\''+obj['type']+'\',\''+obj['id']+'\')'}, translateKey('replaceDeviceBtn'));
    tdSub.appendChild(divSub);
    trSub.appendChild(tdSub);
    tbodySub.appendChild(trSub);
  }
  /* END Geraetetausch */
  
  tblSub.appendChild(tbodySub);
  tdLeft.appendChild(tblSub);

  // checkboxes
  if (includeChecks)
  {
    tblSub = Builder.node('table', {name: 'newDevAction' + id, width: '100%', className: hidden});
    tbodySub = Builder.node('tbody');
    trSub = Builder.node('tr');
    var h = {type: 'checkbox', id: 'ha' + id};
    var sH = "";

    
    if( bIsDev )
    {
      sH = 'iseDevices.setHandling('+id+',id)';
    }
    else
    {
      sH = 'iseChannels.setHandling('+id+',id)';
    }
    switch( obj['handle'] )
    {
      case "YES":
        h = Object.extend(h, {checked: ''});
        break;
      case "READONLY":
        sH = "";
        h = Object.extend(h, {disabled: ''});
        h = Object.extend(h, {readonly: ''});
        break;
      default: break;
    }
    h = Object.extend(h, {onclick: sH});
    tdSub = Builder.node('td', [Builder.node('input', h), translateKey('lblUsable') /*'bedienbar'*/]);
    Element.setStyle(tdSub, {textAlign:"left"});
    trSub.appendChild(tdSub);
    tbodySub.appendChild(trSub);
    trSub = Builder.node('tr');
    var v = {type: 'checkbox', id: 'vi' + id};
    if( obj['vis'] )
    {
      v = Object.extend(v, {checked: ''});
      conInfo("Object "+obj['id']+":"+obj['name']+" is visble");
    }
    else
    {
      conInfo("Object "+obj['id']+":"+obj['name']+" is NOT visble");
    }
    if( bIsDev )
    {
      v = Object.extend(v, {onclick: 'iseDevices.setVisible('+id+',id)'});
    }
    else
    {
      v = Object.extend(v, {onclick: 'iseChannels.setVisible('+id+',id)'});
    }
    tdSub = Builder.node('td', [Builder.node('input', v), translateKey('lblVisible') /*'sichtbar'*/]);
    Element.setStyle(tdSub, {textAlign:"left"});

    trSub.appendChild(tdSub);
    tbodySub.appendChild(trSub);
    trSub = Builder.node('tr');
    var p = {type: 'checkbox', id: 'pr' + id};
    if (obj['proto'])
     p = Object.extend(p, {checked: ''});
    if (bIsDev)
     p = Object.extend(p, {onclick: 'iseDevices.setProto('+id+',id)'});
    else
     p = Object.extend(p , {onclick: 'iseChannels.setProto('+id+',id)'});
    tdSub = Builder.node('td', [Builder.node('nobr', [Builder.node('input', p), translateKey('lblRecorded') /*'protokolliert'*/])]);
    Element.setStyle(tdSub, {textAlign:"left"});
    trSub.appendChild(tdSub);
    tbodySub.appendChild(trSub);
    tblSub.appendChild(tbodySub);
    tdRight.appendChild(tblSub);
  }

  tr.appendChild(tdLeft);
  tr.appendChild(tdRight);
  tbody.appendChild(tr);
  tbl.appendChild(tbody);
  tdParent.appendChild(tbl);
};

spaceCut = function(s) {
  if (s)
  {
    var arTmp = s.split(' ');
    var z = new Array();
    
    for(var i = 0; i < arTmp.length; i++)
    {
      if (arTmp[i] !== "")
      {
        z[z.length] = document.createTextNode(arTmp[i]);
        z[z.length] = document.createElement('br');
      }
    }
    z.pop();
    return z;
  }
  return "";
};

spaceCutHtmlIf = function(s, iCount) {
  if (s)
  {
    var sRet = "";
    var arTmp = s.split(' ');
    var iTmp = 0;
    
    if (arTmp.length > iCount)
    {
      for (var i = 0; i < arTmp.length; i++)
      {
        sRet += arTmp[i] + " ";
        iTmp++;
        if (iTmp == iCount)
        {
          sRet += "<br />";
          iTmp = 0;
        }
      }
      return sRet;
    }
    else { return s; }
  }
  else { return ""; }
};

semCut = function(s) {
  if (s)
  {
    var arTmp = s.split(';');
    var z = new Array();
    for (var i = 0; i < arTmp.length; i++)
    {
      if (arTmp[i] !== "")
      {
        z[z.length] = document.createTextNode(arTmp[i]);
        z[z.length] = document.createElement('br');
      }
    }
    z.pop();
    return z;
  }
  return "";
};

strCut = function(s, bCountOnly, bPlainText) {
  var CUTS = ".:-";
  var MAX_WITHOUT_CUT = 8;
  var bCutChar = false;
  var iSinceLastCut = 0;
  var ret = "";
  var arSplit = new Array();
  var iLastCutPos = 0;
  var i;
  
  if(s)
  {
    for (i = 0; i < s.length; i++) {
      var c = s.charAt(i);
      
      if (!bCountOnly) {
        for (var x = 0; x < CUTS.length; x++) {
          bCutChar = false;
          if (c == CUTS.charAt(x)) {
            arSplit[arSplit.length] = s.slice(iLastCutPos, i+1);
            iLastCutPos = i+1;
            iSinceLastCut = 0;
          }
        }
      }
    
      iSinceLastCut++;
      if (iSinceLastCut == MAX_WITHOUT_CUT) {
        arSplit[arSplit.length] = s.slice(iLastCutPos, i);
        iLastCutPos = i;
        iSinceLastCut = 0;
      }
    }
    if (iLastCutPos !== 0) { arSplit[arSplit.length] = s.slice(iLastCutPos, i); }
    else                   { arSplit[arSplit.length] = s; }
    
    if (bPlainText) {
      var _s = "";
      for (i = 0; i < arSplit.length; i++) {
        _s += arSplit[i] + "<br />";
      } 
      return _s;
    }
    else {
      var z = new Array();
      for (i = 0; i < arSplit.length; i++) {
        z[z.length] = document.createTextNode(arSplit[i]);
        z[z.length] = document.createElement('br');
      }
      return z;
    }
  }
};

tabToBr = function(s) {
  if (s) {
    var arTmp = s.split('\t');
    var z = new Array();
    for(var i = 0; i < arTmp.length; i++)
    {
      if (arTmp[i] !== "")
      {
        z[z.length] = document.createTextNode(arTmp[i]);
        z[z.length] = document.createElement('br');
      }
    }
    z.pop();
    return z;
  }
  return "";
};

insertBR = function(s) {
  var z = new Array();
  var arSplit = s.split('\t');
  for (var i = 0; i < arSplit.length; i++)
  {
    if (arSplit[i] !== "") 
    {
      z[z.length] = document.createTextNode(arSplit[i]);
      z[z.length] = document.createElement('br');
    }
  }
  if (z.length > 1)
    z.pop();
  return z;
};

show = function(id) {
  if ($(id))
    $(id).show();
};

hide = function(id) {
  if($(id))
    $(id).hide();
};

setProtoIfExist = function(id, bProto) {
  if ($("pr" + id)) {
    $("pr" + id).checked = bProto;
  }  
};

setHandlingIfExist = function(id, bHand) {
  if ($("ha" + id)) {
    $("ha" + id).checked = bHand;
  }  
};

setVisibleIfExist = function(id, bVisi) {
  if ($("vi" + id)) {
    $("vi" + id).checked = bVisi;
  }  
};

/*
textEdit = function(ctrlId, id, allowEmptyString, callback, callStrFunc) 
{
  bTxtEditMode = true;
  var iSize = 8;
  if (screen.availWidth >= 1024) iSize = 12;
  if (!$(ctrlId+"inp"))  
  {
    sPrevVal = $(ctrlId).innerHTML;
    var isEmptyAllowed = false;
    if ((allowEmptyString !== null) && (allowEmptyString === true))
    {
      isEmptyAllowed = true;
    }
    var sValue = $(ctrlId).innerHTML;
    while (sValue.indexOf("<br>") > -1){
      sValue = sValue.replace("<br>","");
    }
    while (sValue.indexOf("<BR>") > -1){
      sValue = sValue.replace("<BR>","");
    }
    sValue = sValue.replace(/^\s+/,""); // Left trim
    sValue = sValue.replace(/$\s+/,""); // Left trim         
    sValue = sValue.replace(/<br>/gi, "");
      var s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' maxlength='100' onblur='saveEdit(id, "+id+","+isEmptyAllowed+"," + callback +"," + callStrFunc+")' onkeypress='checkkey(id, "+id+","+isEmptyAllowed+"," + callback + "," + callStrFunc + ", event)'/>";
    //var s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' maxlength='100' onblur=\"saveEdit(id, '"+id+"',"+isEmptyAllowed+"," + callback +"," + callStrFunc+")\" onkeypress=\"checkkey(id, '"+id+"',"+isEmptyAllowed+"," + callback + "," + callStrFunc + ", event)\"/>";
    s = s+"<input type=hidden id='"+ctrlId+"old' value='"+sValue+"'>";
    // var s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' onkeypress='checkkey(id, "+id+","+isEmptyAllowed+"," + callback +", event)'/>";
    $(ctrlId).innerHTML = s;
    $(ctrlId+'inp').focus();
    $(ctrlId+'inp').select();
  }
};
*/

textEdit = function(ctrlId, id, allowEmptyString, callback, callStrFunc) 
{
  bTxtEditMode = true;
  var iSize = 8;
  if (screen.availWidth >= 1024) iSize = 12;
  if (!$(ctrlId+"inp"))  
  {
    sPrevVal = $(ctrlId).innerHTML;
    var isEmptyAllowed = false;
    var s;
    if ((allowEmptyString !== null) && (allowEmptyString === true))
    {
      isEmptyAllowed = true;
    }
    var sValue = $(ctrlId).innerHTML;
    while (sValue.indexOf("<br>") > -1){
      sValue = sValue.replace("<br>","");
    }
    while (sValue.indexOf("<BR>") > -1){
      sValue = sValue.replace("<BR>","");
    }
    sValue = sValue.replace(/^\s+/,""); // Left trim
    sValue = sValue.replace(/$\s+/,""); // Left trim         
    sValue = sValue.replace(/<br>/gi, "");
   // var s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' maxlength='50' onblur='saveEdit(id, "+id+","+isEmptyAllowed+"," + callback +"," + callStrFunc+")' onkeypress='checkkey(id, "+id+","+isEmptyAllowed+"," + callback + "," + callStrFunc + ", event)'/>";
    if (PLATFORM == "Central") {
      s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' maxlength='100' onblur='saveEdit(id, "+id+","+isEmptyAllowed+"," + callback +"," + callStrFunc+")' onkeypress='checkkey(id, "+id+","+isEmptyAllowed+"," + callback + "," + callStrFunc + ", event)'/>";
      s = s+"<input type=hidden id='"+ctrlId+"old' value='"+sValue+"'>";
    } else {
      s = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' maxlength='100' onblur=\"saveEdit(id, '"+id+"',"+isEmptyAllowed+"," + callback +"," + callStrFunc+")\" onkeypress=\"checkkey(id, '"+id+"',"+isEmptyAllowed+"," + callback + "," + callStrFunc + ", event)\"/>";
      s = s+"<input type=hidden id='"+ctrlId+"old' value='"+sValue+"'>";
    }
    $(ctrlId).innerHTML = s;
    $(ctrlId+'inp').focus();
    $(ctrlId+'inp').select();
  }
};

checkkey = function(ctrlId, id, allowEmptyString, callback, callStrFunc, e) {
  var keycode = 0;
  if (window.event) keycode = window.event.keyCode;
  else if(e) keycode = e.which;
  if (keycode == 13)
    saveEdit(ctrlId, id, allowEmptyString, callback, callStrFunc);
};

checkKeyInfo = function(ctrlId, id, allowEmptyString, e) {
  var keycode = 0;
  if (window.event) keycode = window.event.keyCode;
  else if(e) keycode = e.which;
  if (keycode == 13)
    saveEditInfo(ctrlId, id, allowEmptyString);
};

textEditInfo = function(ctrlId, id, allowEmptyString) 
{
  bTxtEditMode = true;
  var iSize = 8;
  if (screen.availWidth >= 1024) iSize = 12;
  if (!$(ctrlId+"inp"))  
  {
    var isEmptyAllowed = false;
    if ((allowEmptyString !== null) && (allowEmptyString === true))
    {
      isEmptyAllowed = true;
    }
    sPrevValInfo = $(ctrlId).innerHTML;
    var sValue = $(ctrlId).innerHTML;
    sValue = sValue.replace(/^\s+/,""); // Left trim    
    sValue = sValue.replace(/$\s+/,""); // Left trim     
    sValue = sValue.replace(/<br>/gi,"");
    sValue = sValue.replace(/&nbsp;/gi,"");
    $(ctrlId).innerHTML = "<input id='"+ctrlId+"inp' value='"+sValue+"' size='"+iSize+"' onblur='saveEditInfo(id, "+id+","+isEmptyAllowed+")' onkeypress='checkKeyInfo(id, "+id+","+isEmptyAllowed+", event)' />";
    $(ctrlId+'inp').focus();
    $(ctrlId+'inp').select();
  }
};

isPasswordAllowed = function(text,minLen,suppressAlert)
{
  var re = new RegExp( '^[a-zA-Z0-9.=!$():;#*ßüäö-]{'+minLen+',}$', 'i' );
  var bRet = re.test( text );
  var bShowAlert = (typeof(suppressAlert)=="undefined");
  //if( !bRet && ( bShowAlert ) ) alert( "Bitte verwenden Sie nur die erlaubten Sonderzeichen [., !, $, (, ), :, ;, #, ß, ä, ö, ü, -]." );
  if( !bRet && ( bShowAlert ) ) alert( translateKey("alertUseOnlySpecialChars") );
  return bRet;
};

/**
 * Prüft, ob ein Text verbotene Sonderzeichen verwendet.
 * Zu diesen Zeichen gehören: <, >, ', ", &, $, [, ], {, } und \
 * --> Es ist alles erlaubt, was nicht verboten ist
 **/
isTextAllowed = function(text, minLen, suppressAlert)
{
  var forbidden   = /[<>'"&$\[\]\{\}\\]/;
  var isForbidden = forbidden.test( text );
  
  if ((isForbidden) && (typeof(suppressAlert) == "undefined"))
  {
    //alert("Bezeichnungen dürfen keines der folgenden Zeichen enthalten: <, >, ', \", &, $, [, ], {, } und \\");
    alert(translateKey("alertCharsNotAllowed"));
  }
  
  return !(isForbidden);
};



isNumber = function(str) {
  var reg = new RegExp('^[0-9]+$');
  return reg.test(str);
};

if (PLATFORM == "Central")
{  
  saveEdit = function(ctrlId, id, allowEmptyString, callback, callStrFunc)
  {
    // verhindern dass das OnBlur-Event ausgelöst wird wenn mit ENTER bestätigt wurde
    if (!bTxtEditMode) return;
    bTxtEditMode = false;
    var divId = ctrlId.substr(0, ctrlId.length - 3);
    var newVal = $(ctrlId).value;
    var oldV = "";
    if($(divId+"old")){
      oldV = $(divId+"old").value;
      oldV = oldV.replace(/\n/,""); //  trim        
      if (NAV_IE) {
        oldV = oldV.replace(/^\s+|$\s+/,""); // trim
      } else {
        oldV = oldV.replace(/^\s+|\s+$/,""); // trim      
      }
    }
    var minLen = 1;
    if ((allowEmptyString !== null) && (allowEmptyString === true))
    {
      minLen = 0;
    }
    if ( isTextAllowed(newVal,minLen) )
    {
      if( id !== 0 )
      {
        if (typeof callback == "function")
        {
          callback(id, newVal);
        }
        iseSystem.saveName(id, newVal, divId, callStrFunc);
        $(divId).innerHTML = newVal;
      }
      else
      {
        if (newVal != oldV){
          iseSystem.checkName( newVal, divId );        
        } else {
          $(divId).innerHTML = sPrevVal;
        }
      }
    }
    else
    {
      $(divId).innerHTML = sPrevVal;
    }
  };
  
  saveEditInfo = function(ctrlId, id, allowEmptyString) 
  {
    // verhindern dass das OnBlur-Event ausgelöst wird wenn mit ENTER bestätigt wurde
    if (!bTxtEditMode) return;
    bTxtEditMode = false;
    var divId = ctrlId.substr(0, ctrlId.length - 3);
    var newVal = $(ctrlId).value;
    var minLen = 1;
    if ((allowEmptyString !== null) && (allowEmptyString === true))
    {
      minLen = 0;
    }
    if ( isTextAllowed(newVal,minLen) )
    {
      $(divId).innerHTML = newVal;
      if( id !== 0 ) { iseSystem.saveDesc(id, newVal); }
    }
    else 
    {
      $(divId).innerHTML = sPrevValInfo;
    }
  };
} 
else 
{
  //Funktionen für das Konfigtool
  saveEdit = function(ctrlId, id, allowEmptyString, callback, callStrFunc)
  {
    // verhindern dass das OnBlur-Event ausgelöst wird wenn mit ENTER bestätigt wurde
    if (!bTxtEditMode) return;
    bTxtEditMode = false;
    var divId = ctrlId.substr(0, ctrlId.length - 3);
    var newVal = $(ctrlId).value;
    var oldV = "";
    if($(divId+"old")){
      oldV = $(divId+"old").value;
      oldV = oldV.replace(/\n/,""); //  trim        
      if (NAV_IE) {
        oldV = oldV.replace(/^\s+|$\s+/,""); // trim
      } else {
        oldV = oldV.replace(/^\s+|\s+$/,""); // trim      
      }
    }
    var minLen = 1;
    if(allowEmptyString != null && allowEmptyString == true)
    {
      minLen = 0;
    }
    if ( isTextAllowed(newVal,minLen) )
    {
      if( id != 0 )
      {
        if (typeof callback == "function")
        {
          callback(id, newVal);
        }else{
          configMetadata.save(id, newVal, divId, callStrFunc);
        }
        $(divId).innerHTML = newVal;
      }
      else
      {
        if (newVal != oldV){
          //iseSystem.checkName( newVal, divId );        
        } else {
          $(divId).innerHTML = sPrevVal;
        }
      }
    }
    else
    {
      $(divId).innerHTML = sPrevVal;
    }
  };
  saveEditInfo = function(ctrlId, id, allowEmptyString) 
  {
    // verhindern dass das OnBlur-Event ausgelöst wird wenn mit ENTER bestätigt wurde
    if (!bTxtEditMode) return;
    bTxtEditMode = false;
    var divId = ctrlId.substr(0, ctrlId.length - 3);
    var newVal = $(ctrlId).value;
    var minLen = 1;
    if(allowEmptyString != null && allowEmptyString == true)
    {
      minLen = 0;
    }
    if ( isTextAllowed(newVal,minLen) )
    {
      $(divId).innerHTML = newVal;
      if( id != 0 ) iseSystem.saveDesc(id, newVal);
    }
    else 
    {
      $(divId).innerHTML = sPrevValInfo;
    }
  };
}


/* * * * * * *  Overlay  * * * * * * * * * * * * * * * * * * * */
showRoomOverlay = function(tdId, chnId) {
  var chnType = homematic("Channel.getChannelType", {"id": chnId});
  iLastChnId = chnId;

  // A channel of type CCESSPOINT_GENERIC_RECEIVER (e. g. DRAP chn 1 and 2) is not eligible for assigning to a room
  if (chnType != "ACCESSPOINT_GENERIC_RECEIVER") {
    translatePage("#roomOverlay");
    var elmPos = getElemCenterPos("#roomOverlay");
    $("roomOverlay").style.top = elmPos.top; //tdPos[1]+"px";
    $("roomOverlay").style.left = elmPos.left; //tdPos[0]+"px";
    iseChannels.showOverlay(chnId, ID_ROOMS);
  } else {
    alert(translateKey("lblChnNotAllowedInRoom"));
  }
};

showFuncOverlay = function(tdId, chnId) {
  var chnType = homematic("Channel.getChannelType", {"id": chnId});
  iLastChnId = chnId;

  // A channel of type ACCESSPOINT_GENERIC_RECEIVER (e. g. DRAP chn 1 and 2) is not eligible for assigning to a function
  if (chnType != "ACCESSPOINT_GENERIC_RECEIVER") {
    translatePage("#funcOverlay");
    var elmPos = getElemCenterPos("#funcOverlay");
    $("funcOverlay").style.top = elmPos.top; //tdPos[1]+"px";
    $("funcOverlay").style.left = elmPos.left; //tdPos[0]+"px";
    iseChannels.showOverlay(chnId, ID_FUNCTIONS);
  } else {
    alert(translateKey("lblChnNotAllowedInFunc"));
  }
};

addRoom = function(ctrlId, roomId) {
  iseChannels.chnToRoom(iLastChnId, roomId, $(ctrlId).checked);
};

addFunc = function(ctrlId, funcId) {
  iseChannels.chnToFunc(iLastChnId, funcId, $(ctrlId).checked);
};

convertDomDate = function(s)
{
  var arTmp = s.split(' ');
  arTmp = arTmp[0].split('-');
  return arTmp[2]+'.'+arTmp[1]+'.'+arTmp[0];
};

convertDomTime = function(s) {
  if (s.length === 0) { return ""; }
  var arTmp = s.split(' ');
  arTmp = arTmp[1].split(':');
  return arTmp[0]+':'+arTmp[1];
};

ExecuteProgram = function(dpid)
{
  var url = "/esp/exec.htm?sid=" + SessionId;
  var pb = "";
  pb += "object o = dom.GetObject( "+dpid+" );";
  pb += "if( o )";
  pb += "{";
  pb += "  o.ProgramExecute();";
  pb += "}";
  var opts = {postBody: ReGa.encode(pb)};
  if(dbg)alert(pb);
  new Ajax.Request(url, opts);
};

setDpState = function(dpid, iState, boolVal)
{
  conInfo("setDPState - dpID: " + dpid + " - value: " + iState);
  var url = "/esp/system.htm?sid="+SessionId;
  var pb = "string action = 'setDpState';";
  pb += "integer dpid = "+dpid+";";
  if (boolVal)
  {
    pb += "boolean iState = "+(iState == 1? true : false)+";";
  }
  else
  {
    pb += "integer iState = '"+iState+"';";
  }
  var opts =
  {
    postBody: ReGa.encode(pb)
  };
  if(dbg)alert(pb);
  new Ajax.Request(url, opts);
};

removeDuplicates = function(s1, s2, splitChar, bPlainText) {
  var arTmp = new Array();
  var iAr;
  var bFound;
  var sSC = ' ';
  if (splitChar) { sSC = splitChar; }
  var ar1 = s1.split(sSC);
  var ar2 = s2.split(sSC);
  
  for (var i1 = 0; i1 < ar1.length; i1++) {
    bFound = false;
    for (iAr = 0; iAr < arTmp.length; iAr++) {
      if (ar1[i1] == arTmp[iAr]) {
        bFound = true;
        break;
      }
    }
    if (!bFound) {
      arTmp[arTmp.length] = ar1[i1];
    }
  }
  for (var i2 = 0; i2 < ar2.length; i2++) {
    bFound = false;
    for (iAr = 0; iAr < arTmp.length; iAr++) {
      if (ar2[i2] == arTmp[iAr]) {
        bFound = true;
        break;
      }
    }
    if (!bFound) {
      arTmp[arTmp.length] = ar2[i2];
    }
  }
  
  var i;
  if (bPlainText)
  {
    var sRet = "";
    for(i = 0; i < arTmp.length; i++) {
      if (arTmp[i] !== "") {
        sRet += arTmp[i] + "<br>";
      }
    }
    sRet = sRet.substr(0, sRet.length - 4);
    return sRet;
  }
  else {
    var z = new Array();
    for(i = 0; i < arTmp.length; i++) {
      if (arTmp[i] !== "") {
        z[z.length] = document.createTextNode(arTmp[i]);
        z[z.length] = document.createElement('br');
      }
    }
    z.pop();
    return z;
  }
};

lastSort = "";
lastDir = 0;

lastSort2 = "";
lastDir2 = 0;

iseSetLastSort = function(sort)
{
  lastSort = sort;
};

iseClearLastSort = function()
{
  lastSort = "";
  iseResetDirection();
  iseClearLastSort2();
};

iseClearLastSort2 = function()
{
  lastSort2 = "";
  iseResetDirection2();
};

iseChangeDirection = function()
{
  if( lastDir == 1 )
  {
    lastDir = 0;
  }
  else
  {
    lastDir = 1;
  }
};

iseChangeDirection2 = function()
{
  if( lastDir2 == 1 )
  {
    lastDir2 = 0;
  }
  else
  {
    lastDir2 = 1;
  }
};

iseResetDirection = function()
{
  lastDir = 0;
};

iseResetDirection2 = function()
{
  lastDir2 = 0;
};

iseArraySwap = function(arr,entryA,entryB)
{
  var tmp = arr[entryA];
  arr[entryA] = arr[entryB];
  arr[entryB] = tmp;
  return arr;
};

iseSortMultiArray = function(arr,dsc,dir,sln, sessionVar,popup)
{
  conInfo("Sort multi array.");
  do
  {
    var n = arr.length - 1;
    var bSwapped = false;
    for(var i=0;i<n;i++)
    {
      if( typeof arr[i][dsc] == "undefined" )
      {
        arr[i][dsc] = "";
      }
      if( typeof arr[i+1][dsc] == "undefined" )
      {
        arr[i+1][dsc] = "";
      }      
      var item = arr[i][dsc].toLowerCase();
      item = item.replace("ö", "o");
      item = item.replace("ä", "a");
      item = item.replace("ü", "u");
      item = item.replace("ß", "ss");
      var nextitem = arr[i+1][dsc].toLowerCase();
      nextitem = nextitem.replace("ö", "o");
      nextitem = nextitem.replace("ä", "a");
      nextitem = nextitem.replace("ü", "u");
      nextitem = nextitem.replace("ß", "ss");
      if( dir === 0 )
      {
        if( item > nextitem )
        {
          iseArraySwap(arr,i,i+1);
          bSwapped = true;
        }
      }
      else
      {
        if( item < nextitem )
        {
          iseArraySwap(arr,i,i+1);
          bSwapped = true;
        }
      }
    }
  } while( bSwapped );
    
  if (!popup) iseTransferSortedArray(arr,dsc,sln, sessionVar);
  
};

iseTransferSortedArray = function(arr,dsc,sortListNumber, sessionVar)
{
  conInfo("Transferring sorted array.");
  var url = "/esp/system.htm?sid="+SessionId;
  var pb = "";
  pb += 'string action = "AddToTempView'+sortListNumber+'";';
  
  var s = "sessionLS";
  if (typeof(sessionVar) != 'undefined') s = sessionVar;
  pb += 'system.SetSessionVar("'+s+'", "'+dsc+'");';
  pb += 'string ids = "';
  for(var i=0;i<arr.length;i++)
  {
    pb += arr[i]["id"];
    if( i != (arr.length-1) )
    {
      pb += "\t";
    }
  }
  pb += '";';
  if(dbg)alert(pb);
  var opts = 
  {
    postBody: ReGa.encode(pb),
    onComplete: function(t)
    {
      if(dbg){alert(t.responseText);}
      if(rsp){conInfo("RELOAD SORTED PAGE");reloadSortedPage();}
      if(gac){conInfo("GET ALL CHANNELS"); if (typeof GetAllChannels() == "function") {GetAllChannels();}}
      isSorting = false;
    }
  };
  new Ajax.Request(url,opts); 

};

iseRemoveSpecialCharacters = function(s, bBrToSpace)
{
  var newVal;
  
  // replace all whitespaces (\f, \n, \t, \v) 
  if( bBrToSpace ) { newVal = s.replace(/\s/g, " "); }
  else             { newVal = s.replace(/\s/g, ""); }
  return newVal;
};

iseStripAll = function(s)
{
  var retVal = s;
  retVal = retVal.replace(/\r/g, "");
  retVal = retVal.replace(/\n/g, "");
  retVal = retVal.replace(/\t/g, "");
  return retVal;
};

rsp = true;
gac = false;
isSorting = false;

IseSort = function(array, colName, reload, popup)
{
  conInfo("Start IseSort");
  if (!isSorting)
  {
    isSorting = true;
    Cursor.set(Cursor.WAIT);
    gac = false;
    rsp = reload;
    conInfo( "IseSort: SET RSP = "+rsp );
    if( lastSort == colName ) { iseChangeDirection(); } else { iseResetDirection(); }
    if (popup) 
    {
      iseSortMultiArray(array,colName,lastDir,1,this.popup);
    }
    else
    {
      iseSortMultiArray(array,colName,lastDir,1);
    }
    lastSort = colName;
  }
};

IseSort2 = function(array, colName, reload, gacCall,utvNo, sessionVar)
{
  conInfo("Start IseSort2");
  if (!isSorting)
  { 
    isSorting = true;
    Cursor.set(Cursor.WAIT);
    if(typeof utvNo == "undefined")
    {
      utvNo = 2;
    }
    gac = gacCall;
    rsp = reload;
    conInfo( "IseSort2: SET RSP = "+rsp );
    if( lastSort2 == colName ) { iseChangeDirection2(); } else { iseResetDirection2(); }
    iseSortMultiArray(array,colName,lastDir2,utvNo, sessionVar);
    lastSort2 = colName;
  }
};

iseUpdateIDArray = new Array();
iseUpdateTMArray = new Array();

iseInitUpdateArrays = function()
{
  iseUpdateIDArray = new Array();
  iseUpdateTMArray = new Array();
};

updateChannelControl = function( chnId, lastTimestamp )
{
  conInfo("updateChannelControl");
  if(dbg)alert("new update dp "+chnId);
  //if(rfr)setTimeout( "callbackUpdateChannelControl(" + chnId + ",'" + lastTimestamp + "')", 10000 );
  var iPos = iseUpdateIDArray.indexOf(chnId);
  if( iPos > -1 )
  {
    iseUpdateTMArray[iPos] = lastTimestamp;
  }
  else
  {
    iseUpdateIDArray.push( chnId );
    iseUpdateTMArray.push( lastTimestamp );
  }

  translatePage("#tblfav");

};

callbackUpdateChannelControl = function( chnId, lastTimestamp )
{
  conInfo("callbackUpdateChannelControl");
  var url = "/esp/system.htm?sid=" + SessionId;
  var pb = "string action = 'getLastTimeOfChn';";
  pb += 'string chnId = "' + chnId + '";';
  var opts = 
  {
    postBody: Rega.encode(pb),
    onComplete: function(t) 
    {
      var sResp = t.responseText;
      // CR or LF may be appended which must be ignored
      if( sResp.length > lastTimestamp.length )
      {
        sResp = sResp.substr( 0, lastTimestamp.length );
      }
      if( sResp != lastTimestamp )
      {
        reloadPage();
      }
      else
      {
        updateChannelControl( chnId, lastTimestamp );
      }
    }
  };  
  
  new Ajax.Request(url, opts);
};

changeTransMode = function(chnId, ctrlId) {
  dlgPopup = new iseMessageBox(ID_TRANSFER_PARAMETERS, chnId);
  PopupClose = function(selIdx)
  {
    dlgPopup.hide();
    if (selIdx === 0) {
      $(ctrlId).innerHTML = translateKey("lblStandard");
    }
    else {
      $(ctrlId).innerHTML = translateKey("lblSecured");
    }
  };
  if (PLATFORM != 'Central') {
    SendRequest('/popupTransEditor.cgi');
  }
  dlgPopup.ShowPopup();
};

recreateControl = function(chnId,sTimeStamp)
{
  var url;
  var pb;
  var opts;
  
  //alert("recreating control "+chnId+" at "+sTimeStamp);
  conInfo("recreateControl");
  if( $("tmc"+chnId) )
  {
    if( typeof sTimeStamp == "undefined" )
    {
      url = "/esp/system.htm?sid="+SessionId;
      pb = "string action = 'getLastTimeOfChn';";
      pb += "integer cId = "+chnId+";";
      opts =
      {
        postBody: ReGa.encode(pb),
        onSuccess: function(t) {
          translatePage("#dpc"+chnId);
        }
      };
      new Ajax.Updater("tmc"+chnId, url, opts);
    }
    else
    {
      $("tmc"+chnId).innerHTML = sTimeStamp;
    }
  }
  
  if( $("dpc" + chnId) )
  {
    var iStatusOnly = 0;
    if (sActPage.indexOf("statusviews") > -1) 
    {
      iStatusOnly = 1;    
    }
    url = "/esp/datapointconfigurator.htm?sid="+SessionId;
    pb = "string action = 'dcCreate';";
    pb += "integer cId = "+chnId+";";
    pb += "integer iStatusOnly = " + iStatusOnly + ";";
    opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(t)
      {
        //alert(t.responseText);
        translatePage("#dpc"+chnId);
      }
    };
    //alert(pb);
    new Ajax.Updater("dpc"+chnId, url, opts);
  }
};

LogoClick = function()
{
  //alert( iseUpdateIDArray.join("_") );
  //alert( iseUpdateTMArray.join("_") );
  //loadLinkList();
  saveObjectModel();

};

updateGroupCell = function(prefix, chnId1, chnId2, content) {
  var sCellId = prefix + chnId1 + chnId2;
  if ($(sCellId)) 
    $(sCellId).innerHTML = content;
  else {
    sCellId = prefix + chnId2 + chnId1;
    if ($(sCellId))
      $(sCellId).innerHTML = content;
  }
};


saveObjectModel = function() {
  var userNameElm = jQuery("#UserName"),
    savingColor = "#afafaf",
    restoreColor = "white";
  conInfo("The CCU is saving the ObjectModel");
  userNameElm.css('color',savingColor);
  homematic("system.saveObjectModel", {}, function () {
    conInfo("ObjectModel saved");
    userNameElm.css('color',restoreColor);
  });
};

pause = function(ms)
{
  var currentTime = new Date();
  var exitTime = currentTime.getTime() + ms;
  while( true )
  {
    currentTime = new Date();
    if( currentTime.getTime() > exitTime ) return;
  }
};

iseFlasher = null;

StartFlashing = function()
{
  iseFlasher = new PeriodicalExecuter(
    function(pe)
    {
      if( $("headerLogo") )
      {
        if( $("headerLogo").src.indexOf("_red") >= 0 )
        {
          $("headerLogo").src = "/ise/img/homematic_logo_small.png";
        }
        else
        {
          $("headerLogo").src = "/ise/img/homematic_logo_small_red.png";
        }
      }
    },
    1);
};

StopFlashing = function()
{
  iseFlasher.stop();
  iseFlasher = null;
};

SwitchOnFlashLight = function()
{
  if ($("headerLogo")) { $("headerLogo").src = "/ise/img/homematic_logo_small_red.png"; }
};

SwitchOffFlashLight = function()
{
  if ($("headerLogo")) { $("headerLogo").src = "/ise/img/homematic_logo_small.png"; }
};

getAjaxLoadElem = function() {
  return jQuery("#ajaxload");
};

getWaitAnimElem = function() {
  return jQuery("#waitAnim");
};

isWaitAnimActive = function() {
  var waitAnimElem = getWaitAnimElem();
  var result = false;
  if (!waitAnimElem.hasClass("hidden")) {
    result = true;
  }
  return result;
};

// Only visible when the waitAnimElem is hidden
// See ShowWaitAnim ...
ShowAjaxLoad = function()
{
  var ajaxLoadElem = getAjaxLoadElem();
  if (ajaxLoadElem && !isWaitAnimActive())
    ajaxLoadElem.show().removeClass('hidden');
};

HideAjaxLoad = function()
{
  var ajaxLoadElem = getAjaxLoadElem();
  if (ajaxLoadElem)
    ajaxLoadElem.hide().addClass('hidden');
};

ShowWaitAnim = function() {
  var waitAnimElem = getWaitAnimElem(),
  ajaxLoadElem = getAjaxLoadElem();
  ajaxLoadElem.hide().addClass('hidden');
  waitAnimElem.show().removeClass('hidden');
};

HideWaitAnim = function() {
  var waitAnimElem = getWaitAnimElem();
  waitAnimElem.hide().addClass('hidden');

};

HideWaitAnimAutomatically = function(seconds) {
  window.setTimeout("HideWaitAnim()", seconds * 1000);

};

buildObj = function(id, trId, ctrlId, name, rooms, funcs, roomIDs, funcIDs) {
  var tmp = new Object();
  tmp['id'] = id;
  tmp['trid'] = trId;
  tmp['ctrlId'] = ctrlId;
  tmp['name'] = name;
  tmp['room'] = rooms;
  tmp['func'] = funcs;
  tmp['fltOpts'] = new Object();
  tmp['fltOpts']['rooms'] = roomIDs;
  tmp['fltOpts']['funcs'] = funcIDs;
  return tmp;
};

RemoveAllRowsFromTable = function(table)
{
  if( $(table) )
  {
    while($(table).rows.length )
    {
      $(table).deleteRow(0);
    }
  }
};

array_merge = function(one,two)
{
  one.push(two);
  return one.flatten();
};

arrayDeleteVal = function(arr, item) {
  var index = arr.indexOf(item);
  arr.splice(index, 1);
};

DeleteObject = function(id)
{
  var url = "/esp/system.htm?sid="+SessionId;
  var pb = "";
  pb += 'string action = "DeleteObject";';
  pb += 'string id = "'+id+'";';
  var opts = 
  {
    postBody: ReGa.encode(pb),
    onSuccess: function(t) 
    {
      if( t.responseText == "false" )
      {
        //if(dbg){alert("Objekt konnte nicht gelöscht werden.");}
        if(dbg){alert(translateKey("alertErrorDeleteObject"));}
      }
      reloadPage();
    }
  };
  new Ajax.Request(url, opts);
};

DeleteObject2 = function(id)
{
  var url = "/esp/system.htm?sid="+SessionId;
  var pb = "";
  pb += 'string action = "DeleteObject";';
  pb += 'string id = "'+id+'";';
  var opts = { 
    postBody: ReGa.encode(pb), 
    onSuccess: function(t) 
    { 
      if( t.responseText == "false" )
      {
        //if(dbg){alert("Objekt konnte nicht gelöscht werden.");}
        if(dbg){alert(translateKey("alertErrorDeleteObject"));}
      }
      dlgPopup.load(); 
    }
  };
  new Ajax.Request(url, opts);
};

iseRefr = function(state)
{
  //rfr = state;
};

conInfo = function(msg)
{
  if( (typeof console != "undefined") && (urlDebug =="true") )
  {
    console.info(msg);
  }
};

conError = function(msg)
{
  if( typeof console != "undefined" )
  {
    console.error(msg);
  }
};

ResetGAC = function()
{
  conInfo("ResetGAC called.");
  GetAllChannels = function()
  {
    conInfo("GetAllChannels: NOP");
  };
};

addLeadingZero = function( iValue )
{
  var sRet = ""+iValue;
  if( iValue < 10 )
  {
    sRet = "0"+iValue;
  }
  return sRet;
};

addTrailingZero = function(val) {
  var sVal = val.toString(),
  decPlace = sVal.split(".")[1];
  if (!decPlace) {
    return sVal + ".00";
  }
  if (decPlace.length == 1) {
    return sVal + "0";
  }
  return sVal;
};

roundValue05 = function(val) {
  var intVal = Math.floor(val);
  if (val - intVal > 0.5) {
    return Math.ceil(val);
  }

  if (val - intVal == 0) {
    return val;
  }

  return intVal + 0.5;
};

round = function(x, n)
{ 
  // x = Fließkommazahl, n = gewünschte Nachkommastellen
  if (!n) n = 2; //wenn n fehlt wird n = 2
  if (n < 1 || n > 14) return false;
  var e = Math.pow(10, n);
  var k = (Math.round(x * e) / e).toString();
  if (k.indexOf('.') == -1) k += '.';
  k += e.toString().substring(1);
  return k.substring(0, k.indexOf('.') + n+1);
};

convertMin2Hour = function(valMin) {
  var min = parseInt(valMin),
  hours = Math.floor(valMin / 60),
  minutes = valMin % 60;

  hours = (isNaN(hours)) ? 0 : hours;
  minutes = (isNaN(minutes)) ? 0 : minutes;

  return hours + ' h : ' + ((minutes <= 9) ? "0"+minutes+" m" : minutes+" m");
};

// Check if a bit is set in val (max. 32 bit operation)
// Returns true/false
isBitSet = function (val, bit) {
  return ((val>>bit) % 2 != 0);
};

// another way to check the bit (64 bit operation possible)
_isBitSet = function(no, index) {
  var bin = no.toString(2);
  // Convert to Binary

  index = bin.length - index;
  // Reverse the index, start from right to left

  return bin[index] == 1;
};

/**
 * Entfernt einen Kanal aus einem Raum.
 **/
removeChannelFromRoom = function(roomId, channelId)
{
  decChnCount(roomId);
  iseChannels.delChnFromID(channelId, roomId, true);
  clearUserTempViewIDs2Room();
  
  var room = RoomList.get(roomId);
  if (room)
  {
    room.removeChannel(channelId);
  }
};

/**
 * Entfernt einen Kanal aus allen Räumen.
 **/
removeChannelFromAllRooms = function(roomId, channelId)
{
  decChnCount(roomId);
  iseChannels.delChnFromAllRooms(channelId, true);
  clearUserTempViewIDs2Room();
  
  RoomList.list().each(function (room) {
    room.removeChannel(channelId);
  });
};

removeChannelFromSubsection = function(subsectionId, channelId)
{
  decChnCount(subsectionId);
  iseChannels.delChnFromID(channelId, subsectionId, true);
  clearUserTempViewIDs2Function();
  
  var subsection = SubsectionList.get(subsectionId);
  if (subsection)
  {
    subsection.removeChannel(channelId);
  }
};

removeChannelFromAllSubsections = function(subsectionId, channelId)
{
  decChnCount(subsectionId);
  iseChannels.delChnFromAllFunctions(channelId, true);
  clearUserTempViewIDs2Function();
  
  SubsectionList.list().each(function (subsection) {
    subsection.removeChannel(channelId);
  });
};

removeRoomOrSubsection = function(id)
{
  var room = RoomList.get(id);
  if (room) { RoomList.remove(room); }
  
  var subsection = SubsectionList.get(id);
  if (subsection) { SubsectionList.remove(subsection); }
  
};

changeRoomOrSubsection = function(id)
{
  var room = RoomList.get(id);
  if (room) { RoomList.beginUpdate(id); }
  
  var subsection = SubsectionList.get(id);
  if (subsection) { SubsectionList.beginUpdate(id); }
};


showDutyCycle = function() {
  if (jQuery("#PagePathSpan").text() == translateKey("startPage")) {
    var ifaceBidCosRF = "BidCos-RF",
      ifaceHmIPRF = "HmIP-RF",
      arInterfaceDutyCycle = {},
      showPartingLine = false,
      dcUnit = "%",
      dcNotAvailable = -1,
      dcAlarm = 89;  // Attention when dc >= 90%

    homematic("Interface.listBidcosInterfaces", {"interface": ifaceBidCosRF}, function (BidCosIFaces) {
      if (BidCosIFaces) {
        var linkElem = jQuery("#iFaceShowAll");
        if ((BidCosIFaces.length > 1) && (!linkElem.hasClass("UILink"))) {
          linkElem
            .addClass("UILink")
            .on("click", function () {
              showDutyCycle(); // actualize the dc value of the start page
              showDCAllInterfaces();
            });
        }

        jQuery.each(BidCosIFaces, function (index, iFace) {
          if (iFace.type == "CCU2") {
            var dutyCycleProgressElem = jQuery("#dutyCycleProgress"),
              dutyCycleProgressBarElm = jQuery("#dutyCycleProgressBar"),
              dutyCycleValElm = jQuery("#dutyCycleVal"),
              trDutyCycle = jQuery("[name='trDutyCycle']"),
              trPartingLineElm = jQuery("#partingLine1"),
              dcVal,
              width, value;

            if (typeof iFace.dutyCycle != "undefined") {
              dcVal = parseInt(iFace.dutyCycle);
              conInfo("dutyCycle - " + ifaceBidCosRF + ": " + dcVal + dcUnit);
              arInterfaceDutyCycle[ifaceBidCosRF] = ((dcVal >= 0) && (dcVal <= 100)) ? dcVal : dcNotAvailable;
            } else {
              conInfo("No gateway status for the interface " + ifaceBidCosRF + " available!");
              arInterfaceDutyCycle[ifaceBidCosRF] = dcNotAvailable;
            }

            if (arInterfaceDutyCycle[ifaceBidCosRF] != dcNotAvailable) {
              dutyCycleValElm.text(arInterfaceDutyCycle[ifaceBidCosRF] + dcUnit);

              width = parseInt(dutyCycleProgressElem.css("width"));
              value = width - (width / 100 * arInterfaceDutyCycle[ifaceBidCosRF]);

              window.setTimeout(function () {
                //dutyCycleProgressBarElm.css("width", value + "px");
                dutyCycleProgressBarElm.css("margin-left", (width - parseInt(value)) + "px");

              }, 25);

              if (arInterfaceDutyCycle[ifaceBidCosRF] > dcAlarm) {
                trDutyCycle.addClass("attention");
              } else {
                trDutyCycle.removeClass("attention");
              }
              trPartingLineElm.show();
              showPartingLine = true;
              trDutyCycle.css("visibility", "visible");
            } else {
              trDutyCycle.css("visibility", "hidden");
            }

            if (!showPartingLine) {
              trPartingLineElm.hide();
            }
            return false; // Leave each loop
          }
        });
      }
      showDutyCycleHmIP();
    });
  }
};

showDutyCycleHmIP = function() {
  // Currently only in use for HAPs. Here we collect the available HAP addresses.
  // The display of the HAP-DutyCycle is done in showDCAllInterfaces()
  if (jQuery("#PagePathSpan").text() == translateKey("startPage")) {
    var linkElem = jQuery("#iFaceShowAll"),
      arRelevantAddresses = [];

    jQuery.each(DeviceList.devices, function(index, dev) {
      //if ((dev.typeName == "HmIP-CCU3") || (dev.typeName.indexOf("HmIP-HAP") > -1)) {
      // We fetch only the HAP addresses
      if (dev.typeName.indexOf("HmIP-HAP") > -1) {
        arRelevantAddresses.push(dev.channels[0].address.split(":")[0]);
      }
    });

    if ((arRelevantAddresses.length > 0) && (!linkElem.hasClass("UILink"))) {
      linkElem
        .addClass("UILink")
        .on("click", function () {
          showDutyCycle(); // actualize the dc value of the start page
          showDCAllInterfaces(arRelevantAddresses);
        });
    }
  }
};

//Attention: To work properly Adapter.Local.Device.Enabled of the crRFD.conf must be set to true
showCarrierSense = function() {
  if (jQuery("#PagePathSpan").text() == translateKey("startPage")) {
    var ccuAddress = homematic("CCU.getSerial"),
      linkElem = jQuery("#lblCarrierSense"),
      arHAPAddresses = [];

    jQuery.each(DeviceList.devices, function(index, dev) {
      if (dev.typeName.indexOf("HmIP-HAP") > -1) {
        arHAPAddresses.push(dev.channels[0].address.split(":")[0]);
      }
    });

    if ((arHAPAddresses.length > 0) && (!linkElem.hasClass("UILink"))) {
      linkElem
        .addClass("UILink")
        .on("click", function () {
          showCarrierSense(); // actualize the cs value of the start page
          showAllCarrierSense(ccuAddress, arHAPAddresses);
        });
    }

    if ((typeof ccuAddress != "undefined") && (ccuAddress != "")) {
      homematic("Interface.getValue", {
        "interface": "HmIP-RF",
        "address": ccuAddress + ":0",
        "valueKey": "CARRIER_SENSE_LEVEL"
      }, function (result) {
        if (result != null) {
          var carrierSenseProgressElm = jQuery("#carrierSenseProgress"),
            carrierSenseProgressBarElm = jQuery("#carrierSenseProgressBar"),
            carrierSenseValElm = jQuery("#carrierSenseVal"),
            trCarrierSense = jQuery("[name='trCarrierSense']"),
            csUnit = "%",
            csVal = parseInt(result),
            width, value;

          if ((typeof csVal == "number") && ((csVal >= 0) && (csVal <= 100))) {
            trCarrierSense.css("visibility", "visible");

            width = parseInt(carrierSenseProgressElm.css("width"));
            value = width - (width / 100 * csVal);

            carrierSenseValElm.text(csVal + csUnit);
            carrierSenseProgressBarElm.css("margin-left", (width - parseInt(value)) + "px");

          } else {
            trCarrierSense.css("visibility", "hidden");
          }
        }
      });
    }
  }
};

// Show the duty cycle of all relevant interfaces and available HAPs
showDCAllInterfaces = function(arRelevantAddresses) {
  var ifaceBidCosRF = "BidCos-RF",
    arInterfaceDutyCycle = {},
    showPartingLine = false,
    dcUnit = "%",
    dcNotAvailable = -1,
    dcAlarm = 89,  // Attention when dc >= 90%
    elmCounter = 0,
    html = "",
    dlg;

  homematic("Interface.listBidcosInterfaces", {"interface": ifaceBidCosRF}, function (BidCosIFaces) {
    html += "<table class='center' style='width: 75%;'>";

    jQuery.each(BidCosIFaces, function(index, iFace){
      var dcVal = (typeof iFace.dutyCycle != 'undefined') ? parseInt(iFace.dutyCycle) : 'unknown';
      elmCounter = index;
      html += "<tr>";
        html += "<td>";
          html += "<table class='center'>";

            html += "<tr class='alignCenter'>";
            if (iFace.type != "CCU2") {
              html += "<td><h2>" + iFace.type + " - " + translateKey('dialogSettingsBidCosRFLblSN') + ": " + iFace.address + "</h2></td>";
            } else {
              html += "<td><h2>" + translateKey('LabelCCU') + getProduct() + "</h2></td>";
            }
            html += "</tr>";

            html += "<tr class='alignCenter'>";
              html += "<td><div id='dutyCycleVal_"+index+"'>" + dcVal + dcUnit + "</div></td>";
            html += "</tr>";

          html += "</table>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td> <div><div id='dutyCycleProgress_"+index+"' class='dutyCycleProgress' align='right' style='margin-left: auto; margin-right: auto;'><div id='dutyCycleProgressBar_"+index+"' class='dutyCycleProgressBar' style='width:100%;'></div></div><div></td>";
      html += "</tr>";

      window.setTimeout(function() {
        var dutyCycleProgressElem = jQuery("#dutyCycleProgress_" + index),
          dutyCycleProgressBarElm = jQuery("#dutyCycleProgressBar_" + index),
          dutyCycleValElm = jQuery("#dutyCycleVal" + index),
          trDutyCycle = jQuery("[name='trDutyCycle']"),
          dcVal,
          width, value;



        if (typeof iFace.dutyCycle != "undefined") {
          dcVal = parseInt(iFace.dutyCycle);
          conInfo("dutyCycle - " + ifaceBidCosRF + ": " + dcVal + dcUnit);
          arInterfaceDutyCycle[ifaceBidCosRF] = ((dcVal >= 0) && (dcVal <= 100)) ? dcVal : dcNotAvailable;
        } else {
          conInfo("No gateway status for the interface " + ifaceBidCosRF + " available!");
          arInterfaceDutyCycle[ifaceBidCosRF] = dcNotAvailable;
        }

        if (arInterfaceDutyCycle[ifaceBidCosRF] != dcNotAvailable) {
          dutyCycleValElm.text(arInterfaceDutyCycle[ifaceBidCosRF] + dcUnit);

          width = parseInt(dutyCycleProgressElem.css("width"));
          value = width - (width / 100 * arInterfaceDutyCycle[ifaceBidCosRF]);

          dutyCycleProgressBarElm.css("width", value + "px");
        }
      }, 50);
    });

    /* Show HmIP HAPs */
    if (typeof arRelevantAddresses == "object" && (arRelevantAddresses.length > 0)) {
      var dc = [], counter = [],
        lastIndex = elmCounter + 1;

      jQuery.each(arRelevantAddresses, function (index, address) {
        counter[index] = lastIndex + index;
        dc[index] = parseInt(homematic("Interface.getValue", {
          "interface": "HmIP-RF",
          "address": address + ":0",
          "valueKey": "DUTY_CYCLE_LEVEL"
        }));

        if (!isNaN(dc[index])) {
          html += "<tr>";
            html += "<td>";
              html += "<table class='center'>";
                html += "<tr><td><hr></td></tr>";
                html += "<tr class='alignCenter'>";
                 html += "<td><h2>" + translateKey('HmIP-HAP') + "  - " + translateKey('dialogSettingsBidCosRFLblSN') + ": " + address + "</h2></td>";
                html += "</tr>";
                html += "<tr class='alignCenter'>";
                  html += "<td><div id='dutyCycleVal_" + counter[index] + "'>" + dc[index] + dcUnit + "</div></td>";
                html += "</tr>";
              html += "</table>";
            html += "</td>";
          html += "</tr>";
          html += "<tr>";
            html += "<td> <div><div id='dutyCycleProgress_" + counter[index] + "' class='dutyCycleProgress' align='right' style='margin-left: auto; margin-right: auto;'><div id='dutyCycleProgressBar_" + counter[index] + "' class='dutyCycleProgressBar' style='width:100%;'></div></div><div></td>";
          html += "</tr>";

          window.setTimeout(function () {
            var dutyCycleProgressElem = jQuery("#dutyCycleProgress_" + counter[index]),
              dutyCycleProgressBarElm = jQuery("#dutyCycleProgressBar_" + counter[index]),
              dutyCycleValElm = jQuery("#dutyCycleVal" + counter[index]),
              trDutyCycle = jQuery("[name='trDutyCycle']"),
              dcVal = parseInt(dc[index]),
              width, value;

            dcVal = ((dcVal >= 0) && (dcVal <= 100)) ? dcVal : dcNotAvailable;

            if (dcVal != dcNotAvailable) {
              dutyCycleValElm.text(dcVal + dcUnit);

              width = parseInt(dutyCycleProgressElem.css("width"));
              value = width - (width / 100 * dcVal);

              dutyCycleProgressBarElm.css("width", value + "px");
            }
          }, 50);
        }
      });
    }
    /* END HAP's */

    html += "</table>";

    dlg = new YesNoDialog(translateKey("lblDutyCycle"), html, "", "html");
    dlg.btnNoHide();
    dlg.btnTextYes(translateKey("btnOk"));
  });
};

// Show the Carrier Sense of the CCU and all available HAPs
showAllCarrierSense = function(ccuAddress, arHapAddress) {
  var csCCU = null,
    csHap = null,
    csHAPs = [],
    csUnit = "%",
    html = "",
    dlg;

  if ((typeof ccuAddress != "undefined") && (ccuAddress != "")) {
   csCCU = homematic("Interface.getValue", {
      "interface": "HmIP-RF",
      "address": ccuAddress + ":0",
      "valueKey": "CARRIER_SENSE_LEVEL"
    });
  }

  jQuery.each(arHapAddress, function(index, hapAddress) {
    csHap =  homematic("Interface.getValue", {
      "interface": "HmIP-RF",
      "address": hapAddress + ":0",
      "valueKey": "CARRIER_SENSE_LEVEL"
    });
    csHAPs.push(csHap);
  });

  var getHtml = function(index, address, csVal) {

    var csValUnknown = "--";

    var lblDev = (index == 0) ? translateKey("LabelCCU") : translateKey('HmIP-HAP');
    lblDev += " - " + translateKey('dialogSettingsBidCosRFLblSN') + ": " + address;

    var csValue = (! isNaN(parseInt(csVal))) ? parseInt(csVal) : csValUnknown,
    html = "";

    if (csValue == csValUnknown) {csUnit = "";}

    html += "<tr>";
    html += "<td>";
    html += "<table class='center'>";

    html += "<tr class='alignCenter'>";
    html += "<td><h2>"+ lblDev +"</h2></td>";
    html += "</tr>";

    html += "<tr class='alignCenter'>";
    html += "<td><div id='carrierSenceVal_"+index+"'>" + csValue + csUnit + "</div></td>"; // CCU Carrier Sense
    html += "</tr>";

    html += "</table>";
    html += "</td>";
    html += "</tr>";

    html += "<tr>";
    html += "<td> <div><div id='carrierSenseProgress_"+index+"' class='dutyCycleProgress' align='right' style='margin-left: auto; margin-right: auto;'><div id='carrierSenseProgressBar_"+index+"' class='dutyCycleProgressBar' style='width:100%;'></div></div><div></td>";
    html += "</tr>";

    html += "<tr><td><hr></td></tr>";

    window.setTimeout(function() {
      var carrierSenseProgressElem = jQuery("#carrierSenseProgress_" + index),
        carrierSenseProgressBarElm = jQuery("#carrierSenseProgressBar_" + index),
        carrierSenseValElm = jQuery("#carrierSenseVal" + index),
        width, value;


      carrierSenseValElm.text(csValue + csUnit);

      width = parseInt(carrierSenseProgressElem.css("width"));
      value = (csValue != csValUnknown) ? width - (width / 100 * csValue) : width;

      carrierSenseProgressBarElm.css("width", value + "px");

    }, 50);

    return html;
  };

  html += "<table class='center' style='width: 75%;'>";
    html += getHtml(0, ccuAddress, csCCU);

    jQuery.each(arHapAddress, function(index, hapAddress) {
      html += getHtml(index + 1, hapAddress, csHAPs[index]);
    });

  html += "</table>";

  dlg = new YesNoDialog(translateKey("lblCarrierSense"), html, "", "html");
  dlg.btnNoHide();
  dlg.btnTextYes(translateKey("btnOk"));

};

encodeStringStatusDisplay = function(elmID, is4Dis, specialSZ)
{
	//Wird zur Zeit nur für die Textzeilen des HM-PB-4Dis-WM und des HM-Dis-WM55 genutzt,
	//da dort einige Zeichen im Speicher an anderer Stelle liegen.
  var jElm = jQuery("#"+elmID),
  inString = jElm.val(),
  outString = "",
  szKey = "0x5f";

  conInfo("encodeStringStatusDisplay - inString: " + inString);

  // Beim HM-PB-4Dis-WM wird die Tilde zum Darstellen des ß benutzt.
  // Hier ist kein ReGa im Spiel.
  // Beim HM-Dis-WM55 ist ReGA im Spiel. Leider wandelt Ise-Script
  // die Tilde in ein " um, so daß der generierte String zerstört wird.
  // Daher hier die Prüfung ....
  szKey = (is4Dis == true) ? "~" : szKey;

  if (specialSZ) {
    szKey = specialSZ;
  }

  if (is4Dis == true) {
    outString = inString.replace(/Ä/g, "[");
    outString = outString.replace(/Ö/g, "#");
    outString = outString.replace(/Ü/g, "$");
    outString = outString.replace(/ä/g, "{");
    outString = outString.replace(/ö/g, "|");
    outString = outString.replace(/ü/g, "}");
    outString = outString.replace(/ß/g, szKey);
    outString = outString.replace(/&/g, "]");
    outString = outString.replace(/=/g, "'");
    jQuery("#"+elmID.replace(/^_/, "")).val(outString);
    return;
  } else {
    outString = inString.replace(/0xc4/g, "0x5b"); //Ä
    outString = outString.replace(/0xd6/g, "0x23"); // Ö
    outString = outString.replace(/0xdc/g, "0x24"); // Ü
    outString = outString.replace(/0xe4/g, "0x7b"); // ä
    outString = outString.replace(/0xf6/g, "0x7c"); // ö
    outString = outString.replace(/0xfc/g, "0x7d"); // ü
    outString = outString.replace(/0xdf/g, szKey); // ß
    outString = outString.replace(/0x26/g, "0x5d"); // &
    outString = outString.replace(/0x3d/g, "0x27"); // =
  }
  jElm.val(outString);
};

decodeStringStatusDisplay = function(sString) {
  var outString = sString.replace(/\[/g,"Ä");
  outString = outString.replace(/#/g,"Ö");
  outString = outString.replace(/\$/g,"Ü");
  outString = outString.replace(/{/g,"ä");
  outString = outString.replace(/\|/g,"ö");
  outString = outString.replace(/}/g,"ü");
  outString = outString.replace(/\_/g,"ß");
  outString = outString.replace(/\]/g,"&");
  outString = outString.replace(/\'/g,"=");
  return outString;
};

// For testing only
showInterfaces = function()
{
  var elemInfoPanel = jQuery("#infoPanel");
  if (elemInfoPanel.hasClass('hidden')) {
    jQuery("#btnInterfaces").removeClass("hidden").bind("click", function() {showAllInterfaces();});
    elemInfoPanel.removeClass("hidden");
  }
};

setReGaBtn = function() {
  homematic("ReGa.isPresent", null, function(result) {
    //console.log("ReGa is present: " + result + " - typeof result: " + typeof result);
    jQuery("#btnRestartReGa").children().first().css("color", "red").html("ReGa<br/>Ready");
  });
};

showHmAPITools = function()
{
  if (getUPL() == UPL_ADMIN) {
    var elemInfoPanel = jQuery("#infoPanel");
    if (elemInfoPanel.hasClass('hidden')) {
      jQuery("#btnAPITools").removeClass("hidden");
        jQuery("#btnShowAPITools").bind("click", function () {
        showAllAPITools();
      });
      elemInfoPanel.removeClass("hidden");
    }
    jQuery("#btnRestartReGa").bind("click", function () {
      jQuery(this).children().first().css("color", "green");
      homematic("CCU.restartReGa");
      setReGaBtn();
    });
  }
};

getParamset = function(method) {
  var iFace = jQuery("#iFace").val(),
    address = jQuery("#address").val(),
    paramsetKey = jQuery("#paramset").val();

  var paramSet = homematic("Interface." + method, {"interface":iFace, "address" : address, "paramsetKey" : paramsetKey});
  console.log(address + ": " + method + " - " + paramsetKey + "\n", paramSet);
};

getDeviceDescription = function(method) {
  var iFace = jQuery("#iFace").val(),
    address = jQuery("#address").val();

  homematic("Interface." + method, {"interface":iFace, "address" : address}, function(result) {
    // result contains the device description
    if (result != null) {
      if (address.split(":").length == 1) {
        var rfAddress = homematic("Device.getRFAddressByAddress", {'address': address});
        console.log(address + "\n" + rfAddress + " hex: 0x" + parseInt(rfAddress.split(":")[1]).toString(16) + "\n\n" + method + "\n", result);
      } else {
        console.log(address + ": " + method + "\n", result);
      }
    } else {
      console.log(address + " doesn't exist");
    }
  });
};

getRFAddressOfAllDevices = function(method) {
  var result = homematic("Device.getRFAddressOfAllDevices"),
    arResult = result.split("\n"),
    arOut = [],
    sOut = "";
  jQuery.each(arResult, function(index,val) {
    arOut = val.split("\t");
    if (typeof arOut[1] != "undefined") {
      sOut = arOut[0] + " hex: 0x" + parseInt(arOut[0].split(":")[1]).toString(16);
      sOut += " - " + arOut[1] + " - " + arOut[2];
      console.log((index + 1) + ": " + sOut);
    }
  });
};

getAllSysVars = function() {
  homematic("SysVar.getAll",{}, function(result) {
    console.log(result);
  });
};

getAllDataPointIds = function() {
  var
    devAddress,
    arValue,
    counter = 0,
    style = "color:blue",
    tmpDevAddress,
    ind,
    html = "";


  homematic("ReGa.getAllDatapoints",{}, function(result) {
    jQuery.each(result, function(index, value){
      arValue = value.split("--");
      ind = (typeof arValue[4] != "undefined") ? 4 : 3;
      if (typeof arValue[ind] != "undefined") { // Device address
        devAddress = arValue[ind].split(":")[0];
        if (tmpDevAddress != devAddress)  {
          tmpDevAddress = devAddress;
          counter++;
        }
        style = (counter%2 == 0) ? "color:blue" : "color:red";
        console.log("%c" + translateString(value), style);
        //html += "<div>" + translateString(value) + "</div>";
      } else {
        console.log(translateString(value));
        //html += "<div>" + translateString(value) + "</div>";
      }
    });

    //var outWindow = window.open();
    //outWindow.document.write(html);
  });
};

getProduct = function() {
 // Returns the mayor number of the WEBUI_VERSION
 return WEBUI_VERSION.split(".")[0];
};

getDevFirmware = function(addr, iface) {
  if (typeof addr == "undefined" ) {
    return "x.y.z";
  }
  var iFace = (typeof iface == "undefined") ? "HmIP-RF" : iface,
    devAddress = addr.split(":")[0],
    devDescr = homematic("Interface.getDeviceDescription", {
      "interface" : iFace,
      "address" : devAddress
    });

  return devDescr.firmware;
};

showAllAPITools = function() {
  var self = this;
  var url = "/tools/HomeMatic-API.html";

  var req = jQuery.ajax({
    url : url +"?sid=" + SessionId,
    cache: false,
    dataType: "html"
  });

  req.done(function(htmlContent) {
    var boxWidth = 470,
      boxHeight = 200;

    if (getUPL() != UPL_ADMIN) {
      boxHeight = 105;
    }

    MessageBox.show("HomeMatic API - CCU-SN: " + homematic("CCU.getSerial") + " - HmIP-Address: " + homematic("CCU.getHmIPAddress"), htmlContent, null, boxWidth, boxHeight, null, null,translateKey("btnCancel"));
    window.setTimeout(function() {
      if (getUPL() != UPL_ADMIN) {
        jQuery("[name='uLevel8']").remove();
      } else if((window.navigator.userAgent.toLowerCase().indexOf("firefox") == -1) && ((window.navigator.userAgent.toLowerCase().indexOf("chrome")) == -1)) {
        jQuery("#allDPIds").hide();
      }
    },100);
  });

  req.fail(function() {
    alert("Error while loading " + url);
  });
};

showAllInterfaces = function() {
  var sOutput = "";
  var iface = "Unknown Iface";

  homematic("Interface.listBidcosInterfaces", {"interface": "BidCos-RF"}, function(interfaceStatus) {
    iface = "BidCos-RF";
    if (interfaceStatus)
    {
      for (var loop = 0; loop < interfaceStatus.length; loop++) {
        sOutput += iface + " address: " + interfaceStatus[loop].address;
        sOutput += "<br/>"+iface + " type: " + interfaceStatus[loop].type;
        sOutput += "<br/>"+iface + " fwVersion: " + interfaceStatus[loop].fwVersion;
        sOutput += "<br/>"+iface + " description: " + interfaceStatus[loop].description;
        sOutput += "<br/>"+iface + " dutyCycle: " + interfaceStatus[loop].dutyCycle;
        sOutput += "<br/>"+iface + " isConnected: " + interfaceStatus[loop].isConnected;
        sOutput += "<br/>"+iface + " isDefault: " + interfaceStatus[loop].isDefault;
        sOutput += "<br/><br/><br/>";
      }
      MessageBox.show(translateKey("dialogAllRFInterfacesTitle"), sOutput, null, 350,150);
    }

    homematic("Interface.listBidcosInterfaces", {"interface": "BidCos-Wired"}, function(interfaceStatus) {
      iface = "BidCos-Wired";
      if (interfaceStatus)
      {
        for (var loop = 0; loop < interfaceStatus.length; loop++) {
          sOutput += iface + " address: " + interfaceStatus[loop].address;
          sOutput += "<br/>"+iface + " type: " + interfaceStatus[loop].type;
          sOutput += "<br/>"+iface + " fwVersion: " + interfaceStatus[loop].fwVersion;
          sOutput += "<br/>"+iface + " description: " + interfaceStatus[loop].description;
          sOutput += "<br/>"+iface + " dutyCycle: " + interfaceStatus[loop].dutyCycle;
          sOutput += "<br/>"+iface + " isConnected: " + interfaceStatus[loop].isConnected;
          sOutput += "<br/>"+iface + " isDefault: " + interfaceStatus[loop].isDefault;
          sOutput += "<br/><br/><br/>";
        }
        MessageBox.close();
        MessageBox.show(translateKey("dialogAllRFInterfacesTitle"), sOutput, null, 350,300);
      }

      homematic("Interface.listBidcosInterfaces", {"interface": "HmIP-RF"}, function(interfaceStatus) {
        iface = "HmIP-RF";
        if (interfaceStatus)
        {
          for (var loop = 0; loop < interfaceStatus.length; loop++) {
            sOutput += iface + " address: " + interfaceStatus[loop].address;
            sOutput += "<br/>"+iface + " type: " + interfaceStatus[loop].type;
            sOutput += "<br/>"+iface + " fwVersion: " + interfaceStatus[loop].fwVersion;
            sOutput += "<br/>"+iface + " description: " + interfaceStatus[loop].description;
            sOutput += "<br/>"+iface + " dutyCycle: " + interfaceStatus[loop].dutyCycle;
            sOutput += "<br/>"+iface + " isConnected: " + interfaceStatus[loop].isConnected;
            sOutput += "<br/>"+iface + " isDefault: " + interfaceStatus[loop].isDefault;
            sOutput += "<br/><br/><br/>";
          }
          MessageBox.close();
          MessageBox.show(translateKey("dialogAllRFInterfacesTitle"), sOutput, null, 350,300);
        }
      });
    });
  });
  return sOutput;
};

setBtnPress = function(elmId, time) {
  var onTime = (time) ? time : 500,
  elm = jQuery("#"+elmId);

  elm.removeClass("ControlBtnOff").addClass("ControlBtnOn");
  window.setTimeout(function() {elm.removeClass("ControlBtnOn").addClass("ControlBtnOff");},onTime);
};

getExtendedDescription = function(oChannelDescr)  {
  var result = "";
  var noDescrNecessary = "noDescrNecessary";
  var chType = "unknown";
  var channelAddress = oChannelDescr.channelAddress,
    channelRegaID = oChannelDescr.channelID,
    deviceType = oChannelDescr.deviceType,
    channelType = oChannelDescr.channelType,
    channelIndex = parseInt(oChannelDescr.channelIndex),
    channelIsVisible = oChannelDescr.isVisible,
    multiMode = oChannelDescr.multiMode;
  var tmpDev;

  if (typeof channelAddress != "undefined") {
    var channel = DeviceList.getChannelByAddress(channelAddress);
    if (channel) {
      chType = channel.channelType;
      channelIsVisible = channel.isVisible;
    }
  } else if (typeof channelType != "undefined") {
    chType = channelType;
  }

  if (chType == "KEY_TRANSCEIVER") {
    if (deviceType.toLowerCase().indexOf("hmip-asir") != -1) {
      result = translateKey("chType_SABOTAGECONTACT");
    }

    if (deviceType.toLowerCase() == "hmip-mod-rc8") {
      result = translateKey("chType_MOD_RC8");
    }

    if (deviceType.toLowerCase().indexOf("hmip-wgs") != -1) {
      if (channelIndex == 5) {
        result = translateKey("chType_" + chType + "_PATSCH");
      }
    }

  }

  if (chType == "SWITCH_TRANSMITTER") {
    var devType = deviceType.toLowerCase();

    switch (devType) {
      case "hmip-mod-oc8" :
        result = translateKey("chType_SWITCH_TRANSMITTER_OC");
        break;
      case "elv-sh-sb8" :
        result = translateKey("chType_SWITCH_TRANSMITTER_LED");
        break;
    }
  }

  // HM-LC-RGBW-WM - special description for the HM-LC-RGBW-WM
  /* Uncomment this to hide the channel description of a general dimmer
  if (chType == "DIMMER") {
    result = (deviceType == "HM-LC-RGBW-WM" ) ? translateKey("chType_DIMMER") : noDescrNecessary;
  }
  */

  if (chType == "DIMMER_TRANSMITTER") {
    if (deviceType == "HmIP-WUA" || deviceType == "ELV-SH-WUA") {
      result = translateKey("chType_UNIVERSAL_ACTOR_TRANSMITTER_010V");
    }

    if (deviceType == "HmIP-BSL") {
      result = translateKey("chType_OPTICAL_SIGNAL_RECEIVERB");
    }
  }

  if (chType == "DIMMER_VIRTUAL_RECEIVER") {
    if (deviceType == "HmIP-WUA" || deviceType == "ELV-SH-WUA") {
      result = translateKey("chType_UNIVERSAL_ACTOR_VIRTUAL_RECEIVER_010V");
    }

    if (deviceType == "HmIP-BSL") {
      // console.log("B firmware:q! " + getDevFirmware(channelAddress));
      result = translateKey("chType_OPTICAL_SIGNAL_RECEIVER");
    }
  }

  if (chType == "SWITCH_VIRTUAL_RECEIVER") {
    if ((deviceType.toLowerCase() == "hmip-ps"
      || deviceType.toLowerCase() == "hmip-psm"
      || deviceType.toLowerCase() == "hmip-psm-pe"
      || deviceType.toLowerCase() == "hmip-ps-uk"
      || deviceType.toLowerCase() == "hmip-psm-it"
      || deviceType.toLowerCase() == "hmip-psm-ch"
      ) && (! channelIsVisible)) {
      result = translateKey("lblHmIP_NotSupported");
    } else {

      switch (deviceType.toLowerCase()) {
        case "elv-sh-sb8":
          result = translateKey("chType_SWITCH_VIRTUAL_RECEIVER_LED");
          break;
        case "hmip-wgt":
        case "hmip-wgt-a":
        case "hmip-wgtc":
        case "hmip-wgtc-a":
          var channelMode = homematic("Interface.getMetadata", {
            "objectId": channel.id,
            "dataId": "channelMode"
          });

          var descrID = (channelMode == "modeBWTH") ? "chType_SWITCH_VIRTUAL_RECEIVER_BWTH" : "chType_SWITCH_VIRTUAL_RECEIVER",
            descr = translateKey(descrID);

          if (channel.nameExtention != "") {
            if (channel.nameExtention != "<br/>" + descr) {
              channel.nameExtention = "<br/>" + descr;
            }
          }

          result = descr;
          break;

        default: result = translateKey("chType_SWITCH_VIRTUAL_RECEIVER");
      }
    }
  }

  if (chType == "SWITCH_SENSOR") {
    tmpDev = deviceType.toLowerCase();
    if (tmpDev == "hm-sec-sir-wm" && channelIndex == 1) {
      result = translateKey("chType_SWITCH_SENSOR_Int");
    } else if (tmpDev == "hm-sec-sir-wm" && channelIndex == 2) {
      result = translateKey("chType_SWITCH_SENSOR_Ext");
    }
  }

  if (chType == "COND_SWITCH_TRANSMITTER") {
    tmpDev = deviceType.toLowerCase();

    switch (tmpDev) {
      case "hmip-stho":
      case "hmip-stho-a":
      case "elv-sh-cth":
        if (channelIndex == 2) result = translateKey("chType_COND_TEMPERATURE");
        if (channelIndex == 3) result = translateKey("chType_COND_HUMIDITY");
        break;
      case "hmip-scth230":
        if (channelIndex == 2) result = translateKey("chType_COND_CO2");
        if (channelIndex == 3) result = translateKey("chType_COND_CO2");
        if (channelIndex == 5) result = translateKey("chType_COND_TEMPERATURE");
        if (channelIndex == 6) result = translateKey("chType_COND_HUMIDITY");
        break;
      case "elv-sh-cap":
        if (channelIndex == 2) result = translateKey("chType_COND_TEMPERATURE");
        if (channelIndex == 3) result = translateKey("chType_COND_AIR_PRESSURE");
        break;
      case "elv-sh-smsi":
        if (channelIndex == 2 || channelIndex == 3) result = translateKey("chType_COND_SOIL_TEMPERATURE");
        if (channelIndex == 4 || channelIndex == 5) result = translateKey("chType_COND_SOIL_MOISTURE");
        break;
    }
  }

  if (chType == "LEVEL_COMMAND_TRANSMITTER_CO2") {
    result = translateKey("chType_COND_CO2");
  }

  if (chType == "LEVEL_COMMAND_TRANSMITTER_HUMIDITY") {
    result = translateKey("chType_COND_HUMIDITY");
  }

  if (chType == "LEVEL_COMMAND_TRANSMITTER_TEMPERATURE") {
    result = translateKey("chType_COND_TEMPERATURE");
  }

  if (chType == "PASSAGE_DETECTOR_DIRECTION_TRANSMITTER") {
    if (channelIndex == 2) result = translateKey("chType_PASSAGE_DETECTOR_DIRECTION_TRANSMITTER_RL");
    if (channelIndex == 3) result = translateKey("chType_PASSAGE_DETECTOR_DIRECTION_TRANSMITTER_LR");
  }

  if (chType == "ROTARY_CONTROL_TRANSCEIVER") {
    if (deviceType.toLowerCase() == "hmip-wrcr") {
      if (channelIndex == 2) result = translateKey("chType_ROTARY_CONTROL_TRANSCEIVER_TR");
      if (channelIndex == 3) result = translateKey("chType_ROTARY_CONTROL_TRANSCEIVER_TL");
    }
  }

  if (chType == "DOOR_RECEIVER") {
    switch ( deviceType.toLowerCase()) {
      case "hmip-mod-tm":
        result = translateKey("chType_DOOR_RECEIVER_MOD_TM");
        break;
      case "hmip-mod-ho":
        result = translateKey("chType_DOOR_RECEIVER_MOD_HO");
        break;
    }
  }

  if (chType == "SIMPLE_SWITCH_RECEIVER") {
    switch ( deviceType.toLowerCase()) {
      case "hmip-mod-tm":
        result = translateKey("chType_SIMPLE_SWITCH_RECEIVER_MOD_TM");
        break;
      case "hmip-mod-ho":
        result = translateKey("chType_SIMPLE_SWITCH_RECEIVER_MOD_HO");
        break;
    }
  }

  if (chType == "MULTI_MODE_INPUT_TRANSMITTER") {
    var getMode = false,
      typeExt = "";

    if (typeof channelRegaID != "undefined") {
      getMode = true;
    } else if (typeof channel != "undefined" ) {
        channelRegaID = channel.id;
        getMode = true;
    }

    if (getMode) {
      if ((multiMode != "--") && (typeof multiMode != "undefined")) {
        typeExt = "_" + multiMode;
      } else {
        if (channelAddress != "undefined") {
          var chn = DeviceList.getChannelByAddress(channelAddress),
          chnMode = parseInt(chn.multiMode);
          if (! isNaN(chnMode)) {
            typeExt = "_" + chnMode;
          } else {
            typeExt = "";
          }
        } else {
          typeExt = "_1";
        }
      }
    }
    result = translateKey("chType_MULTI_MODE_INPUT_TRANSMITTER" + typeExt);
  }

  if (((deviceType.indexOf("HmIPW-") != -1) && (chType.indexOf("BLIND_") != -1))
    || (deviceType.toLowerCase() == "hmip-drbli4")
    || (deviceType.toLowerCase() == "hmip-bbl-2")
    ) {

    if (chType == "BLIND_WEEK_PROFILE") {
      result = translateKey("chType_BLIND_WEEK_PROFILE");
    } else {
      if (typeof channel != "undefined") {
        var virtChannelType = channel.getVirtChannelType(),
        devMode = virtChannelType.split("_")[0].toLowerCase();
        result = (devMode == "shutter") ? translateKey("chType_" + chType.replace("BLIND", "SHUTTER")) : translateKey("chType_" + chType.replace("SHUTTER", "BLIND"));
      }
    }
  }

  if ((chType == "COND_SWITCH_TRANSMITTER_TEMPERATURE") && (deviceType.toLowerCase() == "hmip-ste2-pcb")) {
    if (channelIndex == 3) {
      result = translateKey("chType_COND_SWITCH_TRANSMITTER_TEMPERATURE_DIFF");
    }
  }

  if ((chType == "COND_SWITCH_TRANSMITTER_PARTICULATE_MATTER") && (deviceType.toLowerCase() == "hmip-sfd")) {
    if (channelIndex == 4) {
      result = translateKey("chType_COND_SWITCH_TRANSMITTER_PARTICULATE_MATTER25");
    }

    if (channelIndex == 5) {
      result = translateKey("chType_COND_SWITCH_TRANSMITTER_PARTICULATE_MATTER100");
    }

    if (channelIndex == 6) {
      result = translateKey("chType_COND_SWITCH_TRANSMITTER_PARTICULATE_MATTER10");
    }

  }

  if (chType == "OPTICAL_SIGNAL_RECEIVER") {
    //HmIPW-WRC6 - ch. 13 activates all keys
    //HmIP-WRC6-230 - ch 18 activates all keys
    var maxSingleChn = -1;
    if (deviceType.toLowerCase() == "hmipw-wrc6") {maxSingleChn = 12;}
    else if (deviceType.toLowerCase() == "hmip-wrc6-230") {maxSingleChn = 17;}
    result = (channelIndex <= maxSingleChn) ? translateKey("chType_OPTICAL_SIGNAL_RECEIVER") : translateKey("chType_OPTICAL_SIGNAL_RECEIVERA");
  }

  if (chType == "ACCESS_RECEIVER") {
    if ((deviceType.toLowerCase() == "hmip-dld") || (deviceType.toLowerCase() == "hmip-dld-a") || (deviceType.toLowerCase() == "hmip-dld-s")  ) {
      result = translateKey("chType_ACCESS_RECEIVER") + " " + (channelIndex - 1);
    }
  }

  if (chType == "DOOR_LOCK_STATE_TRANSMITTER") {
    if ((deviceType.toLowerCase() == "hmip-dld") || (deviceType.toLowerCase() == "hmip-dld-a") || (deviceType.toLowerCase() == "hmip-dld-s") ) {
      result = translateKey("chType_DOOR_LOCK_STATE_TRANSMITTER");
    }
  }

  if (chType == "SERVO_TRANSMITTER") {
    if (deviceType.toLowerCase() == "hmip-wsc") {
      result = translateKey("chType_SERVO_TRANSMITTER");
    }
  }

  if (chType == "SERVO_VIRTUAL_RECEIVER") {
    if (deviceType.toLowerCase() == "hmip-wsc") {
      result = translateKey("chType_SERVO_VIRTUAL_RECEIVER");
    }
  }

  if (chType == "ACCESS_TRANSCEIVER") {
    if (deviceType.toLowerCase() == "hmip-fwi") {
      tmpfCounter = (typeof tmpfCounter == "undefined") ? 1 : tmpfCounter;
      result = translateKey("lblUser") + " " + tmpfCounter + " ";
      tmpfCounter++;
      if (typeof tmpfTimer == "undefined") {
        tmpfTimer = window.setTimeout(function () {
          delete tmpfCounter;
          delete tmpfTimer;
        }, 1000);
      }
    }

    if (deviceType.toLowerCase() == "hmip-wkp") {
      tmpCounter = (typeof tmpCounter == "undefined") ? 1 : tmpCounter;
      result = translateKey("lblUser") + " " + tmpCounter + " ";

      if (channelIndex % 2 != 0) {
        result += translateKey("chType_ACCESS_TRANSCEIVER_LOCk");
      } else {
        result += translateKey("chType_ACCESS_TRANSCEIVER_UNLOCk");
        tmpCounter++;
      }

      if (typeof tmpTimer == "undefined") {
        tmpTimer = window.setTimeout(function () {
          delete tmpCounter;
          delete tmpTimer;
        }, 1000);
      }
      //result += (channelIndex % 2 == 0) ? translateKey("chType_ACCESS_TRANSCEIVER_UNLOCk") : translateKey("chType_ACCESS_TRANSCEIVER_LOCk");
    }
  }

  if ((chType == "DISPLAY_INPUT_TRANSMITTER") && (deviceType.toLowerCase() == "hmipw-wgd" || deviceType.toLowerCase() == "hmipw-wgd-pl")) {
    var oddChn = [1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,39, 41];
    jQuery.each(oddChn, function(index, value) {
      if (channelIndex == value) {
        if (channelIndex <=7) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 1 - " + translateKey("chType_DISPLAY_TILE") + " " + (index + 1) + translateKey("chType_DISPLAY_KEY");
          return; // leave each loop
        } else if (channelIndex >=9 && channelIndex <= 15) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 2 - " + translateKey("chType_DISPLAY_TILE") + " " + (index -3) + translateKey("chType_DISPLAY_KEY");
          return; // leave each loop
        } else if (channelIndex >=17 && channelIndex <= 23) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 3 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 7) + translateKey("chType_DISPLAY_KEY");
          return; // leave each loop
        } else if (channelIndex >=25 && channelIndex <= 31) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 4 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 11) + translateKey("chType_DISPLAY_KEY");
          return; // leave each loop
        } else if (channelIndex >=33 && channelIndex <= 39) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 5 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 15) + translateKey("chType_DISPLAY_KEY");
          return; // leave each loop
        } else if (channelIndex == 41) {
          result = translateKey(("chType_DISPLAY_UNKNOWN"));
        }
      }
    });
  }

  if ((chType == "DISPLAY_LEVEL_INPUT_TRANSMITTER") && (deviceType.toLowerCase() == "hmipw-wgd" || deviceType.toLowerCase() == "hmipw-wgd-pl")) {
    var evenChn = [2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40];

    jQuery.each(evenChn, function(index, value) {
      if (channelIndex == value) {
        if (channelIndex <= 8) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 1 - " + translateKey("chType_DISPLAY_TILE") + " " + (index + 1) + translateKey("chType_DISPLAY_LEVEL");
          return; // leave each loop
        } else if (channelIndex >= 10 && channelIndex <= 16) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 2 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 3) + translateKey("chType_DISPLAY_LEVEL");
          return; // leave each loop
        } else if (channelIndex >= 18 && channelIndex <= 24) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 3 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 7) + translateKey("chType_DISPLAY_LEVEL");
          return; // leave each loop
        } else if (channelIndex >= 26 && channelIndex <= 32) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 4 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 11) + translateKey("chType_DISPLAY_LEVEL");
          return; // leave each loop
        } else if (channelIndex >= 34 && channelIndex <= 40) {
          result = translateKey("chType_DISPLAY_SCREEN") + " 5 - " + translateKey("chType_DISPLAY_TILE") + " " + (index - 15) + translateKey("chType_DISPLAY_LEVEL");
          return; // leave each loop
        }
      }
    });
  }

  if ((chType == "DISPLAY_THERMOSTAT_INPUT_TRANSMITTER") && (deviceType.toLowerCase() == "hmipw-wgd" || deviceType.toLowerCase() == "hmipw-wgd-pl")) {
    var oddChn = [43,45,47,49,51],
      evenChn = [42,44,46,48,50];

    jQuery.each(oddChn, function(index, value) {
      if (channelIndex == value) {
        result = translateKey("chType_DISPLAY_SCREEN") + " " + (index + 6) + " - " + translateKey("chType_DISPLAY_TILE") + " 2" + translateKey("chType_DISPLAY_CLIMATE");
        return; // leave each loop
      }
    });

    jQuery.each(evenChn, function(index, value) {
      if (channelIndex == value) {
        result = translateKey("chType_DISPLAY_SCREEN") + " " + (index + 6) + " - " + translateKey("chType_DISPLAY_TILE") + " 1" + translateKey("chType_DISPLAY_CLIMATE");
        return; // leave each loop
      }
    });
  }

  if ((deviceType.toLowerCase() == "hmip-drg-dali") && ((chType == "MAINTENANCE") || (chType == "UNIVERSAL_LIGHT_RECEIVER"))) {
    result = translateKey("chType_DALI_UNIVERSAL_LIGHT_RECEIVER");
    if (channelIndex > 32) {
      result += "<br/>" + translateKey("lblGroup") + " " + (channelIndex - 32);
    }
  }

  if ((deviceType.toLowerCase().includes("hmip-smo230")) || (deviceType.toLowerCase().includes("hmipw-smo230"))) {
    var arAppendix = ["", "LEFT", "RIGHT", "BOTTOM", "VIRTUEL", "ZONE1", "ZONE2", "ZONE3", "VIRTUEL"];

    if ((channelIndex >= 1) && (channelIndex <= 8)) {
      result = translateKey("chType_" + chType + "_" + arAppendix[channelIndex]);
    }
  }

  if (chType == "CLIMATE_TRANSCEIVER") {
    if (deviceType == "ELV-SH-TACO") {
      result = translateKey("chType_CLIMATE_TRANSCEIVER_TEMP");
    }
  }

  /* Uncomment this to hide the channel description of a particular channel type
  if (chType == "KEY") {
    result = noDescrNecessary;
  }
  */

  if (result == "") {result = translateKey("chType_" + chType);}
  if (result == noDescrNecessary) {result = "";}
  return (result.split("_")[0] == "chType") ? "" : result;
};

getElemCenterPos = function(jElemStr) {
  var elmWindow = jQuery(window),
  elm = jQuery(jElemStr),
  viewPortHeight = elmWindow.height(),
  viewPortWidth = elmWindow.width(),
  elmHeight = elm.height(),
  elmWidth = elm.width();

  return {
    "top" : ((viewPortHeight / 2) - (elmHeight / 2)) + "px",
    "left" : ((viewPortWidth / 2) - (elmWidth / 2)) + "px"
  };

};

getVerticalCenterPos = function(jElemStr) {
  var viewPortHeight = jQuery(window).height(),
  elemHeight = jQuery(jElemStr).height();
  return ((viewPortHeight / 2) - (elemHeight / 2)) + "px";
};

getHorizontalCenterPos = function(jElemStr) {
  var viewPortWidth = jQuery(window).width(),
  elemWidth = jQuery(jElemStr).width();
  return ((viewPortWidth / 2) - (elemWidth / 2)) + "px";
};

getDefaultPartyModeString = function() {
  var curDate = new Date(),
  partyTemp = "21",
  strPartyMode = "";

  strPartyMode += partyTemp + ",";
  strPartyMode += (parseInt(curDate.getHours()) + 1) * 60 + ",";
  strPartyMode += curDate.getDate() + ",";
  strPartyMode += curDate.getMonth() + 1 + ",";
  strPartyMode += curDate.getFullYear() - 2000 + ",";

  strPartyMode += (parseInt(curDate.getHours()) + 3) * 60 + ",";
  strPartyMode += curDate.getDate() + ",";
  strPartyMode += curDate.getMonth() + 1 + ",";
  strPartyMode += curDate.getFullYear() - 2000;

  return strPartyMode;
};

isIPv4AddressValid = function(ipAddress) {
  var validator = /^(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))\.(\d|[1-9]\d|1\d\d|2([0-4]\d|5[0-5]))$/;
  return ipAddress.match(validator);
};

// This validates IPv4 (e. g. 192.168.0.1 or 192.168.0.0/16 - whereas the /16 is cut off and not examined) AND IPv6 addresses
isIPAddressValid = function(ipAddress) {
  ipAddress = ipAddress.split("/")[0];
  var validator = /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$|^(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$|^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$/;
  return ipAddress.match(validator);
};

isSubnetMaskValid = function(subNet) {
  var validator = /^(((255\.){3}(255|254|252|248|240|224|192|128|0+))|((255\.){2}(255|254|252|248|240|224|192|128|0+)\.0)|((255\.)(255|254|252|248|240|224|192|128|0+)(\.0+){2})|((255|254|252|248|240|224|192|128|0+)(\.0+){3}))$/g;
  return subNet.match(validator);
};

// Currently this function checks, if the device or channel is an coupling device/channel, e. g. OSRAM-Lightify device or an OSRAM-Lightify gateway
// The parameter is either a device object, channel object or a label
isNonCCUDevice = function(dev_chn_lbl) {
  var result = false;

  // Check if this an coupling device, e. g OSRAM-Lightify device
  if (dev_chn_lbl.typeName != undefined) {
    var headChannelType = dev_chn_lbl.typeName.slice(0,7),
        isGatewayLightify = dev_chn_lbl.typeName.match("VIR-OL-GTW"),
        isGatewayHue = dev_chn_lbl.typeName.match("VIR-HUE-GTW");
    result = (
         (headChannelType == "VIR-LG-")
      || isGatewayLightify != null
      || isGatewayHue != null
    ) ? true : false;
  } else if (typeof dev_chn_lbl == "string") {
    result = (
         (dev_chn_lbl.slice(0,7) == "VIR-LG-")
      || (dev_chn_lbl.match("VIR-OL-GTW") != null)
      || (dev_chn_lbl.match("VIR-HUE-GTW") != null)
    ) ? true : false;
  }
  return result;
};

// Currently this function checks, if the device or channel is a coupling gateway (e. g. OSRAM-Lightify)
// For more tests this function must be extended.
isNonCCUGateway = function(oDevChn) {
  var result = false;
  if (oDevChn != undefined) {
    result = (
       (oDevChn.typeName.match("VIR-OL-GTW") != undefined)
    || (oDevChn.typeName.match("VIR-HUE-GTW") != undefined)
    ) ? true : false;
  }
  return result;
};

isDevTypeHmIPW = function(type) {
  return (type.indexOf("HmIPW-") != -1) ? true : false;
};

// Global object for the diagrams of the dual white controller
getDualWhiteControllerDiagramURLs = function() {
  if (typeof dualWhiteControllerDiagramURL != "object") {
    var path = "/ise/img/icons_hm_lc_dw/";
    dualWhiteControllerDiagramURL = {};

    dualWhiteControllerDiagramURL["0000"] = path + "Crossfade_linear_konstant_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["0001"] = path + "Crossfade_linear_konstant_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["0010"] = path + "Crossfade_linear_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["0011"] = path + "Crossfade_linear_maximal_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["0100"] = path + "Crossfade_quadratisch_konstant_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["0101"] = path + "Crossfade_quadratisch_konstant_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["0110"] = path + "Crossfade_quadratisch_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["0111"] = path + "Crossfade_quadratisch_maximal_niedrigistkalt.png";

    dualWhiteControllerDiagramURL["1000"] = path + "Dim2Warm_linear_halb_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["1001"] = path + "Dim2Warm_linear_halb_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["1010"] = path + "Dim2Warm_linear_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["1011"] = path + "Dim2Warm_linear_maximal_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["1100"] = path + "Dim2Warm_quadratisch_halb_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["1101"] = path + "Dim2Warm_quadratisch_halb_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["1110"] = path + "Dim2Warm_quadratisch_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["1111"] = path + "Dim2Warm_quadratisch_maximal_niedrigistkalt.png";

    dualWhiteControllerDiagramURL["2000"] = path + "Dim2Hot_linear_halb_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["2001"] = path + "Dim2Hot_linear_halb_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["2010"] = path + "Dim2Hot_linear_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["2011"] = path + "Dim2Hot_linear_maximal_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["2100"] = path + "Dim2Hot_quadratisch_halb_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["2101"] = path + "Dim2Hot_quadratisch_halb_niedrigistkalt.png";
    dualWhiteControllerDiagramURL["2110"] = path + "Dim2Hot_quadratisch_maximal_niedrigistwarm.png";
    dualWhiteControllerDiagramURL["2111"] = path + "Dim2Hot_quadratisch_maximal_niedrigistkalt.png";
  }
};

getTimeZoneDefinition = function(timeZone) {
  var tz = [];
  tz["ACST"] = [9.5, 9.5];
  tz["ACST/ACDT"] = [9.5, 10.5];
  tz["AEST"] = [10, 10];
  tz["AEST/AEDT"] = [10, 11];
  tz["AKST/AKDT"] = [-9, -8];
  tz["AST/ADT"] = [-4, -3];
  tz["AWST/AWDT"] = [8, 9];
  tz["BRST/BRDT"] = [-3, -2];
  tz["CET/CEST"] = [1, 2];
  tz["CST"] = [-6, -6];
  tz["CST/CDT"] = [-6, -5];
  tz["EET/EEST"] = [2, 3];
  tz["EST/EDT"] = [-5, -4];
  tz["GMT/BST"] = [0, 1];
  tz["GMT/IST"] = [0, 1];
  tz["HAW"] = [-10, -10];
  tz["HKT"] = [8, 8];
  tz["MSK/MSD"] = [3, 4];
  tz["RMST/RMDT"] = [3, 4];
  tz["MST"] = [-7, -7];
  tz["MST/MDT"] = [-7, -6];
  tz["NST/NDT"] = [-3.5, -2.5];
  tz["NZST/NZDT"] = [12, 13];
  tz["PST/PDT"] = [-8, -7];
  tz["SGT"] = [8, 8];
  tz["ULAT/ULAST"] = [8, 9];
  tz["WET/WEST"] = [0, 1];
  tz["WIB"] = [7, 7];
  return tz[timeZone];
};

getUTCOffset = function(tz) {
  return getTimeZoneDefinition(tz);
};

setPositionAllDevices = function(lon, lat, timeZone) {
  if (ConfigData.isPresent) {

    var arUtcOffset = [];
    arUtcOffset = getUTCOffset(timeZone);

    var utcOffset = arUtcOffset[0] * 60,
      utcOffsetDST = arUtcOffset[1] * 60;

    jQuery.each(DeviceList.devices, function (index, device) {
      var iFace = device.interfaceName;
      if ((iFace.toLowerCase() == "hmip-rf") && (device.typeName.toLowerCase() != "hmip-rcv-50") && (device.typeName.toLowerCase() != "hmip-dld") && (device.typeName.toLowerCase() != "hmip-dld-a") && (device.typeName.toLowerCase() != "hmip-dld-s") ) {
        // Check if the device has the channel *_WEEK_PROFILE
        jQuery.each(device.channels, function (index, channel) {
          if (channel.channelType.indexOf("_WEEK_PROFILE") != -1) {
            conInfo("Set the position of this device: " + channel.address.split(":")[0] + ":0 - lon: " + lon + " - lat: " + lat + " - utcOffset: " + utcOffset + " - utcOffsetDST: " + utcOffsetDST );

            homematic("Interface.putParamset", {
              'interface': iFace,
              'address': channel.address.split(":")[0] + ":0",
              'paramsetKey': 'MASTER',
              'set':
                [
                  {name: 'LONGITUDE', type: 'double', value: lon},
                  {name: 'LATITUDE', type: 'double', value: lat},
                  {name: 'UTC_OFFSET', type: 'double', value: utcOffset},
                  {name: 'UTC_DST_OFFSET', type: 'double', value: utcOffsetDST}
                ]
            }, function (result) {
              conInfo(result);
            });
          } else if (channel.channelType == "HEATING_CLIMATECONTROL_TRANSCEIVER") {
            conInfo("Set the position of this device: " + channel.address.split(":")[0] + ":0 - lon: " + lon + " - lat: " + lat + " - utcOffset: " + utcOffset + " - utcOffsetDST: " + utcOffsetDST );

            homematic("Interface.putParamset", {
              'interface': iFace,
              'address': channel.address.split(":")[0] + ":0",
              'paramsetKey': 'MASTER',
              'set':
                [
                  {name: 'UTC_OFFSET', type: 'double', value: utcOffset},
                  {name: 'UTC_DST_OFFSET', type: 'double', value: utcOffsetDST}
                ]
            }, function (result) {
              conInfo(result);
            });
          }
        });
      }
    });
  } else {
    window.setTimeout(function() {
      conInfo("ConfigData.isPresent: " + ConfigData.isPresent);
      counterSetPosition++;
      if (counterSetPosition < 30) {
        setPositionAllDevices(lon, lat, timeZone);
      }
    }, 2500);
  }
};

setNewDevicePos2SystemPos = function(oDevice) {
  homematic("system.getPositionData", {}, function(posData) {
    var lon = posData[0].split(":")[1],
      lat = posData[1].split(":")[1],
      arUtcOffset = getUTCOffset(posData[2].split(":")[1]),
      utcOffset = arUtcOffset[0] * 60,
      utcOffsetDST = arUtcOffset[1] * 60;

    var iFace = oDevice.iface;
    if (iFace.toLowerCase() == "hmip-rf" && (oDevice.type.toLowerCase() != "hmip-rcv-50") && (oDevice.type.toLowerCase() != "hmip-dld") && (oDevice.type.toLowerCase() != "hmip-dld-a") && (oDevice.type.toLowerCase() != "hmip-dld-s")) {
      // Check if the device has the channel *_WEEK_PROFILE
      jQuery.each(oDevice.chnTypes, function (index, channelType) {
        if (channelType.indexOf("_WEEK_PROFILE") != -1) {
          conInfo("Set the position of this device: " + oDevice.sn +":0 - lon: " + lon + " - lat: " + lat );

          homematic("Interface.putParamset", {
            'interface': iFace,
            'address': oDevice.sn + ":0",
            'paramsetKey': 'MASTER',
            'set':
              [
                {name: 'LONGITUDE', type: 'double', value: lon},
                {name: 'LATITUDE', type: 'double', value: lat},
                {name: 'UTC_OFFSET', type: 'double', value: utcOffset},
                {name: 'UTC_DST_OFFSET', type: 'double', value: utcOffsetDST}
              ]
          }, function (result) {
            conInfo(result);
          });
          return false ; // leave jQuery.each
        }
      });
    }
  });
};

setColorWebUI = function() {
  var colorKeys = [
    "background",
    "activeBackground",
    "contentBackground",
    "white"
  ];

  var counter = 0,
    copyFile = 0;

  var modifiyColorMap = function () {
    copyFile = ((counter + 1) == dlg.key.length) ? 1 : 0;
    homematic("WebUI.setWebUIColors", {"key": colorKeys[counter], "color": dlg.key[counter], "cpFile" : copyFile}, function (result) {
      counter++;
      if (counter == colorKeys.length) {
        window.location.reload();
      } else {
        modifiyColorMap();
      }
    });
  };

  var html = "<table>";
  jQuery.each(colorKeys, function(index, val) {
    html += "<tr><td>"+val+"</td><td><input id='colorPicker_" + val +"' class='_hidden' size='5'/></td></tr>";
  });

  html += "</table>";

  dlg = new YesNoDialog(translateKey("SetWebUIScheme"), html, function(result) {
    if ((result == YesNoDialog.RESULT_YES)) {
      modifiyColorMap();
    }
  }, "html");

  dlg.key = [];
  dlg.btnYesHide();
  dlg.btnTextNo(translateKey("btnCancel"));
  dlg.btnTextYes(translateKey("btnOk"));

  dlg.run = function() {
    jQuery.each(colorKeys, function(index, val) {
      jQuery("#colorPicker_"+val).spectrum({
        preferredFormat: 'hex',
        //showInput: true,
        color: WebUI.getColor(val),
        //showPalette: true,
        cancelText: translateKey('btnCancel'),
        chooseText: translateKey('btnOk'),

        show: function () {
          jQuery("#colorPicker_" + val).val(WebUI.getColor(val));
          dlg.btnYesHide();
        },
        hide: function (color) {
          dlg.changeColor = true;
          dlg.btnYesShow();
          dlg.key[index] = color.toHexString();
        }
      });
    });
  };
  dlg.run();
  dlg.resetHeight();
};

function activateDeviceBetaFw() {
  var showBetaDevFw = jQuery("#inputShowBetaFw").is(":checked"),
    fieldTestActive = "/etc/config/fieldTestActive";

  if (showBetaDevFw) {
    if (! homematic('CCU.existsFile', {'file': fieldTestActive})) {
      homematic("CCU.createFile", {'file': fieldTestActive});
    }
  } else {
    if (homematic('CCU.existsFile', {'file': fieldTestActive})) {
      homematic("CCU.removeFieldTestActive");
    }
  }
};


/**
 *
 * Tmporarily introduced with the HmIP-RGBW - this may not be the best place here
 *
 */

// `hsvToRgb`
// Converts an HSV color value to RGB.
// *Assumes:* h is contained in [0, 1] or [0, 360] and s and v are contained in [0, 1] or [0, 100]
// *Returns:* { r, g, b } in the set [0, 255]

function isOnePointZero(n) {
  return typeof n == "string" && n.indexOf('.') != -1 && parseFloat(n) === 1;
}

// Check to see if string passed in is a percentage
function isPercentage(n) {
  return typeof n === "string" && n.indexOf('%') != -1;
};


function bound01(n, max) {
  if (isOnePointZero(n)) { n = "100%"; }

  var processPercent = isPercentage(n);
  n = Math.min(max, Math.max(0, parseFloat(n)));

  // Automatically convert percentage into number
  if (processPercent) {
    n = parseInt(n * max, 10) / 100;
  }

  // Handle floating point rounding errors
  if ((Math.abs(n - max) < 0.000001)) {
    return 1;
  }

  // Convert into [0, 1] range if it isn't already
  return (n % max) / parseFloat(max);
};


function hsvToRgb(h, s, v) {

  h = bound01(h, 360) * 6;
  s = bound01(s, 100);
  v = bound01(v, 100);

  var i = Math.floor(h),
    f = h - i,
    p = v * (1 - s),
    q = v * (1 - f * s),
    t = v * (1 - (1 - f) * s),
    mod = i % 6,
    r = [v, q, p, p, t, v][mod],
    g = [t, v, v, q, p, p][mod],
    b = [p, p, t, v, v, q][mod];

  return { r: r * 255, g: g * 255, b: b * 255 };
};