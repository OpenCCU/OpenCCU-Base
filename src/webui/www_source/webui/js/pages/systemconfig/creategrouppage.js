CreateGroupPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    //alert("create group page");

    conInfo("create group list page");

    doClearTempView = true;
    if( typeof filter == "string" )
    {
      updateContent("jpages/group/list", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
    }
    else
    {
      updateContent("jpages/group/list");
    }

  };
  
  this.leave = function()
  {
  };

}();
