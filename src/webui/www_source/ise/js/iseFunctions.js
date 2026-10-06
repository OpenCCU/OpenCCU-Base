/**
 * ise/iseFunctions.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @namespace
 **/
ise = {};

// ######################
// ### ISE: FAVORITES ###
// ######################

/**
 * @class
 **/
ise.Favorites = Class.create();

ise.Favorites.prototype = 
{
  initialize: function()
  {
  },
  AddToStatus: function(id,cid)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddToStatus";';
    pb += 'string id = "'+id+'";';
    pb += 'string cid = "'+cid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  RemoveFromStatus: function(id,cid)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "RemoveFromStatus";';
    pb += 'string id = "'+id+'";';
    pb += 'string cid = "'+cid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  AddToList: function(flid,id)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddToList";';
    pb += 'string flid = "'+flid+'";';
    pb += 'string id = "'+id+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  AddSeparator: function(flid)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSeparator";';
    pb += 'string flid = "'+flid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },  
  RemoveFromList: function(flid,id)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "RemoveFromList";';
    pb += 'string flid = "'+flid+'";';
    pb += 'string id = "'+id+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  AddList: function(fname)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddList";';
    pb += 'string fname = "'+fname+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  RemoveList: function(flid)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "RemoveList";';
    pb += 'string flid = "'+flid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        WebUI.enter(FavoriteListPage);
      }
    };
    new Ajax.Request(url, opts);
  },
  CopyToNew: function(flid,bIsPC,bIsPDA,bIsCENTRAL,sargs)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "CopyToNew";';
    pb += 'string flid = "'+flid+'";';
    pb += 'string sargs = "'+sargs+'";';
    pb += 'string bIsPC = "'+bIsPC+'";';
    pb += 'string bIsPDA = "'+bIsPDA+'";';
    pb += 'string bIsCENTRAL = "'+bIsCENTRAL+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert("CopyToNew:"+t.responseText);}
        WebUI.enter(FavoriteListPage);
      }
    };
    new Ajax.Request(url, opts);
  },
  MovePosition: function(flid,id,dir)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "MovePosition";';
    pb += 'string flid = "'+flid+'";';
    pb += 'string id = "'+id+'";';
    pb += 'string dir = "'+dir+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  FavColumnCount: function(id,count)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "FavColumnCount";';
    pb += 'string id = "'+id+'";';
    pb += 'string count = "'+count+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  FavColumnAlign: function(id,align)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "FavColumnAlign";';
    pb += 'string id = "'+id+'";';
    pb += 'string align = "'+align+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  FavNamePosition: function(id,pos)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "FavNamePosition";';
    pb += 'string id = "'+id+'";';
    pb += 'string pos = "'+pos+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  SaveFavoriteList: function(flid,bIsPC,bIsPDA,bIsCENTRAL,sArgs)
  {
    var url = "/esp/favorites.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SaveFavoriteList";';
    pb += 'string flid = "'+flid+'";';
    pb += 'string sargs = "'+sArgs+'";';
    if( bIsPC )
    {
      pb += 'string bIsPC = "true";';
    }
    else
    {
      pb += 'string bIsPC = "false";';
    }
    if(dbg)alert(pb);
    if( bIsPDA ) { pb += 'string bIsPDA = "true";'; } else { pb += 'string bIsPDA = "false";'; }
    if( bIsCENTRAL ) { pb += 'string bIsCENTRAL = "true";'; } else { pb += 'string bIsCENTRAL = "false";'; }
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        if(dbg){alert(t.responseText);}
        WebUI.enter(FavoriteListPage);
      }
    };
    new Ajax.Request(url, opts);
  }

};

iseFavorites = new ise.Favorites();

// ##################
// ### ISE: ROOMS ###
// ##################

/**
 * @class
 **/
ise.Rooms = Class.create();

