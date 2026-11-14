#!/bin/tclsh
source ../once.tcl
sourceOnce cgi.tcl
sourceOnce session.tcl
sourceOnce common.tcl
loadOnce tclrpc.so

set RFD_URL $env(BIDCOS_SERVICE)

proc action_put_page {} {
    global env RFD_URL
  
    set serial ""
    catch { import serial }
    
    division {class="popupTitle"} {
        puts "Ger&auml;te anlernen"
    }
    division {style="background-color: White;overflow: auto;width: 100%;height: 100%;"} {
        table {class="popupTable"} {style="font-size: 8pt;"} {border="1"} {
            table_row {style="padding: 10px 0px 10px 0px;"} {
                table_data {style="background-color:#999999;color: White;vertical-align: middle;font-size: 10pt;"} {
                    puts "BidCoS-RF"
                }
                table_data {width="50%"} {align="left"} {style="vertical-align: top;color: Black;background-color: White;"} {
                    puts "BidCoS-RF - Variante 1: Direktes Anlernen"
                    division {class="popupControls"} {style="border: 0"} {
                        table {
                            table_row {
                                table_data {style="border:0; width:80%"} {
                                    division {id="time_bar"} {style="background-color: White;color: Black;"} {
                                        puts "Anlernmodus nicht aktiv"
                                    }
                                }
                                table_data {style="border:0; width:10%"} {
                                    division {style="width:100px;margin: 5px;"} {onClick="rf_install_mode(true)"} {
                                        puts "BidCoS-RF Anlernmodus"
                                    }
                                }
                            }
                        }
                    }
                    puts "Falls Sie das anzulernende Ger&auml;t im direkten Zugriff haben, ist ein direktes Anlernen einfach m&ouml;glich:"
                    puts "Bet&auml;tigen Sie den Button &quot;BidCoS-RF Anlernmodus&quot;. Dadurch wird der Konfigurationsadapter f&uuml;r eine Minute"
                    puts "in seinen Anlernmodus versetzt. Innerhalb dieser Zeit ist dann der Anlerntaster des anzulernenden Ger&auml;tes zu"
                    puts "bet&auml;tigen. N&auml;here Einzelheiten dazu entnehmen Sie bitte der Bedienungs- bzw. Installationsanleitung"
                    puts "des entsprechenden Ger&auml;tes."
                }
                table_data {width="50%"} {align="left"} {style="vertical-align: top;color: Black;background-color: White;"} {
                    puts "BidCoS-RF - Variante 2: Anlernen mit Seriennummer"
                    table {style="font-size: 8pt;color: Black;"} {width="100%"} {
                        table_row {
                            td {width="20"} {}
                            td "Seriennummer eingeben:"
                            table_data {
                                cgi_text serial=$serial {id="text_serial"}
                            }
                        }
                    }
                    division {class="popupControls"} {style="border: 0"} {
                        table {
                            table_row {
                                table_data {style="border:0; width:80%"} {}
                                table_data {style="border:0; width:10%"} {
                                    division {style="width:100px;margin: 5px;"} {onClick="rf_serial()"} {
                                        puts "Ger&auml;t anlernen"
                                    }
                                }
                            }
                        }
                    }
                    puts "Falls Sie das anzulernende Ger&auml;t nicht im direkten Zugriff haben, ist das Anlernen mittels"
                    puts "der Ger&auml;te-Seriennummer m&ouml;glich. Geben Sie dazu die Seriennummer des anzulernenden Ger&auml;tes ein"
                    puts "und bet&auml;tigen Sie den Button &quot;Ger&auml;t anlernen&quot;."
                }
            }
        }
    }
    division {class="popupControls"} {
        table {
            table_row {
                table_data {style="border:0; width:10%"} {
                    division {style="width: 100px;height: 30px;margin: 5px;"} {onClick="OnBack();"} {
                        puts "Zur&uuml;ck"
                    }
                }
                table_data {style="border:0; width:20%"} {
                    division {id="cp_new_device_count"} {style="width: 150px;border: 0;background-color: White;color: Black;"} {
                    }
                }
                table_data {style="border:0; width:70%"} {}
            }
        }
    }
    puts ""
    cgi_javascript {
        puts "var url = \"$env(SCRIPT_NAME)?sid=\" + SessionId;"
        puts "var add_device_original_count = [get_device_count];"
        puts {
            update_new_device_count = function() {
                var pb = "action=get_device_count";
                var opts = {
                    postBody: pb,
                    sendXML: false,
                    onSuccess: function(transport) {
                        if (transport.responseText.match(/^Success/g)){
                            var result=transport.responseText.split(" ");
                            var new_count = result[1] - add_device_original_count;
                            var nodeText = "";
                            if( new_count == 0 )nodeText="Kein neues Gerät";
                            else if( new_count == 1 )nodeText="Ein neues Gerät";
                            else nodeText = new_count + " neue Geräte";
                            var node = $("cp_new_device_count");
                            node.innerHTML = nodeText;
                        }
                    }
                };
                new Ajax.Request(url, opts);
            }
        }
        puts {
            install_time_remain=0;
            update_time_bar = function() {
                install_time_remain-=1;
                if((install_time_remain%5)==0){
                    var pb = "action=get_install_status";
                    var opts = {
                        postBody: pb,
                        sendXML: false,
                        onSuccess: function(transport) {
                            if (!transport.responseText.match(/^Success/g)){
                                cp_adddev_updater.stop();
                                return;
                            }
                            var result=transport.responseText.split(" ");
                            var serial="";
                            if(result.length >= 3)serial=result[2];
                            if(serial != ""){
                                cp_adddev_updater.stop();
                                dlgPopup.hide();
                                dlgPopup.setWidth(400);
                                dlgPopup.LoadFromFile(url, "action=put_key_dialog&serial="+serial);
                            }else{
                                install_time_remain=result[1];
                                if(install_time_remain<=0){
                                    if ($("time_bar")) { document.getElementById("time_bar").firstChild.nodeValue="Anlernmodus nicht aktiv"; }
                                    cp_adddev_updater.stop();
                                    install_time_remain=0;
                                }
                            }
                        }
                    };
                    new Ajax.Request(url, opts);
                } else if((install_time_remain%5)==2){
                    update_new_device_count();
                }
		if(install_time_remain<=0){
                    document.getElementById("time_bar").firstChild.nodeValue="Anlernmodus nicht aktiv";
                    cp_adddev_updater.stop();
                    update_new_device_count();
                    install_time_remain=0;
                }else{
                    document.getElementById("time_bar").firstChild.nodeValue="Anlernmodus noch "+String(install_time_remain)+"s aktiv";
                }
            }
        
            rf_install_mode = function(activate) {
                var pb = "action=rf_install_mode";
                pb += "&activate="+activate;
                if(!activate && cp_adddev_updater)cp_adddev_updater.stop();
                var opts = {
                    postBody: pb,
                    sendXML: false,
                    onSuccess: function(transport) {
                        if (!transport.responseText.match(/^Success/g)){
                            alert("BidCoS-RF Anlernmodus konnte nicht aktiviert werden.");
                        }
                        result=transport.responseText.split(" ");
                        install_time_remain=result[1];
                        if(install_time_remain > 0){    
                            install_time_remain++;
                            if(!cp_adddev_updater){
                                cp_adddev_updater=new PeriodicalExecuter(update_time_bar, 1);
                            }else if(!cp_adddev_updater.timer){
                                cp_adddev_updater.registerCallback();
                            }
                        }else{
                            install_time_remain=0;
                        }
                        update_time_bar();
                    },
                    onFailure: function(transport) {
                        alert("BidCoS-RF Anlernmodus konnte nicht aktiviert werden.");
                    }
                };
                new Ajax.Request(url, opts);
            }
        }
        puts {
            rf_serial = function() {
                var pb = "action=rf_serial&";
                pb += "serial="+document.getElementById("text_serial").value;
                document.body.style.cursor = "wait";
                var opts = {
                    postBody: pb,
                    sendXML: false,
                    onSuccess: function(transport) {
                        document.body.style.cursor = ""; 
                        if (transport.responseText.match(/^Success/g)){
                            $("text_serial").value="";
                            update_new_device_count();
                        } else if (transport.responseText.match(/^KeyMismatch/g)){
                            rf_install_mode(false);
                            if(cp_addev_updater)cp_adddev_updater.stop();
                            dlgPopup.hide();
                            dlgPopup.setWidth(400);
                            dlgPopup.LoadFromFile(url, "action=put_key_dialog&serial="+$("text_serial").value+"&with_serial=1");
                        } else {
                            alert("BidCoS-RF Anlernen mit Seriennummer "+document.getElementById("text_serial").value+ unescape(" fehlgeschlagen. Bitte %FCberpr%FCfen Sie die Seriennummer."));
                        }
                    },
                    onFailure: function(transport) {
                        document.body.style.cursor = ""; 
                    }
                };
                new Ajax.Request(url, opts);
            }
        }
        puts {
            OnBack = function() {
                rf_install_mode(false);
                PopupClose();
                WebUI.enter(DeviceListPage);
            }
        }
        catch { 
            set install_time_remain [xmlrpc $RFD_URL getInstallMode] 
            if { $install_time_remain } {
                puts "install_time_remain=$install_time_remain;"
                puts {
                    if(!cp_adddev_updater)cp_adddev_updater=new PeriodicalExecuter(update_time_bar, 1);
                    else if(!cp_adddev_updater.timer)cp_adddev_updater.registerCallback();
                    update_time_bar();
                }
            }                
        }
        puts "update_new_device_count();"
        catch {
            import call_js
            puts "$call_js;"
        }
    }
}

