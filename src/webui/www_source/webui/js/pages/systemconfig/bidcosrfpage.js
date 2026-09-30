getGatewayStatus = function()
{
  return [
    {address: "ABC1234567", description: "", isConnected: true, isDefault: true},
    {address: "ABC0234567", description: "", isConnected: false, isDefault: false}
  ];
};

var globalLGWTypeMap = {};
var globalLGWTypes = {LANIF : "Lan Interface", HMWLGW : "HMWLGW", HMLGW2 : "HMLGW2"};
var globalGWClasses = {RF : "RF", Wired : "Wired"};

BidcosRfPage =
{
  UPDATE_INTERVALL: 5,
  
  m_page: null,
  m_optionsContainer: null,
  m_optionsHeadline: null,
  m_optionsForm: null,
  m_gatewayContainer: null,
  m_gatewayTableHead: null,
  m_gatewayTableHeadRow: null,
  m_gatewayTableAddressHeader: null,
  m_gatewayTableKeyHeader: null,
  m_gatewayTableIPHeader: null,
  m_gatewayTableStateHeader: null,
  m_gatewayTableActionHeader: null,
  m_gatewayTableBody: null,
  m_optionsButtonBar: null,
  m_addGatewayButton: null,
  m_applyButton: null,
  m_optionButtonClear: null,
  m_optionsFooter: null,
  m_allocationContainer: null,
  m_allocationHeadline: null,
  m_allocationTable: null,
  m_allocationTableHead: null,
  m_allocationTableHeadRow: null,
  m_allocationTableNameHeader: null,
  m_allocationTableImageHeader: null,
  m_allocationTableAddressHeader: null,
  m_allocationTableGatewayHeader: null,
  m_allocationTableActionHeader: null,
  m_allocationTableBody: null,
  m_onSourceChangedHandler: null,
  m_onAddGatewayHandlerRF: null,
  m_onApplyHandler: null,
  m_onUpdateHandler: null,
  m_periodicalUpdater: null,
  m_defaultGateway: null,

  m_devices: [],


  create: function()
  {
    this.m_page = document.createElement("div");
    this.m_page.className = "bidcosrf_page";

    globalLGWTypeMap[globalLGWTypes.HMWLGW] = "HomeMatic RS485 Gateway";
    globalLGWTypeMap[globalLGWTypes.HMLGW2] = "HomeMatic RF-LAN Gateway";
    globalLGWTypeMap[globalLGWTypes.LANIF] = "HM Configuration Tool LAN";
        
    /* Event-Handler */
    this.m_onUpdateHandler = function() { BidcosRfPage.onUpdate(); };
    this.m_onSourceChangedHandler = function() { BidcosRfPage.onSourceChanged(); };
    this.m_onAddGatewayHandlerRF = function() { BidcosRfPage.onAddGateway(); };
    this.m_onApplyHandler = function() { BidcosRfPage.onApply(); };
    
    /* Periodisches Update */
    this.m_periodicalUpdater = new PeriodicalExecuter(this.m_onUpdateHandler, this.UPDATE_INTERVALL);
    
    /* Allgmeine Einstellungen */
    this.m_optionsContainer = document.createElement("div");
    this.m_page.appendChild(this.m_optionsContainer);
    
    /* Überschrift */
    this.m_optionsHeadline = document.createElement("h1");
    //this.m_optionsHeadline.appendChild(document.createTextNode("Allgmeine Einstellungen"));
    this.m_optionsHeadline.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfLblSettings")));    
    this.m_optionsContainer.appendChild(this.m_optionsHeadline);
    
    /* Formular zur Auswahl der Quelle */
    this.m_optionsForm = document.createElement("form");
    this.m_optionsContainer.appendChild(this.m_optionsForm);
    
    /* Container für BidCoS-RF Gateways */
    this.m_gatewayContainer = document.createElement("div");
    $(this.m_gatewayContainer).hide();
    this.m_optionsContainer.appendChild(this.m_gatewayContainer);
    
    /* Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTable = document.createElement("table");
    this.m_gatewayTable.className = "bidcosrf_table";
    $(this.m_gatewayTable).writeAttribute("cellspacing", "0");
    this.m_gatewayContainer.appendChild(this.m_gatewayTable);
    
    /* Tabellenkopf (Tabelle für BidCoS-RF Gateways) */
    this.m_gatewayTableHead = document.createElement("thead");
    this.m_gatewayTable.appendChild(this.m_gatewayTableHead);
    
    /* Kopfzeile (Tabelle für BidCoS-RF Gateways) */
    this.m_gatewayTableHeadRow = document.createElement("tr");
    this.m_gatewayTableHead.appendChild(this.m_gatewayTableHeadRow);

    /* Spaltenüberschrift Name (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableAddressHeader = document.createElement("th");
    this.m_gatewayTableAddressHeader.className = "bidcosrf_tableheader";
    this.m_gatewayTableAddressHeader.appendChild(document.createTextNode(translateKey("thName")));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableAddressHeader);

    /* Spaltenüberschrift Seriennummer (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableAddressHeader = document.createElement("th");
    this.m_gatewayTableAddressHeader.className = "bidcosrf_tableheader";
    //this.m_gatewayTableAddressHeader.appendChild(document.createTextNode("Seriennummer"));
    this.m_gatewayTableAddressHeader.appendChild(document.createTextNode("Gateway"));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableAddressHeader);
    
    /* Spaltenüberschrift AES-Schlüssel (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableKeyHeader = document.createElement("th");
    this.m_gatewayTableKeyHeader.className = "bidcosrf_tableheader";
    //this.m_gatewayTableKeyHeader.appendChild(document.createTextNode("Zugriffscode"));
    this.m_gatewayTableKeyHeader.appendChild(document.createTextNode(translateKey("dialogSettingsBidcosRFSecurityKey")));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableKeyHeader);

    /* Spaltenüberschrift IP (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableIPHeader = document.createElement("th");
    this.m_gatewayTableIPHeader.className = "bidcosrf_tableheader";
    //this.m_gatewayTableIPHeader.appendChild(document.createTextNode("IP-Adresse"));
    this.m_gatewayTableIPHeader.appendChild(document.createTextNode(translateKey("thIPAddress")));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableIPHeader);

    /* Spaltenüberschrift Status (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableStateHeader = document.createElement("th");
    this.m_gatewayTableStateHeader.className = "bidcosrf_tableheader";
    //this.m_gatewayTableStateHeader.appendChild(document.createTextNode("Status"));
    this.m_gatewayTableStateHeader.appendChild(document.createTextNode(translateKey("thState")));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableStateHeader);
    
    /* Spaltenüberschrift Verbunden (Tabelle für BidCoS-RF Gateways */
    this.m_gatewayTableActionHeader = document.createElement("th");
    this.m_gatewayTableActionHeader.className = "bidcosrf_tableheader";
    //this.m_gatewayTableActionHeader.appendChild(document.createTextNode("Aktion"));
    this.m_gatewayTableActionHeader.appendChild(document.createTextNode(translateKey("thAction")));
    this.m_gatewayTableHeadRow.appendChild(this.m_gatewayTableActionHeader);
    
    /* Tabellenkörper (Tabelle für BidCoS-RF Gateways) */
    this.m_gatewayTableBody = document.createElement("tbody");
    this.m_gatewayTable.appendChild(this.m_gatewayTableBody);
    
    /* Optionsmenü */
    this.m_optionsButtonBar = document.createElement("div");
    this.m_optionsContainer.appendChild(this.m_optionsButtonBar);
    
    /* Button zum Übernehmen der Einstellungen */
    this.m_applyButton = document.createElement("div");
    //this.m_applyButton.appendChild(document.createTextNode("Übernehmen"));
    this.m_applyButton.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfBtnSave")));
    Event.observe($(this.m_applyButton), "click", this.m_onApplyHandler);
    this.m_applyButton.className = "StdButton bidcosrf_optionbutton";
    this.m_optionsButtonBar.appendChild(this.m_applyButton);
    
    /* Button zum Hinzufügen von RF Gateways */
    this.m_addGatewayButton = document.createElement("div");
    $(this.m_addGatewayButton).hide();
    //this.m_addGatewayButton.appendChild(document.createTextNode("Hinzufügen"));
    this.m_addGatewayButton.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfBtnAdd")));
    Event.observe($(this.m_addGatewayButton), "click", this.m_onAddGatewayHandlerRF);
    this.m_addGatewayButton.className = "StdButton bidcosrf_optionbutton";
    this.m_optionsButtonBar.appendChild(this.m_addGatewayButton);  

    /* float-Bereich löschen */
    this.m_optionButtonClear = document.createElement("div");
    this.m_optionButtonClear.className = "bidcosrf_clear";
    this.m_optionsButtonBar.appendChild(this.m_optionButtonClear);
    
    /* Footer für allgemeine Einstellungen*/
    this.m_optionsFooter = document.createElement("div");
    this.m_optionsFooter.className = "bidcosrf_footer";
    this.m_optionsContainer.appendChild(this.m_optionsFooter);
    
    /* Gateway-Zuordnung */  
    this.m_allocationContainer = document.createElement("div");
    this.m_page.appendChild(this.m_allocationContainer);
    
    /* Überschrift für Gateway-Zuordnung */
    this.m_allocationHeadline = document.createElement("h1");
    //this.m_allocationHeadline.appendChild(document.createTextNode("Interface-Zuordnung"));
    this.m_allocationHeadline.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfIfaceAssignment")));
    this.m_allocationContainer.appendChild(this.m_allocationHeadline);
    
    /* Tabelle für Gateway-Zuordnung */
    this.m_allocationTable = document.createElement("table");
    this.m_allocationTable.className = "bidcosrf_table";    
    $(this.m_allocationTable).writeAttribute("cellspacing", "0");
    this.m_allocationContainer.appendChild(this.m_allocationTable);
    
    /* Tabellenkopf (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableHead = document.createElement("thead");
    this.m_allocationTable.appendChild(this.m_allocationTableHead);
    
    /* Zeile im Tabellenkopf (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableHeadRow = document.createElement("tr");
    this.m_allocationTableHead.appendChild(this.m_allocationTableHeadRow);
    
    /* Spaltenüberschrift "Name" (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableNameHeader = document.createElement("th");
    this.m_allocationTableNameHeader.className = "bidcosrf_tableheader";
    //this.m_allocationTableNameHeader.appendChild(document.createTextNode("Name"));
    this.m_allocationTableNameHeader.appendChild(document.createTextNode(translateKey("thName")));
    this.m_allocationTableHeadRow.appendChild(this.m_allocationTableNameHeader);

    /* Spaltenüberschrift "Bild" (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableImageHeader = document.createElement("th");
    this.m_allocationTableImageHeader.className = "bidcosrf_tableheader";
    //this.m_allocationTableImageHeader.appendChild(document.createTextNode("Bild"));
    this.m_allocationTableImageHeader.appendChild(document.createTextNode(translateKey("thPicture")));
    this.m_allocationTableHeadRow.appendChild(this.m_allocationTableImageHeader);

    /* Spaltenüberschrift "Seriennummer" (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableAddressHeader = document.createElement("th");
    this.m_allocationTableAddressHeader.className = "bidcosrf_tableheader";
    //this.m_allocationTableAddressHeader.appendChild(document.createTextNode("Seriennummer"));
    this.m_allocationTableAddressHeader.appendChild(document.createTextNode(translateKey("thSerialNumber")));
    this.m_allocationTableHeadRow.appendChild(this.m_allocationTableAddressHeader);

    /* Spaltenüberschrift "Gatway" (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableGatewayHeader = document.createElement("th");
    this.m_allocationTableGatewayHeader.className = "bidcosrf_tableheader";
    this.m_allocationTableGatewayHeader.appendChild(document.createTextNode("Gateway"));
    this.m_allocationTableHeadRow.appendChild(this.m_allocationTableGatewayHeader);

    /* Spaltenüberschrift "Aktion" (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableActionHeader = document.createElement("th");
    this.m_allocationTableActionHeader.className = "bidcosrf_tableheader";
    //this.m_allocationTableActionHeader.appendChild(document.createTextNode("Aktion"));
    this.m_allocationTableActionHeader.appendChild(document.createTextNode(translateKey("thAction")));
    this.m_allocationTableHeadRow.appendChild(this.m_allocationTableActionHeader);
    
    /* Tabellenkörper (Tabelle für Gateway-Zuordnung) */
    this.m_allocationTableBody = document.createElement("tbody");
    this.m_allocationTable.appendChild(this.m_allocationTableBody);
    
    this.m_gatewayContainer.show();
    this.m_addGatewayButton.show();
    
  },

  destroy: function()
  {
    var i, len;
    
    /* Periodisches Update deaktivieren */
    this.m_periodicalUpdater.stop();
    
    /* Ereignis-Handler  abmelden */
    //Event.stopObserving($(this.m_antennaRadioButton), "click", this.m_sourceChangedHandler);
    //Event.stopObserving($(this.m_hmcfgRadioButton), "click", this.m_sourceChangedHandler);
    Event.stopObserving($(this.m_applyButton), "click", this.m_onApplyHandler);
    Event.stopObserving($(this.m_addGatewayButton), "click", this.m_onAddGatewayHandlerRF);
    
    /* Gateways entfernen */
    for (i = 0, len = this.m_gateways.length; i < len; i++)
    {
      var gateway = this.m_gateways[i];
      gateway.destroy();
    }
    this.m_gateways = [];
    
    /* Geräte entfernen */
    for (i = 0, len = this.m_devices.length; i < len; i++)
    {
      var device = this.m_devices[i];
      device.destroy();
    }
    this.m_devices = [];
  },

  enter: function()
  {


    MainMenu.select("MAINMENU_OPTIONS");

    var PATH_HTML= "" +
      "<span onclick='WebUI.enter(SystemConfigPage);'>"+translateKey("menuSettingsPage")+"</span>" +
      " &gt; " +
      "<span onclick='WebUI.enter(SystemControlPage);'>"+translateKey("submenuSysControl")+"</span>" +
      " &gt; " +
      translateKey("dialogSettingsBidCosRFConfPath"),

    FOOTER_HTML= "" +
      "<table border='0' cellspacing='8'>" +
        "<tr>" +
          "<td style='text-align:center;vertical-align=middle;' ><div class='FooterButton' style='width:auto;padding-left:5px;padding-right:5px;' onclick='WebUI.goBack();'>"+translateKey("footerBtnPageBack")+"</div></td>" +
        "</tr>" +
      "</table>";

    setPath(PATH_HTML);
    setFooter(FOOTER_HTML);
    InterfaceMonitor.stop();
    
    this.create();
    this.loadGateways();
    this.loadDevices();
        
    WebUI.setContent(this.m_page);
  },
  
  leave: function()
  {
    this.destroy();
    InterfaceMonitor.start();
  },
  
  resize: function()
  {
  },
  
  onSourceChanged: function()
  {
 /*   if (this.m_hmcfgRadioButton.checked)
    {
      $(this.m_gatewayContainer).show();
      $(this.m_addGatewayButton).show();
    }
    else
    {
      $(this.m_gatewayContainer).hide();
      $(this.m_addGatewayButton).hide();
    }*/
  },
  
  onAddGateway: function()
  {
  
  	 var wiredGWExists = false;    
     for (var i = 0, len = this.m_gateways.length; i < len; i++)
     {
      var gateway = this.m_gateways[i];
      if( globalGWClasses.Wired == gateway.getGatewayClass()) {
      	wiredGWExists = true;
      	break;
	  }
    }
    new BidcosRfPage.AddGatewayDialog(wiredGWExists)
      .setAction(this.onGatewayAdded, this);
  },
  
  onGatewayAdded: function(dialog)
  {
    var gateway = new BidcosRfPage.Gateway()
      .setGatewayClass(dialog.getGatewayClass())
      .setType(dialog.getType())
      .setUserName(dialog.getUserName())
      .setAddress(dialog.getSerial())
      .setKey(dialog.getKey())
      .setIP(dialog.getIp())
      .setState(translateKey("lanGatewayLblNotActive"));
      
    this.m_gatewayTableBody.appendChild(gateway.getElement());
    this.m_gateways.push(gateway);
  },
  
  
  onApply: function()
  {
    var gatewaysRF = [];
    var gatewaysWired = [];

/*    var useInternalAntenna = this.m_antennaRadioButton.checked;
    if (this.ss.length == 0) 
    {
      useInternalAntenna = true;
    }
  */  
    
    /* RF-Gateways */
    for (var i = 0, len = this.m_gateways.length; i < len; i++)
    {
      var gateway = this.m_gateways[i];
      if( globalGWClasses.RF == gateway.getGatewayClass()) {
		  gatewaysRF.push({
		    type         : gateway.getType(),
        userName     : gateway.getUserName(),
		    serialNumber : gateway.getAddress(),
		    encryptionKey: gateway.getKey(),
		    ipAddress    : gateway.getIP()
		  });
	  }
	  else { //gatewayClass is: Wired
		  gatewaysWired.push({
		    type         : gateway.getType(),
        userName     : gateway.getUserName(),
		    serialNumber : gateway.getAddress(),
		    encryptionKey: gateway.getKey(),
		    ipAddress    : gateway.getIP()
		  });	  	
	  }
    }
    
    homematic("BidCoS_RF.setConfigurationRF", {
     // "useInternalAntenna": useInternalAntenna,
      "interfaces": gatewaysRF
    });
    
    homematic("BidCoS_Wired.setConfigurationWired", {
      "interfaces": gatewaysWired
    });
  
    
 //   if (/*(useInternalAntenna == false) &&*/ (homematic("BidCoS_RF.isKeySet", null))) //TODO Check if this is okay
 //   {
 //     new BidcosRfPage.OptionsDialog();
 //   }
 //   else
 //   {
      //MessageBox.show("Info", "Die Konfiguration wurde an die HomeMatic Zentrale übertragen.\nDie Änderungen werden mit dem nächsten Start der HomeMatic Zentrale wirksam.");
      MessageBox.show(translateKey("dialogInfo"), translateKey("dialogSettingsBidcosRFSaveConfigSucceed"));
      WebUI.enter(BidcosRfPage);
 //   }
  },
  
  onUpdate: function()
  { 
    var that = this;
    //Sort gateways into own arrays to avoid interferences when doing more than one homematic() calls.
    var rfGateways = new Array();
    var wiredGateways = new Array();
    
    for(var i = 0, len = that.m_gateways.length; i < len; i++) 
    {
    	if(that.m_gateways[i].getGatewayClass() == globalGWClasses.Wired) {
    		wiredGateways.push(that.m_gateways[i]);
    	}
    	else {
    		rfGateways.push(that.m_gateways[i]);
    	}
    }
    /* RF-Gateways */
    homematic("Interface.listBidcosInterfaces", {"interface": "BidCos-RF"}, function(gatewayStatus) {
      if (gatewayStatus)
      {
        for (var i = 0, len = rfGateways.length; i < len; i++)
        {
   	      if(globalLGWTypes.HMLGW2 == rfGateways[i].getType()) {//NEW RF Gateway
   	        //var hmlgw2GW = rfGateways[i];
   	        var lgwStatus = null;
		    homematic("Interface.getLGWConnectionStatus", {"interface" : "BidCos-RF", "serial" : rfGateways[i].getAddress()}, function(lgwStatus) {
		        var textB = "";
		        var lgw = null;
		        if(lgwStatus) 
		        {
				    for(var j = 0; j < rfGateways.length; j++) {
				    	if(rfGateways[j].getAddress() == lgwStatus.serial) {
				    	 lgw = rfGateways[j];
				    	}
				    }
		        	if(lgw) 
		        	{
					  if(lgwStatus.connstat == "NO_ERROR") {
					    textB = translateKey("lanGatewayLblConnected");
					  }
					  else if(lgwStatus.connstat == "CONNECT_FAILED") {
					    textB = translateKey("lanGatewayLblNotConnected");
					  }
					  else if(lgwStatus.connstat == "WRONG_KEY") {
					    textB = translateKey("errorWrongPassword");
					  }
					  else if(lgwStatus.connstat == "") {
					  	textB = translateKey("lanGatewayLblNotConnected");
					  }
					  else {
					    textB = translateKey("lanGatewayLblNotConnected");
					  }
					}
					else 
					{
				      textB = translateKey("lanGatewayLblNotActive");
					}
					lgw.setState(textB);
			  }//if lgwstatus
			  else {
			  	lgw.setState(translateKey("lanGatewayLblNotActive"));
			  }
		    });
   	      }
		  else //all other rf gateways
		  {
			  var text = "";
	   	      var status = rfGateways[i].determineState(gatewayStatus);
		      if (status)
		      {
		        text = (status.isConnected) ? translateKey("lanGatewayLblConnected") : translateKey("lanGatewayLblNotConnected");
		        if (status.isDefault) { text += " " + translateKey("lanGatewayLblStandard"); }
		      }
		      else
		      {
		        //text = "inaktiv";
		        text = translateKey("lanGatewayLblNotActive");
		      }
		      rfGateways[i].setState(text);
		  }
        }
        
        //Set address of default gateway
        if (!that.m_defaultGateway)
        {
          for (var i = 0, len = gatewayStatus.length; i < len; i++)
          {
            if (gatewayStatus[i].isDefault)
            {
            //alert("isDEfault"+gatewayStatus[i].address);
              that.m_defaultGateway = new BidcosRfPage.Gateway()
                .setAddress(gatewayStatus[i].address);
            }
          }
        }
        jQuery("." + that.m_defaultGateway.getAddress()).text(translateKey("lanGatewayLblStandard"));
      }
    });
    /* Wired Gateways */
    for(var k = 0, length = wiredGateways.length; k < length ; k++)
    {
      	var wiredLGW = wiredGateways[k];
		    homematic("Interface.getLGWConnectionStatus", {"interface" : "BidCos-Wired", "serial" : wiredLGW.getAddress()}, function(lgwStatus) {
		    var textB = "";
		    var lgw = null;
		    if(lgwStatus) 
		    {
				for(var j = 0; j < wiredGateways.length; j++) {
					if(wiredGateways[j].getAddress() == lgwStatus.serial) {
					 lgw = wiredGateways[j];
					}
				}
		    	if(lgw) 
		    	{
				  if(lgwStatus.connstat == "NO_ERROR") {
					textB = translateKey("lanGatewayLblConnected");
				  }
				  else if(lgwStatus.connstat == "CONNECT_FAILED") {
					textB = translateKey("lanGatewayLblNotConnected");
				  }
				  else if(lgwStatus.connstat == "WRONG_KEY") {
					textB = translateKey("errorWrongPassword");
				  }
				  else if(lgwStatus.connstat == "") {
				  	textB = translateKey("lanGatewayLblNotConnected");
				  }
				  else {
					textB = translateKey("lanGatewayLblNotConnected");
				  }
				}
				else {
				  textB = translateKey("lanGatewayLblNotActive");
				}
				lgw.setState(textB);
		  }//if lgwstatus
		  else {
		  	lgw.setState(translateKey("lanGatewayLblNotActive"));
		  }
		});
    }
  },
  
  loadGateways: function()
  {
    var i, len;
    this.m_gateways = [];
    
    //HomeMatic RF Gateways
    var response = homematic("BidCoS_RF.getConfigurationRF");
    if (response)
    {
 //     this.m_antennaRadioButton.checked = response.useInternalAntenna;
 //     this.m_hmcfgRadioButton.checked = !response.useInternalAntenna;
      this.onSourceChanged();
      
      for (i = 0, len = response.interfaces.length; i < len; i++)
      {
        var gateway = response.interfaces[i];
        this.m_gateways.push(new BidcosRfPage.Gateway()
          .setGatewayClass("RF")
          .setType(gateway.type)
          .setUserName(gateway.userName)
          .setAddress(gateway.serialNumber)
          .setKey(gateway.encryptionKey)
          .setIP(gateway.ipAddress));
      }
    
    }
    
   //HomeMatic Wired Gateways
	response = homematic("BidCoS_Wired.getConfigurationWired");
	if(response)
	{
      for (i = 0, len = response.interfaces.length; i < len; i++)
      {
        var gateway = response.interfaces[i];
        this.m_gateways.push(new BidcosRfPage.Gateway()
          .setGatewayClass(globalGWClasses.Wired)
          .setType(gateway.type)
          .setUserName(gateway.userName)
          .setAddress(gateway.serialNumber)
          .setKey(gateway.encryptionKey)
          .setIP(gateway.ipAddress));
      }
	}

	//Add all gateways to table
    for (i = 0, len = this.m_gateways.length; i < len; i++)
    {
      var gateway = this.m_gateways[i];
      this.m_gatewayTableBody.appendChild(gateway.getElement());
    }
      
  },
  
  removeGateway: function(gateway)
  {
    gateway.destroy();
    this.m_gateways = this.m_gateways.without(gateway);
  },
  
  loadDevices: function()
  {
    var i, len, device;
    this.m_devices = [];
    
    var devices = DeviceList.listDevices().ex_sortBy("name");
    
    for (i = 0, len = devices.length; i < len; i++)
    {
      device = devices[i];
      if (device.interfaceName == "BidCos-RF")
      {
        this.m_devices.push(new BidcosRfPage.Device()
          .setName(device.name)
          .setImageHtml(device.getThumbnailHTML())
          .setAddress(device.address)
          );
      }
    }
    
    for (i = 0, len = this.m_devices.length; i < len; i++)
    {
      device = this.m_devices[i];
      this.m_allocationTableBody.appendChild(device.getElement());
    }
    
    var that = this;
    homematic("Interface.listDevices", {"interface": "BidCos-RF"}, function(deviceList) {
      if (deviceList)
      {
        for (var i = 0, len = that.m_devices.length; i < len; i++)
        {
          var device = that.m_devices[i];
          device.setGatewayFromDeviceList(deviceList);
        }
      }    
    });
  },
  
  getGateways: function()
  {
    return this.m_gateways;
  },
  
  getDefaultGateway: function()
  {
    return this.m_defaultGateway;
  }
};

