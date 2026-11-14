/**
 * subsection.js
 * Gewerk.
 **/
 
Subsection = Class.create({
  
  /**
   * Konstruktor
   **/
  initialize: function(data)
  {
    this.id          = "";
    this.name        = "";
    this.description = "";
    this.channelIds  = [];
    
    this.addChannelEvent    = new eQ3.Event();
    this.removeChannelEvent = new eQ3.Event();
    
    this.update(data);
  },
  
  /**
   * Aktualisiert den Raum
   **/
  update: function(data)
  {
    var channelIds = this.channelIds.clone();
    channelIds.each(function(id) { this.removeChannel(id); }, this);
  
    this.id          = data["id"];            // Id
    this.name        = data["name"];          // Name
    this.description = data["description"];   // Beschreibung
    this.channelIds  = [];                    // Ids der Kanäle
    
    data["channelIds"].each(function(id) { this.addChannel(id); }, this);
  },
  
  /**
   * Fügt einen Kanal hinzu.
   * writeback: [bool] Optional (false). Falls true, wird die Änderung an die HomeMatic Zentrale übermittelt.
   **/
  addChannel: function(channelId, writeBack)
  {
    if (!this.channelIds.ex_contains(channelId))
    {
      if (writeBack) { homematic("Subsection.addChannel", {id: this.id, channelId: channelId}); }
      this.channelIds.push(channelId);
      this.addChannelEvent.fire(this, {channelId: channelId});
    }
  },
  
  /**
   * Entfernt einen Kanal.
   * writeBack: [bool] Optional (false). Falls true, wird die Änderung an die HomeMatic Zentrale übermittelt.
   **/
  removeChannel: function(channelId, writeBack)
  {
    if (this.channelIds.ex_contains(channelId))
    {
      if (writeBack) { homematic("Subsection.removeChannel", {id: this.id, channelId: channelId}); }
      this.channelIds = this.channelIds.without(channelId);
      this.removeChannelEvent.fire(this, {channelId: channelId});
    }
  },
  
  /**
   * Prüft, ob ein Kanal in dem Raum definiert ist.
   **/
  contains: function(channelId)
  {
    return (0 <= this.channelIds.indexOf(channelId));
  },
  
  /**
   * Liefert die Ids aller Programme, die mindestens einen Kanal in dem Gewerk verwenden
   **/
  listProgramIds: function()
  {
    return homematic("Subsection.listProgramIds", {id: this.id});
  }
  
});
