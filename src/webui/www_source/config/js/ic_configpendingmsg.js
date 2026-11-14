/**
 * ic_configpendingmsg.js
 **/
 
ConfigPendingMsgBox = Class.create();

ConfigPendingMsgBox.prototype = Object.extend(new MsgBox(), {
  
  initialize: function(w, h, extraParam)
  {
    this.extraParm = extraParam;
    this.init(w, h);
    this.configpendingcount = 0;
    this.iface   = "";
    this.url     = ""; //s. setReturnURL
    this.sid     = ""; //s. setReturnURL
    this.sidname = ""; //s. setReturnURL
    
    this.returnurl = "";
    this.returnurl_params = "";
    this.go_back = false;

    this.addr = new Array(2);
    this.addr.clear();

    //defines
    this.CONFIGPENDING_SENDER = 0;
    this.CONFIGPENDING_RECEIVER = 1;
    //---

    this.AddDivWrapper("id_configpending_head");
    $("id_configpending_head").className = "popupTitle";
    $("id_configpending_head").style.fontWeight = "bold";
    //this.AddTextNode("id_configpending_head", "Verknüpfungs- und Programmstatus");
    this.AddTextNode("id_configpending_head", translateKey("dialogCreateLinkTitle"));
  
    this.AddDivWrapper("id_configpending_overflow");
    $("id_configpending_overflow").style.overflow = "auto";
    $("id_configpending_overflow").style.width  = "100%";
    $("id_configpending_overflow").style.height = "100%";
  
    this.AddDivWrapper("id_configpending_body", "id_configpending_overflow");
    this.AddDivWrapper("id_configpending_foot", "id_configpending_overflow");
    $("id_configpending_foot").addClassName("CLASS10200");

    this.AddTableHead();
    this.ResetTable();
  },

  m_return: function()
  {
    switch (this.url)
    {
      case "IC_LINKPEERLIST":
        WebUI.enter(LinkListPage);
        break;
      case "IC_SETPROFILES":
        WebUI.enter(LinkEditProfilePage, {
          iface:    this.iface,
          sender:   (this.addr)[this.CONFIGPENDING_SENDER],
          receiver: (this.addr)[this.CONFIGPENDING_RECEIVER]
        });
        break;
      case "GO_BACK": 
        WebUI.goBack();
        break;
      default:
        break;
    }
  },
  
  setReturnURL: function(sidname, sid, url, goBack)
  {
    this.go_back = goBack;
    this.url     = url;
    this.sid     = sid;
    this.sidname = sidname;

    if (goBack === true) { this.url = "GO_BACK"; }

/*    
    if (url == "IC_LINKPEERLIST")
    {
      this.returnurl = UI_PATH + "ic_linkpeerlist.cgi";
      if (LINKLISTSORTBY) this.returnurl_params = "&LINKLISTSORTBY=" + LINKLISTSORTBY;
    }
    else if (url == "IC_SETPROFILES" )
    {
      this.returnurl = UI_PATH + "ic_setprofiles.cgi";
      this.returnurl_params = "&iface="  + this.iface +
        "&sender_address="   + (this.addr)[this.CONFIGPENDING_SENDER] +
        "&receiver_address=" + (this.addr)[this.CONFIGPENDING_RECEIVER];
    }
    else if (url == "GO_BACK" )
    {
      this.returnurl = sPreviousPage;
      this.returnurl_params = sPreviousPageArgs;
    }
    else
    {
      this.returnurl = "";
    }
*/    
  },

  ClearTable: function()
  {
    if($('id_configpending_table_tbody')) $('id_configpending_table_tbody').innerHTML = "";
  },
  
  AddTextNode: function (id, text)
  {    
    var textnode = document.createTextNode(text);
    $(id).appendChild(textnode);
  },

  SetNavigationBar: function ()
  {
    
    $('id_configpending_foot').innerHTML = "";

    if (this.configpendingcount === 0)
    {
      var newInputDiv = this.CreateButton("OK");
      newInputDiv.onclick = function()
      {
        ConfigPendingFrm.hide();
        if (typeof goBack != "undefined" && goBack == true  ) {
          WebUI.reload();
          delete goBack;
        } else {
          ConfigPendingFrm.m_return();
        }
      };
      $('id_configpending_foot').appendChild(newInputDiv);
    }
    else
    {
      //var newInputDiv1 = this.CreateButton("Erneut prüfen");
      var newInputDiv1 = this.CreateButton(translateKey("btnDirectDeviceLinkCheckAgain"));
      newInputDiv1.onclick = function()
      {
        ConfigPendingFrm.hide();
        CheckConfigPending(ConfigPendingFrm.iface, (ConfigPendingFrm.addr)[ConfigPendingFrm.CONFIGPENDING_SENDER], (ConfigPendingFrm.addr)[ConfigPendingFrm.CONFIGPENDING_RECEIVER], ConfigPendingFrm.url, ConfigPendingFrm.go_back);
      };
      $('id_configpending_foot').appendChild(newInputDiv1);
    
      //var newInputDiv2 = this.CreateButton("Ignorieren");
      var newInputDiv2 = this.CreateButton(translateKey("lblIgnore"));
      newInputDiv2.onclick = function()
      {
        ConfigPendingFrm.hide();
        if (typeof goBack != "undefined" && goBack == true  ) {
          WebUI.reload();
          delete goBack;
        } else {
          ConfigPendingFrm.m_return();
        }
      };
      $('id_configpending_foot').appendChild(newInputDiv2);
    }
  },

  AddTableHead: function ()
  {
    var table = document.createElement("table");
    table.id = 'id_configpending_table';
    table.className = "popupTable";
    table.border = "1";

    var thead = document.createElement("thead");
    table.appendChild(thead);

    var tr = document.createElement("tr");
    tr.className = "popupTableHeaderRow";

    var td = new Array(4);

    //var textnode = document.createTextNode("Name");
    var textnode = document.createTextNode(translateKey("thName"));
    td[0] = document.createElement("th");
    td[0].appendChild(textnode);
    tr.appendChild(td[0]);  

    //textnode = document.createTextNode("Typenbezeichnung");
    textnode = document.createTextNode(translateKey("thTypeDescriptorWOLineBreak"));
    td[1] = document.createElement("th");
    td[1].appendChild(textnode);
    tr.appendChild(td[1]);  

    //textnode = document.createTextNode("Bild");
    textnode = document.createTextNode(translateKey("thPicture"));
    td[2] = document.createElement("th");
    td[2].appendChild(textnode);
    tr.appendChild(td[2]);  

    //textnode = document.createTextNode("Hinweis");
    textnode = document.createTextNode(translateKey("thHint"));
    td[3] = document.createElement("th");
    td[3].appendChild(textnode);
    tr.appendChild(td[3]);  

    thead.appendChild(tr);

    $('id_configpending_body').appendChild(table);
  },

  ResetTable: function()
  {

    this.configpendingcount = 0;
  
    try
    {
      Element.remove($('id_configpending_table_tbody'));
    }
    catch (e) {}
  
    var tbody = document.createElement("tbody");
    tbody.id = "id_configpending_table_tbody";
    $('id_configpending_table').appendChild(tbody);

    var tr = document.createElement("tr");
    tr.id = "id_configpending_tr_NODATA";
    tr.style.height = "50px";
    tr.className = "popupTableRowGray";

    //var textnode = document.createTextNode("Die Übertragung ist erfolgreich verlaufen.");
    var textnode = document.createTextNode(translateKey('dialogCreateLinkSuccessContent'));
    var td = document.createElement("td");
    td.colSpan = 4;
    td.appendChild(textnode);
    tr.appendChild(td);  

    tbody.appendChild(tr);
  },

  SetDevice: function(iface, address, direction) 
  {
    if (direction == this.CONFIGPENDING_SENDER) (this.addr)[this.CONFIGPENDING_SENDER]   = address;
    else                                        (this.addr)[this.CONFIGPENDING_RECEIVER] = address;

    this.iface = iface;

    this.SetNavigationBar();
  },

  ShowConfigPending: function(iface, address, name, type, configpending, index, direction) 
  {
    if (direction == this.CONFIGPENDING_SENDER) (this.addr)[this.CONFIGPENDING_SENDER]   = address;
    else                                        (this.addr)[this.CONFIGPENDING_RECEIVER] = address;

    this.iface = iface;

    var tbody = $('id_configpending_table_tbody');
    var devicecount = tbody.getElementsByTagName('tr').length;

    var tr = document.createElement("tr");
    tr.id = "id_configpending_tr_" + this.configpendingcount;
    tr.className = "popupTableRowGray";

    var td = new Array(4);

//    var textnode = document.createTextNode(name.unescapeHTML());
    td[0] = document.createElement("td");
//    td[0].appendChild(textnode);
    td[0].innerHTML = name;
    tr.appendChild(td[0]);  

    textnode = document.createTextNode(type);
    td[1] = document.createElement("td");
    td[1].appendChild(textnode);
    tr.appendChild(td[1]);  

    var img = document.createElement("img");
    img.setAttribute("src",   DEV_getImagePath(type, 50));
    img.setAttribute("title", name);
    img.setAttribute("alt",   address);
  
    var div = document.createElement("div");
    div.setAttribute('id', 'id_configpending_picDiv_' + this.configpendingcount);
    div.onmouseover = function () { picDivShow(jg_250, type, 250, -1, this); };
    div.onmouseout  = function () { picDivHide(jg_250); };
    div.style.position = "relative";
    div.appendChild(img);
  
    td[2] = document.createElement("td");
    td[2].style.backgroundColor = WebUI.getColor("white");
    td[2].appendChild(div);
    tr.appendChild(td[2]);  

    var linecount = $('id_configpending_table_tbody').getElementsByTagName("tr").length;
  
    if (((linecount > 1) && (this.configpendingcount === 1) && (configpending === 1)) ||
        ((linecount > 1) && (this.configpendingcount === 0) && (configpending === 0))) /*linecount > 1 weil die erste Zeile unsichtbar ist.*/
    {
      //Die letzte Meldung soll für dieses Gerät mitgelten. Meldungen zusammenfassen:

      var msg_td = $('id_configpending_tr_0').getElementsByTagName("td")[3];
      var rs = msg_td.getAttribute("rowspan");

      if ( !rs || rs === "") { msg_td.rowSpan = 2; }                 //Attribut neu anlegen
      else                   { msg_td.rowSpan = parseInt(rs) + 1; }  //Attributwert hochzählen
    }
    else if (configpending == 1)
    {
      td[3] = document.createElement("td");
      td[3].style.padding = "5px";
    
      //textnode = document.createTextNode("Die Übertragung der Daten zum Gerät konnte nicht ordnungsgemäß durchgeführt werden. Wählen Sie:");
      textnode = document.createTextNode(translateKey("dialogCreateLinkErrorContent1"));
      td[3].appendChild(textnode);
    
      var ul = document.createElement("ul");

      var li_text = new Array(2);
      //li_text[0] = document.createTextNode("\"Erneut prüfen\", wenn Sie die Übertragung zum Gerät jetzt abschließen möchten. Sorgen Sie dazu bitte dafür, dass sich");
      li_text[0] = document.createTextNode(translateKey("dialogCreateLinkErrorContent2"));
      
      var ul2 = document.createElement("ul");
      var li_text2 = new Array(2);
      //li_text2[0] = document.createTextNode("das Gerät innerhalb der Funkreichweite befindet und aktiv ist,");
      li_text2[0] = document.createTextNode(translateKey("dialogCreateLinkErrorContent3"));
      //li_text2[1] = document.createTextNode("das Gerät im Anlernmodus befindet.");
      li_text2[1] = document.createTextNode(translateKey("dialogCreateLinkErrorContent4"));
      var li2 = new Array(2);
      li2[0] = document.createElement("li");
      li2[0].appendChild(li_text2[0]);
      li2[1] = document.createElement("li");
      li2[1].appendChild(li_text2[1]);
      ul2.appendChild(li2[0]);
      ul2.appendChild(li2[1]);
    
      //li_text[1] = document.createTextNode("\"Ignorieren\", wenn die Zentrale die Übertragung zum Gerät bei nächster Gelegenheit selbstständig durchführen soll. Bis dahin ist dieser Konfigurationsvorgang als offene Servicemeldung sichtbar.");
      li_text[1] = document.createTextNode(translateKey("dialogCreateLinkErrorContent5"));
      
      var li = new Array(2);
      li[0] = document.createElement("li");
      li[0].appendChild(li_text[0]);
      li[0].appendChild(ul2);
      li[1] = document.createElement("li");
      li[1].appendChild(li_text[1]);

      ul.appendChild(li[0]);
      ul.appendChild(li[1]);

      td[3].appendChild(ul);

      if (this.extraParm == "ADD_LINK") {
        var hintAddLink = document.createElement("div");
        hintAddLink.innerHTML = translateKey("dialogCreateLinkErrorContent6");
        td[3].appendChild(hintAddLink);
      }

      td[3].align = "left";
      td[3].style.color = WebUI.getColor("red");
      td[3].style.fontWeight = "bold";

      tr.appendChild(td[3]);  

      this.configpendingcount++;
    }
    else if (configpending === 0)
    {
      //textnode = document.createTextNode("Die Übertragung der Daten zum Gerät wurde erfolgreich abgeschlossen.");
      textnode = document.createTextNode(translateKey("dialogCreateLinkSuccessContent"));
      td[3] = document.createElement("td");
      td[3].appendChild(textnode);
      td[3].align = "left";
      tr.appendChild(td[3]);  
    }
    else
    {
      //textnode = document.createTextNode("Übertragung nicht erfolgt, weil das Gerät unbekannt ist.");
      textnode = document.createTextNode(translateKey("dialogCreateLinkErrorUnknownDevice"));
      td[3] = document.createElement("td");
      td[3].appendChild(textnode);
      td[3].align = "left";
      td[3].style.color = WebUI.getColor("red");
      td[3].style.fontWeight = "bold";
      tr.appendChild(td[3]);  
    }

    tbody.appendChild(tr);
    this.SetNavigationBar();

    $("id_configpending_tr_NODATA").style.display    = "none";
    $("id_configpending_tr_NODATA").style.visibility = "hidden";
  }
});
