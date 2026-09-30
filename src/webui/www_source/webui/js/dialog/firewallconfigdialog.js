/**
 * firewallconfigdialog.js
 **/

FirewallConfigDialog = Class.create({

  /**
   * @constructor
   * Erzeugt einen neuen FirewallConfigDialog
   **/
  initialize: function (dlgWoPasswd) {
    var yOffset = 320;

    var screenWidth = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth = FirewallConfigDialog.CONTENT_WIDTH;
    var frameHeight = FirewallConfigDialog.CONTENT_HEIGHT + yOffset;
    var frameX = parseInt((screenWidth - frameWidth) / 2);
    // var frameY = parseInt((screenHeight - frameHeight) / 2);
    var frameY = 10;

    this.dlgWoPasswd = dlgWoPasswd;

    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";
    this.m_layer.style = "overflow:scroll;";

    this.m_portListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 180, 10)
      .setWidth(150)
      .add({id: "RESTRICTIVE", name: translateKey("dialogSettingsFirewallLblPortsRestricted")})
      .add({id: "MOST_OPEN", name: translateKey("dialogSettingsFirewallLblPortsOpen")});

    this.m_xmlrpcListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 180, 80)
      .setWidth(150)
      .add({id: "full", name: translateKey("dialogSettingsFirewallLblFullAccess")})
      .add({id: "restricted", name: translateKey("dialogSettingsFirewallLblRestrictedAccess")})
      .add({id: "none", name: translateKey("dialogSettingsFirewallLblNoAccess")});


    this.m_hmscriptListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 180, 150)
      .setWidth(150)
      .add({id: "full", name: translateKey("dialogSettingsFirewallLblFullAccess")})
      .add({id: "restricted", name: translateKey("dialogSettingsFirewallLblRestrictedAccess")})
      .add({id: "none", name: translateKey("dialogSettingsFirewallLblNoAccess")});

    this.m_mediolaListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 180, 220)
      .setWidth(150)
      .add({id: "full", name: translateKey("dialogSettingsFirewallLblFullAccess")})
      .add({id: "restricted", name: translateKey("dialogSettingsFirewallLblRestrictedAccess")})
      .add({id: "none", name: translateKey("dialogSettingsFirewallLblNoAccess")});

    this.m_portTextArea = new UI.Textarea()
      .setPosition(10, 320)
      .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
      .setHeight(100)
      .setWrap(true)
      .disableResize();

    this.m_ipTextArea = new UI.Textarea()
      .setPosition(10, 200 + yOffset)
      .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
      .setHeight(100)
      .setWrap(true)
      .disableResize();

    this.m_frame = new UI.Frame()
      //.setTitle("Firewall Konfiguration")
      .setTitle(translateKey("dialogSettingsFirewallTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        .setHtml(translateKey("dialogSettingsFirewallLblFWPolicy"))
      )
      .add(this.m_portListBox)
      .add(new UI.Text()
        .setPosition(10, 40)
        .setHtml(translateKey("dialogSettingsFirewallLblHintPortAccess"))
      )
      .add(new UI.Text()
        .setPosition(10, 80)
        //.setHtml("<b>HomeMatic XML-RPC API:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblXMLRPCAPI"))
      )
      .add(new UI.Text()
        .setPosition(10, 110)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Ermöglicht den direkten Zugriff auf angelernte HomeMatic Geräte")
        .setText(translateKey("dialogSettingsFirewallLblHintXMLRPCAPI"))
      )
      .add(this.m_xmlrpcListBox)

      .add(new UI.Image()
        .setPath("/ise/img/help.png")
        .setWidth(25)
        .setHeight(25)
        .setPosition(475, 75)
        .setName("helpToolTip")
      )

      .add(new UI.Text()
        .setPosition(10, 150)
        //.setHtml("<b>Remote HomeMatic-Script API:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblScriptAPI"))
      )
      .add(new UI.Text()
        .setPosition(10, 180)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Ermöglicht den Zugriff auf die Logikschicht der HomeMatic Zentrale")
        .setText(translateKey("dialogSettingsFirewallLblHintScriptAPI"))
      )
      .add(this.m_hmscriptListBox)
      .add(new UI.Image()
        .setPath("/ise/img/help.png")
        .setWidth(25)
        .setHeight(25)
        .setPosition(475, 145)
        .setName("helpToolTip")
      )

      .add(new UI.Text()
        .setPosition(10, 220)
        .setHtml(translateKey("dialogSettingsFirewallLblMediola"))
      )
      .add(new UI.Text()
        .setPosition(10, 250)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        .setText(translateKey("dialogSettingsFirewallLblHintMediola"))
      )
      .add(this.m_mediolaListBox)
      .add(new UI.Image()
        .setPath("/ise/img/help.png")
        .setWidth(25)
        .setHeight(25)
        .setPosition(475, 215)
        .setName("helpToolTip")
      )

      .add(new UI.Text()
        .setPosition(10, 290)
        .setHtml(translateKey("portEnablingTitle"))
      )

      .add(this.m_portTextArea)

      .add(new UI.Text()
        .setPosition(10, 430)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        .setText(translateKey("portEnablingCaption"))
      )

      .add(new UI.Text()
        .setPosition(10, 170 + yOffset)
        //.setHtml("<b>IP-Adressen für den eingeschränkten Zugriff:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblIPAddresses"))
      )
      .add(this.m_ipTextArea)
      .add(new UI.Text()
        .setPosition(10, 310 + yOffset)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Sie können den Zugriff wahlweise für einzelne IP-Adressen (z.B. 192.168.0.1) oder ganze Adressbereiche (z.B. 192.168.0.0/16) freigeben.")
        .setText(translateKey("dialogSettingsFirewallLblHintIPAddresses"))
      )
      .add(new UI.Button()
        .setPosition(10, FirewallConfigDialog.CONTENT_HEIGHT - 30 + yOffset)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))
        .setId("btnCancel")
        .setAction(this.close, this)
      )
      .add(new UI.Button()
        .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 160, FirewallConfigDialog.CONTENT_HEIGHT - 30 + yOffset)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
      );

    this.loadConfiguration();

    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
    this.setTooltips();
  },

  setTooltips: function() {
    var toolTipsElems = jQuery("[name='helpToolTip']");
    toolTipsElems.data('powertip', translateKey("toolTipFirewallAccessModes"));
    toolTipsElems.powerTip({placement: 'se', followMouse: true});
  },

  loadConfiguration: function () {

    var response = homematic("Firewall.getConfiguration");
    if (null != response) {
      var services = response.services,
       ips = response.ips.join(";\n"),
       userPorts = response.userports.join(";\n"),
       firewallPolicy = response.mode,
       xmlrpcAccess = "full",
       hmscriptAccess = "full",
       mediolaAccess = "restricted";

      for (var i = 0, len = services.length; i < len; i++) {
        var service = services[i];
        if (service.id == "XMLRPC") {
          xmlrpcAccess = service.access;
        }
        if (service.id == "REGA") {
          hmscriptAccess = service.access;
        }
        if (service.id == "NEOSERVER") {
          mediolaAccess = service.access;
        }

      }
      this.m_portListBox.selectItemById(firewallPolicy);
      this.m_xmlrpcListBox.selectItemById(xmlrpcAccess);
      this.m_hmscriptListBox.selectItemById(hmscriptAccess);
      this.m_mediolaListBox.selectItemById(mediolaAccess);
      this.m_ipTextArea.setText(ips);
      this.m_portTextArea.setText(userPorts);
    }
  },

  /**
   * Schließt den FirewallConfigDialog ohne Änderungen zu übernehmen.
   **/
  close: function () {
    Layer.remove(this.m_layer);
  },

  /**
   * Übernimmt die Änderungen und schließt den FirewallConfigDialog anschließend.
   **/
  ok: function () {
    var xmlrpcAccess = this.m_xmlrpcListBox.getSelectedItem().id,
    firewallPolicy = this.m_portListBox.getSelectedItem().id,
    hmscriptAccess = this.m_hmscriptListBox.getSelectedItem().id,
    mediolaAccess = this.m_mediolaListBox.getSelectedItem().id,
    ips = this.m_ipTextArea.getText().trim(),
    userPorts = this.m_portTextArea.getText().replace(/\s+/g, '').split(";");

    // Remove a trailing ; at the end of ips
    ips = (ips.charAt(ips.length -1) == ";") ? ips.slice(0,-1) : ips;
    ips = ips.replace(/\s+/g, '').split(";");

    if ((typeof firstStartInstallWizard !== "undefined") && (firstStartInstallWizard)) {
      WebUI.enter(StartPage);
      delete firstStartInstallWizard;
      homematic("CCU.setSecurityHint");
      homematic("CCU.setFirewallConfigured");
      homematic("CCU.setUserAckInstallWizard", {'userName' : userName});
    }

    if (this.dlgWoPasswd ) {
      homematic("CCU.setFirewallConfigured");
    }

    var isOk = true;
    if (ips[0] != "" ) {
      for (var i = 0, len = ips.length; i < len; i++) {
        var ip = ips[i];
        if (! isIPAddressValid(ip)) {
          isOk = false;
        }
      }
    }

    if (isOk) {
      this.close();
      homematic("Firewall.setConfiguration", {
        services: [
          {name: "XMLRPC", access: xmlrpcAccess},
          {name: "REGA", access: hmscriptAccess},
          {name: "NEOSERVER", access: mediolaAccess}
        ],
        ips: ips,
        userports: userPorts,
        mode: firewallPolicy
      });
    }
    else {
      //MessageBox.show("Eingabefehler", "Verwenden Sie bitte nur IP-Adressen im Format \"1.2.3.4\" und Adressgruppen im Format \"1.2.3.0/8\". Separieren Sie die einzelnen Adressen durch Semikola.");
      MessageBox.show(translateKey("dialogSettingsFirewallErrorMsgTitle"), translateKey("dialogSettingsFirewallErrorMsgContent"));

    }
  }

});