BidcosRfPage.Gateway = function()
{
  this.create();
};

BidcosRfPage.Gateway.prototype =
{
  m_element: null,
  m_type: null,
  m_address: null,
  m_key: null,
  m_ip: null,
  m_userNameLabel: null,
  m_addressLabel: null,
  m_keyLabel: null,
  m_ipLabel: null,
  m_stateLabel: null,
  m_actionCell: null,
  m_deleteButton: null,
  m_onDeleteHandler: null,
  m_editButton: null,
  m_onEditHandler: null,
  m_changeKeyButton : null,
  m_onChangeKeyHandler : null,
  
  create: function()
  {
    /* Event-Handler */
    var that = this;
    this.m_onDeleteHandler = function() { that.onDelete(); };
    this.m_onEditHandler = function() { that.onEdit(); };
    this.m_onChangeKeyHandler = function() { that.onChangeKey(); };
    
    /* Tabellenzeile */
    this.m_element = document.createElement("tr");
    
    /** Gateway-Klasse (RF oder Wired)*/
    this.m_gatewayClass = "";
    
    /** Gateway-Typ (Lan Interface / HMWLGW) */
    this.m_type = "";

    /* userName */
    this.m_userName = "";
    this.m_userNameLabel = document.createElement("td");
    this.m_userNameLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_userNameLabel);

    /* Seriennummer */
    this.m_address = "";
    this.m_addressLabel = document.createElement("td");
    this.m_addressLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_addressLabel);
    
    /* Zugriffscode */
    this.m_keyLabel = document.createElement("td");
    this.m_keyLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_keyLabel);
    
    /* IP-Addresse */
    this.m_ipLabel = document.createElement("td");
    this.m_ipLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_ipLabel);
    
    /* Status */
    this.m_stateLabel = document.createElement("td");
    this.m_stateLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_stateLabel);
    
    /* Aktion */
    this.m_actionCell = document.createElement("td");
    this.m_actionCell.className = "bidcosrf_actioncell";
    this.m_element.appendChild(this.m_actionCell);
    
    /* Löschen-Button */
    this.m_deleteButton = document.createElement("div");
    this.m_deleteButton.className = "StdButton";//"bidcosrf_button";
    //this.m_deleteButton.appendChild(document.createTextNode("Löschen"));
    this.m_deleteButton.appendChild(document.createTextNode(translateKey("btnRemove")));
    Event.observe($(this.m_deleteButton), "click", this.m_onDeleteHandler);
      Element.setStyle(this.m_deleteButton, {
      margin : "1px"
    });
    this.m_actionCell.appendChild(this.m_deleteButton);
    
    /* Edit-Button */
    this.m_editButton = document.createElement("div");
    this.m_editButton.className = "StdButton";//"bidcosrf_button";
    Element.setStyle(this.m_editButton, {
      margin : "1px"
    });
    this.m_editButton.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfBtnSet")));
	Event.observe($(this.m_editButton), "click", this.m_onEditHandler);
    this.m_actionCell.appendChild(this.m_editButton);
    
    
    /* change key button */
    this.m_changeKeyButton = document.createElement("div");
    this.m_changeKeyButton.className = "StdButton";
    this.m_changeKeyButton.id = "changeKeyButtonId";
    Element.setStyle(this.m_changeKeyButton, {
      margin : "1px"
    });
    this.m_changeKeyButton.appendChild(document.createTextNode(translateKey("dialogSettingsBidcosRFChangeAccessCodeTitle")));
    Event.observe($(this.m_changeKeyButton), "click", this.m_onChangeKeyHandler);
    this.m_actionCell.appendChild(this.m_changeKeyButton);
  },
  
  destroy: function()
  {
    Event.stopObserving($(this.m_deleteButton), "click", this.m_onDeleteHandler);
    Event.stopObserving($(this.m_editButton), "click", this.m_onEditHandler);
    Event.stopObserving($(this.m_changeKeyButton), "click", this.m_onChangeKeyHandler);
    $(this.m_element).remove();
  },
  
  getGatewayClass : function()
  {
  	return this.m_gatewayClass;
  },
  
  setGatewayClass: function(gwClass)
  {
    this.m_gatewayClass = gwClass;
    return this;
  },

  setType: function(type)
  {
    this.m_type = type;
    if( (this.m_type !== globalLGWTypes.HMLGW2) && (this.m_type !==  globalLGWTypes.HMWLGW) )
    {
      this.m_changeKeyButton.hide();
    }
    return this;
  },
  
  getType: function()
  {
    return this.m_type;
  },

  setUserName: function(name)
  {
    if(name != undefined) {
      this.m_userName = name;
      this.m_userNameLabel.innerHTML = "";
      this.m_userNameLabel.appendChild(document.createTextNode(name));
    }
    return this;
  },

  getUserName: function()
  {
    return this.m_userName;
  },

  setAddress: function(address)
  {
    this.m_address = address;
    this.m_addressLabel.innerHTML = this.m_address+"<br/>"+globalLGWTypeMap[this.m_type];
    return this;
  },
  
  getAddress: function()
  {
    return this.m_address;
  },
  
  setKey: function(key)
  {
    this.m_key = key;
    
    this.m_keyLabel.innerHTML = "";
    this.m_keyLabel.appendChild(document.createTextNode(key));
    
    return this;
  },
  
  getKey: function()
  {
    return this.m_key;
  },
  
  setIP: function(ip)
  {
    this.m_ip = ip;
    
    this.m_ipLabel.innerHTML = "";
    this.m_ipLabel.appendChild(document.createTextNode(ip));
    
    return this;
  },
  
  getIP: function()
  {
    return this.m_ip;
  },
  
  setState: function(state)
  {

    this.m_stateLabel.innerHTML = "";
    this.m_stateLabel.appendChild(document.createTextNode(state));

    return this;
  },
  
  getElement: function()
  {
    return this.m_element;
  },
  
  determineState: function(status)
  {
    for (i = 0, len = status.length; i < len; i++)
    {
      if (this.m_address == status[i].address)
      {
        return status[i];
      }
    }
    
    return null;
  },
  
  onDelete: function()
  {
    var that = this;
    //new YesNoDialog("Sicherheitsabfrage", "Möchten Sie den HomeMatic Konfigurations-Adapter wirklich löschen?", function(result) {
    new YesNoDialog(translateKey("dialogSafetyCheck"), translateKey("dialogQuestionRemoveCFG"), function(result) {
      
      if (result == YesNoDialog.RESULT_YES)
      {
        BidcosRfPage.removeGateway(that);
      }
    });
  },
  
  onEdit : function()
  {
  	var that = this;
  	new BidcosRfPage.EditGatewayDialog(that).setAction(this.onGatewayEdited, this);
  },

  onGatewayEdited : function(dialog) {
    this.setUserName(dialog.getUserName());
    this.setAddress(dialog.getSerial());
    this.setKey(dialog.getKey());
    this.setIP(dialog.getIPAddress());
  },
  
  onChangeKey : function()
  {
  	var that = this;
  	new BidcosRfPage.ChangeKeyDialog(that).setAction(this.onKeyChanged, this);
  },
  
  keyContainsNoForbiddenCharacter : function(key) {
    if(key.indexOf("<") != -1) { return false; }
    else if(key.indexOf(">") != -1) { return false; }
    else if(key.indexOf("\'") != -1) { return false; }    
    else if(key.indexOf("\"") != -1) { return false; }
    else if(key.indexOf("&") != -1) { return false; }
    else if(key.indexOf("$") != -1) { return false; }
    else if(key.indexOf("?") != -1) { return false; }    
    else if(key.indexOf("[") != -1) { return false; }
    else if(key.indexOf("]") != -1) { return false; }
    else if(key.indexOf("{") != -1) { return false; }
    else if(key.indexOf("}") != -1) { return false; }
    else if(key.indexOf("#") != -1) { return false; }
    else if(key.indexOf("\\") != -1) { return false; }
    else { return true; }
    return false;
  },
  
  onKeyChanged : function(dialog)
  {
    var key = dialog.getKey();
    var key2 = dialog.getKeyRepetition();
    if(key === "") { 
    	MessageBox.show(translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorTitle"), translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorTooShort"));
    }
  	else if(key === key2) { 
  	  if(this.keyContainsNoForbiddenCharacter(key)) {//check if key contains a forbidden character
		  if( homematic("BidCoS.changeLanGatewayKey", {
		    "lgwclass" : this.getGatewayClass(), //RF or Wired
		    "lgwserial": this.getAddress(),
		    "lgwip" : this.getIP(),
		    "newkey" : key,
		    "curkey" : this.getKey()
		  }) ) {
		  	MessageBox.show(translateKey("dialogInfo"), translateKey("dialogSettingsBidcosRFSaveConfigSucceed"));
		  }
		  else {
		  	MessageBox.show(translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorTitle"), translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorGeneral"));
		  }
		}
		else { //!keyContainsNoForbiddenCharacter
			MessageBox.show(translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorTitle"), translateKey("msgForbiddenCharacter"));
		}
	}
  	else {
  	  MessageBox.show(translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorTitle"), translateKey("dialogSettingsBidcosRFChangeAccessCodeDialogErrorDontMatch"));
  	}
  }
  
 
};