proc get_device_count {} {
    global RFD_URL
    set FLAG_INTERNAL 2
    set count 0
    catch {
        set i 0
        set devlist [xmlrpc $RFD_URL listDevices [list bool 0]]
        foreach dev $devlist {
            array_clear dev_descr
            array set dev_descr $dev
            if { $dev_descr(FLAGS) & $FLAG_INTERNAL } continue
            if { "$dev_descr(PARENT)" == "" } {
                incr i
            }
        }
        set count $i
    }
    return $count
}

proc action_get_device_count {} {
    set count [get_device_count]
    if { $count < 0 } {
        puts "Failure"
    } else {
        puts "Success $count "
    }
}

proc action_put_key_dialog {} {
    global env
    
    import serial
    set with_serial 0
    catch { import with_serial }
  
    division {class="popupTitle"} {
        puts "Ger&auml;te anlernen - Sicherheitsabfrage"
    }
    division {style="background-color: White;overflow: auto;width: 100%;height: 100%;"} {
        table {class="popupTable"} {style="font-size: 8pt;"} {border="1"} {
            table_row {style="padding: 10px 0px 10px 0px;"} {
                table_data {align="left"} {style="vertical-align: top;color: Black;background-color: White;"} {
                    puts "Sie haben gerade versucht, das Ger&auml;t $serial"
                    if { $with_serial } {
                        puts "durch Eingabe der Seriennummer"
                    } else {
                        puts "im Anlernmodus"
                    }
                    puts "anzulernen. Dieser Vorgang konnte nicht durchgef&uuml;hrt werden."
                    br
                    puts "Wahrscheinlich ist diesem Ger&auml;t ein dem System unbekannter System-Sicherheitsschl&uuml;ssel"
                    puts "zugeordnet. Bitte geben Sie den zum Ger&auml;t geh&ouml;renden System-Sicherheitsschl&uuml;ssel ein und"
                    puts "starten den Anlernvorgang erneut."
                    table {style="font-size: 8pt;color: Black;"} {width="100%"} {
                        table_row {
                            td {width="10"} {}
                            td "System-Sicherheitsschl&uuml;ssel:"
                            table_data {
                                cgi_text aes_key= {id="text_aes_key"} {width="50"}
                            }
                        }
                    }
                }
            }
        }
    }
    division {class="popupControls"} {
        table {
            table_row {
                table_data {style="border:0; width:10%"} {
                    division {style="width: 100px;height: 30px;margin: 5px;"} {onClick="cancel();"} {
                        puts "Abbrechen"
                    }
                }
                table_data {style="border:0; width:10%"} {
                    division {style="width: 200px;height: 30px;margin: 5px;"} {onClick="try_again();"} {
                        puts "Schl&uuml;ssel setzen und erneut versuchen"
                    }
                }
                table_data {style="border:0; width:80%"} {}
            }
        }
    }
    puts ""
    cgi_javascript {
        puts "var url = \"$env(SCRIPT_NAME)?sid=\" + SessionId;"
        
        puts {
            try_again = function() {
                var pb = "action=set_temp_key&";
                pb += "key="+document.getElementById("text_aes_key").value;
                var opts = {
                    postBody: pb,
                    sendXML: false,
                    onSuccess: function(transport) {
                        if (transport.responseText.match(/^Success/g)){
                            go_back();
                        } else {
                            alert(unescape("Tempor%E4rer System-Sicherheitsschl%FCssel konnte nicht gesetzt werden."));
                        }
                    }
                };
                new Ajax.Request(url, opts);
            }
            cancel = function() {
                dlgPopup.hide();
                dlgPopup.setWidth(800);
                dlgPopup.LoadFromFile(url);
            }
        }
        
        puts "go_back = function() \{"
        if {$with_serial} {
            puts "    var pb=\"&serial=$serial&call_js=rf_serial()\";"
        } else {
            puts "    var pb=\"&call_js=rf_install_mode(true)\";"
        }
        puts {
            dlgPopup.hide();
            dlgPopup.setWidth(800);
            dlgPopup.LoadFromFile(url, pb);
        }
        puts "\}"
    }
}

