ProgramGroupPage = new function()
{
  this.enter = function()
  {
    MainMenu.select("MAINMENU_OPTIONS");
    updateContent("/pages/tabs/admin/programgroups.htm");
  };
  this.leave = function() {};
}();
