InstallWizard = Class.create({
  initialize: function (userLevel) {
    firstStartInstallWizard = true;

    if (userLevel == UPL_ADMIN) {
      WebUI.enter(adminFirstStartup);
    }
    if (userLevel == UPL_USER) {
      WebUI.enter(userFirstStartup);
    }
  }
});

DialogUserPassword = Class.create({
  initialize: function () {
    var self=this;
    this.dlg = new YesNoDialog(translateKey("securitySettingsTitle"), this._getHTMLBody(),function(btnPress) {

      if (btnPress == YesNoDialog.RESULT_NO) {
        // Cancel pressed
          // Don't set the user password when Cancel is pressed
        passWord = "";
        saveUserPassword();
      }
    }, "html");

    this.passChanged = false;
    this.elmBtnNext = jQuery('.YesNoDialog_yesButton')[0];
    this.elmPwd1 = jQuery('#pwd_1');
    this.elmPwd2 = jQuery('#pwd_2');
    this.elmPwdError = jQuery('#pwdError');
    //this.dlg.btnTextNo(translateKey("footerBtnCancel"));
    this.dlg.btnNoHide();
    this.dlg.btnTextYes(translateKey("btnNext"));

    this.dlg.yes = function() {
      var pwdOK = self._checkPassword();

      if (pwdOK) {
        this.close(YesNoDialog.RESULT_YES);
        self._saveUserPassword();

        if (homematic('CCU.existsFile', {'file': "/etc/config/firewallConfigured"})) {
          WebUI.enter(StartPage);
          homematic("CCU.setUserAckInstallWizard");

          if (getUPL() == UPL_USER) {
            new MessageBox.show(translateKey("dglUserNewFwSettingsTitle"),translateKey("dglUserNewFwSettingsContent"));
          }

        } else {
          if(getUPL() == UPL_ADMIN) {
            new DialogChooseSecuritySettings();
          } else {
            WebUI.enter(StartPage);
            homematic("CCU.setUserAckInstallWizard");
          }
        }
      }
    };

    this.dlg.no = function() {
      this.close(YesNoDialog.RESULT_NO);
      delete firstStartInstallWizard;
    };
  },

  _checkPassword: function() {
    var pwd1 = this.elmPwd1.val(),
      pwd2 = this.elmPwd2.val();

    var pwdError_0 = translateKey("noPasswdSet"),
      pwdError_1 = translateKey("passwdNotIdentical");

    if( !isPasswordAllowed( pwd1, 0 ) ) {
      this.elmPwd1.val("");
      this.elmPwd2.val("");
      return false;
    }

    if (pwd1 == "") {
      this._showErrorMessage(pwdError_0);
      return false;
    }

    if (this.elmPwd1.val() != this.elmPwd2.val()) {
      this._showErrorMessage(pwdError_1);
      return false;
    } else {
      this._noErrorMessage();
    }
    return true;

  },

  _showErrorMessage: function(msg) {
    this.elmPwdError.removeClass('visibilityHidden').html(msg);
  },

  _noErrorMessage: function() {
    this.elmPwdError.addClass('visibilityHidden');
  },

  _saveUserPassword: function() {
    if (this.passChanged) {
      passWord = this.elmPwd1.val();
      saveUserPassword();
    }
  },

  _getHTMLBody: function () {
    var self = this;
    onPw1Change = function() {
      self.passChanged = true;
    };

    var html = "";
    html += "<table align='center'>";

      html += "<tr>";
        html += "<td colspan='2'>";
        if (getUPL() == UPL_ADMIN) {
          html += translateKey('dlgAdminPasswdHint1');
        } else {
          html += translateKey('dlgUserPasswdHint1');
        }
        html += "</td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr align='center'>";
        if (getUPL() == UPL_ADMIN) {
          html += "<td colspan='2' style='padding-bottom: 20px;'>" + translateKey('adminPasswordTitle') + "</td>";
        } else {
          html += "<td colspan='2' style='padding-bottom: 20px;'>" + translateKey('userPasswordTitle') + "</td>";
        }
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
          html += translateKey("lblUserPassword");
        html += "</td>";
        html += "<td>";
          html += "<input id='pwd_1' onchange='onPw1Change()' type='password' value="+passWord+">";
        html += "</td>";
      html += "</tr>";
      html += "<tr>";
        html += "<td>";
          html += translateKey("lblUserPasswordRepeat");
        html += "</td>";
        html += "<td>";
         html += "<input id='pwd_2' type='password' value="+passWord+">";
        html += "</td>";

      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2' class='visibilityHidden attention' id='pwdError'>noError</td>";
      html += "</tr>";

    html += "</table>";

    return html;
  }
});

