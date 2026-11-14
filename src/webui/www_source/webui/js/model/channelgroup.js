/**
 * channelgroup.js
 **/
 
/**
 * Kanalgruppe.
 **/
ChannelGroup = Class.create({
  
  /**
   * Konstruktor
   **/
  initialize: function(device, channel)
  {
    this.m_updateChannelHandler = this.m_onUpdateChannel.bind(this);
    
    this.id              = channel.groupId;
    this.device          = device;
    this.typeName        = device.deviceType.name;
    this.typeDescription = device.deviceType.description;
    this.channels        = [];
    this.thumbnailHTML   = null;
    this.imageHTML       = null;    
    
    this.update(device, channel);
  },

  /**
   * Aktualisiert die Kanalgruppe
   **/
  update: function(device, channel)
  {
    if ((device) && (channel))
    {
      var deviceType = device.deviceType;
      
      channel.updateEvent.add(this.m_updateChannelHandler);
      
      this.channels.ex_pushUnique(channel);
      this.channels.ex_sortBy("index");
      this.formName = this.channels.ex_joinItem("index", "+");
      this.name     = this.channels.ex_joinItem("name", " ");
      this.address  = this.channels.ex_joinItem("address", " ");
      this.thumbnailHTML = deviceType.getThumbnailHTML(this.formName);
      this.imageHTML     = deviceType.getImageHTML(this.formName);
    }
    
    this.isUsable        = false;
    this.isVisible       = false;
    this.isLogged        = false;
    this.categories      = [];
    this.modes           = [];
    this.rooms           = [];
    this.subsections     = [];

    this.channels.each(function(channel) {
      this.isUsable  |= channel.isUsable;
      this.isVisible |= channel.isVisible;
      this.isLogged  |= channel.isLogged;
      this.categories.push(channel.category);
      this.modes.push(channel.mode);
      this.rooms = this.rooms.concat(channel.rooms);
      this.subsections = this.subsections.concat(channel.subsections);
    }, this);
    
    this.categories  = this.categories.uniq();
    this.modes       = this.modes.uniq();
    this.rooms       = this.rooms.uniq().ex_sortBy("name");
    this.subsections = this.subsections.uniq().ex_sortBy("name");
  },
  
  m_onUpdateChannel: function(channel, eventArgs)
  {
    this.update(this.device, channel);
  },
  
  addSubsection: function(subsection)
  {
    if (!this.subsections.ex_contains(subsection))
    {
      this.subsections.push(subsection);
      this.subsections.ex_sortBy("name");
    }
  },
  
  removeSubsection: function(subsection)
  {
    if (this.subsections.ex_contains(subsection))
    {
      this.subsections = this.subsections.without(subsection);
    }
  },

  /**
   * Liefert die Ids aller Programme, welche mindestens einen Kanal der Gruppe verwenden
   **/
  listProgramIds: function()
  {
    var ids = [];
    this.channels.each(function(channel) {
      ids = ids.concat(homematic("Channel.listProgramIds", {"id": channel.id})); 
    });
    return ids.uniq();
  }
    
});