BidcosRfPage.Device = function()
{
  this.create();
};
  
BidcosRfPage.Device.prototype =
{
  m_element: null,
  m_name: null,
  m_nameLabel: null,
  m_image: null,
  m_address: null,
  m_addressLabel: null,
  m_gateway: null,
  m_gatewayLabel: null,
  m_roaming: null,
  m_action: null,
  m_configButton: null,
  m_onConfigHandler: null,

  gatewayList : [],

  create: function()
  {

    this.gatewayList = BidcosRfPage.getGateways();

    /* Event-Handler */
    var that = this;
    this.m_onConfigHandler = function() { that.onConfig(); };
    
    /* Tabellenzeile */
    this.m_element = document.createElement("tr");
      
    /* Name */
    this.m_nameLabel = document.createElement("td");
    this.m_nameLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_nameLabel);
      
    /* Bild */
    this.m_image = document.createElement("td");
    this.m_image.className = "bidcosrf_imagecell bidcosrf_imagesize";
    this.m_image.innerHTML = "&nbsp;";
    this.m_element.appendChild(this.m_image);

    /* Seriennummer */
    this.m_addressLabel = document.createElement("td");
    this.m_addressLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_addressLabel);

    /* Gateway */
    this.m_gatewayLabel = document.createElement("td");
    this.m_gatewayLabel.className = "bidcosrf_tablecell";
    this.m_element.appendChild(this.m_gatewayLabel);
      
    /* Aktion */
    this.m_action = document.createElement("td");
    this.m_action.className = "bidcosrf_actioncell";
    this.m_element.appendChild(this.m_action);
    
    /* Button zur Konfiguration */
    this.m_configButton = document.createElement("div");
    this.m_configButton.className = "StdButton";//"bidcosrf_button";
    //this.m_configButton.appendChild(document.createTextNode("Einstellen"));
    this.m_configButton.appendChild(document.createTextNode(translateKey("dialogSettingsBidCosRFConfBtnSet")));
    Event.observe($(this.m_configButton), "click", this.m_onConfigHandler);
    this.m_action.appendChild(this.m_configButton);
  },


  destroy: function()
  {
    Event.stopObserving($(this.m_configButton), "click", this.m_onConfigHandler);
    $(this.m_element).remove();
  },
    
  setName: function(name)
  {
    this.m_name = name;
    
    this.m_nameLabel.innerHtml = "";
    this.m_nameLabel.appendChild(document.createTextNode(name));
    
    return this;
  },
  
  getName: function()
  {
    return this.m_name;
  },
    
  setImageHtml: function(imageHtml)
  {
    var div = document.createElement("div");
    div.innerHTML = imageHtml;
    div.className = "thumbnail";
    
    this.m_image.innerHTML = "";
    this.m_image.appendChild(div);
    
    return this;
  },
    
  setAddress: function(address)
  {
    this.m_address = address;
    
    this.m_addressLabel.innerHtml = "";
    this.m_addressLabel.appendChild(document.createTextNode(address));
    
    return this;
  },
  
  getAddress: function()
  {
    return this.m_address;
  },
   
  setGateway: function(gateway)
  {
    var label = gateway;
    var name = "";

    BidcosRfPage.m_defaultGateway;

    // Has the gateway an user defined name?
    for (var i = 0; i < this.gatewayList.length; i++) {
      if (gateway == this.gatewayList[i].m_address) {
          name = this.gatewayList[i].m_userName;
          label = (name.length > 0) ? name : label;
          break;
      }
    }

    this.m_gateway = label;
    this.m_gatewayLabel.innerHTML = "";
    this.m_gatewayLabel.appendChild(document.createTextNode(label));
    //this.m_gatewayLabel.className += " " + label.replace(/\s+/g,"");
    this.m_gatewayLabel.setAttribute("class", "bidcosrf_tablecell " + label.replace(/\s+/g,""));
    return this;
  },
  
  getGateway: function()
  {
    return this.m_gateway;
  },
  
  setRoaming: function(roaming)
  {
    this.m_roaming = roaming;
    
    return this;
  },
  
  getRoaming: function()
  {
    return this.m_roaming;
  },
  
  getElement: function()
  {
    return this.m_element;
  },
  
  setGatewayFromDeviceList: function(deviceList)
  {
    for (var i = 0, len = deviceList.length; i < len; i++)
    {
      var device = deviceList[i];
      if (device.address == this.m_address)
      {
        this.setGateway(device["interface"]);
        this.setRoaming(device["roaming"]);
      }
    }
    return this;
  },
  
  onConfig: function()
  {
    new BidcosRfPage.EditAssignmentDialog()
      .setDevice(this)
      .setAction(this.saveConfig, this);
  },
  
  saveConfig: function(dialog)
  {
    var gateway = dialog.getGateway();
    var roaming = dialog.getRoaming();
    
    if (gateway)
    {
      homematic("Interface.setBidcosInterface", {
        "interface": "BidCos-RF",
        "deviceId": this.m_address,
        "interfaceId": gateway.getAddress(),
        "roaming": roaming
      });
      //jQuery("."+gateway.getAddress()).attr("class","bidcosrf_tablecell " + gateway.getAddress().replace(/\s+/g,""));
      this.setGateway(gateway.getAddress());
      this.setRoaming(roaming);
    }
  }
};

