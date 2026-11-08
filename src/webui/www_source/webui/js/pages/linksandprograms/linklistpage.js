LinkListPage = new function()
{
  var m_menuId = "MAINMENU_LINKSANDPROGRAMS";
  
  this.enter = function(options)
  {
    var args = "";
    
    MainMenu.select(m_menuId);
    if (typeof(options) != "undefined") 
    {    
      for(key in options)
      {
        args += "&" + key + "=" + options[key];
      }
    }
    updateContent("/config/ic_linkpeerlist.cgi", args);
  };
  
  this.leave = function()
  {
  };

}();
