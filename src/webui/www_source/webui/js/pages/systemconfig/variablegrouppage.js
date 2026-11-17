VariableGroupPage = new function()
{
  this.enter = function()
  {
    MainMenu.select("MAINMENU_OPTIONS");
    updateContent("/pages/tabs/admin/variablegroups.htm");
  };
  this.leave = function() {};
}();
