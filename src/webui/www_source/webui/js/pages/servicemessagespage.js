ServiceMessagesPage = new function()
{
  
  this.enter = function(options)
  {
    MainMenu.select(null);
    if ($("msgServices")) { $("msgServices").addClassName("Messages_Selected"); }
    loadServiceMessages(options);
  };
  
  this.leave = function()
  {
    if ($("msgServices")) { $("msgServices").removeClassName("Messages_Selected"); }
  };

}();
