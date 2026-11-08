DeviceConfigPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  /**
   * @param options
   *        iface
   *        address
   *        redirect_url
   *        with_group
   **/
  this.enter = function(options)
  {
  	ResetPostString();
    
    MainMenu.select(MAINMENU_ID);    
    var poststr = "";
    if (typeof(options) != "undefined")
    {
      for(key in options)
      {
        poststr += "&" + key + "=" + options[key];
      }
    }

    updateContent(UI_PATH + 'ic_deviceparameters.cgi', poststr);
  };
  
  this.leave = function()
  {
  };

}();