DialogChooseSecuritySettings = Class.create({
  initialize: function(dlgWoPasswd) {
    var self = this;
    this.mode;
    this.dlgWoPasswd = dlgWoPasswd;

    firstStartInstallWizard = ((typeof firstStartInstallWizard == "undefined") || (!firstStartInstallWizard)) ? false : true;

    this.dlg = new YesNoDialog(translateKey("securitySettingsTitle"), this._getHTMLBody(),function(btnPress) {
      if (btnPress == YesNoDialog.RESULT_NO) {
        // Back pressed
        if (firstStartInstallWizard) {
          new DialogUserPassword();
        } else {WebUI.enter(SystemControlPage);}
      }

      if (btnPress == YesNoDialog.RESULT_YES) {
        // Next pressed
        switch (self.mode) {
          case "express":
            new DialogExpressSettings(self.dlgWoPasswd);
            break;
          case "user":
            new DialogUserDefinedSettings(self.dlgWoPasswd);
            break;
          default: "express";
        }
      }


    }, "html");

    this.dlg.setWidth(400);
    this.dlg.btnTextNo(translateKey("footerBtnPageBack"));
    this.dlg.btnTextYes(translateKey("btnNext"));

    if (this.dlgWoPasswd) {
      this.dlg.btnNoHide();
    }

  },

  _getHTMLBody: function() {
    var self = this;
    this.mode = "express"; // Default
    setSelectedMode = function() {
      self.mode = jQuery("input[name='secSettings']:checked").val();
    };

    var html = "";
    html += "<table align='center'>";

      html += "<tr>";
        html += "<td colspan='2'>";
          html += translateKey("lblSetting");
        html += "</td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td><input type='radio' id='optionExpress' name='secSettings' value='express' onchange='setSelectedMode();' checked><label for='optionExpress'>"+translateKey('dialogSetSecurityLevelExpress')+"</label></td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td><input type='radio' id='optionUserDef' name='secSettings' value='user' onchange='setSelectedMode();' ><label for='optionUserDef'>"+translateKey('dialogSetSecurityLevelUserDefined')+"</label></td>";
      html += "</tr>";

    html += "</table>";
      return html;
  }

});

