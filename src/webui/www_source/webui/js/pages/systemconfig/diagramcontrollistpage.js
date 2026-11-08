DiagramListPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    conInfo("create diagram list page");

    doClearTempView = true;
    if( typeof filter == "string" )
    {
      updateContent("jpages/diagram/control", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
    }
    else
    {
      updateContent("jpages/diagram/control");
    }

  };
  
  this.leave = function()
  {
  };

}();
