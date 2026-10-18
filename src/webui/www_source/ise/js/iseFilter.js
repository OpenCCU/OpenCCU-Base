/**
 * ise/iseFilter.js
 **/

/**
 * @fileOverview Speichert und verwaltet Filter-Kriterien für Listen-Ansichten
 * @author Michael Niehaus (ise)
 **/

// file: iseFilter
// author: Michael Niehaus
// date created: 15.05.2007
//
// speichert und verwaltet Filter-Kriterien für Listen-Ansichten
FLT_OBJ_TYP_VARS = 1;
bFilterUsed = false;           

/**
 * @class
 **/
iseFilter = Class.create();
iseFilter.prototype = {
  initialize: function(sPageId, fltObjType) {
    this.filRooms = "";
    this.filFuncs = "";
    this.filType = "";
    this.filDesc = "";
    this.filSn = "";
    this.filName = "";
    this.filUnit = "";
    this.filChnLink = "";
    this.filVarTypes = "";
    this.filTime = "";
    this.filDate = "";
    this.bSend = false;
    this.bRec = false;
    this.bStd = false;
    this.bSafe = false;
    this.bTypeLogic = false;
    this.bTypeValueList = false;
    this.bTypeNumber = false;
    this.bTypeAlarm = false;
    this.bTypeString = false;
    this.bColLeft = false;
    this.bColCenter = false;
    this.bNameLeft = false;
    this.bNameTop = false;
    this.bPC = false;
    this.bPDA = false;
    this.bCentral = false;
    this.filUsers  = "";
    this.pageID = sPageId;
    this.fltObjType = 0;
    if (fltObjType)
      this.fltObjType = fltObjType;
  },
  

  saveModeFilter: function(bModeStd, bModeSafe)
  {
    this.bStd  = bModeStd;
    this.bSafe = bModeSafe;
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "saveModeFilter";';
    pb += 'string sPageId = "'+this.pageID+'";';
    pb += 'integer iModeStd = '+(bModeStd? 1:0)+';';
    pb += 'integer iModeSafe = '+(bModeSafe? 1:0)+';';
    var opt =
    {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url, opt);
  },
  
  saveIfaceFilter: function(bSend, bRec)
  {
    this.bSend = bSend;
    this.bRec = bRec;
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "saveIfaceFilter";';
    pb += 'string sPageId = "'+this.pageID+'";';
    pb += 'integer iSend = '+(bSend? 1:0)+';';
    pb += 'integer iRec = '+(bRec? 1:0)+';';
    var opt =
    {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url, opt);
  },

  
  addStringFilter: function(fId, s) {
    s = s.toLowerCase();
    switch(fId) {
      case FIL_NAME:  this.filName    = s; break;
      case FIL_DESC:  this.filDesc    = s; break;
      case FIL_SN:    this.filSn      = s; break;
      case FIL_TYPE:  this.filType    = s; break;
      case FIL_ROOMS: this.filRooms   = s; break;
      case FIL_FUNCS: this.filFuncs   = s; break;
      case FIL_UNIT:  this.filUnit    = s; break;
      case FIL_TIME:  this.filTime    = s; break;
      case FIL_DATE:  this.filDate    = s; break;
      case FIL_USERS: this.filUsers   = s; break;
      case FIL_CHN:   this.filChnLink = s; break;
      default:        break;
    }
  },
  
  isFilterType: function(ft) {
    switch(ft) {
      case FIL_MODE_AES: return this.bSafe;
      case FIL_MODE_STD: return this.bStd;
      case FIL_IFACE_S : return this.bSend;
      case FIL_IFACE_R : return this.bRec;
      default          : return null;
    }
  },
  
  objPassFilter: function(obj)
  {
    var bFound;
    var arF;
    var arO;
    var i;
    var x;
    
    bFilterUsed = true;                  
    if (this.filRooms !== "")
    {
      bFound = false;
      arF = this.filRooms.split('\t');
      
      if (obj['fltOpts']['rooms'])
      {
        arO = obj['fltOpts']['rooms'].split('\t');
        for (i = 0; i < arF.length; i++)
        {
          if (!bFound)
          {
            for (x = 0; x < arO.length; x++)
            {
              if (arF[i] == arO[x])
              {
                bFound = true;
                break;
              }
            }
          }
        }
      }
      if (!bFound) { return false; }
    }
    
    if (this.filFuncs !== "")
    {
      bFound = false;
      arF    = this.filFuncs.split('\t');
       arO   = obj['fltOpts']['funcs'].split('\t');
       
      for (i = 0; i < arF.length; i++)
      {
        if (!bFound) {
          for (x = 0; x < arO.length; x++)
          {
            if (arF[i] == arO[x])
            {
              bFound = true;
              break;
            }
          }
        }
      }
      if (!bFound) { return false; }
    }
    if (this.filType !== "")
    {
      if (typeof obj['type'] != 'undefined') {
        if (obj['type'] === "")                      { return false; }
        if (obj['type'].toLowerCase().indexOf(this.filType) == -1) { return false; }
      }
    }
    if (this.filDesc !== "")
    {
      //conInfo( "iseFilter: filDesc="+this.filDesc );
      if (typeof obj['desc'] != 'undefined')
      {
        //conInfo( "iseFilter: desc="+obj['desc'] );
        if (obj['desc'] === "")                      { return false; }
        if (obj['desc'].toLowerCase().indexOf(this.filDesc) == -1) { return false; }
      }
    }
    if (this.filName !== "")
    {
      if (typeof obj['name'] != 'undefined')
      {
        if (obj['name'] === "")                      { return false; }
        var transName = translateString(obj['name']);
        if (transName.toLowerCase().indexOf(this.filName) == -1) { return false; }
      }
    }
    if (this.filSn !== "")
    {
      if (typeof obj['sn'] != 'undefined')
      {
        if (obj['sn'] === "")                     { return false; }
        if (obj['sn'].toLowerCase().indexOf(this.filSn) == -1) { return false; }
      }
    }
    if (this.filUnit !== "")
    {
      if (typeof obj['unit'] != 'undefined')
      {
        if (obj['unit'] === "")                      { return false; }
        if (obj['unit'].toLowerCase().indexOf(this.filUnit) == -1) { return false; }
      }
    }

    if (this.filTime !== "")
    {
      if (typeof obj['time'] != 'undefined')
      {
        if (obj['time'] === "")                      { return false; }
        if (obj['time'].toLowerCase().indexOf(this.filTime) == -1) { return false; }
      }
    }
    
    if (this.filDate !== "")
    {
      if (typeof obj['date'] != 'undefined')
      {
        if (obj['date'] === "")                      { return false; }
        if (obj['date'].toLowerCase().indexOf(this.filDate) == -1) { return false; }
      }
    }
    if (this.filChnLink !== "")
    {
      if (obj['chn'] === "")                         { return false; }
      if (obj['chn'].toLowerCase().indexOf(this.filChnLink) == -1) { return false; }
    }
/*    
    if (this.filVarTypes !== "")
    {
    }
*/    
    if (this.bSend != this.bRec) {
      if (this.bSend) {
        if (obj['fltOpts']['iCat'] == 2)
          return false;
      }
      if (this.bRec) {
        if (obj['fltOpts']['iCat'] == 1)
          return false;
      }
    }
    if (this.bStd != this.bSafe) {
      if (this.bStd) {
        if (obj['fltOpts']['iTrans'] === true) { return false; }
      }
      if (this.bSafe) {
        if (obj['fltOpts']['iTrans'] === false) { return false; }
      }
    }
    if (this.fltObjType == FLT_OBJ_TYP_VARS) {
      if (this.varTypeFilterSet()) {
        var bRet = false;
        if (this.bTypeLogic) {
          if ((obj['type'] == "bool") || (obj['type'] == "boolean") ) { bRet = true; }
        }
        if (this.bTypeValueList) {
          if (obj['type'] == "enum")
          {
            if (!bRet) { bRet = true; }
          }
        }
        if (this.bTypeNumber) {
          if (obj['type'] == "generic")
          {
            if (!bRet) { bRet = true; }
          }
        }
        if (this.bTypeAlarm) {
          if (obj['type'] == "alarm")
          {
            if (!bRet) { bRet = true; }
          }
        }
        if (this.bTypeString) {
          if (obj['type'] == "string")
          {
            if (!bRet) { bRet = true; }
          }
        }
        return bRet;
      }
    }
    if (this.bColLeft != this.bColCenter) {
      if (this.bColLeft) {
        if (obj['colpos'] == 1) { return false; }
      }
      if (this.bColCenter) {
        if (obj['colpos'] == 0) { return false; }
      }
    }
    if (this.bNameLeft != this.bNameTop) {
      if (this.bNameLeft) {
        if (obj['namepos'] == 1) { return false; }
      }
      if (this.bNameTop) {
        if (obj['namepos'] == 0) { return false; }
      }
    }
    if (this.bPC){
      if (!obj['devPC']) { return false; }
    }
    if (this.bPDA) {
      if (!obj['devPDA']) { return false; }
    }
    if (this.bCentral) {
      if (!obj['devCentral']) { return false; }
    }
    if (this.filUsers) {
      bFound = false;
      arF = this.filUsers.split('\t');
      arO = obj['users'].split('\t');
      for (i = 0; i < arF.length; i++) {
        if (!bFound) {
          for (x = 0; x < arO.length; x++) {
            if (arF[i] == arO[x]) {
              bFound = true;
              break;
            }
          }
        }
      }
      if (!bFound)
        return false;
    }
    return true;
  },
  
  varTypeFilterSet: function() {
    if (this.bTypeLogic) return true;
    if (this.bTypeValueList) return true;
    if (this.bTypeNumber) return true;
    if (this.bTypeAlarm) return true;
    if (this.bTypeString) return true;
    return false; 
  },
    
  isFilterId: function(id, fId)
  {
    var i;
    var rIds;
    var fIds;
    
    switch(fId) 
    {
      case FIL_ROOMS:
        rIds = this.filRooms.split('\t');
        for(i = 0; i < rIds.length; i++) {
          if (rIds[i] == id) { return true; }
        }
        return false;
      case FIL_FUNCS:
        fIds = this.filFuncs.split('\t');
        for(i = 0; i < fIds.length; i++) {
          if (fIds[i] == id) { return true; }
        }
        return false;
      case FIL_USERS:
        fIds = this.filUsers.split('\t');
        for(i = 0; i < fIds.length; i++) {
          if (fIds[i] == id) { return true; }
        }
        return false;
      default:
        return false;
    }
  },
  
  clearFilters: function(id, ftType)
  {
//    var url = "/esp/system.htm?sid="+SessionId;
//    var pb = 'string action = "clearFilter";';
//    pb += 'string sPageId = "' + this.pageID + '";';
//    var opt =
//    {
//      postBody: ReGa.encode(pb),
//      onSuccess: function(t)
//      {
//        conInfo( t.responseText );
//      }
//    }
//    new Ajax.Request(url, opt);
    
    this.filRooms = "";
    this.filFuncs = "";
    this.filType = "";
    this.filDesc = "";
    this.filSn = "";
    this.filName = "";  
    this.filUnit = ""; 
    this.filChnLink = "";
    this.filVarTypes = "";
    this.filTime = "";
    this.filDate = "";
    this.bSend = false;
    this.bRec = false;
    this.bStd = false;
    this.bSafe = false; 
    this.bColLeft = false;
    this.bColCenter = false;
    this.bNameLeft = false;
    this.bNameTop = false;
    this.bPC = false;
    this.bPDA = false;
    this.bCentral = false;
    this.filUsers  = "";
    this.bTypeLogic = false;
    this.bTypeValueList = false;
    this.bTypeNumber = false;
    this.bTypeAlarm = false;
    this.bTypeString = false;
    conInfo("Alle Filter wurden zurückgesetzt.");
  }
};
 
