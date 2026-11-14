/**
 * SubsectionList.js
 * Gewerkeliste.
 **/
 
/**
 * Gewerkeliste
 **/
SubsectionList = Singleton.create({

  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.addEvent    = new eQ3.Event();
    this.removeEvent = new eQ3.Event();
  
    this.m_subsections = {};
  },
  
  /**
   * Callback. Aktualiert ein Gewerk.
   **/
  update: function(data, callback)
  {
    var subsection;
    
    if (data !== null)
    {
      var id = data["id"];
      subsection = this.m_subsections[id];
      
      if (typeof(subsection) != "undefined")
      {
        subsection.update(data);
      }
      else
      {
        subsection = new Subsection(data);
        this.m_subsections[id] = subsection;
        this.addEvent.fire(this, {subsection: subsection});
      }
    }
    
    if (callback) { callback(); }
    return subsection;
  },
    
  /**
   * Liefert die Liste aller Gewerke.
   **/
  list: function()
  {
    return Object.values(this.m_subsections);
  },

  /**
   * Lädt die Gewerkeliste erneut
   **/
  reload: function(loader)
  {
    var _this_   = this;
    var _loader_ = loader;

    homematic("Subsection.getAll", null, function(subsections) {
      if (subsections)
      {
        subsections.each(function (subsection) { _this_.update(subsection); });
      }
//      _loader_.reportLoadingState(1);
      _loader_.ready();
    });
  },
  
  /**
   * Startet die Aktualisierung eines Gewerks.
   **/
  beginUpdate: function(id, callback)
  {
    var _this_     = this;
    var _callback_ = callback;
    
    homematic("Subsection.get", {"id": id}, function(data) { _this_.update(data, _callback_); });
  },
  
  /**
   * Liefert ein Gewerk anhand seiner Id.
   **/
  get: function(id)
  {
    return this.m_subsections[id];
  },
  
  remove: function(subsection)
  {
    var channelIds = subsection.channelIds.clone();
    
    delete this.m_subsections[subsection.id];
    channelIds.each(function(id) { subsection.removeChannel(id); });
    this.removeEvent.fire(this, {subsection: subsection});
  }
  
}); 