/*-------------------------------------------------------------------------------------------------------------------*/

/**
* @class BidcosRfPage.EditGatewayDialog
* @brief Dialog zum Bearbeiten von Lan Gateway Einstellungen
**/
BidcosRfPage.EditGatewayDialog = Class.create({


  /** Constructor
  * Fuehrt die Grundinitialisierung durch und zeigt das Dialogfenster an.
  **/	
  initialize: function(gateway)
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = 500;
    var frameHeight  = 200;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);
	var m_gateway = gateway;
  
  	this.m_action = null;//Callback function
  
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer"; 


    //Create content ui-elements
    //Type
    this.m_typeText = new UI.Text()
      .setPosition(180, 10)
      .setWidth(frameWidth - 190)
      .setText(globalLGWTypeMap[m_gateway.getType()]);

    //Name
    this.m_nameTextEdit = new UI.TextEdit()
      .setPosition(180,40)
      .setWidth(frameWidth - 190)
      .setText(m_gateway.getUserName());

    //Serial  
    this.m_serialTextEdit = new UI.TextEdit()
      .setPosition(180, 70)
      .setWidth(frameWidth - 190)
      .setText(m_gateway.getAddress());
	
	//Key    
    this.m_keyTextEdit = new UI.TextEdit()
      .setPosition(180, 100)
      .setWidth(frameWidth - 190)
      .setText(m_gateway.getKey());
    
    //IP Address
    this.m_ipTextEdit = new UI.TextEdit()
      .setPosition(180, 130)
      .setWidth(frameWidth - 190)
      .setText(m_gateway.getIP());

	this.m_keyLabel = new UI.Text();

	//Create and assemble frame
    this.m_frame = new UI.Frame()
      .setTitle(translateKey("dialogSettingsBidcosRFEditGatewayTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      /* Add Cancel Button */
      .add(new UI.Button()
        .setPosition(10, frameHeight - 30)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))
        .setAction(this.cancel, this)
      )
      /* Add OK Button */
      .add(new UI.Button()
        .setPosition(frameWidth - 160, frameHeight - 30)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
      )
      
      //Content:
      //Type label
      .add(new UI.Text().setPosition(10,10).setText(translateKey("dialogSettingsBidCosRFLblType")))
      .add(this.m_typeText) 

      //Name
      .add(new UI.Text()
        .setPosition(10,40)
        .setText(translateKey("dialogSettingsBidCosRFLblName"))
      )
      .add(this.m_nameTextEdit)

      //Serial
      .add(new UI.Text()
        .setPosition(10, 70)
        //.setText("Seriennummer:")
        .setText(translateKey("dialogSettingsBidCosRFLblSN"))
      )
      .add(this.m_serialTextEdit)
      //Accesscode / passphrase
      .add(this.m_keyLabel
        .setPosition(10,100)
        .setText( 
          (m_gateway.getType() == "Lan Interface" ?  translateKey("dialogSettingsBidCosRFLblAccessCode") : translateKey("dialogSettingsBidCosRFLblPassphrase"))
        ) 
      )
      .add(this.m_keyTextEdit)
      //IP label
      .add(this.m_ipTextEdit)
      .add(new UI.Text()
        .setPosition(10,130)
        //.setText("IP-Adresse (optional):")
        .setText(translateKey("dialogSettingsBidCosRFLblIPAddress"))
      );

  
    /* Add frame to layer and layer to Layer */
    this.m_layer.appendChild(this.m_frame.getElement()); 
    Layer.add(this.m_layer);
  },
  
  /**
   * @fn close
   * @brief Schließt das Dialogfenster
   **/
  close: function()
  {
    Layer.remove(this.m_layer);
  },
  
  /** @fn ok
   *  @brief Führt die Callbackfuntion aus nachdem auf "OK" geklickt wurde
   *        und schließt anschließend das Dialogfenster
   **/
  ok: function()
  {
    if (this.m_action) { this.m_action(this); }
    this.close();
  },
  
  /** @fn cancel
   * @brief Schließt das Dialogfenster, nachdem auf "Abbrechen" geklickt wurde 
   **/
  cancel: function()
  {
    this.close();
  },
  
   /**
   * @fn setAction
   * @brief Setzt die Callbackfuntion, die aufgerufen wird, wenn OK-gedrückt wurde
   * @param action  Callbackfuntion "onOK"
   * @param context Optional. Kontext, an die action gebunden wird
   **/
  setAction: function(action, context)
  {
    if (context) { this.m_action = action.bind(context); }
    else         { this.m_action = action; }
    
    return this;
  },

  getUserName: function()
  {
    return this.m_nameTextEdit.getText();
  },

  getSerial: function()
  {
    return this.m_serialTextEdit.getText();
  },
  
  getKey: function()
  {
    return this.m_keyTextEdit.getText();
  },
  
  getIPAddress : function()
  {
    return this.m_ipTextEdit.getText();
  }
  
});

