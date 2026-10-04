/**
 * cp_functions.js
 **/

showAddDeviceCP = function(activate_install_mode)
{
  var path = "/config/";
  if (PLATFORM != "Central") {path = "/configapp/";}
  
  if(activate_install_mode)CreateCPPopup(path + "cp_add_device.cgi", "call_js=rf_install_mode(true);");
  else CreateCPPopup(path + "cp_add_device.cgi");
  dlgPopup.setWidth("925");
};

showDisplayCP = function()
 {
  CreateCPPopup("/config/cp_display.cgi");
};

showSecurityCP = function()
 {
  CreateCPPopup("/config/cp_security.cgi");
};

showNetworkSecurityCP = function()
 {
  CreateCPPopup("/config/cp_cert.cgi");
};

showNetworkCP = function()
 {
  CreateCPPopup("/config/cp_network.cgi");
};
showGeneralSettingsCP = function()
 {
  CreateCPPopup("/pages/jpages/system/StorageSettings/show");
};

showCouplingCP = function()
{
  CreateCPPopup("/pages/jpages/system/Coupling/show");
};

showTimeCP = function() {
  CreateCPPopup("/config/cp_time.cgi");
};

showSoftwareCP = function()
{
  CreateCPPopup("/config/cp_software.cgi");
};

showMaintenanceCP = function()
{
  CreateCPPopup("/config/cp_maintenance.cgi");
};

showNewFirmwareDownload = function() {
  CreateCPPopup("/config/cp_maintenance.cgi", "downloadOnly=1");
};

CreateCPPopup = function(src, pb) {
  dlgPopup = new cpMessageBox(src, pb);

  PopupClose = function() {
    dlgPopup.close();
    if (typeof addOnUninstall != "undefined") {
      reloadPage();
      delete addOnUninstall;
    }
  };
  
  PopupReload = function() {
    dlgPopup.reload();
    dlgPopup.ShowPopup();
  };
  
  dlgPopup.ShowPopup();
};

CreateCPPopup2 = function(src, pb) {
  dlgPopup2 = new cpMessageBox2(src, pb);
  
  Popup2Close = function() {
    dlgPopup2.close();
  };
  
  Popup2Reload = function() {
    dlgPopup2.reload();
    dlgPopup2.ShowPopup();
  };
  
  dlgPopup2.ShowPopup();
  //new Draggable("messagebox");
};

cp_adddev_updater=null;

cpMessageBox = Class.create();

cpMessageBox.prototype =
{
  initialize: function(src, pb)
  {
    if ( $('messagebox') ){
      $("messagebox").hide();
    }

    this.setTopMargin(6);
    this.setLeftMargin(6);
    
    this.setWidth(800);

    this.createMessagebox();
    this.LoadFromFile(src, pb);
  },

  getViewPortDim : function()
  {
    var win = jQuery(window);
    return {"height":win.height(), "width": win.width()};
  },

  setWidth: function(width)
  {
    this.width = width;
    if( $('messagebox') ){
        $('messagebox').style.width=this.width+"px";
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
      //newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;left: 50%;margin-left: -"+(this.width/2)+"px;margin-top: -"+(this.height/2)+"px;";
      newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;left: 50%;margin-left: -"+(this.width/2)+"px;margin-top: -"+"0px;";
      if( typeof( newDiv.style.cssText ) == "string" ) {
        newDiv.style.cssText = newDivStyle.nodeValue;
      } else {
        newDiv.setAttributeNode(newDivStyle);
      }
      $('centerbox').appendChild(newDiv);
      jQuery("#messagebox").draggable();
    }  else {
        $('messagebox').style.width=this.width+"px";
    }
    jQuery("#messagebox").addClass("j_translate");
  },
  
  removeMessagebox: function()
  {
    if ( $('messagebox') )
    {
      $('messagebox').remove();
    }
  },

   // TODO Es wird zur Zeit nur die Höhe ausgewertet und angepasst, die Breite ist noch nicht implementiert.
  /**
   * Adds a scrollbar and adjusts the position when the height of the message
   * box is > than those of the viewport
   * @param boxHeight
   */
  setCSS: function(boxHeight) {
    var centerBoxSel = jQuery("#centerbox"),
      messageBoxSel = jQuery("#messagebox"),
      oViewPortDim = this.getViewPortDim();

    if (parseInt(oViewPortDim.height) < boxHeight )
    {
      centerBoxSel.css({
        "overflow":"scroll",
        "height":"95%",
        "top":"-0px"
      });
      messageBoxSel.css("margin-top", "-0px");
    } else {
      centerBoxSel.css({"overflow":"", "top":"50%"});
    }
  },

  readaptSize: function()
  {
    var messageBox = $("messagebox");
    if ( messageBox )
    {
      var h = messageBox.getHeight();
      messageBox.style.marginLeft = (messageBox.style.width.replace(/px$/,"")/2)*(-1)+"px";
      messageBox.style.marginTop = (h/2)*(-1)+"px";
      this.setCSS(h);
    }
  },

  show: function()
  {
    this.readaptSize();
    $('trlayer').show();
    $('centerbox').show();
  },  
  
  close: function(o)
  {
    $('centerbox').hide();
    this.remove();
    $('trlayer').hide();
  },
  
  hide: function(o)
  {
    $('messagebox').style.display="none";
  },
  
  remove: function()
  {
    this.removeMessagebox();
  },
  
  LoadFromFile: function(src, pb) {
    var url = src;
    if(url.indexOf('?sid=') === -1)
    {                                                                                 
      url = url+'?sid='+SessionId;                                                  
    }                                                                 
    var t = this;
    var opt = 
    {
      method: 'post',
      evalScripts: true,
      postBody: pb,
      sendXML: false,
      onComplete: function(trans) {
        centerMessageBox();
        $('messagebox').style.display="";
      }
    };
    new Ajax.Updater('messagebox',url,opt);
  },
  
  ShowPopup: function() {
    this.show();
  }
};