DialogExpressSettings = Class.create({
  initialize: function(dlgWoPasswd) {
    var self = this;
    var tmpMode = this._getSecurityLevel(); // Read current security level for setting the radio box.

    this.idLOW = "LOW";
    this.idMID = "MEDIUM";
    this.idHIGH= "HIGH";
    this.idCUSTOM= "CUSTOM";
    this.modeHasChanged = false;
    this.dlgWoPasswd = dlgWoPasswd;

    this.mode = (tmpMode != null) ? tmpMode : this.idHIGH;

    this.dlg = new YesNoDialog(translateKey("securitySettingsTitle"), this._getHTMLBody(),function(btnPress) {

      if (btnPress == YesNoDialog.RESULT_NO) {
        // Back pressed
        new DialogChooseSecuritySettings();
      }

      if (btnPress == YesNoDialog.RESULT_YES) {
        // Next pressed
        var newMode = self._getSelectedLevel();

        if (tmpMode == null || self.modeHasChanged) {
          conInfo("Write new level: " + newMode);
          self._setSecurityLevel(newMode);
        } else {
          conInfo("The mode hasn't changed!");
        }

        if ((typeof firstStartInstallWizard !== "undefined") && (firstStartInstallWizard)) {
          WebUI.enter(StartPage);
          delete firstStartInstallWizard;
          homematic("CCU.setSecurityHint");
          homematic("CCU.setFirewallConfigured");
          homematic("CCU.setUserAckInstallWizard");
        }

        if (self.dlgWoPasswd) {
          homematic("CCU.setFirewallConfigured");
        }

        if (newMode == self.idCUSTOM) {
          new DialogUserDefinedSettings(self.dlgWoPasswd);
        }

      }
    }, "html");
    this.dlg.setWidth(400);
    this._initRadioGroup();
    this.dlg.btnTextNo(translateKey("footerBtnPageBack"));
    this.dlg.btnTextYes(translateKey("btnOk"));
    this._setHelp();
  },

  _initRadioGroup: function () {
    var elm = jQuery("[type=radio]");
    elm.val([this.mode]);
    setSelectedMode();
  },

  _getSelectedLevel: function () {
    return this.mode;
  },

  _setSecurityLevel: function (mode) {
    var self = this;
    homematic("CCU.setSecurityLevel", {"level": mode}, function() {
      if (mode != self.idCUSTOM) {
        homematic("User.restartLighttpd", {}, function () {
          conInfo("Lighttpd restarted");
        });
      }
    });
  },

  _getSecurityLevel: function () {
    // Returns null when the
    return homematic("CCU.getSecurityLevel");
  },

  _getHTMLBody: function() {
    var self = this;

    setSelectedMode = function() {
      var secLevelHighElm = jQuery("[name='secLevelHigh']"),
      secLevelMidElm = jQuery("[name='secLevelMid']"),
      secLevelLowElml = jQuery("[name='secLevelLow']");

      self.mode = jQuery("input[name='secMode']:checked").val();
      self.modeHasChanged = true;

      switch (self.mode) {
        case "HIGH":
          secLevelMidElm.hide();
          secLevelLowElml.hide();
          secLevelHighElm.show();
          break;
        case "MEDIUM":
          secLevelHighElm.hide();
          secLevelLowElml.hide();
          secLevelMidElm.show();
          break;
        case "LOW":
          secLevelHighElm.hide();
          secLevelMidElm.hide();
          secLevelLowElml.show();
          break;
      }
      self.dlg.resetHeight();
    };

    var helpStyle = "cursor: pointer; width:25px; height:25px; position:relative; top:2px";

    var html = "";
    html += "<table align='center'>";

      html += "<tr>";
        html += "<td colspan='2'>"+translateKey('secLevelUserHint')+"</td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'>";
          html += translateKey('dlgSecurityLevelHeader');
        html += "</td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr align='left'>";
        html += "<td style='width:50%'><input type='radio' id='secHigh' name='secMode' value='HIGH' onchange='setSelectedMode();' checked><label for='secHigh'>"+translateKey('secLevelHigh')+"</label></td>";
        html += "<td style='width:50%; text-align:center'><div><img id=\"showHelpSecurityHigh\" src=\"/ise/img/help.png\" style='"+helpStyle+"'></div></td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td colspan='2'>";
          html += "<table>";
            html += "<tr name='secLevelHigh' class='hidden'>";
              html += "<td style='font-weight: normal'>"+translateKey('secLevelHighCaptionA')+"</td>";
            html += "</tr>";
            html += "<tr name='secLevelHigh' class='hidden'>";
              html += "<td style='font-weight: normal'>"+translateKey('secLevelHighCaptionB')+"</td>";
            html += "</tr>";
          html += "</table>";
        html += "<td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr align='left'>";
        html += "<td style='width:50%'><input type='radio' id='secMed' name='secMode' value='MEDIUM' onchange='setSelectedMode();' ><label for='secMed'>"+translateKey('secLevelMid')+"</label></td>";
        html += "<td style='width:50%; text-align:center'><div><img id=\"showHelpSecurityMid\" src=\"/ise/img/help.png\" style='"+helpStyle+"'></div></td>";
      html += "</tr>";
      html += "<tr>";
        html += "<td colspan='2'>";
          html += "<table>";
            html += "<tr name='secLevelMid' class='hidden'>";
              html += "<td style='font-weight: normal'>"+translateKey('secLevelMidCaption')+"</td>";
            html += "</tr>";
          html += "</table>";
        html += "<td>";
      html += "</tr>";

      html += "<tr align='center'>";
        html += "<td colspan='2'><hr></td>";
      html += "</tr>";

      html += "<tr align='left'>";
        html += "<td style='width:50%'><input type='radio' id='secLow' name='secMode' value='LOW' onchange='setSelectedMode();' ><label for='secLow'>"+translateKey('secLevelLow')+"</label></td>";
        html += "<td style='width:50%; text-align:center'><div><img id=\"showHelpSecurityLow\" src=\"/ise/img/help.png\" style='"+helpStyle+"'></div></td>";
      html += "</tr>";
      html += "<tr>";
        html += "<td colspan='2'>";
          html += "<table>";
            html += "<tr name='secLevelLow' class='hidden'>";
              html += "<td style='font-weight: normal'>";
                html += translateKey("secLevelLowCaptionA");
              html += "</td>";
            html += "</tr>";

          html += "</table>";
        html += "<td>";
      html += "</tr>";

      if(self.mode == self.idCUSTOM) {

        html += "<tr align='center'>";
          html += "<td colspan='2'><hr></td>";
        html += "</tr>";

        html += "<tr align='left'>";
          html += "<td style='width:50%'><input type='radio' id='secCustom' name='secMode' value='CUSTOM' onchange='setSelectedMode();' ><label for='secCustom'>"+translateKey('secLevelCustom')+"</label></td>";
          html += "<td style='width:50%; text-align:center'><div><img id=\"showHelpSecurityCustom\" src=\"/ise/img/help.png\" style='"+helpStyle+"'></div></td>";
        html += "</tr>";
        html += "<tr>";
          html += "<td colspan='2'>";
            html += "<table>";
              html += "<tr name='secLevelCustom'>";
                html += "<td style='font-weight: normal'>";
                  html += "";
                html += "</td>";
              html += "</tr>";
            html += "</table>";
          html += "<td>";
        html += "</tr>";
      }
    html += "</table>";
    return html;
  },

  _setHelp: function() {
    var helpContainer = ["#showHelpSecurityHigh","#showHelpSecurityMid","#showHelpSecurityLow","#showHelpSecurityCustom"];
    var help = [
      translateKey("showHelpSecurityHigh"),
      translateKey("showHelpSecurityMid"),
      translateKey("showHelpSecurityLow"),
      translateKey("showHelpSecurityCustom")
      ];

    jQuery.each(helpContainer, function(index, container){
     var element = jQuery(container);
     var tooltip = help[index];
     element.data('powertip', tooltip);
     element.powerTip({placement: 'se', followMouse: true});
    });
  }

});

DialogUserDefinedSettings = Class.create({
  initialize: function(dlgWoPasswd) {
    var self = this;
    this.dlg = new FirewallConfigDialog(dlgWoPasswd);

    window.setTimeout(function() {
      var btnCancel = jQuery("#btnCancel");
      btnCancel.click(function () {
        Layer.remove(self.dlg.m_layer);
        new DialogChooseSecuritySettings();
      });
    },100);
  }
});
