/**
 * @file controlbtn.js
 * @brief ControlBtn
 **/
 
ControlBtn = {

  CLASSNAME_ON : "ControlBtnOn",          //< CSS-Klasse "aktiv"
  CLASSNAME_OFF: "ControlBtnOff",          //< CSS-Klasse "inaktiv"
  CLASSNAME_PUSHED: "ControlBtnPushed",    //< CSS-Klasse "gedrückt"

  /**
   * Entfernt alle ControlBtn-Klassenamen von dem Element
   **/
  removeClassNames: function(element)
  {
    if (element)
    {
      $(element).removeClassName(this.CLASSNAME_ON);
      $(element).removeClassName(this.CLASSNAME_OFF);
      $(element).removeClassName(this.CLASSNAME_PUSHED);
    }
  },

  /**
   * Zeigt das Element als aktiven ControlBtn an
   **/
  on: function(element)
  {
    if (element)
    {
      this.removeClassNames(element);
      $(element).addClassName(this.CLASSNAME_ON);
    }
  },
  
  /**
   * Zeigt das Element als inaktiven ControlBrn an
   **/
  off: function(element)
  {
    if (element)
    {
      this.removeClassNames(element);
      $(element).addClassName(this.CLASSNAME_OFF);
    }
  },
  
  /**
   * Zeigt das Element als gedrückten ControlBtn an
   **/
  pushed: function(element)
  {
    if (element)
    {
      this.removeClassNames(element);
      $(element).addClassName(this.CLASSNAME_PUSHED);
    }
  }

};

// ControlBtn for JQuery generated Ids
JControlBtn = {
  CLASSNAME_ON : "ControlBtnOn",          //< CSS-Klasse "aktiv"
  CLASSNAME_OFF: "ControlBtnOff",          //< CSS-Klasse "inaktiv"
  CLASSNAME_PUSHED: "ControlBtnPushed",    //< CSS-Klasse "gedrückt"

  /**
   * Entfernt alle ControlBtn-Klassenamen von dem Element
   **/
  removeClass: function(element)
  {
    if (element)
    {
      element.removeClass(this.CLASSNAME_ON);
      element.removeClass(this.CLASSNAME_OFF);
      element.removeClass(this.CLASSNAME_PUSHED);
    }
  },

  /**
   * Zeigt das Element als aktiven ControlBtn an
   **/
  on: function(element)
  {
    if (element)
    {
      this.removeClass(element);
      element.addClass(this.CLASSNAME_ON);
    }
  },

  /**
   * Zeigt das Element als inaktiven ControlBrn an
   **/
  off: function(element)
  {
    if (element)
    {
      this.removeClass(element);
      element.addClass(this.CLASSNAME_OFF);
    }
  },

  /**
   * Zeigt das Element als gedrückten ControlBtn an
   **/
  pushed: function(element)
  {
    if (element)
    {
      this.removeClass(element);
      element.addClass(this.CLASSNAME_PUSHED);
    }
  },

  /**
   * Zeigt das Element kurz gedrückt an und geht dann wieder in den inaktiven Zustand
   */
  pressed: function(element)
  {
    var self = this;
    if (element)
    {
      this.on(element);
      window.setTimeout(function() {self.off(element);}, 250);
    }
  }
};