FirewallConfigDialog_CCU2 = Class.create({

  /**
   * @constructor
   * Erzeugt einen neuen FirewallConfigDialog
   **/
  initialize: function () {
    var screenWidth = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth = FirewallConfigDialog.CONTENT_WIDTH;
    var frameHeight = FirewallConfigDialog.CONTENT_HEIGHT;
    var frameX = parseInt((screenWidth - frameWidth) / 2);
    var frameY = parseInt((screenHeight - frameHeight) / 2);

    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    this.m_xmlrpcListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 160, 10)
      .setWidth(150)
      //.add({id: "full"      , name: "Vollzugriff"})
      //.add({id: "restricted", name: "eingeschränkt"})
      //.add({id: "none"      , name: "kein Zugriff"});
      .add({id: "full", name: translateKey("dialogSettingsFirewallLblFullAccess")})
      .add({id: "restricted", name: translateKey("dialogSettingsFirewallLblRestrictedAccess")})
      .add({id: "none", name: translateKey("dialogSettingsFirewallLblNoAccess")});


    this.m_hmscriptListBox = new UI.ListBox()
      .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 160, 80)
      .setWidth(150)
      //.add({id: "full"      , name: "Vollzugriff"})
      //.add({id: "restricted", name: "eingeschränkt"})
      //.add({id: "none"      , name: "kein Zugriff"});
      .add({id: "full", name: translateKey("dialogSettingsFirewallLblFullAccess")})
      .add({id: "restricted", name: translateKey("dialogSettingsFirewallLblRestrictedAccess")})
      .add({id: "none", name: translateKey("dialogSettingsFirewallLblNoAccess")});

    this.m_ipTextArea = new UI.Textarea()
      .setPosition(10, 180)
      .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
      .setHeight(100)
      .setWrap(true);

    this.m_frame = new UI.Frame()
      //.setTitle("Firewall Konfiguration")
      .setTitle(translateKey("dialogSettingsFirewallTitle"))
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        //.setHtml("<b>HomeMatic XML-RPC API:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblXMLRPCAPI"))
    )
      .add(new UI.Text()
        .setPosition(10, 40)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Ermöglicht den direkten Zugriff auf angelernte HomeMatic Geräte")
        .setText(translateKey("dialogSettingsFirewallLblHintXMLRPCAPI"))
    )
      .add(this.m_xmlrpcListBox)
      .add(new UI.Text()
        .setPosition(10, 80)
        //.setHtml("<b>Remote HomeMatic-Script API:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblScriptAPI"))
    )
      .add(new UI.Text()
        .setPosition(10, 110)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Ermöglicht den Zugriff auf die Logikschicht der HomeMatic Zentrale")
        .setText(translateKey("dialogSettingsFirewallLblHintScriptAPI"))
    )
      .add(this.m_hmscriptListBox)
      .add(new UI.Text()
        .setPosition(10, 150)
        //.setHtml("<b>IP-Adressen für den eingeschränkten Zugriff:</b>")
        .setHtml(translateKey("dialogSettingsFirewallLblIPAddresses"))
    )
      .add(this.m_ipTextArea)
      .add(new UI.Text()
        .setPosition(10, 290)
        .setWidth(FirewallConfigDialog.CONTENT_WIDTH - 20)
        //.setText("Sie können den Zugriff wahlweise für einzelne IP-Adressen (z.B. 192.168.0.1) oder ganze Adressbereiche (z.B. 192.168.0.0/16) freigeben.")
        .setText(translateKey("dialogSettingsFirewallLblHintIPAddressesWithoutIPv6"))
    )
      .add(new UI.Button()
        .setPosition(10, FirewallConfigDialog.CONTENT_HEIGHT - 30)
        //.setText("Abbrechen")
        .setText(translateKey("btnCancel"))

        .setAction(this.close, this)
    )
      .add(new UI.Button()
        .setPosition(FirewallConfigDialog.CONTENT_WIDTH - 160, FirewallConfigDialog.CONTENT_HEIGHT - 30)
        //.setText("OK")
        .setText(translateKey("btnOk"))
        .setAction(this.ok, this)
    );

    this.loadConfiguration();

    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },

  loadConfiguration: function () {

    var response = homematic("Firewall.getConfiguration");
    if (null != response) {
      var services = response.services,
      ips = response.ips.join(";\n"),
      xmlrpcAccess = "full",
      hmscriptAccess = "full";


      for (var i = 0, len = services.length; i < len; i++) {
        var service = services[i];
        if (service.id == "XMLRPC") {
          xmlrpcAccess = service.access;
        }
        if (service.id == "REGA") {
          hmscriptAccess = service.access;
        }
      }

      this.m_xmlrpcListBox.selectItemById(xmlrpcAccess);
      this.m_hmscriptListBox.selectItemById(hmscriptAccess);
      this.m_ipTextArea.setText(ips);
    }
  },

  /**
   * Schließt den FirewallConfigDialog ohne Änderungen zu übernehmen.
   **/
  close: function () {
    Layer.remove(this.m_layer);
  },

  /**
   * Übernimmt die Änderungen und schließt den FirewallConfigDialog anschließend.
   **/
  ok: function () {
    var xmlrpcAccess = this.m_xmlrpcListBox.getSelectedItem().id,
    hmscriptAccess = this.m_hmscriptListBox.getSelectedItem().id,
    ips = this.m_ipTextArea.getText().replace(/\s+/g, '').split(";"),
    firewallPolicy = "MOST_OPEN",
    userPorts = "";

    var isOk = true;
    if (ips[0] != "" ) {
      for (var i = 0, len = ips.length; i < len; i++) {
        var ip = ips[i];
        if (! isIPAddressValid(ip)) {
          isOk = false;
        }
      }
    }

    if (isOk) {
      this.close();
      homematic("Firewall.setConfiguration", {
        services: [
          {name: "XMLRPC", access: xmlrpcAccess},
          {name: "REGA", access: hmscriptAccess}
        ],
        ips: ips,
        userports: userPorts,
        mode: firewallPolicy
      });
    }
    else {
      //MessageBox.show("Eingabefehler", "Verwenden Sie bitte nur IP-Adressen im Format \"1.2.3.4\" und Adressgruppen im Format \"1.2.3.0/8\". Separieren Sie die einzelnen Adressen durch Semikola.");
      MessageBox.show(translateKey("dialogSettingsFirewallErrorMsgTitle"), translateKey("dialogSettingsFirewallErrorMsgContent"));
    }
  }
});

FirewallConfigDialog.CONTENT_WIDTH = 500;
FirewallConfigDialog.CONTENT_HEIGHT = 400;


