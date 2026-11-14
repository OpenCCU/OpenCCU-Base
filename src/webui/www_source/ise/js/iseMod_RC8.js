iseMOD_RC8 = Class.create();
iseMOD_RC8.prototype = {

  initialize: function(chId, chAddress)
  {
    var self = this;
    this.channelId     = chId;
    this.channelAddress = chAddress;
    this.noFunctionElm = jQuery("#"+self.channelId+"noFunction");
    this.TFKElm = jQuery("#"+self.channelId+"scControl");
    this.WaitElm = jQuery("#"+self.channelId+"resultWaitAnim");

    var tmp = homematic("Interface.getMasterValue", {"interface": "HmIP-RF", "address": this.channelAddress, "valueKey": "CHANNEL_OPERATION_MODE"},function(result) {

      var optionTFK = 3;

      self.WaitElm.hide();

      if (result == optionTFK) {
        self.noFunctionElm.hide();
        self.TFKElm.show();
      } else {
        self.noFunctionElm.show();
        self.TFKElm.hide();
      }
    });

  }
};