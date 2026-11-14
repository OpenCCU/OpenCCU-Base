LinksAndProgramsPage = new function()
{
  var m_menuId = "MAINMENU_LINKSANDPROGRAMS";
  
  this.enter = function(options)
  {
    MainMenu.select(m_menuId);
    loadLinkProg(options);
  };
  
  this.leave = function()
  {
  };

}();