/*-------------------------------------------------------------------------------------------------------------------*/

BidcosRfPage.ChangeKeyDialog = Class.create({
	
  initialize : function(gateway) {
    
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = 500;
    var frameHeight  = 175;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);
	var m_gateway = gateway;
  
  	this.m_action = null;//Callback function
  
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer"; 
    
    this.m_newKeyTextEdit = new UI.TextEdit() 
      .setPosition(180, 40)
      .setWidth(frameWidth - 190);	

    this.m_newKeyTextRepetition = new UI.TextEdit() 
      .setPosition(180, 70)
      .setWidth(frameWidth - 190);
      
    this.m_newKeyRepetitionLabel = new UI.Text();
    this.m_newKeyRepetitionLabel.setPosition(10, 70);
    this.m_newKeyRepetitionLabel.setHtml(translateKey("dialogSettingsBidcosRFChangeAccessCodeNewKeyRepetition"));
	
    
    //Create new frame and add stuff
    this.m_frame = new UI.Frame()
      .setTitle(translateKey("dialogSettingsBidcosRFChangeAccessCodeTitle")) 
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      /* Add Cancel Button */
      .add(new UI.Button()
        .setPosition(10, frameHeight - 30)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))
        .setAction(this.cancel, this)
      )
      /* Add OK Button */
      .add(new UI.Button()
        .setPosition(frameWidth - 160, frameHeight - 30)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
        
      )
      //New key
      .add(new UI.Text()
        .setPosition(10, 40)
        .setText(translateKey("dialogSettingsBidcosRFChangeAccessCodeNewKey"))
      )
      .add(this.m_newKeyTextEdit)
      //Repetition of new key
	  .add(this.m_newKeyRepetitionLabel)
	  .add(this.m_newKeyTextRepetition);
    
    /* Add frame to layer and layer to Layer */
    this.m_layer.appendChild(this.m_frame.getElement()); 
    Layer.add(this.m_layer);
  },
  
  getKey : function() 
  {
    return this.m_newKeyTextEdit.getText();
  },
  
  getKeyRepetition : function()
  {
    return this.m_newKeyTextRepetition.getText();
  },
	
  /**
  * @fn setAction
  * @brief Setzt die Callbackfuntion, die aufgerufen wird, wenn OK-gedrückt wurde
  * @param action  Callbackfuntion "onOK"
  * @param context Optional. Kontext, an die action gebunden wird
  **/
  setAction : function(action, context)
  {
    if (context) { this.m_action = action.bind(context); }
    else         { this.m_action = action; }
    
    return this;
  },
  
    /**
   * @fn close
   * @brief Schließt das Dialogfenster
   **/
  close: function()
  {
    Layer.remove(this.m_layer);
  },
  
  /** @fn ok
   *  @brief Führt die Callbackfuntion aus nachdem auf "OK" geklickt wurde
   *        und schließt anschließend das Dialogfenster
   **/
  ok: function()
  {
    if (this.m_action) { this.m_action(this); }
    this.close();
  },
  
  /** @fn cancel
   * @brief Schließt das Dialogfenster, nachdem auf "Abbrechen" geklickt wurde 
   **/
  cancel: function()
  {
    this.close();
  }

});

