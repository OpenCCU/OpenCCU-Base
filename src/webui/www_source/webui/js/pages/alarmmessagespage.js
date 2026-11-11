AlarmMessagesPage = new function()
{
  
  this.enter = function(options)
  {
    MainMenu.select(null);
    if ($("msgAlarms")) { $("msgAlarms").addClassName("Messages_Selected"); }
    loadAlarmMessages(options);
  };
  
  this.leave = function()
  {
    if ($("msgAlarms")) { $("msgAlarms").removeClassName("Messages_Selected"); }
  };

}();
