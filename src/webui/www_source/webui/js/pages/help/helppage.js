HelpPage = new function()
{

  var m_menuId = "MAINMENU_HELP";
  
  this.enter = function(options)
  {
    MainMenu.select(m_menuId);
    loadHelp(options);
  };
  
  this.leave = function()
  {
  };

}();