/*-------------------------------------------------------------------------------------------------------------------*/


/**
 * @class BidcosRfPage.AddGatewayDialog
 * @brief Dialog zum Hinzufuegen von HM-CFG-LAN-Einstellungen.
 **/
BidcosRfPage.AddGatewayDialog = Class.create({

  /**
   * @constructor
   * Führt eine Grundinitialisierung durch und zeigt das Dialogfenster an.
   **/
  initialize: function(wiredGWExists, gateway)
  {
    var that = this;
    var imageHeight = 48;
    BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT += imageHeight;

    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH;
    var frameHeight  = BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);

    var previewHMLGW2 = "/ise/img/icons_lan_gateways/48/HMLGW2.png",
      previewLANIF = "/ise/img/icons_lan_gateways/48/CFG_LAN.png",
      previewHMWLGW = "/ise/img/icons_lan_gateways/48/HMWLGW.png";

    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";
    
    this.m_action = null;
    this.m_id     = null;


    this.m_gatewayImg = new UI.Image()
      .setPosition(180 + (BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH / 2) ,10)
      .setPath(previewHMLGW2); // As default preselected

    this.m_typeListBox = new UI.ListBox()
      .setPosition(180,10 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190)
      .add({id:globalLGWTypes.HMLGW2, name: "RF: "+globalLGWTypeMap[globalLGWTypes.HMLGW2]})
      .add({id:globalLGWTypes.LANIF, name: "RF: "+globalLGWTypeMap[globalLGWTypes.LANIF]})
      .setOnChangeCallback(function() {
        var gateWay = this.getSelectedItem(),
          gateWayID = gateWay.id,
          jElem = "#bidcosAccessCode";
        var offsetPreviewPic = 180;

        switch (gateWayID) {
          case globalLGWTypes.HMLGW2:
            jQuery(jElem).text(translateKey("dialogSettingsBidCosRFLblPassphrase"));
            that.m_gatewayImg.setPath(previewHMLGW2);
            break;
          case globalLGWTypes.LANIF:
            jQuery(jElem).text(translateKey("dialogSettingsBidCosRFLblAccessCode"));
             that.m_gatewayImg.setPath(previewLANIF);
            break;
          case globalLGWTypes.HMWLGW:
            jQuery(jElem).text(translateKey("dialogSettingsBidCosRFLblPassphrase"));
            that.m_gatewayImg.setPath(previewHMWLGW);
            break;
        }
        that.m_gatewayImg.setPosition(offsetPreviewPic + (BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH / 2),10);
      });
    
    /*Add Wired LGW only if there is no one*/
    if(!wiredGWExists) {
    	this.m_typeListBox.add({id:globalLGWTypes.HMWLGW, name: "Wired: "+globalLGWTypeMap[globalLGWTypes.HMWLGW]});
    }

    this.m_nameTextEdit = new UI.TextEdit()
      .setPosition(180, 40 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190);
    
    this.m_serialTextEdit = new UI.TextEdit()
      .setPosition(180, 70 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190);
    
    this.m_keyTextEdit = new UI.TextEdit()
      .setPosition(180, 70 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190);
    
    this.m_keyTextEdit = new UI.TextEdit()
      .setPosition(180, 100 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190);
      
    this.m_ipTextEdit = new UI.TextEdit()
      .setPosition(180, 130 + imageHeight)
      .setWidth(BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH - 190);
    
    this.m_frame = new UI.Frame()
      //.setTitle("HomeMatic Konfigurations-Adapter hinzufügen")
      .setTitle(translateKey("dialogSettingsBidCosRFAddCFGTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)

      .add(this.m_gatewayImg)

      .add(new UI.Text().setPosition(10,10 + imageHeight).setText(translateKey("dialogSettingsBidCosRFLblType")))

      .add(this.m_typeListBox)

      .add(new UI.Text()
        .setPosition(10,40 + imageHeight)
        .setText(translateKey("dialogSettingsBidCosRFLblName"))
      )
      .add(this.m_nameTextEdit)
      .add(new UI.Text()
        .setPosition(10, 70 + imageHeight)
        //.setText("Seriennummer:")
        .setText(translateKey("dialogSettingsBidCosRFLblSN"))
      )
      .add(this.m_serialTextEdit)
      .add(new UI.Text()
        .setPosition(10, 100 + imageHeight)
        //.setText("Zugriffscode:")
        .setText(translateKey("dialogSettingsBidCosRFLblAccessCode"))
        .setID("bidcosAccessCode")
      )
      .add(this.m_keyTextEdit)
      .add(new UI.Text()
        .setPosition(10,130 + imageHeight)
        //.setText("IP-Adresse (optional):")
        .setText(translateKey("dialogSettingsBidCosRFLblIPAddress"))
      )
      .add(this.m_ipTextEdit)
      .add(new UI.Button()
        .setPosition(10, BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT - 30)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))
        .setAction(this.cancel, this)
      )
      .add(new UI.Button()
        .setPosition(340, BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT - 30)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
      );

    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
    BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT -= imageHeight;
  },

  /**
   * @fn setGateway
   * @brief Setzt die Felder Seriennummer, Zugriffscode und IP-Adresse
   **/
  setGateway: function(gateway)
  {
    this.m_nameTextEdit.setText(gateway.userName);
    this.m_serialTextEdit.setText(gateway.serial);
    this.m_keyTextEdit.setText(gateway.key);
    this.m_ipTextEdit.setText(gateway.ip);
    this.m_id = gateway.id;
    
    return this;
  },
  
  /**
   * @fn setAction
   * @brief Setzt die Callbackfuntion, die aufgerufen wird, wenn OK-gedrückt 
   *        wurde
   *
   * @param action  Callbackfuntion "onOK"
   * @param context Optional. Kontext, an die action gebunden wird
   **/
  setAction: function(action, context)
  {
    if (context) { this.m_action = action.bind(context); }
    else         { this.m_action = action; }
    
    return this;
  },

  getUserName: function()
  {
    return this.m_nameTextEdit.getText();
  },

  /**
   * @fn getSerial
   * @brief Liefert den Inhalt des Feldes Seriennummer
   **/
  getSerial: function()
  {
    return this.m_serialTextEdit.getText().toUpperCase();
  },
  
  /**
   * @fn getKey
   * @brief Liefert den Inhalt des Feldes Zugriffcode
   **/
  getKey: function()
  {
	if((this.getType() == globalLGWTypes.HMWLGW) || (this.getType() == globalLGWTypes.HMLGW2)) {
	    return this.m_keyTextEdit.getText();//Passphrases should not be upper case
	}
	else {
		return this.m_keyTextEdit.getText().toUpperCase();
	}
  },
  
  /**
   * @fn getType
   * @brief Liefert den Inhalt des Feldes Zugriffcode
   **/
  getType: function()
  {
    return this.m_typeListBox.getSelectedItem().id;
  },  
  
  /**
   * @fn getType
   * @brief Liefert den Inhalt des Feldes Zugriffcode
   **/
  getGatewayClass: function()
  {
    var gwClass = globalGWClasses.RF;
    if (this.m_typeListBox.getSelectedItem().id == globalLGWTypes.HMWLGW) {
   		gwClass = globalGWClasses.Wired;
    }
    return gwClass;
  }, 
  
  /**
   * @fn getIp
   * @brief Liefert den Inhalt des Feldes IP-Addresse
   **/
  getIp: function()
  {
    return this.m_ipTextEdit.getText();
  },
  
  /**
   * @fn getId
   * @brief Liefert die Id des momentan bearbeiten Gateways.
   **/
  getId: function()
  {
    return this.m_id;
  },
  
  /**
   * @fn close
   * @brief Schließt das Dialogfenster
   **/
  close: function()
  {
    Layer.remove(this.m_layer);
  },
  
  /**
   * @fn ok
   * @brief Führt die Callbackfuntion aus nachdem auf "OK" geklickt wurde
   *        und schließt anschließend das Dialogfenster
   **/
  ok: function()
  {
    if (this.m_action) { this.m_action(this); }
    this.close();
  },
  
  /**
   * @fn cancel
   * @brief Schließt das Dialogfenster, nachdem auf "Abbrechen" geklickt wurde 
   **/
  cancel: function()
  {
    this.close();
  }

});

