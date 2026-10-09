/**
 * Kopfleiste
 **/
HeaderBar = new function()
{

  // This causes problems with Mac & Safari
  this.load_ = function()
  {
    new Ajax.Updater("header", "/ise/htm/header.htm", {
      evalScripts: true,
      asynchronous: false
    });
  };

  this.load = function() {
    jQuery.ajax({
    url: "/ise/htm/header.htm",
    dataType: "html",
    async: false,
    context: document.body
    }).done(function(response) {
      jQuery("#header").html(response);
    });
  };

}();