cpMessageBox2 = Class.create();

cpMessageBox2.prototype =
{
  initialize: function(src, pb)
  {
    this.setTopMargin(6);
    this.setLeftMargin(6);
    
    this.setWidth(400); 
    
    this.createMessagebox();
    this.LoadFromFile(src, pb);
  },
  
  setWidth: function(width)
  {
    this.width = width;
    if( $('messagebox2') ){
        $('messagebox2').style.width=this.width+"px";
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
    if ( !$('messagebox2') )
    {      
      var newDiv = document.createElement("div");
      var newDivId = document.createAttribute("id");
      newDivId.nodeValue = "messagebox2";
      newDiv.setAttributeNode(newDivId);
      var newDivStyle = document.createAttribute("style");
      newDivStyle.nodeValue = "width:"+this.width+"px;height:"+this.height+"px;padding: 0px;position:absolute;left: 50%;margin-left: -"+(this.width/2)+"px;margin-top: -"+(this.height/2)+"px;";
      
      if( typeof( newDiv.style.cssText ) == "string" )
        newDiv.style.cssText = newDivStyle.nodeValue;
      else
        newDiv.setAttributeNode(newDivStyle);
      
      $('centerbox2').appendChild(newDiv);
      jQuery("#messagebox2").draggable();
    }  else {
        $('messagebox2').style.width=this.width+"px";
    }
  },
  
  removeMessagebox: function()
  {
    if ( $('messagebox2') )
    {
      $('messagebox2').remove();
    }
  },
  
  readaptSize: function()
  {
    if ( $('messagebox2') )
    {
      var h = $("messagebox2").getHeight();
      $('messagebox2').style.marginLeft = ($('messagebox2').style.width.replace(/px$/,"")/2)*(-1)+"px";
      $('messagebox2').style.marginTop = (h/2)*(-1)+"px";
    }
  },
  
  show: function()
  {
    this.readaptSize();
    $('trlayer2').show();
    $('centerbox2').show();
  },  
  
  close: function(o)
  {
    $('centerbox2').hide();
    this.remove();
    $('trlayer2').hide();
  },
  
  hide: function(o)
  {
    $('messagebox2').style.display="none";
  },
  
  remove: function()
  {
    this.removeMessagebox();
  },
  
  LoadFromFile: function(src, pb)
  {
    var url = src;
    if(url.indexOf('?sid=') === -1)
    {                                                                                 
      url = url+'?sid='+SessionId;                                                  
    }                                                                 
    var t = this;
    var opt = 
    {
      method: 'post',
      evalScripts: true,
      postBody: pb,
      sendXML: false,
      onComplete: function(trans) {
        centerMessageBox();
        $('messagebox2').style.display="";
      }
    };
    new Ajax.Updater('messagebox2',url,opt);
  },
  
  ShowPopup: function()
  {
    this.show();
  }
};
