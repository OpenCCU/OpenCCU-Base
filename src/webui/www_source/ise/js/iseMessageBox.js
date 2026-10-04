/**
 * ise/iseMessageBox.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/
iseMessageBox = Class.create();

iseMessageBox.prototype =
{
  /* id:        Steuert welches Popup dargestellt wird
   * type:      "Extra-parameter" für einige Popups
   * secondary: Popup wird über einem anderen Popup eingeblendet
   * showAll:   All Kanäle auch virtuelle sollen angezeigt werden
   * popUpTitle:Titel der auf dem popUp angezeigt werden soll
   */
  initialize: function(id, type, secondary)
  {
    iseRefr(false);
    this.id = id;
    this.setTopMargin(6);
    this.setLeftMargin(6);
    this.type = type;
    this.operations = iufVisible|iufReadyState|iufOperated|iufReadable|iufWriteable|iufEventable;
    this.bMultiChooser = true;
    this.title = "";
    this.pb = "";
    this.file = "";
    this.draggable = true;

    if(secondary)
    {
      this.secondary = true;
      this.oldMsgHtml = $("messagebox").innerHTML;
      this.oldMsgWidth = $("messagebox").getWidth();
      //$("messagebox").style.width = this.width + "px"; 
    }
    else
    {
      this.secondary = false;
      if (PLATFORM == 'Central') {
        this.createMessagebox();
      }
    }
    
    this.initPopup(id);
    
    this.load();
  },
  initPopup: function(id)
  {
    switch(this.id)
    {
      case ID_ROOMS:
        this.setTitle( translateKey("dialogEditRoom") /*"Raum bearbeiten"*/ );
        this.setWidth75Perc();
        this.setHeight(700);
        this.setFile( "/pages/msg/popupPropEditor.htm" );
        break;
      case ID_FUNCTIONS: 
        this.setTitle( translateKey("dialogEditFunction") /*"Gewerk bearbeiten"*/ );
        this.setWidth75Perc();
        this.setHeight(700);
        this.setFile( "/pages/msg/popupPropEditor.htm" );
        break;
      case ID_TRANSFER_PARAMETERS:
        
        if (PLATFORM == 'Central') {
          this.setTitle( translateKey("dialogChooseTransmitMode") /*"Auswahl &Uuml;bertragungsmodus"*/ );
          this.addToPostBody( 'integer chnId = '+this.type+';' );
          this.setWidth(1000);
          this.setFile( "/pages/msg/popupTransEditor.htm" );
        } else {
          
        //ConfigTool
          ResetPostString();
          poststr += "&title=" + translateKey("dialogChooseTransmitMode") /*"Auswahl &Uuml;bertragungsmodus"*/;
          poststr += "&channel=" + this.type;
          //SendRequest('/popupTransEditor.cgi');
        }
        
        break;
      case ID_CHANNEL_LIST:
        this.setTitle( translateKey("dialogChannelOverView") /*"Kanal&uuml;bersicht"*/ );
        //this.stretchToMax();
        this.setHeight(700);
        this.setFile( "/pages/msg/channelList.htm" );
        break;
      case ID_NEW_SYSVAR:
        this.setTitle( translateKey("dialogCreateNewSysVar") /*"Systemvariable neu anlegen"*/ );
        this.addToPostBody( 'integer varid = 0;' );
        this.addToPostBody( 'integer createNew = 1;' );
        this.setWidth(900);
        this.setFile( "/pages/tabs/admin/msg/newSysVar.htm" );
        break;
      case ID_EDIT_SYSVAR:
        this.setTitle( translateKey("dialogEditSysVar") /*"Systemvariable bearbeiten"*/ );
        this.addToPostBody( 'integer varid = '+this.type+';' );
        this.addToPostBody( 'integer createNew = 0;' );
        this.setWidth(900);
        this.setFile( "/pages/tabs/admin/msg/newSysVar.htm" );
        break;
      case ID_CREATE_SCRIPT:
        this.setTitle( translateKey("dialogCreateScript") /*"Skript erstellen"*/ );
        this.addToPostBody( 'string iSDID = "'+this.type+'";' );
        this.setWidth(800);
        this.setFile( "/pages/msg/createScript.htm" );
        break;
      case ID_EDIT_SCRIPT:
        this.setTitle( translateKey("dialogEditScript") /*"Skript bearbeiten"*/ );
        this.addToPostBody( 'string sdid = "'+this.type+'";' );
        //this.setWidth(800);
        this.setWidth('auto');
        this.setFile( "/pages/msg/editScript.htm" );
        break;
      case ID_CONTROL_TEST:
        this.setWidth(800);
        this.setFile( "/pages/msg/controls.htm" );
        break;
      case ID_INSERT_VALUE:
        this.setTitle( translateKey("dialogEnterValue") /*"Wert eingeben"*/ );
        this.addToPostBody( 'string type = "'+this.type+'";' );
        this.setWidth(800);
        this.setFile( "/pages/msg/insertValue.htm" );
        break;
      case ID_INSERT_STRING:
        this.setTitle( translateKey("dialogEnterText") /*"Text eingeben" */);
        this.addToPostBody( 'integer type = '+this.type+';' );
        this.setWidth(800);
        this.setFile( "/pages/msg/insertValue.htm" );
        break;
      case ID_SET_VALUE_RANGE:
        this.setTitle( translateKey("dialogRangeOfValues") /*"Einstellung des Wertebereiches"*/ );
        this.addToPostBody( 'integer type = '+this.type+';' );
        this.setWidth(500);
        this.setFile( "/pages/msg/setValueRange.htm" );
        break;
      case ID_DEL_SYS_VARIABLE:
        this.setTitle( translateKey("dialogDeleteSysVar") /*"Systemvariable l&ouml;schen" */);
        this.setWidth(800);
        this.setFile( "/pages/msg/delSysVariable.htm" );
        break;
      case ID_SYS_VARS_SELECTION:
        this.setTitle( translateKey("dialogProgChoseSysVar") );
        this.addToPostBody( 'integer iShowAll = ' + this.type + ';' );
        this.addToPostBody( 'integer iSecondary = '+(this.secondary?"1":"0")+';' );
        this.setWidth(1000);
        this.setFile( "/pages/msg/sysVarsSelection.htm" );
        this.draggable = false; // Scroll bar not working otherwise
        break;
      case ID_PROGRAM_CHOOSER:
        this.setTitle( translateKey("dialogCreateFavChooseProg") /*"Favoritenerstellung - Programmauswahl" */);
        this.addToPostBody( 'integer iSecondary = '+(this.secondary?"1":"0")+';' );
        this.setWidth(1000);
        this.setFile( "/pages/msg/programChooser.htm" );
        break;
      case ID_TIMEMODULE:
        this.setTitle( translateKey("dialogSetTimeModul") /*"Einstellung des Zeitmoduls"*/ );
        this.addToPostBody( 'integer tmId = '+this.type+';' );
        this.setWidth(700);
        this.setFile( "/pages/tabs/admin/msg/timemodule.htm" );
        break;
      case ID_STATUSINFO:
        this.setTitle( translateKey("dialogStateInfoDataTransferToDeviceSuccess") /*"Statusinfo: Daten&uuml;bertragung zum Ger&auml;t erfolgreich abgeschlossen!" */);
        this.setWidth(800);
        this.setFile( "/pages/msg/statusinfo.htm" );
        break;
      case ID_STATUSINFO_WARNING:
        this.setTitle( translateKey("dialogWarningDataTransferToDeviceNotPossible") /*"Warnung: &Uuml;bertragung der Daten zum Ger&auml;t nicht m&ouml;glich!" */);
        this.setWidth(800);
        this.setFile( "/pages/msg/statusinfoWarning.htm" );
        break;
      case ID_USER_ACCOUNT_CONFIG_ADMIN:
        this.setTitle(translateKey("dialogUserAccountTitle")/*"Benutzerkonto - Konfiguration"*/);
        this.addToPostBody( 'integer userid = '+this.type+';' );
        this.setWidth(800);
        this.setFile( "/pages/msg/userAccountConfigAdmin.htm" );
        break;   
      case ID_USER_ACCOUNT_CONFIG_USER:
        this.setTitle(translateKey("dialogUserAccountTitle")/*"Benutzerkonto - Konfiguration" */);
        this.setWidth(1000);
        this.setFile( "/pages/msg/userAccountConfigUser.htm" );
        break;
      case ID_AUTO_LOGIN_CONFIG:
        this.setTitle( translateKey("dialogConfigAutomaticLogin") /*"Automatische Anmeldung - Konfiguration" */);
        this.setWidth(800);
        this.setFile( "/pages/msg/autoLoginConfig.htm" );
        break;
       case ID_CHOOSE_LED:
        this.setTitle( translateKey("dialogEnterValue") /*"Wert eingeben" */);
        this.addToPostBody( 'string type = "'+this.type+'";' );
        this.setWidth(800);
        this.setFile( "/pages/msg/chooseOULED16.htm" );
        break;       
        case ID_SET_OUCFM_MODE:
        this.setTitle( translateKey("dialogEnterValue") /*"Wert eingeben" */);
        this.addToPostBody( 'string type = "'+this.type+'";' );
        this.setWidth(400);
        this.setFile( "/pages/msg/setOUCFMMode.htm" );
        break;
        case ID_SET_STATUS_DISPLAY:
        this.setTitle( translateKey("dialogEnterValue") /*"Wert eingeben" */);
        this.addToPostBody( 'string type = "'+this.type+'";' );
        this.setWidth(400);
        this.setFile( "/pages/msg/setStatusDisplay.htm" );
        break;
      //ConfigTool
      case ID_BIDCOS_INTERFACE:
        ResetPostString();
        poststr += "&title=" + translateKey("dialogChooseBidCosInterface") /*"Auswahl BidCoS-Interface"*/;
        poststr += "&channel=" + this.type;
        //SendRequest('/popupInterfaceEditor.cgi');
        break;
      
      default:
        throw new Error("unknown id (" + this.id + ")");
        break;
    }

    if (this.draggable) {
      jQuery("#messagebox").draggable();
    }

  },
  buildPostBody: function()
  {
    this.pb += 'integer id='+this.id+';';
    this.pb += 'string title="'+this.title+'";';
    this.pb += 'string sOperations = "'+this.operations+'";';
    if( this.bMultiChooser )
    {
      this.pb += 'string MultiChooser = "'+this.type+'";';
    }
  },
  load: function()
  {
    this.buildPostBody();
    //alert("PB:"+this.pb);
    //alert("FN:"+this.file);
    this.LoadFromFile(this.file, this.pb);
  },
  setWidth: function(width)
  {
    this.width = width;
  },
  stretchToMax: function() {
    var maxHeight = 0;
    if (window.innerHeight) {
      maxHeight = window.innerHeight;
    }
    else {
      if (window.document.documentElement && window.document.documentElement.clientHeight) 
        maxHeight = window.document.documentElement.clientHeight;
      else
        maxHeight = window.document.body.offsetHeight;
      
      this.setHeight(maxHeight - 50);  
    }
    var maxWidth = screen.availWidth;
    this.setWidth(maxWidth - 50);
  },
  setWidth75Perc: function(perc)
  {
    var maxHeight = 0;
    if (window.innerHeight)
    {
      maxHeight = window.innerHeight;
    }
    else
    {
      if (window.document.documentElement && window.document.documentElement.clientHeight) 
        maxHeight = window.document.documentElement.clientHeight;
      else
        maxHeight = window.document.body.offsetHeight;
      
      // this.setHeight(maxHeight * 0.5);  
    }

    var maxWidth = document.body.offsetWidth;
    if(typeof(perc) == 'undefined')
    {
      this.setWidth(maxWidth * 0.75);
    }
    else
    {
      this.setWidth(maxWidth * perc);
    }
  },
  setHeight: function(height)
  {
    this.height = height;
  },
  setLeftMargin: function(left)
  {
    this.LeftMargin = left;
  },
  setTopMargin: function(top)
  {
    this.TopMargin = top;
  },  
  createMessagebox: function()
  {
    if ( !$('messagebox') )
    {      
      var newDiv = document.createElement("div");
      var newDivId = document.createAttribute("id");
      newDivId.nodeValue = "messagebox";
      newDiv.setAttributeNode(newDivId);
      var newDivStyle = document.createAttribute("style");
      
      if ((this.baseId != ID_CHANNEL_LIST))
      {
        newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;left: 50%;margin-left: -"+(this.width/2)+"px;margin-top: -"+(this.height/2)+"px;";
      }
      else
      {
        newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;";
      }
      
      if( typeof( newDiv.style.cssText ) == "string" )
        newDiv.style.cssText = newDivStyle.nodeValue;
      else
        newDiv.setAttributeNode(newDivStyle);
      
      $('centerbox').appendChild(newDiv);

    }
  },
  removeMessagebox: function()
  {
    if ( $('messagebox') )
    {
      $('messagebox').remove();
    }
  },
  readaptSize: function()
  {
    if ( $('messagebox') )
    {
      if ((this.baseId != ID_CHANNEL_LIST))
      {
        var h = $("messagebox").getHeight();
        $('messagebox').style.marginLeft = ($('messagebox').style.width.replace(/px$/,"")/2)*(-1)+"px";
        $('messagebox').style.marginTop = (h/2)*(-1)+"px";
      }
    }
  },
  show: function()
  {
    this.readaptSize();
    $('trlayer').show();
    $('centerbox').show();
  },
  hide: function(o)
  {
    $('centerbox').hide();
    this.remove();
    $('trlayer').hide();
    //this.peKeepAlive.stop();
    iseRefr(true);
  },
  remove: function()
  {
    this.removeMessagebox();
  },
  LoadFromFile: function(fn,pb) {
    var url = fn;
    if(url.indexOf('?sid=') === -1)
    {                                                                                 
      url = url+'?sid='+SessionId;                                                  
    }                                                                 
    var t = this;
    var opt = 
    {
      method: 'post',
      evalScripts: true,
      postBody: ReGa.encode(pb), 
      onComplete: function(trans)
      {
        //$("messagebox").style.width = t.width + "px";
        jQuery("#messagebox").width(t.width + "px");
        jQuery("#tableContainer").css("max-height",(parseInt(jQuery(window).height() * 0.75)) + "px");
        centerMessageBox();
        iseRefr(true);
        translatePage("#messagebox");
      }
    };
    new Ajax.Updater('messagebox',url,opt);
  },  
  ShowPopup: function() {
    this.show();
  },
  
  restorePrevious: function() {
    $("messagebox").innerHTML = this.oldMsgHtml;
    $("messagebox").setStyle({width: this.oldMsgWidth + "px"});
  },
  
  keepAlive: function() {
    var url = "/esp/system.htm?sid="+SessionId+"&action=keepAlive";
    new Ajax.Updater("divTitle", url, {});
  },
  
  addToPostBody: function(pb)
  {
    this.pb += pb;
  },

  setOperations: function(iOperations)
  {
    this.operations = iOperations;
  },

  setTitle: function(sTitle)
  {
    this.title = sTitle;
  },
  
  setFile: function(sFile)
  {
    this.file = sFile;
  }
};
