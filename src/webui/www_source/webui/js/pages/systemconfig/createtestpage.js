CreateTestPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);

    conInfo("create testpage");

    doClearTempView = true;
    if( typeof filter == "string" )
    {
      updateContent("jpages/testjavapage", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
    }
    else
    {
      updateContent("jpages/testjavapage");
    }
  };
  
  this.leave = function()
  {
  };

}();
