#!/bin/tclsh

#source $env(DOCUMENT_ROOT)/tcl/extern/cgi.tcl

source $env(DOCUMENT_ROOT)/once.tcl
sourceOnce $env(DOCUMENT_ROOT)/cgi.tcl
sourceOnce $env(DOCUMENT_ROOT)/session.tcl
#sourceOnce $env(DOCUMENT_ROOT)/common.tcl
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
    
    set iface [lindex [array names interfaces] 0]
    catch {
        import iface
    }
    
    set url $interfaces($iface)
    
    set channel "channel n.a"
    set title "title n.a"
    
    catch {import channel}
    catch {import title}
	
   
    puts "<div class=\"popupTitle\">$title - $channel</div>"
    puts {
        <div style="background-color: White;overflow:auto;width: 100%;height: 100%;">
        <table class="popupTable" style="border:solid 1px Black;" cellpadding="10" cellspacing="0">
            <colgroup>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:7%;"/>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:23%;"/>
                <col style="width:30%;"/>
            </colgroup>
            <tr>
                <th style="border:solid 1px Black;height:40px;">Name</th>
                <th style="border:solid 1px Black;height:40px;">Typen-Bezeichnung</th>
                <th style="border:solid 1px Black;height:40px;">Bild</th>
                <th style="border:solid 1px Black;height:40px;">Bezeichnung</th>
                <th style="border:solid 1px Black;height:40px;">Seriennummer</th>
                <th colspan="2" style="border:solid 1px Black;height:50px;">BidCoS-Interface</th>
            </tr>
            <tr class="popupGrayCells" style="padding: 10px;">
    }
    
    array set dev_descr [xmlrpc $url getDeviceDescription [list string $channel]]
    set name $dev_descr(ADDRESS)
    catch { 
        array set dev_metadata [xmlrpc $url getAllMetadata [list string $dev_descr(ADDRESS)]] 
        set name $dev_metadata(NAME)
    }

    set bidcos_iface_list [xmlrpc $url listBidcosInterfaces]

    puts "<td style='border:solid 1px Black;'>$name</td>"
    puts "<td style='border:solid 1px Black;'>$dev_descr(TYPE)</td>"
    puts "<td style='text-align:left;border:solid 1px Black;background-color:White'>"
    
    puts "<div id='picDivT$channel' style='margin:0; padding:0; position:relative; width:50px; height:50px;'"
    puts " onmouseover='picDivShow(jg_250, \"$dev_descr(TYPE)\", 250, -1, this);' onmouseout='picDivHide(jg_250);' "
    puts "></div>"
  
    puts "</td>"
    puts "<td style='border:solid 1px Black;'>"
    puts "<div id='devdesc'></div>"
    puts "</td>"
    puts "<td style='border:solid 1px Black;'>$dev_descr(ADDRESS)</td>"
    puts "<td style='text-align:left;background-color:White;width:50px;border:solid 1px Black;'>"
    puts "<select id='selIface' onchange='updateDescription();'>"
    
    foreach _bidcos_iface $bidcos_iface_list {
        array set bidcos_iface $_bidcos_iface
        if { "$dev_descr(INTERFACE)" == "$bidcos_iface(ADDRESS)" } {
            set selected "selected"
        } else {
            set selected ""
        }
        puts "<option $selected>$bidcos_iface(ADDRESS)</option>"
    }
    puts "</select><br />"
    set checked ""
    if { $dev_descr(ROAMING) } { set checked "checked" }
    puts "<input type='checkbox' id='chkRoaming' $checked></input>"
    puts "Zuordnung automatisch anpassen"
    puts "</td>"
    
    puts "<td style='border:solid 1px Black;width:200px;'>"
    puts "<div id='ifaceDesc'></div>"
    puts "</td>"
    puts {
                </tr>
            </table>
        </div>
    }

    puts {
        <div class="popupControls" style="width:100%">
            <table>
                <tr>
                    <td><div onclick="closeOK()" style="width:100px;margin: 5px;">OK</div></td>
                    <td><div onclick="closeCancel()" style="width:100px;margin: 5px;">Abbrechen</div></td>
                </tr>
            </table>
        </div>
    }
    puts "<script type='text/javascript'>"
    puts "sPicDivId = 'picDivT$channel';"
    puts "sDevLbl = '$dev_descr(TYPE)';"
    
    puts {$('devdesc').innerHTML = DEV_getDescription( sDevLbl );}
    puts "var channel_id = '$channel';"
    puts "var iface_descriptions = new Object();"
    puts "var iface_ids = new Object();"
    
    set index 0
    foreach _bidcos_iface $bidcos_iface_list {
        array set bidcos_iface $_bidcos_iface
        puts "iface_descriptions\[$index\] = '$bidcos_iface(DESCRIPTION)';"
        puts "iface_ids\[$index\] = '$bidcos_iface(ADDRESS)';"
        incr index
    }
    
    puts {
            var jg_0 = new jsGraphics(sPicDivId);  
            InitGD(jg_0, 50);
            Draw(jg_0, sDevLbl, 50, -1);

            updateDescription = function() {
                $("ifaceDesc").innerHTML = iface_descriptions[$("selIface").selectedIndex];
            }
            
            closeCancel = function() {
                PopupClose();
            }
    
            closeOK = function() {
                var selIface = iface_ids[$("selIface").selectedIndex];
                var chkRoaming = $("chkRoaming").checked;
                configMetadata.saveBidcosInterface( channel_id, selIface, chkRoaming );
                PopupClose(selIface, chkRoaming);
            }
            
            updateDescription();
            
        </script>
    }
}

cgi_eval {
    cgi_debug -on
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

