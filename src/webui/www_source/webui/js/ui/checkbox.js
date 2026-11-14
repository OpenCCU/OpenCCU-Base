/**
 * ui.checkbox.js
 **/
 
UI.Checkbox = Class.create(UI.Component, {

  /**
   * Konstruktor. Erzeugt eine neue Checkbox.
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_action = null;
    
    this.m_checkbox = document.createElement("input");
    this.m_checkbox.type = "checkbox";
    this.m_checkbox.checked = UI.Checkbox.DEFAUT_CHECKED;
    Event.observe(this.m_checkbox, "click", this.onChange.bind(this));
    
    this.m_text = document.createElement("span");
    this.m_text.appendChild(document.createTextNode(UI.Checkbox.DEFALUT_TEXT));
    
    this.m_element = document.createElement("div");
    this.m_element.className = UI.Checkbox.CLASS_NAME;
    this.m_element.appendChild(this.m_checkbox);
    this.m_element.appendChild(this.m_text);
  },

  setText: function(text)
  {
    this.m_text.innerHTML = "";
    this.m_text.appendChild(document.createTextNode(text));
    return this;
  },
  
  setIsChecked: function(isChecked)
  {
    this.m_checkbox.checked = isChecked;
    return this;
  },
  
  isChecked: function()
  {
    return this.m_checkbox.checked;
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
  
  setAction: function(action, context)
  {
    this.m_action = action.bind(context);
    return this;
  },

  onChange: function()
  {
    if (this.m_action)
    {
      this.m_action(this);
    }
  }
  
});

UI.Checkbox.CLASS_NAME = "UICheckbox";
UI.Checkbox.DEFAULT_TEXT = "UI Checkbox";
UI.Checkbox.DEFAULT_CHECKED = false;