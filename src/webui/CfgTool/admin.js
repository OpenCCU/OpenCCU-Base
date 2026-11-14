/**
 * mainmenu/admin.json
 * Hauptmenü für den Administrator
 **/

/*
[
  {id: "Startseite", align: "left", action: function() { WebUI.enter(StartPage); }   , submenu:
	[ 
	]
  },
  {id: "Status und Bedienung", align: "left", action: function() { WebUI.enter(ControlPage); }, submenu: 
	[
		{id: "Geräte"   , action: function() { WebUI.enter(ControlDevicesPage); }  },
		{id: "Räume"    , action: function() { WebUI.enter(ControlRoomsPage); }    },
		{id: "Gewerke"  , action: function() { WebUI.enter(ControlFunctionsPage); }},
		{id: "Favoriten", action: function() { WebUI.enter(ControlFavoritesPage); }},
		{id: "Programme", action: function() { WebUI.enter(ControlProgramsPage); } },
		{id: "Systemvariable" , action: function() { WebUI.enter(ControlVariablesPage); }},
		{id: "Systemprotokoll", action: function() { WebUI.enter(ControlProtocolPage); } }
	]
  },
  {id: "Programme und Verknüpfungen", align: "left", action: function() { ConfigData.check(function() { WebUI.enter(LinksAndProgramsPage); }); }, submenu: 
  [
    {id: "Direkte Geräteverknüpfungen"                , action: function() { ConfigData.check(function() { WebUI.enter(LinkListPage); }); }   },
    {id: "Programmerstellung & Zentralenverknüpfungen", action: function() { ConfigData.check( function() { WebUI.enter(ProgramListPage); }); }}  
  ]},
  {id: "Einstellungen", align: "left", action: function() { ConfigData.check(function() { WebUI.enter(SystemConfigPage); }); }, submenu: 
  [ 
    {id: "Geräte - Posteingang", action: function() { ConfigData.check(function() { WebUI.enter(NewDeviceListPage); }); } },
    {id: "Geräte"              , action: function() { ConfigData.check(function() { WebUI.enter(DeviceListPage); }); }    } ,
    {id: "Räume"               , action: function() { ConfigData.check(function() { WebUI.enter(RoomListPage); }); }      },
    {id: "Gewerke"             , action: function() { ConfigData.check(function() { WebUI.enter(FunctionListPage); }); }  },
    {id: "Benutzerverwaltung"  , action: function() { WebUI.enter(UserAdminPageAdmin); }},
    {id: "Systemvariable"      , action: function() { ConfigData.check(function() { WebUI.enter(VariableListPage); }); }  },
    {id: "Favoriten"           , action: function() { ConfigData.check(function() { WebUI.enter(FavoriteListPage); } ); } },
    {id: "Systemsteuerung"     , action: function() { WebUI.enter(SystemControlPage);  } }    
  ]},
  {id: "Hilfe"          , align: "right", action: function() { WebUI.enter(HelpPage); }, submenu: [ ]},
  {id: "Geräte anlernen", align: "right", action: function() { ConfigData.check(function() { showAddDeviceCP(true); }); }, submenu: [ ]}
]

*/
 /*  {id: "Einstellungen", align: "left", action: function() { CreateCPPopup('/configapp/cp_settings.cgi'); }, subMenu: [ ]}  */
  /*{id: "Einstellungen", align: "left", action: function() { ConfigData.check(function() { WebUI.enter(SystemConfigPage); }); }, submenu: [ ]} */
 [

  {id: "Geräte", align: "left", action: function() { ConfigData.check(function() { WebUI.enter(DeviceListPage); }); }  , submenu: [ ]},
  {id: "Direkte Geräteverknüpfungen", align: "left", action: function() { ConfigData.check(function() { WebUI.enter(LinkListPage); }); }, submenu: []},
  {id: "Geräte anlernen", align: "left", action: function() { ConfigData.check(function() { showAddDeviceCP(true); }); }, submenu: [ ]},  
  {id: "Einstellungen", align: "left", action: function() { CreateCPPopup('/configapp/cp_settings.cgi'); }, submenu: [ ]}
  //{id: "V1.600", align: "left", action: function() { createTest('/configapp/test.cgi'); }, submenu: [ ]}
  //{id: "New Einstellungen", align: "left", action: function() {ConfigData.check(function() { WebUI.enter(SystemControlPage); });  }, submenu: [ ]}   
 ]
 