ise.Rooms.prototype =  {
  initialize: function() {
  },
  
  // ### CREATE ROOM ###
  // [name]: room object name
  CreateRoomRetId: function(name, baseId) {
    name = name.replace(/[\r\n\t]/g, "");
    // alert("iseRooms.CreateRoomRetId: " + name + "(" + name.length + ")");
    var url = '/esp/rooms.htm?sid='+SessionId;
    var pb = "";
    pb += 'string action = "createRoomRetId";';
    pb += 'string name = "'+name+'";';
    var opt = 
    {
      postBody: ReGa.encode(pb),
      onComplete: function(transport) {
        buildPropTable(baseId);

        var id = transport.responseText;
        RoomList.beginUpdate(id);
      }
    };
    new Ajax.Request(url,opt);
  },
  
  // ### DELETE ROOM ###
  // [id]: room object id
  DeleteRoom: function(id) {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "deleteRoom";';
    pb += 'integer id = '+id+';';
    var opt = {
      postBody: ReGa.encode(pb),
      
      onSuccess: function(t) {
      }
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE ROOM NAME ###
  // [id]: room object id
  // [newName]: new name of room object
  ChangeRoomName: function(id, newName) {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeRoomName";';
    pb += 'integer id = '+id+';';
    pb += 'string newname = "'+newName+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE ROOM COLOR###
  // [id]: room object id
  // [newColor]: new color of room object
  ChangeRoomColor: function(id, newColor) {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeRoomColor";';
    pb += 'integer id = '+id+';';
    pb += 'string newcolor = "'+newColor+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE ROOM COMMENT###
  // [id]: room object id
  // [newColor]: new color of room object
  ChangeRoomComment: function(id, newComment) {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeRoomComment";';
    pb += 'integer id = '+id+';';
    pb += 'string newcomment = "'+newComment+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE ROOM SYMBOL###
  // [id]: room object id
  // [newSymbol]: new symbol of room object
  ChangeFunctionSymbol: function(id, newSymbol) {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeRoomSymbol";';
    pb += 'integer id = '+id+';';
    pb += 'string newsymbol = "'+newSymbol+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  buildRoomOverlay: function() {
    var url = "/esp/rooms.htm?sid="+SessionId;
    var pb = "string action = 'buildRoomOverlay';";
    var opts = {
      postBody: ReGa.encode(pb),
      onComplete: function() {
        translatePage('#roomOverlay');
      }
    };
    //new Ajax.Updater("roomOverlay", url, opts);
    new Ajax.Updater("roomOverlay", url, opts);
  }
};

iseRooms = new ise.Rooms();

// ######################
// ### ISE: FUNCTIONS ###
// ######################

/**
 * @class
 **/
ise.Functions = Class.create();

ise.Functions.prototype = {
  initialize: function() {
  },
  
  // ### CREATE FUNCTION ###
  // [name]: function object name
  CreateFunctionRetId: function(name, baseId) {
    var url = '/esp/functions.htm?sid='+SessionId;
    var pb = "";
    pb += 'string action = "createFunctionRetId";';
    pb += 'string name = "'+name+'";';
    var opt =  {
      postBody: ReGa.encode(pb),
      onComplete: function(transport) {
        buildPropTable(baseId);
        
        var id = transport.responseText;
        SubsectionList.beginUpdate(id);
      }
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE FUNCTION NAME ###
  // [id]: function object id
  // [newName]: new name of function object
  ChangeFunctionName: function(id, newName) {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeFunctionName";';
    pb += 'integer id = '+id+';';
    pb += 'string newname = "'+newName+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### DELETE FUNCTION ###
  // [id]: function object id
  DeleteFunction: function(id) {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "deleteFunction";';
    pb += 'integer id = '+id+';';
    var opt = {
      postBody: ReGa.encode(pb),
      
      onSuccess: function(t) {
      }
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE FUNCTION COLOR###
  // [id]: function object id
  // [newColor]: new color of function object
  ChangeFunctionColor: function(id, newColor) {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeFunctionColor";';
    pb += 'integer id = '+id+';';
    pb += 'string newcolor = "'+newColor+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE FUNCTION COMMENT###
  // [id]: function object id
  // [newComment]: new comment of function object
  ChangeFunctionComment: function(id, newComment) {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeFunctionComment";';
    pb += 'integer id = '+id+';';
    pb += 'string newcomment = "'+newComment+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  // ### CHANGE FUNCTION SYMBOL###
  // [id]: function object id
  // [newSymbol]: new symbol of function object
  ChangeFunctionSymbol: function(id, newSymbol) {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "";
    pb += 'string action = "changeFunctionSymbol";';
    pb += 'integer id = '+id+';';
    pb += 'string newsymbol = "'+newSymbol+'";';
    var opt = {
      postBody: ReGa.encode(pb)
    };
    new Ajax.Request(url,opt);
  },
  
  buildFuncOverlay: function() {
    var url = "/esp/functions.htm?sid="+SessionId;
    var pb = "string action = 'buildFuncOverlay';";
    var opts = {
      postBody: ReGa.encode(pb),
      onComplete: function() {
        translatePage('#funcOverlay');
      }
    };
    new Ajax.Updater("funcOverlay", url, opts);
  }
};

iseFunctions = new ise.Functions();

// #####################
// ### ISE: CHANNELS ###
// #####################

/**
 * @class
 **/
ise.Channels = Class.create();

ise.Channels.prototype = {
  initialize: function() {
  },
  
  // ### SetReadyConfig
  // [ctrlId]: id of checkbox
  // [id]: id of channel 
  setReadyConfig: function(ctrlId, id) {
    var url   = "/esp/channels.htm?sid="+SessionId;
    var state = ($(ctrlId)._isReady) ? 0 : 1;
    
    var pb = "string chnId = " + id + ";";
    pb    += "string action= 'setReadyConfig';";
    pb    += "string state = " + state + ";";
    
    if(dbg){alert($(ctrlId).checked);}
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb),evalScripts:true,onComplete:function(t){if(dbg){alert(t.responseText);}}});
  },
  
  setHandling: function(id, ctrlId) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = " + id + ";";
    pb += "string action= 'setHandling';";
    pb += "integer iHandling = "+($(ctrlId).checked? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  },
  
  setVisible: function(id, ctrlId)
  {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = " + id + ";";
    pb += "string action= 'setVisible';";
    pb += "integer iVis = "+($(ctrlId).checked? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  },
  
  setProto: function(id, ctrlId) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = " + id + ";";
    pb += "string action= 'setProto';";
    pb += "integer iProto = "+($(ctrlId).checked? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  },
  
  setTransMode: function(id, bAES) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = " + id + ";";
    pb += "string action= 'setTransMode';";
    pb += "integer iAES = "+(bAES? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts: true});
  },
  
  chnToRoom: function(idChn, idRoom, bAdd) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = "+  idChn + ";";
    pb += "integer roomId = "+  idRoom + ";";
    pb += "string action= 'chnToRoom';";
    pb += "integer iAdd = "+  (bAdd ? 1 : 0) + ";";
    new Ajax.Updater('dummy', url, {postBody: ReGa.encode(pb), evalScripts: true});
    
    var room = RoomList.get(idRoom);
    if (room)
    {
      if (bAdd) { room.addChannel(idChn); }
      else      { room.removeChannel(idChn); }
    }
  },
  
  delChnFromAllRooms: function(chnId,bCTV) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "";
    pb += "string chnId = '"+  chnId + "';";
    pb += "string action= 'delChnFromAllRooms';";
    if( bCTV )
    {
      pb += 'system.SetSessionVar("sessionCTV", "true");';
    }
    else
    {
      pb += 'system.SetSessionVar("sessionCTV", "false");';
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(t)
      {
        reloadSortedPage();
      }
    };
    new Ajax.Request(url, opts);
  },

  delChnFromAllFunctions: function(chnId,bCTV) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "";
    pb += "string chnId = '"+  chnId + "';";
    pb += "string action= 'delChnFromAllFunctions';";
    if( bCTV )
    {
      pb += 'system.SetSessionVar("sessionCTV", "true");';
    }
    else
    {
      pb += 'system.SetSessionVar("sessionCTV", "false");';
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(t)
      {
        reloadSortedPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  
  chnToFunc: function(idChn, idFunc, bAdd) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = "+  idChn + ";";
    pb += "integer funcId = "+  idFunc + ";";
    pb += "string action= 'chnToFunc';";
    pb += "integer iAdd = "+  (bAdd ? 1 : 0) + ";";
    new Ajax.Updater('dummy', url, {postBody: ReGa.encode(pb), evalScripts: true});

    var subsection = SubsectionList.get(idFunc);
    if (subsection)
    {
      if (bAdd) { subsection.addChannel(idChn); }
      else      { subsection.removeChannel(idChn); }
    }    
  },
  
  showOverlay: function(idChn, baseId) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer chnId = "+  idChn + ";";
    pb += "integer baseId = "+  baseId + ";";
    pb += "string action= 'showOverlay';";
    new Ajax.Updater('dummy', url, {postBody: ReGa.encode(pb), evalScripts: true});
  },
  
  // sChns: tab-separiert wg. ESP foreach
  addChnsToID: function(sChns, destId, clearAll, onComplete) {
    var _onComplete_ = onComplete;
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer destId = "+  destId + ";";
    pb += "string arChn = '"+  sChns + "';";
    pb += "string action= 'addChnsToID';";
    pb += 'system.SetSessionVar("sessionLS","");';
    if(clearAll)
    {
      pb += "boolean clearAll = "+clearAll+";";
    }
    else
    {
      pb += "boolean clearAll = false;";
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(t)
      {
        reloadSortedPage();
        if (_onComplete_) { _onComplete_(); }
      }
    };
    new Ajax.Updater("dummy", url, opts );
  },
  
  Test: function(ids)
  {
    var url = '/esp/channels.htm?sid='+SessionId;
    var pb = '';
    pb += 'string action = "Test";';
    pb += 'string ids = "'+ids+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts:true,
      onComplete:function(t)
      {
        if(dbg)alert(t.responseText);
      }
    };
    new Ajax.Request( url, opts );
  },
  
  delChnFromID: function(chnId, destId,bCTV) {
    var url = "/esp/channels.htm?sid="+SessionId;
    var pb = "integer destId = "+  destId + ";";
    pb += "string chnId = '"+  chnId + "';";
    pb += "string action= 'delChnFromID';";
    if( bCTV )
    {
      pb += 'system.SetSessionVar("sessionCTV", "true");';
    }
    else
    {
      pb += 'system.SetSessionVar("sessionCTV", "false");';
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      evalScripts: true,
      onComplete: function(t)
      {
        reloadSortedPage();
      }
    };
    new Ajax.Request(url, opts);
  },
  
  saveDisplayValues: function(chnId)
  {
    var id = chnId;
    conInfo( "iseChannels.saveDisplayValues("+id+");" );
    var url = "/esp/channels.htm?sid="+SessionId;
    var sEnum = "TEXT\tBEEP\tUNIT\tBACKLIGHT";
    var pb = "";
    pb += 'string action = "saveDisplayValues";';
    pb += 'string chnId = "'+id+'";';
    pb += 'string varTEXT = "'+$("txt"+id).value+'";';
    pb += 'string varUNIT = "'+$("selUnit"+id).selectedIndex+'";';
    pb += 'string varBACKLIGHT = "'+$("selLight"+id).selectedIndex+'";';
    pb += 'string varBEEP = "'+$("selBeep"+id).selectedIndex+'";';
    if( $("cBulb"+id).checked ) { sEnum += "\tBULB"; }
    if( $("cSwitch"+id).checked ) { sEnum += "\tSWITCH"; }
    if( $("cWnd"+id).checked ) { sEnum += "\tWINDOW"; }
    if( $("cDoor"+id).checked ) { sEnum += "\tDOOR"; }
    if( $("cBlind"+id).checked ) { sEnum += "\tBLIND"; }
    if( $("cScene"+id).checked ) { sEnum += "\tSCENE"; }
    if( $("cPhone"+id).checked ) { sEnum += "\tPHONE"; }
    if( $("cBell"+id).checked ) { sEnum += "\tBELL"; }
    if( $("cArrUp"+id).checked ) { sEnum += "\tARROW_UP"; }
    if( $("cArrDown"+id).checked ) { sEnum += "\tARROW_DOWN"; }
    if( $("cClock"+id).checked ) { sEnum += "\tCLOCK"; }
    sEnum += "\tSUBMIT";
    pb += 'string StateEnum = "'+sEnum+'";';
    conInfo( "StateEnum=["+sEnum+"]" );
    new Ajax.Request(url, {postBody: ReGa.encode(pb),onSuccess:function(t){conInfo(t.responseText);}});
  }
};

iseChannels = new ise.Channels();

// #####################
// ### ISE: DEVICES  ###
// #####################

/**
 * @class
 **/
ise.Devices = Class.create();

ise.Devices.prototype = {

  //deviceOperateGroupOnly: false,

  initialize: function() {
    //this.deviceOperateGroupOnly = false;
  },
  setOperateGroupOnly: function(mode) {
    //this.deviceOperateGroupOnly = mode;
  },

  // ### SetReadyConfig
  // [id]: id of device 
  setReadyConfig: function(id) {
    // dazugehörige Tabellenreihe ausblenden
    //$("tr" + id).hide();
    var url = "/esp/devices.htm?sid="+SessionId;
    var pb = "integer devId = " + id + ";";
    pb += "string action= 'setReadyConfig';";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts: true, onComplete: function(t){if(dbg){alert(t.responseText);}}});
    
    // Gerät in Geräteliste übernehmen
    //DeviceList.beginUpdateDevice(id);

     // Gerät in Geräteliste übernehmen
    DeviceList.beginUpdateDevice(id, function() {
     //DeviceList.devices[id].isOperateGroupOnly = this.deviceOperateGroupOnly;
      if (DeviceList.devices[id]) {
        DeviceList.devices[id].isOperateGroupOnly = false;
        DeviceList.devices[id].inInbox = false;
      }

    });

  },
  
  setHandling: function(id, ctrlId) {
    var url = "/esp/devices.htm?sid="+SessionId;
    var pb = "integer devId = " + id + ";";
    pb += "string action= 'setHandling';";
    pb += "integer iHandling = "+($(ctrlId).checked? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  },
  
  setVisible: function(id, ctrlId, mode) {
    var url = "/esp/devices.htm?sid="+SessionId;
    var pb = "integer devId = " + ((mode) ? DeviceList.getChannel(id).deviceId : id) + ";";
    pb += "string action= 'setVisible';";

    if (mode) {
      homematic("Device.setVisibility", {"id": DeviceList.getChannel(id).deviceId, "isVisible": true});
      pb += "integer iVis = " + 1 + ";";
    } else {
      pb += "integer iVis = " + ($(ctrlId).checked? 1: 0) + ";";
    }
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  },
  
  setProto: function(id, ctrlId)
  {
    var url = "/esp/devices.htm?sid="+SessionId;
    var pb = "integer devId = " + id + ";";
    pb += "string action= 'setProto';";
    pb += "integer iProto = "+($(ctrlId).checked? 1: 0)+";";
    new Ajax.Updater("dummy", url, {postBody: ReGa.encode(pb), evalScripts:true});
  }
};

iseDevices = new ise.Devices();




// #####################
// ### ISE: SYSTEM   ###
// #####################

/**
 * @class
 **/
ise.System = Class.create();

ise.System.prototype = {
  initialize: function() {
  },
  
  checkName: function(name,divid)
  {
    var url = "/esp/exec.htm?sid="+SessionId;
    var pb = '';
    pb += 'string sUniqueName;';
    pb += 'dom.CheckName("'+name+'", &sUniqueName, ID_SYSTEM_VARIABLES );';
    pb += 'Write( sUniqueName );';
    var opts = { postBody: ReGa.encode(pb) };
    new Ajax.Updater(divid,url,opts);
  },
  
  saveName: function(id, name, ctrlId, callStrFunc) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "saveName";';
    pb += 'string id = "'+id+'";';
    pb += 'string name = "'+name+'";';
    if(dbg){alert(pb);}
    var opts = {
      postBody: ReGa.encode(pb), 
      onComplete: function(resp) {
        if ($(ctrlId)) {
           // alert("iseSystem.saveName: " + resp.responseText + "(" + resp.responseText.length + ")");
          if (callStrFunc) {
            if (callStrFunc == CALL_STRCUT) 
              $(ctrlId).innerHTML = strCut(resp.responseText, true, true);
            if (callStrFunc == CALL_SPACECUT) 
              $(ctrlId).innerHTML = spaceCutHtmlIf(resp.responseText, 1);
          }
          else
            $(ctrlId).innerHTML = resp.responseText;
        }
      } 
    };
    new Ajax.Request(url, opts);  
  },
  
  saveDesc: function(id, desc) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "integer objId = " + id + ";";
    pb += "string desc   = '"+desc+"';";
    pb += "string action = 'saveDesc';";    
    if(dbg){alert(pb);}
    new Ajax.Request(url, {postBody: ReGa.encode(pb),onComplete:function(t){if(dbg){alert(t.responseText);}}});  
  },
  
  saveDpProto: function(id, ctrlId) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'saveDpProto';";
    pb += "integer dpId = " + id + ";";
    pb += "integer iProto   = '"+($(ctrlId).checked ? 1 : 0)+"';";
    new Ajax.Request(url, {postBody: ReGa.encode(pb)});  
  },
  
  saveDpVisibility: function(id, ctrlId) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'saveDpVisibility';";
    pb += "integer dpId = " + id + ";";
    pb += "integer iVisible   = '"+($(ctrlId).checked ? 1 : 0)+"';";
    new Ajax.Request(url, {postBody: ReGa.encode(pb)});  
  },

  saveDpInternal: function(id, ctrlId) {                              
    var url = "/esp/system.htm?sid="+SessionId;                       
    var pb = "string action = 'saveDpInternal';";                     
    pb += "integer dpId = " + id + ";";             
    pb += "integer iInternal   = '"+($(ctrlId).checked ? 1 : 0)+"';";
    new Ajax.Request(url, {postBody: ReGa.encode(pb)});
  },

  ClearHistoryData: function()
  {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "ClearHistoryData";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  }
};

iseSystem = new ise.System();

// #######################
// ### ISE: PROGRAMS   ###
// #######################

/**
 * @class
 **/
ise.Programs = Class.create();

ise.Programs.prototype = {
  initialize: function() {
  },

  SetBreakOnRestart: function( rid, id, value )
  {
    
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetBreakOnRestart";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);    
    if(dbg)alert(pb);
  },

  SetActive: function( id, value )
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetActive";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },

  SetVisible: function( id, value )
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetVisible";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },

  SetOperate: function( id, value )
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetOperate";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },
  
  ProgramUpdate: function( id )
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "ProgramUpdate";';
    pb += 'string id = "'+id+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },

  SetVisibility: function( id, value )
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetVisibility";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },

  // ### CONDITIONS ###
  AddCondition: function(rid) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddCondition";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  DeleteCondition: function(rid,cid) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "DeleteCondition";';
    pb += 'string rid = "'+rid+'";';
    pb += 'string cid = "'+cid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  // ### SINGLE CONDITIONS ###
  AddSingleCondition: function(rid, cid) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSingleCondition";';
    pb += 'string cid = "'+cid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule( rid );
      }
    };
    new Ajax.Request(url,opts);
  },
  DeleteSingleCondition: function(rid,cid,sid) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "DeleteSingleCondition";';
    pb += 'string cid = "'+cid+'";';
    pb += 'string sid = "'+sid+'";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  // ### SINGLE DESTINATIONS ###
  AddSingleDestination: function(rid,did) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSingleDestination";';
    pb += 'string did = "'+did+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  DeleteSingleDestination: function(rid,did,sid) {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "DeleteSingleDestination";';
    pb += 'string did = "'+did+'";';
    pb += 'string sid = "'+sid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  // #### SUB RULES ####
  AddSubRule: function(rid)
  {
    if(dbg){alert("Call AddSubRule");}
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSubRule";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);
  },
  AddSubRuleDest: function(rid)
  {
    if(dbg){alert("Call AddSubRuleDest");}
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSubRuleDest";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        reloadPage();
        //ReloadRule( rid );
      }
    };
    new Ajax.Request(url,opts);
  },
  AddSubRuleBreakOnRestart: function(rid)
  {
    if(dbg){alert("Call AddSubRuleBreakOnRestart");}
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddSubRuleBOR";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);
  },
  AddNewElseRule: function(rid)
  {
    if(dbg){alert("Call AddNewElseRule");}
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "AddNewElseRule";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //ReloadRule(rid);
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);
  },
  DeleteSubRule: function(rid)
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "DeleteSubRule";';
    pb += 'string rid = "'+rid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //ReloadRule(rid);
        //ReloadProgram(pid);
        reloadPage();
      }
    };
    new Ajax.Request(url,opts);
  },
  OperatorType: function(rid,id,value)
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetOperatorType";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  Delete: function(id)
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "DeleteProgram";';
    pb += 'string id = "'+id+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        WebUI.enter(ProgramListPage);
      }
    };
    new Ajax.Request(url,opts);
  },
  Restore: function(orig,copy)
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "RestoreProgram";';
    pb += 'string orig = "'+orig+'";';
    pb += 'string copy = "'+copy+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        WebUI.enter(ProgramListPage);
      }
    };
    new Ajax.Request(url,opts);
  },
  CopyToNewProgram: function(id)
  {
    var url = "/esp/programs.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "CopyToNewProgram";';
    pb += 'string id = "'+id+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        WebUI.enter(ProgramListPage);        
      }
    };
    new Ajax.Request(url,opts);
  }
};

