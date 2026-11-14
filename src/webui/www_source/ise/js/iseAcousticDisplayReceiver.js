/**
 * Created by grobelnik on 01.08.2016.
 */


iseAcousticDisplayReceiver = Class.create();

iseAcousticDisplayReceiver.prototype = {
  initialize: function (id, chnAddress, iface) {
    var self = this;
    this.id = id;

    statusDisplayDialog = new StatusDisplayDialogAcousticEPaper(translateKey("statusDisplayDialogTitle"), this._getAnchor(), "", function (result) {
      if (result == this.RESULT_YES) {
        var configString = this.getConfigString();
        //conInfo("statusDisplayDialog configString: " + configString);

        if (configString.length > 0) {
          conInfo("Set the display");
          setDpState(self.id, configString);
        } else {
          conInfo("Don't set the display");
        }
      }
    },"html");

  },

  _getAnchor: function() {
  return "<table style='margin: 0 auto;'>" +
     "<tbody id='statusDisplayDialog'>" +
     "</tbody>" +
  "</table>";
  }
};