/* * * * * * * * * * * * *   HILFS-FUNKTIONEN   * * * * * * * * * * * * * * */

// Prüft anhand der filterOptions ob Checkboxen in Submenüs gesetzt werden sollen
selectFilters = function(fltObj, divToShow)
{
  var i;
  var inplist;
  
  switch(divToShow) {
    case "btnFilterRoomSub":
      inplist = $("btnFilterRoomSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var roomId = parseInt(inplist[i].id.substr(2));
        inplist[i].checked = fltObj.isFilterId(roomId, FIL_ROOMS);
      }
      break;
    case "btnFilterFuncSub":
      inplist = $("btnFilterFuncSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var funcId = parseInt(inplist[i].id.substr(2));
        inplist[i].checked = fltObj.isFilterId(funcId, FIL_FUNCS);
      }
      break;
    case "btnFilterVarTypeSub":
      $("cbLogic").checked = fltObj.bTypeLogic;
      $("cbValueList").checked = fltObj.bTypeValueList;
      $("cbNumber").checked = fltObj.bTypeNumber;
      $("cbAlarm").checked = fltObj.bTypeAlarm;
      $("cbString").checked = fltObj.bTypeString;
      break;
    case "btnFilterIfaceSub":
      $("cbSend").checked = fltObj.isFilterType(FIL_IFACE_S); 
      $("cbRec").checked = fltObj.isFilterType(FIL_IFACE_R); 
      break;
    case "btnFilterModeSub":
      $("cbSafe").checked = fltObj.isFilterType(FIL_MODE_AES); 
      $("cbStd").checked = fltObj.isFilterType(FIL_MODE_STD); 
      break;
    case "btnFilterNameSub":
      $("ftName").value = fltObj.filName;
      break;
    case "btnFilterTypeSub":
      $("ftType").value = fltObj.filType;
      break;
    case "btnFilterDescSub":
      $("ftDesc").value = fltObj.filDesc;
      break;
    case "btnFilterSNSub":
      $("ftSN").value = fltObj.filSn;
      break;
    case "btnFilterTimeSub":
      $("ftTime").value = fltObj.filTime;
      break;
    case "btnFilterDateSub":
      $("ftDate").value = fltObj.filDate;
      break;
    case "btnFilterColPos":
      $("cbColLeft").checked = fltObj.bColLeft;
      $("cbColCenter").checked = fltObj.bColCenter;
      break;
    case "btnFilterNamePos":
      $("cbNameLeft").checked = fltObj.bNameLeft;
      $("cbNameTop").checked = fltObj.bNameTop;
      break;
    case "btnFilterEndDev":
      $("cbPC").checked = fltObj.bPC;
      $("cbPDA").checked = fltObj.bPDA;
      $("cbCentral").checked = fltObj.bCentral;
      break;
    case "btnFilterUsersSub":
      inplist = $("btnFilterUsersSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var userId = parseInt(inplist[i].id.substr(2));
        inplist[i].checked = fltObj.isFilterId(userId, FIL_USERS);
      }
      break;
    case "btnFilterChn":
      $("ftChn").value = fltObj.filChnLink;
      break;
    default:
      break;
  }
};


