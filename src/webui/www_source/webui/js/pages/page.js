/**
 * page.js
 **/

/**
 * Basisklasse für alle Seiten
 **/ 
Page = Class.create({
  __interfaces__: [IPage],
  MAINMENU_ID: null,
  
  /**
   * Callback. Wird beim Betreten der Seite aufgerufen.
   **/
  enter: function(options)
  {
    MainMenu.select(MAINMENU_ID);
  },
  
  /**
   * Callback. Wird beim Verlassen der Seite aufgerufen.
   **/
  leave: function()
  {
  },
  
});