isePrograms = new ise.Programs();

// ##############################
// ### ISE: SINGLE CONDITIONS ###
// ##############################

/**
 * @class
 **/
ise.SingleCondition = Class.create();

ise.SingleCondition.prototype =
{
  initialize: function()
  {
  }, 
  
  OperatorType: function(cid,id,value)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetOperatorType";';
    pb += 'string cid = "'+cid+'";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        conInfo( t.responseText );
        ReloadRule(rid);
      }
    };
    new Ajax.Request(url,opts);
  },
  
  SetValueRange: function(id,rv1,rv2,ct)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueRange";';
    pb += 'string id = "'+id+'";';
    pb += 'string rv1 = "'+rv1+'";';
    pb += 'string rv2 = "'+rv2+'";';
    pb += 'string ct = "'+ct+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleCondition(id);
      }
    };
    new Ajax.Request(url,opts);        
  },

  SetLeftValue: function(scid,lv)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetLeftValue";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string lv = "'+lv+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  },
  
  SetRightValue: function(scid,rv)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetRightValue";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string rv = "'+rv+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
      }
    };
    new Ajax.Request(url,opts);    
  },
  
  SetLeftAndRightValue: function(scid,lv,rv)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetLeftAndRightValue";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string rv = "'+rv+'";';
    pb += 'string lv = "'+lv+'";';    
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        conInfo( t.responseText );
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  },
  
  SetLeftValType: function(scid,lvt)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetLeftValType";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string lvt = "'+lvt+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetConditionType2: function(scid,ct)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetConditionType2";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string ct = "'+ct+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        conInfo(t.responseText);
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetChannel: function(scid,chid)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetChannel";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string chid = "'+chid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetTimeModule: function(scid,tmid)
  {
    var url = "/esp/sico.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetTimeModule";';
    pb += 'string scid = "'+scid+'";';
    pb += 'string tmid = "'+tmid+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleCondition(scid);
      }
    };
    new Ajax.Request(url,opts);    
  }
};

