WGDWeatherCombinedParamDialog = Class.create(YesNoDialog,{

  run: function() {
    var self  = this;
    window.setTimeout(function() {
      self.init();
      },50);
    },

  init: function () {
    /* for later use
    this.deviceType = this.extraParam.deviceType;
    this.channelType = this.extraParam.channelType;
    this.channelAddress = this.extraParam.chnAddress;
    */
    this.arInitialValue = this.extraParam.value.split(",");

    this.initDDI = this.arInitialValue[0];
    this.initDDIVal = this.initDDI.split("=")[1];

    this.initDDS = this.arInitialValue[1];
    this.initDDSVal = this.initDDS.split("=")[1];

    this.DDI_Elm = jQuery("#weatherDisplayID");
    this.DDS_Elm = jQuery("#weatherDisplayText");

    this.initDialog();
  },

  initDialog: function() {
    this.DDI_Elm.val(this.initDDIVal);
    this.DDS_Elm.val(this.initDDSVal);
  },

  getConfigString: function() {
    return  "DDI=" + this.DDI_Elm.val() + ",DDS=" + this.DDS_Elm.val();
  }

});