iseSFilterCheckEnterEsc = function(keyCode, idSet, idCancel)
{
  switch (keyCode) {
    case 13:
      // Set the filter -- press ok
      jQuery("#"+ idSet).click();
      break;
    case 27:
      jQuery("#"+ idCancel).click();
      break;
    default:
      break;
  }
};

// Freitext-Filter
setSFilter = function(fltObj, ctrlId, fId)
{
  //conInfo("iseFilter: setSFilter "+fId+" to "+$(ctrlId).value);
  fltObj.addStringFilter(fId, $(ctrlId).value);
};

// Integer-Filter (Modus, Iface)
setIFilter = function(fltObj, sId)
{
  switch (sId)
  {
    case "mode": 
      fltObj.saveModeFilter($("cbStd").checked, $("cbSafe").checked);
      break;
    case "iface":
      fltObj.saveIfaceFilter($("cbSend").checked, $("cbRec").checked);
      break;
    default:
      break;
  }
};

// ID-Filter (rooms, funcs)
setAFilter = function(fltObj, sId)
{
  var sTmp = "";
  var inplist;
  var i;
  
  switch(sId) {
    case "rooms":
      inplist = $("btnFilterRoomSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var roomId = parseInt(inplist[i].id.substr(2));
        if (inplist[i].checked)
          sTmp += roomId + "\t";
      }
      sTmp = sTmp.substr(0, sTmp.length - 1);
      fltObj.addStringFilter(FIL_ROOMS, sTmp);
      break;
    case "funcs":
      inplist = $("btnFilterFuncSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var funcId = parseInt(inplist[i].id.substr(2));
        if (inplist[i].checked)
          sTmp += funcId + "\t";
      }
      sTmp = sTmp.substr(0, sTmp.length - 1);
      fltObj.addStringFilter(FIL_FUNCS, sTmp);
      break;
    case "vartypes":
      fltObj.bTypeLogic = $("cbLogic").checked;
      fltObj.bTypeValueList = $("cbValueList").checked;
      fltObj.bTypeNumber = $("cbNumber").checked;
      fltObj.bTypeAlarm = $("cbAlarm").checked;
      fltObj.bTypeString = $("cbString").checked;
      break;
    case "colPos":
      fltObj.bColLeft = $("cbColLeft").checked; 
      fltObj.bColCenter = $("cbColCenter").checked; 
      break;
    case "namePos":
      fltObj.bNameLeft = $("cbNameLeft").checked; 
      fltObj.bNameTop = $("cbNameTop").checked; 
      break;
    case "endDev":
      fltObj.bPC = $("cbPC").checked;
      fltObj.bPDA = $("cbPDA").checked;
      fltObj.bCentral = $("cbCentral").checked;
      break;
    case "users":
      inplist = $("btnFilterUsersSub").getElementsByTagName('input');
      for (i = 0; i < inplist.length; i++) {
        var userId = parseInt(inplist[i].id.substr(2));
        if (inplist[i].checked)
          sTmp += userId + "\t";
      }
      sTmp = sTmp.substr(0, sTmp.length - 1);
      fltObj.addStringFilter(FIL_USERS, sTmp);
      break;
    default:
      break;
  }
};

