/**
 * layer.js
 * Ebenen
 **/

/**
 * Ebenenverwaltung.
 **/ 
Layer = new function()
{
  /*####################*/
  /*# Private Elemente #*/
  /*####################*/
  
  var Z_INDEX_INCR = 100;    // Inkrement für den Ebenenindex
  
  var m_container = null;   // Container für die Ebenen
  var m_maxIndex  = 0;      // Maximal vergebener Index
  var m_layer     = {};     // Speichert die Indizes aller Ebenen
  
  /**
   * Ermittelt den maximalen Z-Index.
   **/
  var getMaxIndex = function()
  {
    var maxIndex = 0;
    
    for (id in m_layer)
    {
      var index = m_layer[id];
      if (index > maxIndex) { maxIndex = index; }
    }
    
    return maxIndex;
  };
  
  /*########################*/
  /*# Öffentliche Elemente #*/
  /*########################*/
  
  /**
   * Initialisiert die Ebenenverwaltung.
   **/
  this.init = function()
  {
    m_container = document.createElement("div");
    m_maxIndex  = 0;
    m_layer     = {};
    
    Element.addClassName(m_container, "LayerContainer");
    $("body").appendChild(m_container);
  };
  
  /**
   * Fügt eine Ebene hinzu.
   **/
  this.add = function(layer)
  {
    m_maxIndex     = getMaxIndex() + Z_INDEX_INCR;
    layer._layerId = m_maxIndex;
    m_layer[layer._layerId] = m_maxIndex;
    
    Element.setStyle(layer, {"zIndex": m_maxIndex, "position": "absolute"});
    m_container.appendChild(layer);    
  };
  
  /**
   * Entfernt eine Ebene.
   **/
  this.remove = function(layer)
  {
    try {
      delete(m_layer[layer._layerId]);
      m_maxIndex = getMaxIndex();
      m_container.removeChild(layer);
    } catch(e) {};
  };
  
}();
