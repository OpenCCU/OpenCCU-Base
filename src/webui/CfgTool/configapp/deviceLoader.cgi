#!/bin/tclsh
source ../once.tcl
sourceOnce cgi.tcl
sourceOnce session.tcl
sourceOnce common.tcl
loadOnce tclrpc.so

array set interfaces ""
array set interface_descriptions ""

proc read_interfaces {} {
    global interfaces interface_descriptions INTERFACES_FILE env
    set retval 1
    if { [ info exist env(BIDCOS_SERVICE) ] } {
        set interfaces(default) "$env(BIDCOS_SERVICE)"
        set interface_descriptions(default) "Default BidCoS Interface"
    } else {
        set fd -1
        catch {set fd [open $INTERFACES_FILE r]}
        if { $fd >=0 } {
            set contents [read $fd]
            while { [regexp -indices {</ipc[^>]*>} $contents range] } {
                set section [string range $contents 0 [lindex $range 1]]
                set contents [string range $contents [expr [lindex $range 1] + 1] end]
                if { 
                        [regexp {<name[^>]*>([^<]+)</name} $section dummy name] &&
                        [regexp {<url[^>]*>([^<]+)</url} $section dummy url] &&
                        [regexp {<info[^>]*>([^<]+)</info} $section dummy description ]
                } {
                    array set interfaces [list $name $url]
                    array set interface_descriptions [list $name $description]
                }
            }
            close $fd
        } else {
            puts "Could not open interface file"
            set retval 0
        }
    }
    return $retval
}

