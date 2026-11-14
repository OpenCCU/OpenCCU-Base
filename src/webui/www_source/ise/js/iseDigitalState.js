/**
 * Created by grobelnik on 01.08.2016.
 */


iseDigitalState = Class.create();

iseDigitalState.prototype = {
  initialize: function (id, initState) {

    var elmOn, elmOff;
    var currentState = parseInt(initState);
    currentState = currentState.toString(2).reverse();

    for (var loop = 0; loop <= 7; loop++) {
      elmOn = jQuery('#bit' + loop + '1');
      elmOff = jQuery('#bit' + loop + '0');

      if (parseInt(currentState[loop]) == 1) {
        elmOn.prop('checked', true);
      } else {
        elmOff.prop('checked', true);
      }
    }
  }
};
