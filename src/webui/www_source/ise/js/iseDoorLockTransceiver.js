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

    this.btnLockTargetLevel.on("click", function() {
      self.getDialog();
      self.selOptionElem = jQuery("#"+self.id+"selOptionElem");
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
  }
};