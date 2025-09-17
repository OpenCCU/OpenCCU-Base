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
          html += "<select id='"+this.id+"selOptionElem'>";
          jQuery.each(this.arOptions, function(index, opt) {
            html += "<option value='"+index+"'>"+opt+"</option>";
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
          html += "<div class='ControlBtnInfo' style='line-height:50px;'>${dlp_LOCK_STATE}: ${dlpLockState_"+this.opts.infoLockState+"}</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
          html += "<div class='ControlBtnInfo' style='line-height:50px;'>${dlp_LOCK_STATE_REASON}: ${dlpLockStateReason_"+this.opts.infoReason+"}</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
          html += "<div class='ControlBtnInfo' style='line-height:50px;'>${dlp_LOCK_TEACH_IN_STATE}: ${dlpLockTeachInState_"+this.opts.infoTeachInState+"}</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td>";
         html += "<div class='ControlBtnInfo' style='line-height:50px;'>${dlp_LAST_LOCK_DRIVE_LOAD}: "+this.opts.infoLastLockDriveLoad+"</div>";
        html += "</td>";
      html += "</tr>";

    html += "</table>";

    return html;
  }

};