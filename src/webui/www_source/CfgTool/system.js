// #####################
// ### Metadata  ###
// #####################

configMetadata = new function()
{
  this.save = function(id, name, ctrlId, callStrFunc) {
    var url = "/configapp/metadata.cgi";
    var pb = '';
    pb += 'cmd=set_metadata&';
    pb += 'id='+id+'&';
    pb += 'value='+encodeURIComponent(name)+'&';
    if(dbg){alert(pb);}
    var opts = {
      postBody: pb,
      sendXML: false,
      onComplete: function(resp) {
        if ($(ctrlId)) {
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
  };
  
  this.saveProfileParam = function(object_id, profile_id, param_id, value) {
    var url = "/configapp/metadata.cgi";
    var pb = '';
    pb += 'cmd=set_parameter&';
    pb += 'id='+object_id+';'+profile_id+';'+param_id+'&';
    pb += 'value='+encodeURIComponent(value)+'&';
    if(dbg){alert(pb);}
    var opts = {
      postBody: pb,
      sendXML: false
    };
    new Ajax.Request(url, opts);  
  };
  
  this.saveBidcosInterface = function(object_id, iface_id, roaming) {
    var url = "/configapp/metadata.cgi";
    var pb = '';
    pb += 'cmd=set_bidcos_interface&';
    pb += 'device='+object_id+'&';
    pb += 'bidcos_iface='+iface_id+'&';
    pb += 'roaming='+roaming+'&';
    if(dbg){alert(pb);}
    var opts = {
      postBody: pb,
      sendXML: false
    };
    new Ajax.Request(url, opts);  
  };
  
}();

configRefresher = Class.create();

configRefresher.prototype =
{
  initialize: function(iPollingInterval)
  {
    this.updating = false;
    this.ajax = null;
    this.pe = new PeriodicalExecuter(this.refresh, iPollingInterval);
    this.refresh();    
  },
  refresh: function()
  {
    if( rfr )
    {
      if( !this.updating )
      {
        this.updating = true;
        var t = this;
        SwitchOnFlashLight();
        var url = "/configapp/updateui.cgi";
        var pb = '';
        var opts =
        {
          method: 'post',
          postBody: pb,
          evalScripts:true,
          sendXml: false,
          onComplete: function()
          {
            delete t.ajax;
            t.ajax = null;
            iseRefrTimer = 0;
            t.updating = false;
          }
        };
        if(dbg)alert(pb);
        this.ajax = new Ajax.Updater("dummy", url, opts);
      }
    }
  }
};

// nur für das Konfigtool implementiert, Code aus der Datei /www/configapp/js/function.js der Version 1.4
//ID_BIDCOS_INTERFACE = 1024;

changeBidcosIface = function(chnId, ctrlId) {
  dlgPopup = new iseMessageBox(ID_BIDCOS_INTERFACE, chnId);
  PopupClose = function(iface, roaming) {
    dlgPopup.hide();
    if( iface != undefined )$(ctrlId).innerHTML = iface + (roaming?"/*":"");
  };
  SendRequest('/popupInterfaceEditor.cgi');
  dlgPopup.ShowPopup();
};

setServiceMessage = function() {
  var service_count = (homematic("Interface.getServiceMessageCount", {"interface": "default"}));
  $("msgServices").firstChild.data = "Servicemeldungen (" + service_count + ")";
          
  if (service_count == 0)
  {
    $("imgServices").src = "/ise/img/dot/green.png"; 
  } else {
    $("imgServices").src = "/ise/img/dot/yellow.png"; 
  }
                                       
  return service_count;
};