iseSingleCondition = new ise.SingleCondition();

// ################################
// ### ISE: SINGLE DESTINATION ###
// ################################

/**
 * @class
 **/
ise.SingleDestination = Class.create();

ise.SingleDestination.prototype =
{
  initialize: function()
  {
    this.reload = true;
  },  
  SetDP: function(id,value)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetDP";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        //reloadPage();
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);
  },
  SetChannel: function(id,value)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetChannel";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetParam: function(id,value)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetParam";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValue: function(id,value,unit, isSysVar)
  {
    isSysVar = (typeof isSysVar == "undefined") ? false : true;

		var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValue";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    pb += 'boolean isSysVar = "'+isSysVar+'";';
    if( typeof( unit ) != "undefined" )
    {
      pb += 'string unit = "'+unit+'";';
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        conInfo( t.responseText );
        if(iseSingleDestination.reload) { ReloadSingleDestination(id); }
        iseSingleDestination.reload = true;
      }
    };
    new Ajax.Request(url,opts);    
  },

  SetValueMinMax: function(id,value,unit,min,max)
  {
    var valElm = jQuery("#valSD_"+id);

    value = value.toString().replace(/,/g, ".");

    value = parseFloat(value);
    if (isNaN(value))
    {
      value = min;
    }

    if ((unit == "°C") && (value != min) && (value != max)) {
      value = roundValue05(value);
    }

		if (unit == "%") {min = min * 100; max = max * 100;}
		
		if (value < min) {value = min;}
		if (value > max) {value = max;}

    valElm.val(addTrailingZero(value));

		var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValue";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    if( typeof( unit ) != "undefined" )
    {
      pb += 'string unit = "'+unit+'";';
    }
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        conInfo( t.responseText );
        if(iseSingleDestination.reload) { ReloadSingleDestination(id); }
        iseSingleDestination.reload = true;
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValueType: function(id,value)
  {
		var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueType";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        if(iseSingleDestination.reload) reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValueAndType: function(id,value,type)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueAndType";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    pb += 'string type = "'+type+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        PopupClose();
        //if(iseSingleDestination.reload) reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValueAndTypeMinMax: function(id,value,type, min, max)
  {
   	 
		if ((value < min) || (isNaN(value))) {value = min;}
		if (value > max) {value = max;}
    
		var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueAndType";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    pb += 'string type = "'+type+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        PopupClose();
        //if(iseSingleDestination.reload) reloadPage();
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValueAndDP: function(id,value,dp, elem)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    var arValue = elem.value.split("|");

    if (arValue[1]) {
      var arParamName = arValue[1].split("."),
        paramName = arParamName[arParamName.length - 1];
      if (paramName == "PARTY_MODE_SUBMIT") {
        value = getDefaultPartyModeString();
      }
    }
    pb += 'string action = "SetValueAndDP";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    pb += 'string dp = "'+dp+'";';
    var opts = 
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);
  },
  SetValueParam: function(id,value)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueParam";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetValueParamType: function(id,value)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetValueParamType";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = "'+value+'";';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);    
  },
  SetScript: function(id,script)
  {
    var url = "/esp/side.htm?sid="+SessionId;
    var pb = '';
    pb += 'string action = "SetScript";';
    pb += 'string id = "'+id+'";';
    pb += 'string value = ^'+script.replace(/\^/g, '^#\'^\'#^')+'^;';
    var opts =
    {
      postBody: ReGa.encode(pb),
      onComplete:function(t)
      {
        if(dbg){alert( t.responseText );}
        ReloadSingleDestination(id);
      }
    };
    new Ajax.Request(url,opts);    
  }
};

