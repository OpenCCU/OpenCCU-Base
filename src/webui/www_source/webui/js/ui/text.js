/**
 * ui.text.js
 **/

UI.Text = Class.create(UI.Component, {

  /**
   * Konstruktor. Erzeugt ein neues Textfeld.
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    
    this.m_text = document.createElement("span");
    this.m_text.appendChild(document.createTextNode(UI.Text.DEFAULT_TEXT));
    
    this.m_element = document.createElement("div");
    this.m_element.className = UI.Text.CLASS_NAME;
    this.m_element.appendChild(this.m_text);
  },

  /**
   * Setzt den Text-Inhalt des Textfeldes
   **/
  setText: function(text)
  {
    this.m_text.innerHTML = "";
    this.m_text.appendChild(document.createTextNode(text));
    return this;
  },

  /**
   * Setzt den HTML-Inhalt des Textfeldes
   **/
  setHtml: function(html)
  {
    this.m_text.innerHTML = html;
    return this;
  },

  setID : function(id) {
    this.m_text.setAttribute("id",id);
    return this;
  },

  setAlignment: function(alignment)
  {
    Element.setStyle(this.m_element, {"textAlign": alignment});
    return this;
  },

  setStyle: function(style)
  {
    Element.setStyle(this.m_element, style);
    return this;
  },

  /**
   * Setzt die Breite des Labels.
   * Neben numerischen Angaben ist auch der Wert "auto" erlaubt.
   **/
  setWidth: function(width)
  {
    if (width != "auto") { width = parseInt(width) + "px"; }
    Element.setStyle(this.m_element, { width: width });
    return this;
  },
  
  /**
   * Setzt die Höhe des Labels.
   * Neben numerischen Angaben ist auch der Wert "auto" erlaubt.
   **/
  setHeight: function(height)
  {
    if (height != "auto") { height = parseInt(height) + "px"; }
    Element.setStyle(this.m_element, { height    : height });
    return this;
  },

  setClass: function(name)
  {
    this.m_element.className = name;
    return this;
  },
});
 
/**
 * Konstanten
 **/
UI.Text.CLASS_NAME = "UIText";
UI.Text.DEFAULT_TEXT = "UI Text";
