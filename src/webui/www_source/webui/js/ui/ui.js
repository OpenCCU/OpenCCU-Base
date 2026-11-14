/**
 * ui.js
 **/

/**
 * Namensraum für UI-Komponenten
 **/ 
UI = { };

/**
 * Basisklasse für UI-Komponenten
 **/
UI.Component = Class.create({

  /**
   * Konstruktor. Erstellt eine UI-Komponente
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_element = document.createElement("div");
  },

  /**
   * Liefert das DOM-Element der Komponente
   **/
  getElement: function()
  {
    return this.m_element;
  },

  setName: function(name)
  {
    this.m_element.name = name;
    return this;
  },

  /**
   * Setzt die Breite der Komponente in Pixeln
   **/
  setWidth: function(width)
  {
    Element.setStyle(this.m_element, {
      width: parseInt(width) + "px"
    });
    return this;
  },
  
  /**
   * Setzt die Höhe der Komponente in Pixeln
   **/
  setHeight: function(height)
  {
    Element.setStyle(this.m_element, {
      height: parseInt(height) + "px"
    });
    return this;
  },
  
  /**
   * Setzt die Position der Komponente
   **/
  setPosition: function(x, y)
  {
    Element.setStyle(this.m_element, {
      top : parseInt(y) + "px",
      left: parseInt(x) + "px"
    });
    return this;
  },
  
  /**
   * Ermittelt, ob die Komponente aktiv ist
   **/
  isEnabled: function() 
  {
    return this.m_isEnabled;
  },
  
	/**
	 * Aktiviert bzw. Deaktiviert die Komponente.
	 **/
	setIsEnabled: function(isEnabled)
	{
		if (isEnabled) { this.enable();  }
		else           { this.disable(); }
		
		return this;
	},
	
  /**
   * Setzt die Komponente auf aktiv.
   **/
  enable: function()
  {
    if (this.m_overlay) { Element.remove(this.m_overlay); }
    this.m_overlay = null;
    this.m_isEnabled = true;
    return this;
  },
  
  /**
   * Setzt die Komponente auf inaktiv.
   **/
  disable: function()
  {
    if (!this.m_overlay)
    {
      this.m_overlay = document.createElement("div");
      this.m_overlay.className = "UIDisabled";
      this.m_element.appendChild(this.m_overlay);
    }
    this.m_isEnabled = false;
    return this;
  },

  /**
   * Entfernt die Komponente
   **/
  dispose: function()
  {
    Element.remove(this.m_element);
    return this;
  }
  
});

UI.InputComponent = Class.create(UI.Component, {

  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_element   = document.createElement("div");
  },
  
  enable: function()
  {
    if (this.m_isEnabled === false)
    {
      this.m_element.disabled = false;
      this.m_element.readonly = true;
      this.m_isEnabled = true;
    }
    return this;
  },
  
  disable: function()
  {
    if (this.m_isEnabled === true)
    {
      this.m_element.disabled = true;
      this.m_element.readonly = false;
      this.m_isEnabled = false;
    }
    return this;
  }

});

/**
 * Basisklasse für UI-Container
 **/
UI.Container = Class.create(UI.Component, {

  /**
   * Erstellt einen UI-Container
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_element = document.createElement("div");
    this.m_content = m_element;
  },

  /**
   * Fügt dem Container eine neue Komponente hinzu
   **/
  add: function(component)
  {
    this.m_components.push(component);
    this.m_content.appendChild(component.getElement());
    return this;
  },
  
  /**
   * Entfernt eine Komponente aus dem Container
   **/
  remove: function(component)
  {
    this.m_components = this.m_components.without(component);
    component.dispose();
    return this;
  },
  
  /**
   * Entfernt den Container und alle seine Komponenten
   **/
  dispose: function()
  {
    var components = this.m_components;
    components.each(function(component) {
      this.remove(component);
    }, this);
    Element.remove(this.m_element);
    return this;
  }

});
