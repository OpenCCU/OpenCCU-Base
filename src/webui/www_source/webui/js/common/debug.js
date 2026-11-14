/**
 * debug.js
 **/
 
/**
 * Debug-Schnittstelle
 **/
Debug =  Singleton.create({
  isEnabled: true,
  
  /**
   * Wirft eine Exception, falls eine Zusicherung nicht
   * eingehalten werden kann.
   **/
  assert: function(expression, message)
  {
    if (true !== expression) 
    {
      if (typeof(message) != "undefined") { throw new Error("assertion failed: " + message); }
      else { throw new Error("assertions failed"); }
    }
  }
});
