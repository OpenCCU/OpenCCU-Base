iseHmIPPassageDetector = Class.create();
iseHmIPPassageDetector.prototype = {


  initialize: function(opts) {
    conInfo(opts);
    this.opts = opts;
    this.chn3Elm = jQuery("#"+this.opts.devId + this.opts.chnId);
  },

  show: function() {
    this.chn3Elm.show();
  },

  hide: function() {
    this.chn3Elm.hide();
  }
};