filterBtn = function(id) {
  if ($(id))
    $(id).style.backgroundColor = WebUI.getColor("clickable");
};

filterBtnSelected = function(id) {
  if ($(id))
    $(id).style.backgroundColor = WebUI.getColor("active");
};

colorFilterBtns = function(fltObj)
{
  if (fltObj.filName !== "") filterBtnSelected('btnFilterName'); 
  else filterBtn('btnFilterName');
  if (fltObj.filType !== "") filterBtnSelected('btnFilterType'); 
  else filterBtn('btnFilterType');
  if (fltObj.filDesc !== "") filterBtnSelected('btnFilterDesc'); 
  else filterBtn('btnFilterDesc');
  if (fltObj.filSn !== "") filterBtnSelected('btnFilterSN'); 
  else filterBtn('btnFilterSN');
  if (fltObj.filRooms !== "") filterBtnSelected('btnFilterRoom');
  else filterBtn('btnFilterRoom');
  if (fltObj.filFuncs !== "") filterBtnSelected('btnFilterFunc');
  else filterBtn('btnFilterFunc');
  if (fltObj.bSend||fltObj.bRec) filterBtnSelected('btnFilterIface');
  else filterBtn('btnFilterIface');
  if (fltObj.bStd||fltObj.bSafe) filterBtnSelected('btnFilterMode');
  else filterBtn('btnFilterMode');
  if (fltObj.varTypeFilterSet()) filterBtnSelected('btnFilterVarType');
  else filterBtn('btnFilterVarType'); 
  if (fltObj.filTime !== "") filterBtnSelected('btnFilterTime'); 
  else filterBtn('btnFilterTime');
  if (fltObj.filDate !== "") filterBtnSelected('btnFilterDate'); 
  else filterBtn('btnFilterDate');
  if (fltObj.bColLeft) filterBtnSelected('btnFilterColPos'); 
  else filterBtn('btnFilterColPos');
  if (fltObj.bColCenter) filterBtnSelected('btnFilterColPos'); 
  else filterBtn('btnFilterColPos');
  if (fltObj.bNameLeft) filterBtnSelected('btnFilterNamePos'); 
  else filterBtn('btnFilterNamePos');
  if (fltObj.bNameTop) filterBtnSelected('btnFilterNamePos'); 
  else filterBtn('btnFilterNamePos');
  if (fltObj.filChnLink !== "") filterBtnSelected('btnFilterChn'); 
  else filterBtn('btnFilterChn');
  
};