proc action_rf_install_mode {} {
    global RFD_URL
    set activate true
    catch { import activate }
    catch {
        xmlrpc $RFD_URL setInstallMode [list bool $activate]
        set time [xmlrpc $RFD_URL getInstallMode]
        puts "Success $time "
        return
    } errMsg
    puts "$errMsg"
}

proc action_get_install_status {} {
    global RFD_URL
    catch {
        set time [xmlrpc $RFD_URL getInstallMode]
        set serial [xmlrpc $RFD_URL getKeyMismatchDevice [list bool true]]
        puts "Success $time $serial "
        return
    } errMsg
    puts "$errMsg"
}

proc action_rf_serial {} {
    global RFD_URL
    if {[catch {
        import serial
        xmlrpc $RFD_URL addDevice [list string $serial]
        puts "Success"
        return
    } errMsg] == -7 } {
        puts "KeyMismatch"
    } else {
#        puts "KeyMismatch"
        puts "$errMsg"
    }
}

proc action_set_temp_key {} {
    global RFD_URL
    if {[catch {
        import key
        xmlrpc $RFD_URL setTempKey [list string $key]
        puts "Success"
        return
    } errMsg]} {
        puts "$errMsg"
    }
}

cgi_eval {
    #cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }

    set action "put_page"

    http_head
    catch { import action }
    if {[session_requestisvalid 8] > 0} then action_$action
}