iseSingleDestination = new ise.SingleDestination();

// ###################
// ### ISE: USER   ###
// ###################

/**
 * @class
 **/
ise.User = Class.create();

ise.User.prototype =
{
  initialize: function()
  {    
  },
  
  buildSysVarTable: function(uid) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'BuildUserSvTable';";
    pb += "integer userId = "+ uid +";";
    var opts = {
      postBody: ReGa.encode(pb),
      onComplete: function(response) {
        if (200 == response.status) {
          translatePage("#userSysVarTbl");
        }
      }
    };
    new Ajax.Updater("userSysVarTbl", url, opts);
  },
  
  addSysVarBuildTable: function(uid, newVarId) {
    //var url = "/esp/system.htm?sid="+SessionId+"&curDateTime="+new Date().getTime();
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'AddSysVarAndBuildTable';";
    pb += "integer userId = "+ uid +";";
    pb += "integer newSvId = " + newVarId + ";";
    var opts = {
      postBody: ReGa.encode(pb),
      onSuccess: function(t) {
        // funktioniert seltsamerweise nur durch den "Updater"-Aufruf nicht
        $("userSysVarTbl").innerHTML = t.responseText;
      },
      onComplete: function(response) {
        if (200 == response.status) {
          translatePage("#userSysVarTbl");
        }
      }
    };
    new Ajax.Updater("userSysVarTbl", url, opts);
  },
  
  DeleteSysVar: function(uid, svId) {
    var url = "/esp/system.htm?sid=" + SessionId;
    var pb = "string action = 'UserDeleteSysVarBuildTable';";
    pb += "integer userId = "+ uid +";";
    pb += "integer svId = " + svId + ";";
    var opts = {
      postBody: ReGa.encode(pb),
      onSuccess: function(t) {
        // funktioniert seltsamerweise nur durch den "Updater"-Aufruf nicht
        $("userSysVarTbl").innerHTML = t.responseText;
      },
      onComplete: function(response) {
        if (200 == response.status) {
          translatePage("#userSysVarTbl");
        }
      }
    };
    new Ajax.Updater("userSysVarTbl", url, opts);
  },
  
  buildTmpSysVarTable: function(arSysVars) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'BuildUserSvTable';";
    pb += "integer userId = 0;";
    
    var s = "";
    for(var i = 0; i < arSysVars.length; i++) {
      s += arSysVars[i] + "\t";
    }
    s = s.substr(0, s.length - 1);
    pb += "string sTmpVars = '"+s+"';";
    var opts = {
      postBody: ReGa.encode(pb),
      onSuccess: function(t) {
        // funktioniert seltsamerweise nur durch den "Updater"-Aufruf nicht
        $("userSysVarTbl").innerHTML = t.responseText;
      },
      onComplete: function(response) {
        if (200 == response.status) {
          translatePage("#userSysVarTbl");
        }
      }
    };
    new Ajax.Updater("userSysVarTbl", url, opts);
  },
  
  setAutoLogin: function(alPC, alPDA) {
    var url = "/esp/system.htm?sid="+SessionId;
    var pb = "string action = 'setAutoLogin';";
    pb += "integer alPC = " + alPC + ";";
    pb += "integer alPDA = " + alPDA + ";";
    
    var opts = {
      postBody: ReGa.encode(pb), 
      onComplete:function()
      {
        PopupClose();
        reloadPage();
      }
    };
    new Ajax.Request(url, opts);
  }
};

iseUser = new ise.User();