BidcosRfPage.AddGatewayDialog.CONTENT_WIDTH  = 500;    //< Breite des Dialogfensters
BidcosRfPage.AddGatewayDialog.CONTENT_HEIGHT = 200; //< Höhe des Dialogfensters


BidcosRfPage.EditAssignmentDialog = Class.create({

  m_gateways: null,
  m_device: null,
  m_action: null,
  m_frame: null,
  m_defaultGateway : null,

  initialize: function()
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = BidcosRfPage.EditAssignmentDialog.CONTENT_WIDTH;
    var frameHeight  = BidcosRfPage.EditAssignmentDialog.CONTENT_HEIGHT;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);
    var gateways     = BidcosRfPage.getGateways();
  
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    this.m_gatewaysListbox = new UI.ListBox()
      .setPosition(100,10)
      .setWidth(BidcosRfPage.EditAssignmentDialog.CONTENT_WIDTH - 110);
      
    this.m_gateways = [];
    
    this.m_gatewaysListbox.add({name: translateKey("dialogSettingsBidcosRFIFaceAssignmentLblStandard"), value: BidcosRfPage.getDefaultGateway()});
    
    for (var i = 0, len = gateways.length; i < len; i++)
    {
      var gateway = gateways[i];
      if(gateway.getGatewayClass() == "RF") {
        //this.m_gateways.push({name: gateway.getAddress(), value: gateway});
        var name = (gateway.getUserName().length > 0) ? gateway.getUserName(): gateway.getAddress();

        this.m_gateways.push({name: name, value: gateway});
        if (this.m_gateways[i]) {
          this.m_gatewaysListbox.add(this.m_gateways[i]);
        }
      }
    }
      
    this.m_roamingCheckbox = new UI.Checkbox()
      //.setText("feste Zuordnung aufheben")
      .setText(translateKey("dialogSettingsBidcosRFIFaceAssignmentLblNullify"))
      .setPosition(100,40)
      .setWidth(BidcosRfPage.EditAssignmentDialog.CONTENT_WIDTH - 110)
      .setIsChecked(true);
      
    
    this.m_frame = new UI.Frame()
      //.setTitle("Interface-Zuordnung: ")
      .setTitle(translateKey("dialogSettingsBidcosRFIFaceAssignmentTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        //.setText("Gateway:")
        .setText(translateKey("dialogSettingsBidcosRFIFaceAssignmentLblGateway"))
        
      )
      .add(this.m_gatewaysListbox)
      .add(this.m_roamingCheckbox)
      .add(new UI.Button()
        .setPosition(10, BidcosRfPage.EditAssignmentDialog.CONTENT_HEIGHT - 30)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))
        .setAction(this.cancel, this)
      )
      .add(new UI.Button()
        .setPosition(240, BidcosRfPage.EditAssignmentDialog.CONTENT_HEIGHT - 30)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
      );
    
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },
  
  setDevice: function(device)
  {
    this.m_device = device;
    
    this.m_frame.setTitle(translateKey("dialogSettingsBidcosRFIFaceAssignmentTitle") + device.getName());
    this.m_roamingCheckbox.setIsChecked(device.getRoaming());
    for (var i = 0, len = this.m_gateways.length; i < len; i++)
    {
      var gateway = this.m_gateways[i];
      if (device.getGateway() == gateway.value.getAddress())
      {
        this.m_gatewaysListbox.selectItem(gateway);
        break;
      }
    }
 
    return this;
  },
  
  setAction: function(action, context)
  {
    if (context) { this.m_action = action.bind(context); }
    else         { this.m_action = action; }
    
    return this;
  },

  getGateway: function()
  {
    return this.m_gatewaysListbox.getSelectedItem().value;
  },
  
  getRoaming: function()
  {
    return this.m_roamingCheckbox.isChecked();
  },
  
  /**
   * @fn close
   * @brief Schließt das Dialogfenster
   **/
  close: function()
  {
    Layer.remove(this.m_layer);
  },
  
  /**
   * @fn ok
   * @brief Führt die Callbackfuntion aus nachdem auf "OK" geklickt wurde
   *        und schließt anschließend das Dialogfenster
   **/
  ok: function()
  {
    if (this.m_action) { this.m_action(this); }
    this.close();
  },
  
  /**
   * @fn cancel
   * @brief Schließt das Dialogfenster, nachdem auf "Abbrechen" geklickt wurde 
   **/
  cancel: function()
  {
    this.close();
  }
  
});

