/**
 * eq3.js
 **/

/**
 * Namensraum eQ3
 **/
eQ3 = {};

/**
 * Singleton für systemweite Hilfsfunktionen
 **/
eQ3.system = {

	MAX_OBJECT_ID: 1000000,		//< größte Id, die ein Objekt haben kann
	
	m_objects: {},						//< Enthält alle registierten Objekte
	m_id: 0,									//< vorgeschlagene Id für das nächste Objekt
	m_objectCount: 0,					//< Aktuelle Anzahl der registrieten Objekte (zu Debug-Zwecken)
	
	/**
	 * @fn m_getNextFreeId
	 * @brief Liefert die nächste freie Id, die einem Objekt zugewiesen 
	 *        werden kann. 
	 **/
	m_getNextFreeId: function()
	{
		var MAX_OBJECT_ID = eQ3.system.MAX_OBJECT_ID;
		var objects       = eQ3.system.m_objects;
		var id            = eQ3.system.m_id;
		var count         = 0;
		
		while ( typeof(objects[id]) != "undefined" )
		{
			id++;
			if (id > MAX_OBJECT_ID) { id = 0; }
			
			count++;
			if (count > MAX_OBJECT_ID) { throw new Error("max. object count reached"); }
		}
		
		if (id < MAX_OBJECT_ID) { eQ3.system.m_id = id + 1; }
		else                    { eQ3.system.m_id = 0;      }
		
		return id;
	},
	
	/**
	 * @fn registerObject
	 * @brief Weist einem Objekt eine Id zu, über die es angesprochen werden kann
	 */
	registerObject: function(object)
	{
		var id = eQ3.system.m_getNextFreeId();
		eQ3.system.m_objects[id] = object;
		eQ3.system.m_objectCount++;
		
		return id;
	},
	
	/**
	 * @fn unregisterObject
	 * @brief Gibt eine Objekt-Id frei.
	 **/
	unregisterObject: function(id)
	{
		if ( typeof(eQ3.system.m_objects[id]) != "undefined" )
		{
			eQ3.system.m_objects[id] = undefined;
			eQ3.system.m_objectCount--;
		}
	},
	
	/**
	 * @fn getObjectById
	 * @brief liefert ein registriertes Objekt anhand seiner Id.
	 **/
	getObjectById: function(id)
	{
		return eQ3.system.m_objects[id];
	}
	
};

/**
 * @fn $o
 * @brief Shortcut für eq3.system.getObjectById
 **/
$o = eQ3.system.getObjectById;


/**
 * Klasse für allgemeine Ereignisse
 **/
eQ3.Event = Class.create({

  /**
   * Konstruktor. Erstellt ein neues Ereignis.
   **/
  initialize: function()
  {
    this.reset();
  },
  
  /**
   * Fügt einen neuen Event-Listener hinzu.
   * Falls ein Listener bereits existiert, wird er kein zweites Mal hinzugefügt.
   **/
  add: function(listener)
  {
    if (!this.m_listeners.ex_contains(listener))
    {
      this.m_listeners.push(listener);
    }
    return this;
  },
  
  /**
   * Entfernt einen Event-Listener.
   **/
  remove: function(listener)
  {
    this.m_listeners = this.m_listeners.without(listener);
    return this;
  },
  
  /**
   * Löst das Ereinigs aus
   **/
  fire: function(source, event)
  {
    var listeners = this.m_listeners.clone();
    listeners.each(function(listener) { listener(source, event); });
    return this;
  },
  
  /**
   * Setzt das Ereignis zurück.
   * Löscht alle angemeldeten Event-Handler.
   **/
  reset: function()
  {
    this.m_listeners = [];
    return this;
  }

});
