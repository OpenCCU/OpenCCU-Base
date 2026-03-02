iseDoorLockTransceiver = Class.create();
iseDoorLockTransceiver.prototype = {

  initialize: function(chId, opts)
  {
    conInfo("iseDoorLockTransceiver", opts);
    this.id = chId;
    this.opts = opts;

    this.paramID = this.opts.paramID;
    this.arOptions = this.opts.options.split(",");
    this.initElem();
  },

  initElem: function() {
    var self = this;
    this.btnLockTargetLevel = jQuery("#"+this.id+"lockTargetLevel");
    this.btnShowInfoPanel = jQuery("#"+this.id+"showDLPInfoPanel");

    this.btnLockTargetLevel.on("click", function() {
      self.getDialog();
      self.selOptionElem = jQuery("#"+self.id+"selOptionElem");
    });

    this.btnShowInfoPanel.on("click", function() {
      self.getInfoPanel();
    });

  },
  
  getDialog: function() {
    var self = this,
      content = this.getDialogHtml();

    var dlg = new YesNoDialog(translateKey("dialogSetDLPTargetLevelTitle"), content, function(result) {

      if (result == YesNoDialog.RESULT_YES) {
        setDpState(self.paramID, self.selOptionElem.val() );
      }

    },"html");

    dlg.btnTextNo(translateKey("dialogBack"));
    dlg.btnTextYes(translateKey("btnOk"));
    dlg.setWidth(700);

  },

  getInfoPanel: function() {
    var self = this,
      content = this.getInfoPanelHtml();

    var dlg = new YesNoDialog(translateKey("dialogInfo"), content, function(result) {

    },"html");
    dlg.btnNoHide();
    dlg.btnTextYes(translateKey("btnOk"));
    dlg.setWidth(((parseInt(self.opts.infoTeachInState) > 4) && (parseInt(self.opts.infoTeachInState) < 10)) ? 800 : 600); // for very long Messages we increase the size of the dialog.
  },

  getDialogHtml: function() {
    var html = "";

    html += "<table align='center'>";
      html += "<tr>";
        html += "<td>";
          html += "<span>Option: </span> <select id='"+this.id+"selOptionElem'>";
          jQuery.each(this.arOptions, function(index, opt) {
            html += "<option value='"+(index + 3)+"'>"+opt+"</option>";
          });
          html += "</select>";
        html += "</td>";
      html += "</tr>";

      html += "<tr><td><hr></td></tr>";

      html += "<tr><td>"+translateKey('helpLockTargetLevel')+"</td></tr>";

    html += "</table>";

    return html;
  },

  getInfoPanelHtml: function() {
    var html = "";

    html += "<table align='center'>";

      html += "<tr>";
        html += "<td>";
          html += "<div class='ControlBtnInfo' style='line-height:50px;'>${stringTableLockState}: ${dlpLockState_"+this.opts.infoLockState+"}, ${stringTableLockStateReason}: ${dlpLockStateReason_"+this.opts.infoReason+"}</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
          html += "<div class='ControlBtnInfo' style='line-height:50px;'>${stringTableLockTeachInState}: ${dlpLockTeachInState_"+this.opts.infoTeachInState+"}</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
         html += "<div class='ControlBtnInfo' style='line-height:50px;'>${stringTableLastLockDriveLoad}: "+this.opts.infoLastLockDriveLoad+"</div>";
        html += "</td>";
      html += "</tr>";

    html += "</table>";

    return html;
  }

};