/**
 * webui.js
 * Allgemeine Funktionen der Web-Oberfläche.
 **/
 
WebUI = Singleton.create({  
  HEADER_HEIGHT: 72,   // Höhe der Kopfzeile
  MENUBAR_HEIGHT: 34,   // Höhe der Navigationsleiste
  FOOTER_HEIGHT: 47,   // Höhe der Fußleiste
  BOTTOM_HEIGHT: 27,   // Höhe des weißen Bereichs unter der Seite
  MIN_WIDTH: 750,  
  MIN_HEIGHT: 400,
 
  serviceMessageCount: 0, 

  initialize: function()
  {
    this.STATIC_HEIGHT = this.HEADER_HEIGHT + this.MENUBAR_HEIGHT + this.FOOTER_HEIGHT + this.BOTTOM_HEIGHT;
    
    this.currentPage         = null;    // Aktuelle Seite
    this.currentPageOptions  = null;    // Argumente der aktuellen Seite
    this.previousPage        = null;    // Vorherige Seite
    this.previousPageOptions = null;    // Argumente der vorherigen Seite
    this.onResizeHandler     = null;    // EventHandler: onResize

    this.onResizeHandler = this.onResize.bindAsEventListener(this);
    Event.observe(window, "resize", this.onResizeHandler);
  },  
  
  /**
   * Ereignis. Wird bei der Änderung der Fenstergröße aufgerufen.
   * Passt die Steuerelemente entsprechend der Änderung an.
   */
  onResize: function()
  {
    var height       = WebUI.getHeight();
    var width        = WebUI.getWidth();
    var bodyOverflow = "hidden";

    if (width  < this.MIN_WIDTH)  { width  = this.MIN_WIDTH;  bodyOverflow = "auto"; }
    if (height < this.MIN_HEIGHT) { height = this.MIN_HEIGHT; bodyOverflow = "auto"; }
    var contentHeight = height - this.STATIC_HEIGHT;

    if ($("body"))    { Element.setStyle("body", {"overflow": bodyOverflow, "width": width  + "px", "height": height + "px"}); }
    if ($("header"))  { Element.setStyle("header" , {"height": this.HEADER_HEIGHT  + "px", "width": width + "px"}); }
    if ($("menubar")) { Element.setStyle("menubar", {"height": this.MENUBAR_HEIGHT + "px", "width": width + "px"}); }
    if ($("content")) { Element.setStyle("content", {"height": contentHeight       + "px", "width": width + "px"}); }
    if ($("footer"))  { Element.setStyle("footer" , {"height": this.FOOTER_HEIGHT  + "px", "width": width + "px"}); }

    if (this.currentPage) { this.currentPage.resize(); }

    if(typeof dcTimeout == "undefined") {
      dcTimeout = window.setTimeout(function () {
        showDutyCycle();
        showCarrierSense();
        delete dcTimeout;
      }, 10);
    }
  },

  /*########################*/
  /*# Öffentliche Elemente #*/
  /*########################*/
  
  start: function()
  {

    var isFF = navigator.userAgent.indexOf("Firefox") !== -1;

    if (! ConfigData.isPresent) {
      ConfigData.load();
    } else {
      window.setTimeout(function() {
        jQuery("#PagePath").text(translateKey('startPage')).css('color',"white");
        jQuery("#footer table tbody tr").html("");
      }, 100);
    }
    this.USERNAME = jQuery.trim(homematic('User.getUserFirstName', {'userID': userId}));

    if ((this.USERNAME == "") || (typeof this.USERNAME == "undefined")) {
      this.USERNAME = jQuery.trim(homematic('User.getUserName', {'userID': userId}));
    }

    this.USERLANGUAGE = homematic('User.getLanguage', {'userID': userId});
    this.HIDESTICKYUNREACH = (homematic("CCU.getStickyUnreachState", {}) == 0 ) ? false : true ;
    userIsNoExpert = homematic('User.isNoExpert', {"id": userId});

    // Refresher starten
    iseRefrCycle = 3;
    conInfo("Refreshing started.");
    if (((typeof preventInterfaceCheck == "undefined") || (! preventInterfaceCheck)) && (PLATFORM == 'Central')) {new iseRefresher(iseRefrCycle);}
    iseRefrTimer = 0;
    checkDutyCycleInterval = 60;

    var bodyElem = $("body");

    if (!isFF) {
      bodyElem.innerHTML = "";
    }
    // Dummy-Element
    var dummyElement = document.createElement("div");
    dummyElement.id  = "dummy";
    Element.setStyle(dummyElement, {display:"none"});
    bodyElem.appendChild(dummyElement);

    // Globale Werte (SessionId)
    var globalValues = document.createElement("div");
    globalValues.id = "global_values";
    Element.setStyle(globalValues, {display: "none", visibility: "hidden"});
    var globalValuesForm = document.createElement("form");
    globalValuesForm.action = "#";
    var globalValuesSid = document.createElement("global_sid");
    globalValuesSid.id = "global_sid";
    globalValuesSid.type = "hidden";
    globalValuesSid.name = "sid";
    globalValuesSid.value = SessionId;
    globalValuesForm.appendChild(globalValuesSid);
    globalValues.appendChild(globalValuesForm);
    bodyElem.appendChild(globalValues);

    // picDiv: Vergrößerte Bild von HomeMatic Geräten und Kanälen
    var picDiv = document.createElement("div");
    picDiv.id = "picDiv";
    Element.setStyle(picDiv, {
      position: "absolute",
      left: "0px",
      top: "0px",
      width: "250px",
      height: "250px",
      zIndex: "9999",
      visibility: "hidden",
      margin: "0",
      padding: "0",
      backgroundColor: WebUI.getColor("white")
    });
    bodyElem.appendChild(picDiv);
      jg_250 = new jsGraphics("picDiv");
      InitGD(jg_250, 250);

      // Elemente für Popup-Fenster der ersten Ebene
      var trLayer = document.createElement("div");
      trLayer.id = "trlayer";
      Element.setStyle(trLayer, {
        position: "absolute",
        top: "0",
        left: "0",
        width: "100%",
        height: "100%",
        zIndex: "149", // 259
        backgroundImage: "url('/ise/img/tr50.gif')",
        display: "none"
      });
    bodyElem.appendChild(trLayer);

      var centerBox = document.createElement("div");
      centerBox.id = "centerbox";
      Element.setStyle(centerBox, {
        position: "absolute",
        zIndex: "159",  // 299
        width: "100%",
        top: "50%",
        display: "none"
      });
    bodyElem.appendChild(centerBox);

      var progressBox = document.createElement("div");
      progressBox.id = "progressbox";
      Element.setStyle(progressBox, {
        position: "absolute",
        zIndex: "159",  // 299
        width: "100%",
        top: "50%",
        display: "none"
      });
    bodyElem.appendChild(progressBox);

      // Elemente für Popup-Fenster der zweiten Ebene
      var trLayer2 = document.createElement("div");
      trLayer2.id = "trlayer2";
      Element.setStyle(trLayer2, {
        position: "absolute",
        top: "0",
        left: "0",
        width: "100%",
        height: "100%",
        backgroundImage: "url('/ise/img/tr50.gif')",
        display: "none"
      });
    bodyElem.appendChild(trLayer2);

      var centerBox2 = document.createElement("div");
      centerBox2.id = "centerbox2";
      Element.setStyle(centerBox2, {
        position: "absolute",
        zIndex: "299",
        width: "100%",
        top: "50%",
        display: "none"
      });
    bodyElem.appendChild(centerBox2);

      // Elemente für den Seiteninhalt
      Layer.init();
      var layer0 = document.createElement("div");
      Element.addClassName(layer0, "Layer0");
      Layer.add(layer0);

      var header = document.createElement("div");
      header.id = "header";
      header.lang = this.USERLANGUAGE;
      layer0.appendChild(header);
      loadTextResource();

      var menuBar = document.createElement("div");
      menuBar.id = "menubar";
      layer0.appendChild(menuBar);

      var content = document.createElement("div");
      content.id = "content";
      Element.setStyle(content, {cursor: "wait"});
      layer0.appendChild(content);

      var footer = document.createElement("div");
      footer.id = "footer";
      layer0.appendChild(footer);

      // Weitere Elemente
      var popupContainer = document.createElement("div");
      popupContainer.id = "popup_container";
      Element.setStyle(popupContainer, {display: "none"});
    bodyElem.appendChild(popupContainer);

      var canvas = document.createElement("div");
      canvas.id = "canvas";
      Element.setStyle(canvas, {display: "none"});
    bodyElem.appendChild(canvas);


      jQuery(".Layer0").hide();
      HeaderBar.load();

      if (!forceUpdate) {
        jQuery("#AlarmServiceMsg").show();
        MainMenu.create($("menubar"));
      }

      //    MainMenu.show();
      //    mainMenu = createMainMenu($("menubar"));
      $("content").style.cursor = "default";


      var fileSecHint = "/etc/config/userAckSecurityHint",
        result = homematic('CCU.existsFile', {'file': fileSecHint}),
        securityDialogDisplayed = false;



    if (result) {
      WebUI.resize();

      if (!isFF) {
        jQuery(".Layer0").show();
        WebUI.enter(StartPage);
      } else {
        WebUI.enter(StartPage);
        jQuery(".Layer0").fadeIn(1000, function() {jQuery("#webuiloader_wrapper").remove();});
      }

    } else {
      showSecurityDialog();
      securityDialogDisplayed = true;
    }
    if (PLATFORM == 'Central') {
      if ((typeof preventInterfaceCheck == "undefined") || (! preventInterfaceCheck))
      {
        regaMonitor = new ReGaMonitor();
        InterfaceMonitor.start();
      }
      homematic.com.init();

      // Initialize the clickhandler of the duty cycle element
      window.setTimeout(function () {
        showDutyCycle();
      }, 1);

      // Check the dutyCycle and carrierSense periodically
      new PeriodicalExecuter(function () {
        showDutyCycle();
        showCarrierSense();
      }, checkDutyCycleInterval);

      if (getProduct() >= 3) {

        /* See SPHM-566
        if (!homematic('CCU.existsFile', {'file': LegacyAPIMigrationDialog.CONFIRM_FILE})) {

          // Get the number of HmIP devices without the HmIP-RCV-50
          var countHmIPDevices = 0;
          homematic("Interface.listDevices", {"interface": "HmIP-RF"}, function (deviceList) {
            if (deviceList) {
              for (var i = 0; i < deviceList.length; i++) {
                var device = deviceList[i];
                if (device.children.length > 0 && (device.type != "HmIP-RCV-50")) {
                  countHmIPDevices++;
                }
              }

              // When the number of HmIP devices is > 0 (this is not the case with a factory new CCU) then show the HmIP-RCV-50 migration hint.
              if (countHmIPDevices > 0) {
                MessageBox.show(translateKey("dialogMigrationRCV50Title"), translateKey("dialogMigrationRCV50"), function () {
                  homematic("CCU.createFile", {'file': LegacyAPIMigrationDialog.CONFIRM_FILE}, function (result) {
                    conInfo("createFile " + LegacyAPIMigrationDialog.CONFIRM_FILE + " - result: " + result);
                  });
                }, 600, 200);
              } else {
                homematic("CCU.createFile", {'file': LegacyAPIMigrationDialog.CONFIRM_FILE}, function (result) {
                  conInfo("createFile " + LegacyAPIMigrationDialog.CONFIRM_FILE + " - result: " + result);
                });
              }
            }
          });
        }
        */

        var usrPwd = homematic('User.hasUserPWD', {'userID': userId});
        if (usrPwd == false) {
          var result = homematic('CCU.existsFile', {'file': "/etc/config/userprofiles/userAckInstallWizard_" + userName.replace(" ", ";")});
          if (!result) {
            var checkUpdateContentRunning = window.setInterval(function () {
              if (!bUpdateContentRunning) {
                clearInterval(checkUpdateContentRunning);
                new InstallWizard(getUPL());
              }
            }, 100);
          } else if (!homematic('CCU.existsFile', {'file': "/etc/config/firewallConfigured"})) {
            new DialogChooseSecuritySettings(true);
          }
        } else {
          // Admin password set
          // Firewall settings not yet set (default)
          if ((getUPL() == UPL_ADMIN) && (!homematic('CCU.existsFile', {'file': "/etc/config/firewallConfigured"}))) {
            new DialogChooseSecuritySettings(true);
          }

          // User password set
          // The User will see a hint that new firewall settings are active
          if ((getUPL() == UPL_USER) && (!homematic('CCU.existsFile', {'file': "/etc/config/userprofiles/userAckInstallWizard_" + userName.replace(" ", ";")}))) {
            new MessageBox.show(translateKey("dglUserNewFwSettingsTitle"), translateKey("dglUserNewFwSettingsContent"));
            homematic("CCU.setUserAckInstallWizard", {'userName': userName});
          }
        }
      }
    } else {
      WebUI.serviceMessageCount = setServiceMessage();
      new PeriodicalExecuter(function () {
        var newServiceMessageCount = setServiceMessage();
        if ((newServiceMessageCount != WebUI.serviceMessageCount) && (WebUI.currentPage == ServiceMessagesPage)) {
          WebUI.serviceMessageCount = newServiceMessageCount;
          WebUI.reload();
        }
      }, 5);
    }
  },
  
  /**
   * Read-Only. Breite des Browserfensters (Pixel).
   */
  getWidth: function()
  {
    var width = 0;
  
    if (window.innerWidth)
    {
      width = window.innerWidth;
    }
    else if ((window.document.documentElement) &&
           (window.document.documentElement.clientWidth))
    {
      width = window.document.documentElement.clientWidth;
    }
    else
    {
      width = window.document.body.offsetWidth;
    }
  
    return width;
  },
  
  /**
   * Read-Only. Höhe des Browserfensters (Pixel).
   */
  getHeight: function()
  {
    var height = 0;
  
    if (window.innerHeight)
    {
      height = window.innerHeight;
    }
    else if ((window.document.documentElement) && 
           (window.document.documentElement.clientHeight))
    {
      height = window.document.documentElement.clientHeight;
    }
    else
    {
      height = window.document.body.offsetHeight;
    }
  
    return height;  
  },
  
  setContent: function(contentElement)
  {
    var contentBody = $("content");
    if (contentBody)
    {
      contentBody.innerHTML = "";
      contentBody.appendChild(contentElement);
    }
    else
    {
      throw new Error("content not defined");
    }
  },
  /**
   * Ruft das onResize-Ereignis auf.
   **/
  resize: function()
  {
    this.onResize();
  },
  
  /**
   * Lädt eine Seite.
   */
  enter: function(page, options)
  {
    Debug.assert(Interface.isImplemented(page, IPage), "WebUI.enter: Interface IPage not implemented");
    
    if (this.currentPage) { this.currentPage.leave(); }
    
    this.previousPage        = this.currentPage;
    this.previousPageOptions = this.currentPageOptions;
    this.currentPage         = page;
    this.currentPageOptions  = options;
    
    this.currentPage.enter(options);
  },
  
  reload: function()
  {
    if (this.currentPage)
    {
      this.currentPage.leave();
      this.currentPage.enter(this.currentPageOptions);
    }
  },
  
  /**
   * Zurück zur vorherigen Seite.
   */
  goBack: function()
  {
    if (null !== this.previousPage) { this.enter(this.previousPage, this.previousPageOptions); }
  },
  
  /**
   * @var m_isColorMapLoaded
   * @brief [intern] Gibt an, ob die Farbtabelle bereits geladen ist
   **/
  m_isColorMapLoaded: false,
  
  /**
   * @var m_colorMap
   * @brief [intern] Farbtabelle
   **/
  m_colorMap: {},

  /**
   * @fn getColor
   * @brief Liefert den Wert einer Systemfarbe anhand ihrer Id.
   *
   * @param  colorId [string] Id des Systemfarbe
   * @return [string] Farbwert
   **/
  getColor: function(colorId)
  {
    if (false == this.m_isColorMapLoaded)
    {
      this.m_loadColorMap();
    }
  
    var color = this.m_colorMap[colorId];
  
    if (typeof(color) != "string")
    {
      throw new Error("WebUI.getColor: unknown color id (" + colorId  + ")");
    }
  
    return color;
  },
  
  /**
   * @fn m_loadColorMap
   * @brief [intern] Lädt die Farbtabelle
   **/
  m_loadColorMap: function()
  {
    this.m_colorMap = homematic("WebUI.getColors");
    this.m_isColorMapLoaded = true;
  }
 
});
