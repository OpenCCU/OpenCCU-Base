/**
 * ic_neweasymode.js
 **/

NewEasyModeForm = Class.create();

NewEasyModeForm.prototype = Object.extend(new MsgBox(), {
  initialize: function(w, h, base_pnr, ps_id, ps_ids, special_input_id) {

  this.init(w, h);
  this.w = w;
  this.h = h;  
  this.base_pnr = base_pnr;  
  this.ps_id    = ps_id;  
  this.ps_ids   = ps_ids;  
  this.special_input_id = special_input_id;  

  this.ProfilesMap = new Array();
  },
  AddProfile: function(pnr, pname) {

  this.ProfilesMap[pnr] = pname;
  },
  ProfileOverwriteWarning: function() {

  var profileExists = false;
  var cur_usertext = $F('EasyModeName');
  for (var Name in this.ProfilesMap)
  {
    if (this.ProfilesMap[Name] == cur_usertext)
    {
      profileExists = true;
      break;
    }
  }

  if (profileExists) {$('id_overwrite_warning').style.visibility = 'visible'; $('save_new_profile').style.visibility = 'hidden';}
  else               {$('id_overwrite_warning').style.visibility = 'hidden'; $('save_new_profile').style.visibility = 'visible';}
  },
  StoreNewProfile: function() {
  ResetPostString();
  profile = is_newProfile(this.ps_id); 
  poststr += "&cmd=SAVE";
  poststr += "&base_pnr="     +this.base_pnr;
  poststr += "&ps_id="        +profile;
  poststr += "&ps_ids="       +profile;
  poststr += "&sensor="		+$F('dev_descr_sender_tmp').split("-")[0];
  poststr += "&actor="	+$F('dev_descr_receiver_tmp');
  poststr += "&EasyModeName=" +elv_toQueryString($F('EasyModeName'));
  
  AddSeparateSettings('separate_' + this.special_input_id + '_', this.base_pnr);
  AddSeparateSettings('subset_'   + this.special_input_id + '_', this.base_pnr);
  SendRequest('ic_neweasymode.cgi');

  this.hide();

  //ProgressBar = new ProgressBarMsgBox("Profilvorlage wird erstellt...", 1);
  ProgressBar = new ProgressBarMsgBox(translateKey("dialogSettingsCreateProfileTemplateTitle"), 1);
  ProgressBar.show();
    ProgressBar.StartKnightRiderLight();
  }
});
