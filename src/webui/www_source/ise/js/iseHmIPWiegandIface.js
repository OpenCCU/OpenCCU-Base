iseHmIPWiegandIface = Class.create();
iseHmIPWiegandIface.prototype = {

  initialize: function (opts) {
    conInfo("iseHmIPWiegandIface");

    this.opts = opts;

    this.iface = this.opts.chInterface;
    this.chAddress = this.opts.chnAddress;
    this.deviceType = this.opts.deviceType;

    this.chnid = this.opts.chnID;

    this.idCodeID = this.opts.idCodeID;
    this.anchorCodeCommandElm = jQuery("#anchorCodeCommand_" + this.chnid);
    this.anchorClearErrorElm = jQuery("#anchorClearError_" + this.chnid);

    this.selectedCodeIDElm;
    this.selectedCodeID = 0;
    this.selectedCommandMode = 1;

    this.selectedClearError;

    this.initBtnElms();
  },

  initBtnElms: function () {
    var self = this;
    this.anchorCodeCommandElm.click(function() {self._setCodeCommand();});
    this.anchorClearErrorElm.click(function() {self._clearError();});
  },

  _setCodeCommand: function() {
    // open Dialog for setting one of the following code commands
    // ERASE, START_OF_LEARN, STOP_OF_LEARN
    var self = this;
    dlg = new YesNoDialog(translateKey("dialogCodeCommandTitle"), this._getHTMLSetCodeCommand(), function(result) {
      if (result == YesNoDialog.RESULT_YES) {

        var codeCommand = parseInt(self.selectedCommandMode),
          codeID = parseInt(self.selectedCodeID);


        // Transmit the code command incl. codeID - putPutparamset
        console.log("interface: " + self.iface, "chAddress: " + self.chAddress);
        console.log("codeID: " + codeID, "codeCommand: " + codeCommand);

        homematic("Interface.putParamset",{'interface': self.iface, 'address' : self.chAddress, 'paramsetKey' : 'VALUES', 'set':
            [
              {name:'CODE_COMMAND', type: 'int', value: codeCommand},
              {name:'CODE_ID', type: 'int', value: codeID}
            ]
        },function(result){console.log(result);});

      }
    }, "html");
    dlg.btnTextNo(translateKey("btnCancel"));
    dlg.btnTextYes(translateKey("btnOk"));
  },
  _getHTMLSetCodeCommand: function () {
    // ERASE, START_OF_LEARN, STOP_OF_LEARN
    var self = this;

    getCodeID = function(elm) {
      jQuery("[name='codeID']").prop("checked", false);
      self.selectedCodeIDElm = jQuery(elm);
      self.selectedCodeIDElm.prop("checked",true);
      self.selectedCodeID = parseInt(self.selectedCodeIDElm.val());
      //console.log("selected val: " + self.selectedCodeID);
    };

    setCommandMode = function(val) {
      self.selectedCommandMode = parseInt(val);
      //console.log("selected command: " + self.selectedCommandMode);
    };

    var result = "";

    result = "<div style='width: 500px; margin: 0 auto;'>";

      result += "<div>"+translateKey('helpFWICodeCommand')+"</div>";
      result += "<hr>";

      result += "<table class='alignCenter'>";
        result += "<tr>";
          for (var loop = 1; loop < 21; loop++) {
            result += "<td>"+loop+"</td>";
          }
        result += "</tr>";

        result += "<tr>";
          for (var loop = 1; loop < 21; loop++) {
            result += "<td><input type='checkbox' name='codeID' value='"+loop+"' onclick='getCodeID(this);'></td>";
          }
        result += "</tr>";
      result += "</table>";

       result += "<hr>";

      result += "<table>";
        result += "<tr>";
          result += "<td style='padding-right:20px;'>"+translateKey('lblFWISetCodeCommand')+"</td>";
          result += "<td>";
            result += "<select onclick='setCommandMode(this.value);'>";
              result += "<option value='1'>"+translateKey('codeStartOfLearn')+"</option>";
              result += "<option value='2'>"+translateKey('codeStopOfLearn')+"</option>";
              result += "<option value='0'>"+translateKey('codeErase')+"</option>";
            result += "</select>";
          result += "</td>";
        result += "</tr>";
      result += "</table>";
    result += "</div>";


    return result;
  },

  _clearError: function() {
    var self = this;
    // open Dialog with a selection option for
    // SABOTAGE_STICKY, BLOCKED_TEMPORARY, BLOCKED_PERMANENT, ALL
    dlg = new YesNoDialog(translateKey("dialogClearErrorTitle"), this._getHTMLClearError(), function(result) {
      if (result == YesNoDialog.RESULT_YES) {
        var clearError = parseInt(self.selectedClearError);
        conInfo("Send Clear error: " + clearError);
        homematic("Interface.putParamset",{'interface': self.iface, 'address' : self.chAddress, 'paramsetKey' : 'VALUES', 'set':
            [
              {name:'CLEAR_ERROR', type: 'int', value: clearError}
            ]
        },function(result){self.selectedClearError = 4;});
      } else {
        //self.selectedClearError = 4; Is this necessary?
      }
    }, "html");
    dlg.btnTextNo(translateKey("btnCancel"));
    dlg.btnTextYes(translateKey("btnOk"));
  },

  _getHTMLClearError: function() {
    var self = this;
    clearError = function (val) {
      self.selectedClearError = parseInt(val);
      //console.log("selected command: " + self.selectedCommandMode);
    };

    // SABOTAGE_STICKY, BLOCKED_TEMPORARY, BLOCKED_PERMANENT, ALL
    var result = "";
    if (this.deviceType == "HmIP-FWI") {
      result += "<div>" + translateKey('helpFWIClearError') + "</div>";
    } else {
      // e. g. HmIP-WKP
      result += "<div>" + translateKey('helpClearError') + "</div>";
    }
    result += "<hr>";

    result += "<table>";
      result += "<tr>";
        result += "<td style='padding-right:20px;'>"+translateKey('lblFWIClearError')+"</td>";
        result += "<td>";
          result += "<select onclick='clearError(this.value);'>";
          if (this.deviceType == "HmIP-FWI") {
            self.selectedClearError = 3;
            result += "<option value='3'>" + translateKey('clearAll') + "</option>";
            //result += "<option value='0'>" + translateKey('stringTableSabotageContactWasActive') + "</option>";
            result += "<option value='0'>" + translateKey('stringTableSabotage') + "</option>";
            result += "<option value='1'>" + translateKey('stringTableBlockedTemporarily') + "</option>";
            result += "<option value='2'>" + translateKey('stringTableBlockedPermanently') + "</option>";
          } else {
            self.selectedClearError = 4;
            result += "<option value='4'>" + translateKey('clearAll') + "</option>";
            //result += "<option value='0'>" + translateKey('stringTableSabotage') + "</option>";
            //result += "<option value='1'>" + translateKey('stringTableSabotageContactWasActive') + "</option>";
            result += "<option value='1'>" + translateKey('stringTableSabotage') + "</option>";
            result += "<option value='2'>" + translateKey('stringTableBlockedTemporarily') + "</option>";
            result += "<option value='3'>" + translateKey('stringTableBlockedPermanently') + "</option>";
          }
          result += "</select>";
        result += "</td>";
      result += "</tr>";
    result += "</table>";
    return result;

  }
};