proc put_page {} {
    global interfaces
    
    import devid
    import iface
    import arIdx
    import autoExpGr
    import insIdx
    
    set FLAG_INTERNAL 2
    
    set url $interfaces($iface)

    puts { <script type="text/javascript"> }
    
    puts "var autoExpGr = $autoExpGr;"
    puts "var arIdx = $arIdx;"
    puts "var iInsIdx = $insIdx;"
    puts {
            deviceData = null;
            Group = new Array(); // [0]: Member 1,  [1]: Member 2
            GroupsArray = new Array(); // Alle Gruppen
            chns = new Array(); // Objekte Kanäle ohne gruppe
            iElemsAdded = 0;
            deviceData = arDevices[arIdx];
            arDevices[arIdx]['showChnCnt'] = 0;
    }
    
    
    array set dev_descr [xmlrpc $url getDeviceDescription [list string $devid]]
    
    
    set group_count 0
    foreach chanid $dev_descr(CHILDREN) {
      array_clear chan_descr
      array set chan_descr [xmlrpc $url getDeviceDescription [list string $chanid]]
      
      if { $chan_descr(FLAGS) & $FLAG_INTERNAL } continue
      
      # Virtual channels of actors should only be visible when the expert mode is activated
      # DIRECTION 0 = no direct links, 1 = channel is a sender, 2 = channel is an actor
      # Only virtual channels of actors should be blocked in case of a non-expert mode
      # The virtual channels of the BidCos-RF (KEY) should always be visible.    
      if {[regexp {VIRTUAL_} $chan_descr(TYPE)] == 0 || [session_is_expert] || $chan_descr(DIRECTION) != "2"} {         
        
        array_clear master_paramset_descr
        catch { array set master_paramset_descr [xmlrpc $url getParamsetDescription [list string $chanid] [list string MASTER] ] }
        
        array_clear values_paramset_descr
        array set values_paramset_descr [xmlrpc $url getParamsetDescription [list string $chanid] [list string VALUES] ]
        
        array_clear chan_metadata
        set name "$chan_descr(PARENT_TYPE) $chan_descr(ADDRESS)"
        catch { 
            array set chan_metadata [xmlrpc $url getAllMetadata [list string $chan_descr(ADDRESS)]] 
            catch { set name $chan_metadata(NAME) }
        }
        
        
        puts "var channel = new Object();"
        puts "channel\['id'\] = '$chanid';"
        puts "channel\['type'\] = '$dev_descr(TYPE)';"
        puts "channel\['desc'\] = DEV_getDescription('$dev_descr(TYPE)');"
        puts "channel\['sn'\] = '$chanid';"
        
        set category ""
        if { $chan_descr(DIRECTION) == 1} { set category "Sender"}
        if { $chan_descr(DIRECTION) == 2} { set category "Empfänger"}
        
        puts "channel\['iface'\] = '$category';"
        puts "channel\['deviface'\] = '$iface';"
        
        #ToDo: gucken, ob im MASTER-Paramset AES schreibbar ist
        set aes_op 0
        catch {
            array set aes_param_descr $master_paramset_descr(AES_ACTIVE)
            set aes_op $aes_param_descr(OPERATIONS)
        }
        puts "channel\['transOp'\] = $aes_op;"
        set aes_mode "Standard";
        if { $chan_descr(AES_ACTIVE) } { set aes_mode "Gesichert" }
        puts "channel\['trans'\] = '$aes_mode';"
        puts "channel\['name'\] = '$name';"
        puts "channel\['func'\] = '';"
        puts "channel\['room'\] = '';"
        puts "channel\['gm'\] = 0;"
        puts "channel\['ready'\] = 1;"
        puts "channel\['vis'\] = 1;"
        puts "channel\['chnNr'\] = $chan_descr(INDEX);"
        puts "channel\['show'\] = true;"
        set install_test 0
        if { [info exists values_paramset_descr(INSTALL_TEST)] } {
            array set install_test_descr $values_paramset_descr(INSTALL_TEST)
            if { $install_test_descr(OPERATIONS) & 2 } {
                set install_test 1
            }
        }
        puts "channel\['install_test'\] = $install_test"
        puts "channel\['fltOpts'\] = new Object();"
        puts "channel\['fltOpts'\]\['iCat'\] = '$chan_descr(DIRECTION)';"
        puts "channel\['fltOpts'\]\['iTrans'\] = $chan_descr(AES_ACTIVE);"
        puts "channel\['fltOpts'\]\['rooms'\] = '';"
        puts "channel\['fltOpts'\]\['funcs'\] = '';"

        if { [info exists chan_descr(GROUP)]  } {
            if { [info exists chan_group_index($chan_descr(GROUP))] } {
                set group_index $chan_group_index($chan_descr(GROUP))
                puts "GroupsArray\[$group_index\]\[1\] = channel;"
            } else {
                set group_index $group_count
                puts "GroupsArray\[$group_index\] = new Array();"
                puts "GroupsArray\[$group_index\]\[0\] = channel;"
                set chan_group_index($chanid) $group_index
                incr group_count
            }
        } else {
            puts "chns\[chns.length\] = channel;"
        }
      }
    }

    puts {
        if (deviceData != null) {
            var iChnPassedFilter = 0;
            var td = null;
            var tr = null;
            var img = null;
    
            // Kanalgruppen
            for (var i = 0; i < GroupsArray.length; i++) {
                arSubIds[arSubIds.length] = ""+ deviceData['id'] + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'];
                tr = $("tblDevices").insertRow(iInsIdx++);iElemsAdded++;
                tr.id =  ""+ deviceData['id'] + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'];
                GroupsArray[i]['trid'] = tr.id;
                td = Builder.node('td'); tr.appendChild(td);
                td = Builder.node('td');
                img = Builder.node('img', {src: "/ise/img/plus.png", id:"img" + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'], alt:"plus",
                                 onclick: "expandGroup('" + deviceData['id'] + "','" + GroupsArray[i][0]['id'] + "','" + GroupsArray[i][1]['id'] + "')"});
                if (GroupsArray[i][0]['show'] || GroupsArray[i][1]['show']) {
                    arImgIds[arImgIds.length] = "img" + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'];
                    td.appendChild(img);
                }
                tr.appendChild(td);
      
                td = Builder.node('td', {colSpan: 2, className: 'GrayBkg'}, (GroupsArray[i][0]['name'] + " + " + GroupsArray[i][1]['name'])); tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][0]['type'] + " " + GroupsArray[i][1]['type'])); tr.appendChild(td);
      
                td = Builder.node('td', {className: 'WhiteBkg'}, [
                    Builder.node('div', {id: 'picDiv' + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'], style: "position:relative; width:50px; height:50px;",
                        onmouseover:"picDivShow(jg_250, '"+deviceData['type']+"', 250, '" + GroupsArray[i][0]['chnNr']+ "+" + GroupsArray[i][1]['chnNr']+"', this);", onmouseout:"picDivHide(jg_250);"})
                ]);
                Element.setStyle(td, {textAlign: 'left'});
                tr.appendChild(td);
      
      
                td = Builder.node('td', {className: 'GrayBkg'}, ''); tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'},  strCut(GroupsArray[i][0]['sn'])); tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][0]['iface'])); tr.appendChild(td);
                td = Builder.node('td', {id: 'trans' + GroupsArray[i][0]['id'] + GroupsArray[i][1]['id'], 
                     className: 'GrayBkg'}, removeDuplicates(GroupsArray[i][0]['trans'], GroupsArray[i][1]['trans'], "")); tr.appendChild(td);

                td = Builder.node('td', {className: 'WhiteBkg'}, ''); 
                writeDeviceAction(td, false, false, false, GroupsArray[i][0], true);
                tr.appendChild(td);
                
                var jg_0 = new jsGraphics('picDiv' + GroupsArray[i][0]['id'] +GroupsArray[i][1]['id']);
                InitGD(jg_0, 50);
                Draw(jg_0, deviceData['type'], 50, "" + GroupsArray[i][0]['chnNr']+"+"+GroupsArray[i][1]['chnNr']);
      
                // Kanal-elemente
                var sTrId = "" + deviceData['id'] + GroupsArray[i][0]['id'] +  GroupsArray[i][1]['id'];
      
                for(var x = 0; x < 2; x++) {
                    if (GroupsArray[i][x]['show']) {
                        if (fltD.objPassFilter(GroupsArray[i][x]))
                            iChnPassedFilter++;
                    }
            
                    arSubIds[arSubIds.length] = sTrId + (x+1);
                    tr = $("tblDevices").insertRow(iInsIdx++);iElemsAdded++;
                    tr.id = sTrId + (x+1);
                    GroupsArray[i][x]['trid'] = tr.id;
                    Element.setStyle(tr, {display: "none"});
                    td = Builder.node('td', {colSpan:3}); tr.appendChild(td);
          
                    if (NAV_IE) {
                        td = Builder.node('td', {className: 'WhiteBkg'}, 
                            [Builder.node('div', {id:'tdn' + GroupsArray[i][x]['id'], style: "height:100%;width:100%;cursor:pointer", onclick: "textEdit(id, '"+GroupsArray[i][x]['id']+";NAME', false, 0, 0)"}, GroupsArray[i][x]['name']) ]); tr.appendChild(td);
                    } else {
                        td = Builder.node('td', {className: 'WhiteBkg', style: "cursor:pointer", id:'tdn' + GroupsArray[i][x]['id'], onclick: "textEdit(id, '"+GroupsArray[i][x]['id']+";NAME', false, 0, 0)"}, GroupsArray[i][x]['name']); tr.appendChild(td);
                    }
          
                    td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][x]['type'])); tr.appendChild(td);
                    td =  Builder.node('td', {className: 'WhiteBkg'}, [
                        Builder.node('div', {id: 'picDiv' + GroupsArray[i][x]['id'], style: "position:relative; width:50px; height:50px;",
                            onmouseover:"picDivShow(jg_250, '"+deviceData['type']+"', 250, "+GroupsArray[i][x]['chnNr']+", this);", onmouseout:"picDivHide(jg_250);"})
                    ]);
                    Element.setStyle(td, {textAlign: 'left'});
                    tr.appendChild(td);
                    td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][x]['desc'])); tr.appendChild(td);
                    td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][x]['sn'])); tr.appendChild(td);
                    td = Builder.node('td', {className: 'GrayBkg'}, strCut(GroupsArray[i][x]['iface'])); tr.appendChild(td);
                    var trsId = "trs" + GroupsArray[i][x]['id'];
                    var s = "WhiteBkg";
                    var sClick = "";
                    var sCur = "";
                    if (GroupsArray[i][x]['transOp'] < 3) {
                        s = "GrayBkg";
                    } else {
                        sClick = "changeTransMode('"+ GroupsArray[i][x]['id'] + "','" + trsId + "')";
                        sCur = "cursor:pointer";
                    }
          
                    if (sClick != "") {
                        td = Builder.node('td', {className: s}, 
                            [Builder.node('div', {id: trsId, style: "height:100%;width:100%;"+sCur, onclick: sClick}, GroupsArray[i][x]['trans']) ]); tr.appendChild(td);   
                    } else {
                        td = Builder.node('td', {className: s}, GroupsArray[i][x]['trans']);
                    }
                    tr.appendChild(td);
            
                    td = Builder.node('td', {className: 'WhiteBkg'}, ''); 
                    writeDeviceAction(td, false, false, false, GroupsArray[i][x]);
                    tr.appendChild(td);
                    
                    var jg_0 = new jsGraphics('picDiv' + GroupsArray[i][x]['id']);
                    InitGD(jg_0, 50);
                    Draw(jg_0, deviceData['type'], 50, GroupsArray[i][x]['chnNr']);
                }
            }
            

            // Kanäle
            for (var i = 0; i < chns.length; i++) {
                arSubIds[arSubIds.length] = ""+ deviceData['id'] + chns[i]['id'];
                tr = $("tblDevices").insertRow(iInsIdx++);iElemsAdded++;
                tr.id = ""+ deviceData['id'] + chns[i]['id'];
                chns[i]['trid'] = tr.id;
                if (!fltD.objPassFilter(chns[i]))
                    Element.setStyle(tr, {display: 'none'});
                else
                    iChnPassedFilter++;
                td = Builder.node('td', {colSpan: 2}); tr.appendChild(td);
      
                if (NAV_IE) {
                    td = Builder.node('td', {colSpan: 2, className: 'WhiteBkg'}, 
                        [Builder.node('div', {id:'tdn' + chns[i]['id'], style: "height:100%;width:100%;cursor:pointer", onclick: "textEdit(id, '"+chns[i]['id']+";NAME', false, 0, 0)"}, chns[i]['name'], true) ]); tr.appendChild(td);
                } else {
                    td = Builder.node('td', {colSpan: 2, className: 'WhiteBkg', style: "cursor:pointer", id:'tdn' + chns[i]['id'], onclick: "textEdit(id, '"+chns[i]['id']+";NAME', false, 0, 0)"}, chns[i]['name'], true); tr.appendChild(td);
                }
                Element.setStyle(td, {padding: 0, margin: 0});
                if (NAV_IE) {
                    arIeBugCells[arIeBugCells.length] = 'tdn' + chns[i]['id'];
                }
                tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(chns[i]['type'])); tr.appendChild(td);
                td = Builder.node('td', {className: 'WhiteBkg'}, [
                    Builder.node('div', {id: 'picDiv' + chns[i]['id'], style: "margin:0; padding:0; position:relative; width:50px; height:50px;",
                        onmouseover:"picDivShow(jg_250, '"+deviceData['type']+"', 250, "+chns[i]['chnNr']+", this);", onmouseout:"picDivHide(jg_250);"})
                ]); 
                Element.setStyle(td, {textAlign: 'left'});
                tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(chns[i]['desc'])); tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(chns[i]['sn'])); tr.appendChild(td);
                td = Builder.node('td', {className: 'GrayBkg'}, strCut(chns[i]['iface'])); tr.appendChild(td);
                var trsId = "trs" + chns[i]['id'];
                var s = "WhiteBkg";
                var sClick = "";
                var sCur = "";
                if (chns[i]['transOp'] < 3) {
                    s = "GrayBkg";
                } else {
                    sClick = "changeTransMode('"+chns[i]['id']+ "','" + trsId + "')";
                    sCur = "cursor:pointer";
                }
    
                if (sClick != "") {
                    td = Builder.node('td', {className: s}, 
                        [Builder.node('div', {id: trsId, style: "height:100%;width:100%;"+sCur, onclick: sClick}, chns[i]['trans']) ]);   
                } else {
                    td = Builder.node('td', {className: s}, chns[i]['trans']);
                }
                tr.appendChild(td);
      
                td = Builder.node('td', {className: 'WhiteBkg'}, ''); 
                writeDeviceAction(td, false, false, false, chns[i], 0, chns[i]['install_test']);
                tr.appendChild(td);
                var jg_0 = new jsGraphics('picDiv' + chns[i]['id']);
                InitGD(jg_0, 50);
                Draw(jg_0, deviceData['type'], 50, chns[i]['chnNr']);
            }
  
            arDevices[arIdx]['chnsgr'] = GroupsArray;
            if (GroupsArray.length > 0) {
                // IE-Bug: Doppelte Darstellung des rechten Rands vermeiden
                //for (var i = 0; i < arIeBugCells.length; i++) {
                //  Element.setStyle($(arIeBugCells[i]), {borderRight: "0px"});
                //}
            }
            arDevices[arIdx]['chns'] = chns;
            for (var i = arIdx +1; i < arDevPosis.length; i++) {
                arDevPosis[i] = arDevPosis[i] + iElemsAdded;
            }
            if ( autoExpGr ) {
                if (arDevices[arIdx]['chnsgr']) {
                    for (var i = 0; i < arDevices[arIdx]['chnsgr'].length; i++) {
                        if( fltD.objPassFilter(arDevices[arIdx]['chnsgr'][i][0]) && fltD.objPassFilter(arDevices[arIdx]['chnsgr'][i][1]) )
                        {
                            expandGroup(arDevices[arIdx]['id'],
                                arDevices[arIdx]['chnsgr'][i][0]['id'],
                                arDevices[arIdx]['chnsgr'][i][1]['id']);
                        }
                    }
                }
            }
            
            if (iChnPassedFilter == 0) {
                bPassFilterName= fltD.objPassFilter(arDevices[arIdx]);
                if(bPassFilterName){
                    for (var i = 0; i < arDevices[arIdx]['chns'].length; i++){
                        show(arDevices[arIdx]['chns'][i]['trid']);
                        show('picDiv' + arDevices[arIdx]['chns'][i]['id']);  
                    } 
                } else {
                    hide("tr" + arDevices[arIdx]['id']); 
                    hide("picDiv" + arDevices[arIdx]['id']);
                    if (arDevices[arIdx]['chnsgr']) {
                        for (var i = 0; i < arDevices[arIdx]['chnsgr'].length; i++) {
                            hide("" + arDevices[arIdx]['id'] + arDevices[arIdx]['chnsgr'][i][0]['id'] + arDevices[arIdx]['chnsgr'][i][1]['id']);
                        }
                    }
                }
            }
            bEIP = false;
            document.body.style.cursor="auto";
        }
    }
    
    puts "</script>"
}

cgi_eval {
    # cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }
    read_interfaces
    
    http_head

    if {[session_requestisvalid 0] > 0} then {
        put_page
    }

}

