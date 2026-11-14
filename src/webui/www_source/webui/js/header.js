/**
 * header.js
 * Kopfzeile
 **/
 
Header = new function()
{
  var m_element = null;

  var clearPath = function()
  {
  };
  
  var addToPath = function(text)
  {
  };
  
  this.create = function(element)
  {
    new Ajax.Updater("header", "/ise/htm/header.htm", {evalScripts: true, asynchronous: false});  
    
  };
  
  this.setPath = function(page)
  {
  };

};