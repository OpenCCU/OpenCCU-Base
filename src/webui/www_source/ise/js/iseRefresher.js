/**
 * ise/iseRefresher.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/ 
iseRefresher = Class.create();

iseRefresher.prototype =
{
  initialize: function(iPollingInterval)
  {
    this.updating = false;
    this.ajax = null;
    this.pe = new PeriodicalExecuter(this.refresh, iPollingInterval); 
  },
  refresh: function()
  {
    if( rfr )
    {
      if( !this.updating )
      {
        this.updating = true;
        var t = this;
        SwitchOnFlashLight();
        var url = "/esp/system.htm?sid="+SessionId+"&action=UpdateUI";
        var pb = '';
        pb += 'string channels = "'+iseUpdateIDArray.join("\t")+'";';
        pb += 'string timestamps = "'+iseUpdateTMArray.join("\t")+'";';
        var opts =
        {
          method: 'post',
          postBody: ReGa.encode(pb),
          evalScripts:true,
          onComplete: function()
          {
            delete t.ajax;
            t.ajax = null;
            iseRefrTimer = 0;
            t.updating = false;
          }
        };
        if(dbg)alert(pb);
        this.ajax = new Ajax.Updater("dummy", url, opts);
      }
    }
  }
};