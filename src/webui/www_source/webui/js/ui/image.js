/**
 * image.js
 **/

/**
 * Image
 **/
UI.Image = Class.create(UI.Component, {

  /**
   * Konstruktor. Erstellt ein Image
   **/ 
  initialize: function()
  {
    this.m_isEnabled = true;

    this.m_element = document.createElement("img");
    this.m_element.className = UI.Image.CLASS_NAME;
  },

  setPath: function(path)
  {
    this.m_element.alt = "";
    this.m_element.src = path;
    return this;
  },

  setId: function(id)
  {
    this.m_element.id = id;
    return this;
  },

  getId: function()
  {
    return this.m_element.id;
  }
});

/**
 * Konstanten
 **/
UI.Image.CLASS_NAME   = "UIImage";
