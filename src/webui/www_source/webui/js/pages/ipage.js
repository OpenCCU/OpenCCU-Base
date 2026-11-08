/**
 * ipage.js
 **/

/**
 * Schnittstelle für Seiten
 **/
IPage = Interface.create({

  /**
   * Callback für das Betreten der Seite
   **/
  enter: function(options) {},
  
  /**
   * Callback für das Verlassen der Seite
   **/
  leave: function() {},
  
});
