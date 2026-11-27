# License overview

The content of OpenCCU-Base are published under different licenses.<br>
If not stated otherwise, alls files are published under the "Homematic Software License" version 2.0 (HMSL 2.0).

Der Inhalt von OpenCCU-Base wird unter verschiedenen Lizenzbedingungen veröffentlicht.<br>Sofern nicht anders deklariert, werden alle Dateien unter den Bedingungen der "Homematic Software Lizenz" version 2.0 (HMSL 2.0) veröffentlicht.

# Exceptions

|Path|License|
|----|-------|
lib/libXmlRpc.so | LGPL-2.1
lib/libxmlparser.so | LGPL-2.1
src/bcm2835_raw_uart/bcm2835_raw_uart.c | GPL-2.0
src/eq3_char_loop/eq3_char_loop.c | GPL-2.0+
src/eq3_char_loop/mxs_raw_auart.c | GPL-2.0+
src/libxmlrpc | LGPL-2.1
src/libxmlparser | LGPL-2.1
src/webui | Apache 2.0 if not stated otherwise
src/webui/www/webui/js/extern/codemirror6 | MIT (CodeMirror, see [codemirror-MIT.txt](codemirror-MIT.txt))
src/webui/www/ise/img/occu-logo*.png, src/webui/www/ise/img/rssi-icon.png | Copyright of the OpenCCU team, no reuse in own distributions or forks (see [openccu-images.md](openccu-images.md))
webui | Apache 2.0 if not stated otherwise

# Third-party icons

The WebUI (`src/webui`, installed to `www`) contains icons of third parties. They are not covered by the Apache 2.0 license of the WebUI and keep the licenses of their authors; the help page of the WebUI names their sources:

- icons from [Icons8](https://icons8.com) (e.g. the menu icons in `src/webui/www/ise/img/menuicons` and icons of the HB devices), used under the free [Icons8 license](https://icons8.com/license), which requires a link to Icons8
- icons by [popcic](https://www.flaticon.com/authors/popcic) from [Flaticon](https://www.flaticon.com) (icons of the HB devices), used under the free Flaticon license, which requires to credit the author and Flaticon
- icons of the [KNX-UF iconset](https://github.com/OpenAutomationProject/knx-uf-iconset) of the OpenAutomationProject (e.g. in `src/webui/www/ise/img/icons_hb_dis_ep_75bw` and `src/webui/www/ise/img/garage`), licensed under [CC BY-SA 3.0 DE](https://creativecommons.org/licenses/by-sa/3.0/de/)