BidcosRfPage.EditAssignmentDialog.CONTENT_WIDTH  = 400; 
BidcosRfPage.EditAssignmentDialog.CONTENT_HEIGHT = 150;

BidcosRfPage.OptionsDialog = Class.create({
  m_passwordEdit: null,
  m_frame: null,

  initialize: function()
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = BidcosRfPage.OptionsDialog.CONTENT_WIDTH;
    var frameHeight  = BidcosRfPage.OptionsDialog.CONTENT_HEIGHT;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);
  
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";
    
    this.m_passwordEdit = new UI.PasswordEdit()
      .setPosition(90, 50)
      .setWidth(BidcosRfPage.OptionsDialog.CONTENT_WIDTH - 100);
    
    this.m_frame = new UI.Frame()
      //.setTitle("Sicherheitsschlüssel: ")
      .setTitle(translateKey("dialogSettingsBidcosRFEnterSecKeyTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        //.setText("Bitte geben Sie den System-Sicherheitsschlüssel ein:")
        .setText(translateKey("dialogSettingsBidcosRFEnterSecKeyContent"))
      )
      .add(this.m_passwordEdit)
      .add(new UI.Button()
        .setPosition(90, BidcosRfPage.OptionsDialog.CONTENT_HEIGHT - 30)
        .setText("OK")
        .setAction(this.ok, this)
      );
    
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },
    
  /**
   * @fn close
   * @brief Schließt das Dialogfenster
   **/
  close: function()
  {
    Layer.remove(this.m_layer);
  },
  
  /**
   * @fn ok
   * @brief Führt die Callbackfuntion aus nachdem auf "OK" geklickt wurde
   *        und schließt anschließend das Dialogfenster
   **/
  ok: function()
  {
    var key = this.m_passwordEdit.getText();
    
    homematic("BidCoS_RF.createKeyFile", { "key": key });
    
    if (homematic("BidCoS_RF.validateKey", {"key": key}))
    {
      //MessageBox.show("Info", "Die Konfiguration wurde an die HomeMatic Zentrale übertragen.\nDie Änderungen werden mit dem nächsten Start der HomeMatic Zentrale wirksam.");
      MessageBox.show(translateKey("dialogInfo"), translateKey("dialogSettingsBidcosRFSaveConfigSucceed"));
    }
    else
    {
      //MessageBox.show("Hinweis", "Der eingegebene Schlüssel entspricht nicht dem aktuellen System-Sicherheitsschlüssel.\nDie Konfiguration wurde an die HomeMatic Zentrale übertragen.\nDie Änderungen werden mit dem nächsten Start der HomeMatic Zentrale wirksam.\nFalls Probleme auftreten, wiederholen Sie ggf. die Eingabe.", null, 320, 120);
      MessageBox.show(translateKey("dialogHint"), translateKey("dialogSettingsBidcosRFSaveConfigFailure"), null, 320, 120);
    }
    
    this.close();
  }
  
  
});

BidcosRfPage.OptionsDialog.CONTENT_WIDTH  = 250; 
BidcosRfPage.OptionsDialog.CONTENT_HEIGHT = 130;
