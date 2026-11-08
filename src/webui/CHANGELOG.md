# webui ChangeLog

Changes of the WebUI of the CCU (`src/webui`, installed to `www`). Newest
entries first.

The WebUI has no version of its own: changes not yet released are collected
under `Unreleased` and get the OpenCCU-Base release they are part of when it
is published.

## Unreleased

### Changes

Entries marked "OpenCCU patch NNNN" were an OpenCCU rootfs patch of the
WebUI before; they describe the function of the patch and what was corrected
or extended in it.

* the file selection of the software update, the certificate upload, the
  backup restore, the add-on installation and the device firmware upload
  shows a "Choose file..." button and the name of the selected file instead
  of the browser's file input; the selection stays reachable with the
  keyboard and the button shows a focus outline. Except for the add-on
  installation, it only offers files of the expected type, and the buttons
  of the software update, certificate upload and backup restore are placed
  below their description. Compared to the patch, the download button of the
  software update is aligned with the other buttons again, other file inputs
  of the WebUI stay visible, the software update also offers tar archives
  and SD card, rootfs and bootfs images, and cancelling the file selection
  shows "No file selected." again instead of failing (OpenCCU patch 0163).
  ([#323](https://github.com/OpenCCU/OpenCCU-Base/pull/323), @jens-maus)
* the error pages shown while the CCU is starting (HTTP 500 and 503) center
  the logo and the hint to wait in the middle of the window and no longer
  show a separate heading. The content is shown and the check for the
  redirect starts as soon as the page is translated instead of waiting for
  the logo, and the check no longer blocks the page while it waits for an
  answer and continues after a failed request; without JavaScript the
  content stays hidden as before (OpenCCU patch 0149).
  ([#322](https://github.com/OpenCCU/OpenCCU-Base/pull/322), @jens-maus)
* style fixes across the WebUI: the lists of "Status and control", the
  device, room, function, program, system variable and user lists, the
  service and alarm messages, the system protocol, the device parameters,
  direct links and profiles, the firmware overview and the diagram and group
  pages of the HMServer use the full width with fixed widths only for icon,
  picture and action columns, have uniform header cells and buttons, and no
  longer set their height in JavaScript. Entries of "Status and control" are
  highlighted under the mouse pointer, and an administrator opens the
  configuration of a device by clicking its picture there. Device pictures
  are centered, the blind and light controls are aligned, the German tree
  view buttons of the footer break into two lines, a horizontal rule uses the
  separator color of the theme and checkboxes are aligned with their label.
  The key and serial number of a Homematic IP device are entered in upper
  case. The favorites viewer uses a table of its own, and the header of the
  function column in the room list is closed with `</th>`. Loading the
  channels of a room or function in the room and function list resets only
  the sort header, so the filter buttons keep working (OpenCCU patch 0039).
  ([#321](https://github.com/OpenCCU/OpenCCU-Base/pull/321), @jens-maus)
* fix the time settings reporting that the position was transferred to the
  Homematic IP devices when reading the position data failed: the page now
  says that the devices were not updated, and a newly added device keeps its
  own position in this case instead of stopping with a script error.
  ([#320](https://github.com/OpenCCU/OpenCCU-Base/pull/320), @jens-maus)
* fix saving a system variable in its dialog when its name or description
  contains characters the dialog does not allow, as scripts or other tools
  can set them: the dialog then refused to save, also when only the value
  was changed. Name and description are now only checked when they were
  edited, and are escaped for the ReGa script. Saving also no longer
  replaces the name, the description and the value names of variables like
  the presence variable (`${sysVarPresence}`) by their translation, and a
  quote no longer cuts off the description.
  ([#319](https://github.com/OpenCCU/OpenCCU-Base/pull/319), @jens-maus)
* HTML-escape the names of devices, channels, rooms, functions, programs,
  system variables and favorites where the WebUI writes them into HTML, so
  markup like `<b>` that scripts or other tools put into a name is shown as
  text, and a quote no longer cuts off the name in the edit dialog of a
  system variable. This covers "Status and control", the device, room,
  function and program lists, the favorites and their editor, the start
  page, the service and alarm messages, the program editor and the channel
  chooser. Room and function names that are updated after assigning a
  channel, and the alarm and service entries of the event log, are also
  escaped for JavaScript now, as they broke on an apostrophe or a quote.
  ([#315](https://github.com/OpenCCU/OpenCCU-Base/pull/315); refs
* fix the translation of pages decoding %-escapes like `%3C` in names and
  other texts next to a translation placeholder, so that they could be
  rendered as HTML. Only the translations themselves are decoded now. Unknown
  placeholders in translated strings are no longer decoded either, and `$&`
  or `$1` in a translation is inserted literally.
  ([#316](https://github.com/OpenCCU/OpenCCU-Base/pull/316); refs
  [OpenCCU/OpenCCU#4632](https://github.com/OpenCCU/OpenCCU/issues/4632),
  @jens-maus)
* more countries, cities and time zones in the time settings: the location
  list has about 150 countries with their cities and IANA time zones, and the
  time zone selection also offers all zones of `zone.tab`. The UTC offsets
  set on Homematic IP devices with a week profile are now taken from the
  configured time zone of the system instead of a fixed table, also in the
  southern hemisphere. A saved time zone with an old name (like
  `Europe/Kiev`) is selected under its current name (OpenCCU patch 0054).
  ([#318](https://github.com/OpenCCU/OpenCCU-Base/pull/318), @jens-maus)
* advanced editing of a system variable: the type of a system variable
  used in programs can no longer be changed, and the dialog names these
  programs. An existing variable cannot become an alarm variable and an alarm
  variable keeps its type. A changed value list shows the affected programs
  and can move their references to the new positions of the entries, a
  narrowed number range shows the programs that use values outside it. An
  empty value list and a minimum above the maximum are rejected. Value names
  and value lists are shown as text and escaped for the ReGa script, and the
  value selection follows the edited value names and value list. Saving no
  longer keeps the value names and value list of the previous type (OpenCCU
  patch 0206).
  ([#317](https://github.com/OpenCCU/OpenCCU-Base/pull/317), @jens-maus)
* change the value of a system variable directly: administrators can click
  the value of a system variable (except the presence variable) in "Status
  and control", in a favorite on the start page or in the system variables of
  the user on the start page to change it in a small dialog, and the dialog
  for creating and editing a system variable gets a field for its value. The
  channel assignment of the dialog is now a checkbox. Text values and units
  are shown as text in the dialog, so a text value with entities like `&amp;`
  is no longer changed on saving and `</textarea>` in it no longer breaks the
  dialog (OpenCCU patch 0097).
  ([#313](https://github.com/OpenCCU/OpenCCU-Base/pull/313), @jens-maus)
* fix the program list, the device, channel, room and function lists, the
  inbox, the service messages and the user administration failing to load,
  and deleting a program or choosing a user on the login page failing, when
  the name of a device, channel, room, function, program or user, or the
  description of a program, contains an apostrophe, a quote or a backslash,
  as scripts or other tools can set them. These names are now escaped
  wherever they are passed to JavaScript, and program names in the program
  list XML.
  ([#312](https://github.com/OpenCCU/OpenCCU-Base/pull/312); refs
  [OpenCCU/OpenCCU#4632](https://github.com/OpenCCU/OpenCCU/issues/4632),
  @jens-maus)
* show the value of the presence variable in "Status and control", in the
  favorites and in the system variables of a user on the start page, where
  it was empty. It is shown as the name of its value, like a logic value.
  ([#314](https://github.com/OpenCCU/OpenCCU-Base/pull/314), @jens-maus)
* fix the program chooser, the system protocol and the value input of
  number variables in the program editor failing, and texts that a program
  sets or compares for string variables being cut off in the program editor,
  when value names, value list entries, units or descriptions of system
  variables contain apostrophes, quotes or backslashes. These texts are now also
  HTML-escaped in the program editor, the program list, the system variable
  views of "Status and control", the alarm messages, the favorites and the
  system variables of a user, so markup like `<b>` is shown as text. Values
  of string variables are still shown as HTML. Texts with quotes or
  backslashes that a program sets or compares for string variables, and
  units with quotes, can now also be saved in the program editor.
  ([#306](https://github.com/OpenCCU/OpenCCU-Base/pull/306); refs
  [OpenCCU/OpenCCU#4632](https://github.com/OpenCCU/OpenCCU/issues/4632),
  @jens-maus)
* sort the entries of the main menu's submenus alphabetically and show an
  icon before each entry. The programs menu gets a "Test script" entry, and
  the overview pages of the main menu show their sections as flexible tiles
  (OpenCCU patch 0061). The menu icons are Icons8 Material icons, see
  `www/ise/img/menuicons/README.md`.
  ([#308](https://github.com/OpenCCU/OpenCCU-Base/pull/308), @jens-maus)
* remove the unused Expect-based multipart reader from `cgi.tcl`; file uploads
  that still reach `cgi.tcl` always use the binary-safe Tcl reader, as before
  (OpenCCU patch 0203).
  ([#307](https://github.com/OpenCCU/OpenCCU-Base/pull/307), @jens-maus)
* upload firmware, add-on, backup and certificate files through a small shell
  CGI that writes the file directly to `/usr/local/tmp` instead of the Tcl
  multipart parser of `cgi.tcl`, and update `cgi.tcl` to 1.10.0. Only the
  known upload targets and files written by this CGI are accepted, and a
  certificate upload needs a readable certificate with its matching private
  key (OpenCCU patch 0031).
  ([#301](https://github.com/OpenCCU/OpenCCU-Base/pull/301); refs
  [OpenCCU/OpenCCU#4633](https://github.com/OpenCCU/OpenCCU/issues/4633),
  [OpenCCU/OpenCCU#4636](https://github.com/OpenCCU/OpenCCU/issues/4636),
  @jens-maus)
* fix the system variable chooser of the program editor staying empty, and
  the system variable lists losing a variable or failing to load, when the
  name, description, value list, value names or unit of a system variable
  contain an apostrophe (allowed in value lists and value names since
  [OpenCCU/OpenCCU#4413](https://github.com/OpenCCU/OpenCCU/pull/4413)), a
  backslash or `</script>`. These texts are now escaped wherever they are
  passed to JavaScript.
  ([#304](https://github.com/OpenCCU/OpenCCU-Base/pull/304); refs
  [OpenCCU/OpenCCU#4632](https://github.com/OpenCCU/OpenCCU/issues/4632),
  @jens-maus)
* show the duty cycle and carrier sense of every gateway (CCU, LAN gateways,
  HmIP-HAP) as one row each on the start page. Rows of gateways that
  disappear are now hidden and disconnected gateways are shown as not
  available (OpenCCU patch 0057).
  ([#293](https://github.com/OpenCCU/OpenCCU-Base/pull/293); refs
  [OpenCCU/OpenCCU#4629](https://github.com/OpenCCU/OpenCCU/issues/4629),
  @jens-maus)
* group the device firmware update dialog of the start page by device type
  and current version, with device counts and expandable serial numbers.
  Versions are now compared and sorted numerically, and the table layout is
  fixed (OpenCCU patch 0183).
  ([#292](https://github.com/OpenCCU/OpenCCU-Base/pull/292); refs
  [OpenCCU/OpenCCU#4465](https://github.com/OpenCCU/OpenCCU/issues/4465),
  [OpenCCU/OpenCCU#4625](https://github.com/OpenCCU/OpenCCU/issues/4625),
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  @jens-maus)
* show available add-on updates on the start page. Add-on names are now
  escaped, each add-on check has a 5 second time limit, and the notice loads
  without delaying the start page (OpenCCU patch 0131).
  ([#290](https://github.com/OpenCCU/OpenCCU-Base/pull/290); refs
  [OpenCCU/OpenCCU#4619](https://github.com/OpenCCU/OpenCCU/issues/4619),
  [OpenCCU/OpenCCU#4621](https://github.com/OpenCCU/OpenCCU/issues/4621),
  @jens-maus)
* fix the switching thresholds of HmIP power meters failing to save for
  values like 1.1, and round decimal ties correctly (1.005 to 1.01).
  ([#286](https://github.com/OpenCCU/OpenCCU-Base/pull/286); refs
  [#276](https://github.com/OpenCCU/OpenCCU-Base/issues/276),
  [OpenCCU/OpenCCU#4615](https://github.com/OpenCCU/OpenCCU/issues/4615),
  @jens-maus)
* show the "Restore Config" button for BidCos-Wired devices too, now that
  hs485d supports restoring the stored configuration to a device (not yet
  tested on Wired hardware).
  ([#217](https://github.com/OpenCCU/OpenCCU-Base/pull/217); refs
  [#214](https://github.com/OpenCCU/OpenCCU-Base/issues/214),
  [#216](https://github.com/OpenCCU/OpenCCU-Base/issues/216), @jens-maus)
* validate and round parameter values in the device settings with the
  decimal places of their range instead of a fixed precision. Stored values
  no longer carry floating point residue, and the visible input of BidCos-RF
  parameters also shows the rounded or clamped value (OpenCCU patch 0119).
  ([#276](https://github.com/OpenCCU/OpenCCU-Base/pull/276); refs
  [OpenCCU/OpenCCU#4602](https://github.com/OpenCCU/OpenCCU/issues/4602),
  [OpenCCU/OpenCCU#4606](https://github.com/OpenCCU/OpenCCU/issues/4606),
  [OpenCCU/OpenCCU#4613](https://github.com/OpenCCU/OpenCCU/issues/4613),
  @jens-maus)
* add a "Service messages" checkbox to the device settings to switch off the
  service messages of a device. Saving it now succeeds for devices without
  service message datapoints (OpenCCU patch 0044).
  ([#275](https://github.com/OpenCCU/OpenCCU-Base/pull/275); refs
  [OpenCCU/OpenCCU#4600](https://github.com/OpenCCU/OpenCCU/issues/4600),
  @jens-maus)
* keep the list filters in the browser and restore them when a list is shown
  again; active filter captions are shown in red. Filters of slowly loading
  popups are now restored too, and filtering keeps working when the browser
  blocks storage (OpenCCU patch 0056).
  ([#273](https://github.com/OpenCCU/OpenCCU-Base/pull/273); refs
  [OpenCCU/OpenCCU#601](https://github.com/OpenCCU/OpenCCU/issues/601),
  [OpenCCU/OpenCCU#617](https://github.com/OpenCCU/OpenCCU/issues/617),
  [OpenCCU/OpenCCU#638](https://github.com/OpenCCU/OpenCCU/issues/638),
  [OpenCCU/OpenCCU#4590](https://github.com/OpenCCU/OpenCCU/issues/4590),
  [OpenCCU/OpenCCU#4592](https://github.com/OpenCCU/OpenCCU/issues/4592),
  [OpenCCU/OpenCCU#4595](https://github.com/OpenCCU/OpenCCU/issues/4595),
  @jens-maus)
* separate values and units consistently with a space (e.g. "21.5 °C", "100
  ms") and show ESI energy values with two decimals. Fix the free value
  input showing "°;C", a Tcl error in the PDA temperature control, and the
  ESI energy cost being calculated from the rounded value (OpenCCU patch
  0132). ([#262](https://github.com/OpenCCU/OpenCCU-Base/pull/262); refs
  [OpenCCU/OpenCCU#1670](https://github.com/OpenCCU/OpenCCU/issues/1670),
  [OpenCCU/OpenCCU#4584](https://github.com/OpenCCU/OpenCCU/issues/4584),
  [OpenCCU/OpenCCU#4585](https://github.com/OpenCCU/OpenCCU/issues/4585),
  @jens-maus)
* show humidity, battery voltage and valve state in the heating controls of
  the control pages ("n/a" for groups without a humidity sensor). A humidity
  or valve state of 0 % is now shown instead of hidden (OpenCCU patch 0042).
  ([#261](https://github.com/OpenCCU/OpenCCU-Base/pull/261); refs
  [OpenCCU/OpenCCU#2106](https://github.com/OpenCCU/OpenCCU/issues/2106),
  [OpenCCU/OpenCCU#2274](https://github.com/OpenCCU/OpenCCU/issues/2274),
  [OpenCCU/OpenCCU#2276](https://github.com/OpenCCU/OpenCCU/issues/2276),
  [OpenCCU/OpenCCU#4579](https://github.com/OpenCCU/OpenCCU/issues/4579),
  [OpenCCU/OpenCCU#4582](https://github.com/OpenCCU/OpenCCU/issues/4582),
  @jens-maus)
* remove the field test option ("Show unpublished device firmware"); device
  firmware is always fetched from the regular update server. The storage
  settings dialog no longer refers to the removed option (OpenCCU patch
  0205). ([#259](https://github.com/OpenCCU/OpenCCU-Base/pull/259); refs
  [OpenCCU/OpenCCU#3477](https://github.com/OpenCCU/OpenCCU/issues/3477),
  [OpenCCU/OpenCCU#4574](https://github.com/OpenCCU/OpenCCU/issues/4574),
  [OpenCCU/OpenCCU#4575](https://github.com/OpenCCU/OpenCCU/issues/4575),
  @jens-maus)
* place the channel highlights onto the buttons of the device images of
  HmIP-WRC6, HmIP-BSL and HmIPW-DRAP/-DRS4/-DRS8. All WRC6 variants now use
  the corrected positions, also in the older configuration pages (OpenCCU
  patch 0125). ([#256](https://github.com/OpenCCU/OpenCCU-Base/pull/256);
  refs
  [OpenCCU/OpenCCU#4565](https://github.com/OpenCCU/OpenCCU/issues/4565),
  [OpenCCU/OpenCCU#4570](https://github.com/OpenCCU/OpenCCU/issues/4570),
  [OpenCCU/OpenCCU#4571](https://github.com/OpenCCU/OpenCCU/issues/4571),
  @jens-maus)
* let the text filters of the WebUI lists match regular expressions. Fix
  plain names with "(", ")" or "+" no longer matching, and keep the filter
  text as entered when a filter is reopened (OpenCCU patch 0170).
  ([#253](https://github.com/OpenCCU/OpenCCU-Base/pull/253); refs
  [OpenCCU/OpenCCU#2035](https://github.com/OpenCCU/OpenCCU/issues/2035),
  [OpenCCU/OpenCCU#2530](https://github.com/OpenCCU/OpenCCU/issues/2530),
  [OpenCCU/OpenCCU#4560](https://github.com/OpenCCU/OpenCCU/issues/4560),
  [OpenCCU/OpenCCU#4561](https://github.com/OpenCCU/OpenCCU/issues/4561),
  @jens-maus)
* add "Rename channels" to the device configuration dialog to name all
  channels "<device name><separator><number>". The confirmation is now
  translated and invalid names are rejected before renaming (OpenCCU patch
  0099). ([#252](https://github.com/OpenCCU/OpenCCU-Base/pull/252); refs
  [OpenCCU/OpenCCU#1020](https://github.com/OpenCCU/OpenCCU/issues/1020),
  [OpenCCU/OpenCCU#4552](https://github.com/OpenCCU/OpenCCU/issues/4552),
  [OpenCCU/OpenCCU#4556](https://github.com/OpenCCU/OpenCCU/issues/4556),
  [OpenCCU/OpenCCU#4558](https://github.com/OpenCCU/OpenCCU/issues/4558),
  @jens-maus)
* fix the system protocol logging event-only boolean datapoints (e.g. the
  valve state flags of HmIP-eTRV-CL) by their label regardless of value; it
  now shows their yes/no value. Add the missing "Quick veto duration" label.
  ([#251](https://github.com/OpenCCU/OpenCCU-Base/pull/251); refs
  [OpenCCU/OpenCCU#4557](https://github.com/OpenCCU/OpenCCU/issues/4557),
  @jens-maus)
* add a Homematic IP log level to the logging settings and start, stop and
  restart the HMIPServer via monit. Fix a missing log level preselecting
  "Tracing" instead of "Errors" (OpenCCU patch 0164).
  ([#250](https://github.com/OpenCCU/OpenCCU-Base/pull/250); refs
  [OpenCCU/OpenCCU#1682](https://github.com/OpenCCU/OpenCCU/issues/1682),
  [OpenCCU/OpenCCU#4549](https://github.com/OpenCCU/OpenCCU/issues/4549),
  [OpenCCU/OpenCCU#4550](https://github.com/OpenCCU/OpenCCU/issues/4550),
  @jens-maus)
* add the Tailscale VPN switch and authentication button to the network
  settings. Saving changed network settings now says that they take effect
  after a restart, and the help mentions Tailscale's relays (OpenCCU patch
  0136). ([#249](https://github.com/OpenCCU/OpenCCU-Base/pull/249); refs
  [OpenCCU/OpenCCU#4543](https://github.com/OpenCCU/OpenCCU/issues/4543),
  [OpenCCU/OpenCCU#4544](https://github.com/OpenCCU/OpenCCU/issues/4544),
  [OpenCCU/OpenCCU#4545](https://github.com/OpenCCU/OpenCCU/issues/4545),
  @jens-maus)
* add up/down arrows to reorder the shared system variables of a user. Fix
  the arrows emptying the variable table when creating a new user; they are
  now shown only for existing users (OpenCCU patch 0095).
  ([#247](https://github.com/OpenCCU/OpenCCU-Base/pull/247); refs
  [OpenCCU/OpenCCU#1000](https://github.com/OpenCCU/OpenCCU/issues/1000),
  [OpenCCU/OpenCCU#4538](https://github.com/OpenCCU/OpenCCU/issues/4538),
  @jens-maus)
* allow assigning rooms and functions to channel 0 (maintenance channel) of
  devices and show maintenance channels in the device list. Internal
  channels are now reported as not writable, and the empty channel chooser
  hint is shown again (OpenCCU patch 0104).
  ([#246](https://github.com/OpenCCU/OpenCCU-Base/pull/246); refs
  [OpenCCU/OpenCCU#1040](https://github.com/OpenCCU/OpenCCU/issues/1040),
  [OpenCCU/OpenCCU#4533](https://github.com/OpenCCU/OpenCCU/issues/4533),
  @jens-maus)
* let administrators open the channel settings and the program editor
  directly from the status and control pages, returning there afterwards.
  Fix the editor losing its origin after its own reloads and settings links
  failing before the device list has loaded (OpenCCU patch 0138).
  ([#245](https://github.com/OpenCCU/OpenCCU-Base/pull/245); refs
  [OpenCCU/OpenCCU#1444](https://github.com/OpenCCU/OpenCCU/issues/1444),
  [OpenCCU/OpenCCU#4528](https://github.com/OpenCCU/OpenCCU/issues/4528),
  [OpenCCU/OpenCCU#4529](https://github.com/OpenCCU/OpenCCU/issues/4529),
  [OpenCCU/OpenCCU#4530](https://github.com/OpenCCU/OpenCCU/issues/4530),
  @jens-maus)
* add a "when"/"not when" selection to negate single program conditions. The
  program list now also shows "not in value range" for negated number
  variable conditions (OpenCCU patch 0077).
  ([#244](https://github.com/OpenCCU/OpenCCU-Base/pull/244); refs
  [OpenCCU/OpenCCU#884](https://github.com/OpenCCU/OpenCCU/issues/884),
  [OpenCCU/OpenCCU#4524](https://github.com/OpenCCU/OpenCCU/issues/4524),
  @jens-maus)
* show the free user space on the maintenance page and disable the update
  buttons if it is below the 2.8 GB an update needs. If the free space
  cannot be determined, show it as unknown instead of 0 MB and do not block
  the update (OpenCCU patch 0165).
  ([#242](https://github.com/OpenCCU/OpenCCU-Base/pull/242); refs
  [OpenCCU/OpenCCU#1737](https://github.com/OpenCCU/OpenCCU/issues/1737),
  [OpenCCU/OpenCCU#4513](https://github.com/OpenCCU/OpenCCU/issues/4513),
  [OpenCCU/OpenCCU#4514](https://github.com/OpenCCU/OpenCCU/issues/4514),
  @jens-maus)
* add "Activate"/"Deactivate" buttons to the channel configuration dialog to
  forward button events of a channel to the CCU without a program. Fix the
  missing translations, the wrong interface for HmIP key channels and the
  result message (OpenCCU patch 0171).
  ([#240](https://github.com/OpenCCU/OpenCCU-Base/pull/240); refs
  [OpenCCU/OpenCCU#2017](https://github.com/OpenCCU/OpenCCU/issues/2017),
  [OpenCCU/OpenCCU#1567](https://github.com/OpenCCU/OpenCCU/issues/1567),
  [OpenCCU/OpenCCU#4507](https://github.com/OpenCCU/OpenCCU/issues/4507),
  [OpenCCU/OpenCCU#4508](https://github.com/OpenCCU/OpenCCU/issues/4508),
  @jens-maus)
* fix the device, room and function control pages jumping to the first entry
  when a column header is clicked or a stored filter is reapplied; the
  selected entry now stays selected.
  ([#239](https://github.com/OpenCCU/OpenCCU-Base/pull/239); fixes
  [OpenCCU/OpenCCU#4505](https://github.com/OpenCCU/OpenCCU/issues/4505),
  @jens-maus)
* speed up the translation of WebUI pages by caching translations. Fix texts
  added or overridden by the device settings pages not being used, and
  translations with HTML entities being shown literally (OpenCCU patch
  0111). ([#237](https://github.com/OpenCCU/OpenCCU-Base/pull/237); refs
  [OpenCCU/OpenCCU#1136](https://github.com/OpenCCU/OpenCCU/issues/1136),
  [OpenCCU/OpenCCU#4503](https://github.com/OpenCCU/OpenCCU/issues/4503),
  @jens-maus)
* fix descending sorts of the device list and channel chooser swapping
  entries that compare as equal, and sort empty (null) values first instead
  of as the text "null".
  ([#236](https://github.com/OpenCCU/OpenCCU-Base/pull/236); refs
  [#234](https://github.com/OpenCCU/OpenCCU-Base/issues/234),
  [OpenCCU/OpenCCU#4500](https://github.com/OpenCCU/OpenCCU/issues/4500),
  @jens-maus)
* fix the simplified device view showing the wrong virtual channels (e.g. of
  HmIPW-DRS4 or HmIP-BSM) when channels are named. The channels of a device
  are listed by channel number again by default.
  ([#235](https://github.com/OpenCCU/OpenCCU-Base/pull/235); fixes
  [OpenCCU/OpenCCU#2939](https://github.com/OpenCCU/OpenCCU/issues/2939),
  [OpenCCU/OpenCCU#2944](https://github.com/OpenCCU/OpenCCU/issues/2944),
  @jens-maus)
* sort WebUI lists naturally ("Raum 2" before "Raum 10"), ignoring case and
  accents. Missing values now sort as empty, and descending sorts keep equal
  rows in order (OpenCCU patch 0145).
  ([#234](https://github.com/OpenCCU/OpenCCU-Base/pull/234); refs
  [OpenCCU/OpenCCU#1491](https://github.com/OpenCCU/OpenCCU/issues/1491),
  [OpenCCU/OpenCCU#4499](https://github.com/OpenCCU/OpenCCU/issues/4499),
  @jens-maus)
* fix the device list showing old gateway names after renaming, adding or
  removing a LAN gateway; the names are now refreshed after saving.
  ([#233](https://github.com/OpenCCU/OpenCCU-Base/pull/233); refs
  [#230](https://github.com/OpenCCU/OpenCCU-Base/issues/230),
  [OpenCCU/OpenCCU#4497](https://github.com/OpenCCU/OpenCCU/issues/4497),
  @jens-maus)
* fix gateway names with markup or quotes being inserted unescaped into the
  interface filter of the device list, which allowed script injection.
  ([#232](https://github.com/OpenCCU/OpenCCU-Base/pull/232); refs
  [#230](https://github.com/OpenCCU/OpenCCU-Base/issues/230),
  [OpenCCU/OpenCCU#4497](https://github.com/OpenCCU/OpenCCU/issues/4497),
  @jens-maus)
* fix the new run-at-startup option wrapping over several lines in the
  program editor; it is now labelled "autostart" in an "Options" column. Add
  the "autostart" checkbox to the program list as well.
  ([#231](https://github.com/OpenCCU/OpenCCU-Base/pull/231); fixes
  [OpenCCU/OpenCCU#4496](https://github.com/OpenCCU/OpenCCU/issues/4496),
  @jens-maus)
* show the assigned HM-LGW-O-TW-W-EU gateway of BidCos-RF devices in the
  device list and device settings, where the gateway and roaming can also be
  changed. Fix device and gateway names with special characters, a wrong
  initial roaming state, and the display changing even when the assignment
  was rejected (OpenCCU patch 0161).
  ([#230](https://github.com/OpenCCU/OpenCCU-Base/pull/230); refs
  [OpenCCU/OpenCCU#1620](https://github.com/OpenCCU/OpenCCU/issues/1620),
  [OpenCCU/OpenCCU#1671](https://github.com/OpenCCU/OpenCCU/issues/1671),
  [OpenCCU/OpenCCU#1672](https://github.com/OpenCCU/OpenCCU/issues/1672),
  [OpenCCU/OpenCCU#4493](https://github.com/OpenCCU/OpenCCU/issues/4493),
  [OpenCCU/OpenCCU#4494](https://github.com/OpenCCU/OpenCCU/issues/4494),
  [OpenCCU/OpenCCU#4495](https://github.com/OpenCCU/OpenCCU/issues/4495),
  @jens-maus)
* show the duty cycle and firmware of a HM-LGW-O-TW-W-EU only while it is
  connected, not next to a connection error.
  ([#229](https://github.com/OpenCCU/OpenCCU-Base/pull/229); refs
  [#228](https://github.com/OpenCCU/OpenCCU-Base/issues/228),
  [OpenCCU/OpenCCU#4492](https://github.com/OpenCCU/OpenCCU/issues/4492),
  @jens-maus)
* fix LAN gateways keeping an old connection state when the state request
  fails or reports another gateway; they are now shown as "not active".
  ([#228](https://github.com/OpenCCU/OpenCCU-Base/pull/228); refs
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  @jens-maus)
* add an "Advanced Settings" dialog to the control panel for OpenCCU options
  and the system name, which is shown in the page title and breadcrumb. Fix
  system names with special characters, report failed service restarts and
  validate the backup count before saving (OpenCCU patch 0135).
  ([#227](https://github.com/OpenCCU/OpenCCU-Base/pull/227); refs
  [OpenCCU/OpenCCU#1415](https://github.com/OpenCCU/OpenCCU/issues/1415),
  [OpenCCU/OpenCCU#4484](https://github.com/OpenCCU/OpenCCU/issues/4484),
  [OpenCCU/OpenCCU#4487](https://github.com/OpenCCU/OpenCCU/issues/4487),
  [OpenCCU/OpenCCU#4488](https://github.com/OpenCCU/OpenCCU/issues/4488),
  @jens-maus)
* fix the LAN gateway table showing "undefined%" or "undefined" when the
  duty cycle or firmware is missing; the cells stay empty instead.
  ([#226](https://github.com/OpenCCU/OpenCCU-Base/pull/226); refs
  [#225](https://github.com/OpenCCU/OpenCCU-Base/issues/225),
  [OpenCCU/OpenCCU#4483](https://github.com/OpenCCU/OpenCCU/issues/4483),
  @jens-maus)
* show the duty cycle and firmware version of the LAN gateways in the LAN
  gateway configuration. Column headers are now translated, and a gateway
  missing from an update no longer keeps stale values (OpenCCU patch 0032).
  ([#225](https://github.com/OpenCCU/OpenCCU-Base/pull/225); refs
  [OpenCCU/OpenCCU#301](https://github.com/OpenCCU/OpenCCU/issues/301),
  [OpenCCU/OpenCCU#4481](https://github.com/OpenCCU/OpenCCU/issues/4481),
  [OpenCCU/OpenCCU#4482](https://github.com/OpenCCU/OpenCCU/issues/4482),
  @jens-maus)
* add the program option "Bei Zentralenstart ausführen" (run at startup) to
  the program editor; new programs no longer run at startup unless it is
  checked. ([#223](https://github.com/OpenCCU/OpenCCU-Base/pull/223); refs
  [OpenCCU/OpenCCU#223](https://github.com/OpenCCU/OpenCCU/issues/223),
  @jens-maus)
* sort favorites manually with up/down buttons in the favorites overview,
  per user list. Fix editing a favorite losing its position in other users'
  lists and favorite names with special characters breaking the page
  (OpenCCU patch 0062).
  ([#220](https://github.com/OpenCCU/OpenCCU-Base/pull/220); refs
  [OpenCCU/OpenCCU#4461](https://github.com/OpenCCU/OpenCCU/issues/4461),
  [OpenCCU/OpenCCU#4473](https://github.com/OpenCCU/OpenCCU/issues/4473),
  @jens-maus)
* improve the system variable selection dialog: select a variable by
  clicking its row, highlight internal variables and show long names in
  full, and add a "String" type filter. Rows can now also be selected with
  the keyboard (Enter/Space), and the missing filter row is fixed (OpenCCU
  patch 0029). ([#218](https://github.com/OpenCCU/OpenCCU-Base/pull/218);
  refs
  [OpenCCU/OpenCCU#4467](https://github.com/OpenCCU/OpenCCU/issues/4467),
  [OpenCCU/OpenCCU#4468](https://github.com/OpenCCU/OpenCCU/issues/4468),
  @jens-maus)
* show the "Restore Config" button only for BidCos-RF devices; on other
  interfaces it always ended with an error.
  ([#216](https://github.com/OpenCCU/OpenCCU-Base/pull/216); refs
  [#214](https://github.com/OpenCCU/OpenCCU-Base/issues/214),
  [OpenCCU/OpenCCU#4466](https://github.com/OpenCCU/OpenCCU/issues/4466),
  @jens-maus)
* fix the alarm messages page leaving the message column empty when the
  alarm was triggered by a system variable or a time module; it now shows
  the trigger and the alarm's message text.
  ([#215](https://github.com/OpenCCU/OpenCCU-Base/pull/215); refs
  [OpenCCU/OpenCCU#291](https://github.com/OpenCCU/OpenCCU/issues/291),
  @jens-maus)
* add a "Restore Config" button to the device settings page to send the
  stored configuration back to a device. It is now shown only to
  administrators, and a failed restore shows an error message (OpenCCU patch
  0151). ([#214](https://github.com/OpenCCU/OpenCCU-Base/pull/214); refs
  [OpenCCU/OpenCCU#4462](https://github.com/OpenCCU/OpenCCU/issues/4462),
  @jens-maus)
* name the object in the delete confirmation dialogs of system variables,
  users, favorites and direct links. Names with special characters are now
  shown correctly and the German word order is fixed (OpenCCU patch 0156).
  ([#213](https://github.com/OpenCCU/OpenCCU-Base/pull/213); refs
  [OpenCCU/OpenCCU#1579](https://github.com/OpenCCU/OpenCCU/issues/1579),
  [OpenCCU/OpenCCU#4458](https://github.com/OpenCCU/OpenCCU/issues/4458),
  @jens-maus)
* support the HmIP-RFUSB stick as central radio module, named "Homematic IP
  Central Control Unit" like the HmIP-CCU3 and RPI-RF-MOD. The device
  settings dialogs now also know the HmIP-RFUSB and show its image (OpenCCU
  patch 0155). ([#210](https://github.com/OpenCCU/OpenCCU-Base/pull/210),
  @jens-maus)
* fix a control panel popup that fails to load staying empty and blocking
  the WebUI; it is now closed and an error is shown. Network errors (HTTP
  status 0) are now handled as well (OpenCCU patch 0089).
  ([#205](https://github.com/OpenCCU/OpenCCU-Base/pull/205); refs
  [OpenCCU/OpenCCU#4453](https://github.com/OpenCCU/OpenCCU/issues/4453),
  @jens-maus)
* use the common thermostat settings page for HmIP-eTRV-B, -B1, -C, -C-2 and
  -CL, with window-open temperature, valve, decalcification and boost
  parameters. The valve percentage options now always select the stored
  value (OpenCCU patch 0193).
  ([#200](https://github.com/OpenCCU/OpenCCU-Base/pull/200); refs
  [OpenCCU/OpenCCU#2919](https://github.com/OpenCCU/OpenCCU/issues/2919),
  [OpenCCU/OpenCCU#2893](https://github.com/OpenCCU/OpenCCU/issues/2893),
  [OpenCCU/OpenCCU#4444](https://github.com/OpenCCU/OpenCCU/issues/4444),
  @jens-maus)
* show the maintenance datapoints of a device, such as battery, signal
  strength, error flags and operating time, in the tile of its maintenance
  channel. Wrong labels were fixed: an active sabotage error is no longer
  shown as "Sabotage OK" (and vice versa), heating and cooling are no longer
  swapped, and the particulate matter sensor error gets a readable label
  (OpenCCU patch 0144).
  ([#199](https://github.com/OpenCCU/OpenCCU-Base/pull/199); refs
  [OpenCCU/OpenCCU#1487](https://github.com/OpenCCU/OpenCCU/issues/1487),
  [OpenCCU/OpenCCU#1538](https://github.com/OpenCCU/OpenCCU/issues/1538),
  [OpenCCU/OpenCCU#2008](https://github.com/OpenCCU/OpenCCU/issues/2008),
  [OpenCCU/OpenCCU#4440](https://github.com/OpenCCU/OpenCCU/issues/4440),
  [OpenCCU/OpenCCU#4441](https://github.com/OpenCCU/OpenCCU/issues/4441),
  [OpenCCU/OpenCCU#4442](https://github.com/OpenCCU/OpenCCU/issues/4442),
  @jens-maus)
* add a "Restart in recovery mode" button to the maintenance page. The
  restart was changed to report an error when saving the settings or
  starting the restart fails, without leaving the recovery flag behind, so a
  later normal restart no longer boots into the recovery system unexpectedly
  (OpenCCU patch 0012).
  ([#194](https://github.com/OpenCCU/OpenCCU-Base/pull/194); refs
  [OpenCCU/OpenCCU#4433](https://github.com/OpenCCU/OpenCCU/issues/4433),
  [OpenCCU/OpenCCU#4434](https://github.com/OpenCCU/OpenCCU/issues/4434),
  [OpenCCU/OpenCCU#4435](https://github.com/OpenCCU/OpenCCU/issues/4435),
  [OpenCCU/OpenCCU#4436](https://github.com/OpenCCU/OpenCCU/issues/4436),
  [OpenCCU/OpenCCU#4379](https://github.com/OpenCCU/OpenCCU/issues/4379),
  [#134](https://github.com/OpenCCU/OpenCCU-Base/issues/134), @jens-maus)
* remove the ineffective ReGa version selection ("Version Logikschicht")
  from the maintenance page. The unused API method `User.setReGaVersion`,
  which let any user write `/etc/config/ReGaHssVersion`, is removed as well
  (OpenCCU patch 0008).
  ([#193](https://github.com/OpenCCU/OpenCCU-Base/pull/193); refs
  [OpenCCU/OpenCCU#4431](https://github.com/OpenCCU/OpenCCU/issues/4431),
  @jens-maus)
* add an offset and an optional duration in minutes to the daytime and
  nighttime astro options of the time module in programs. The program text
  of normal time ranges was fixed to show the end time ("until HH:MM") again
  (OpenCCU patch 0073).
  ([#188](https://github.com/OpenCCU/OpenCCU-Base/pull/188); refs
  [OpenCCU/OpenCCU#4426](https://github.com/OpenCCU/OpenCCU/issues/4426),
  [OpenCCU/OpenCCU#4427](https://github.com/OpenCCU/OpenCCU/issues/4427),
  @jens-maus)
* keep the right column of the status and control pages by device, room and
  function in sync with the name filter of the left column. The case where
  the filter hides all entries was fixed: the right column is now emptied
  instead of still showing the channels of an entry that is no longer listed
  (OpenCCU patch 0196).
  ([#186](https://github.com/OpenCCU/OpenCCU-Base/pull/186); refs
  [OpenCCU/OpenCCU#2973](https://github.com/OpenCCU/OpenCCU/issues/2973),
  [OpenCCU/OpenCCU#2974](https://github.com/OpenCCU/OpenCCU/issues/2974),
  [OpenCCU/OpenCCU#4424](https://github.com/OpenCCU/OpenCCU/issues/4424),
  [OpenCCU/OpenCCU#4425](https://github.com/OpenCCU/OpenCCU/issues/4425),
  @jens-maus)
* add favicons, an Apple touch icon and a web app manifest, so browsers and
  mobile devices show a high-resolution icon and the app name OpenCCU. The
  manifest icons were made usable as normal app icons too, not only as
  maskable icons (OpenCCU patch 0036).
  ([#158](https://github.com/OpenCCU/OpenCCU-Base/pull/158); refs
  [OpenCCU/OpenCCU#1559](https://github.com/OpenCCU/OpenCCU/issues/1559),
  [OpenCCU/OpenCCU#4409](https://github.com/OpenCCU/OpenCCU/issues/4409),
  [OpenCCU/OpenCCU#4410](https://github.com/OpenCCU/OpenCCU/issues/4410),
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  @jens-maus)
* fix the type label of the HmIP-PS-A, which was shown as "HMIP-PS-A" (from
  eQ-3). (@cn-eq3)
* show HTML system variable values in the user favorites on the start page
  as HTML also after they are updated, and stop quotes in values from
  breaking the update. The escaping was corrected so that backslashes in
  values are no longer lost and no longer break the update (OpenCCU patch
  0075). ([#136](https://github.com/OpenCCU/OpenCCU-Base/pull/136); refs
  [OpenCCU/OpenCCU#856](https://github.com/OpenCCU/OpenCCU/issues/856),
  [OpenCCU/OpenCCU#4383](https://github.com/OpenCCU/OpenCCU/issues/4383),
  [OpenCCU/OpenCCU#4385](https://github.com/OpenCCU/OpenCCU/issues/4385),
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  @jens-maus)
* add a button to the system variable list to show or hide internal system
  variables, show them with a grey background, and add a checkbox to mark a
  variable as internal. The page was corrected to reload only after the
  change is saved, so it no longer shows the old state (OpenCCU patch 0134).
  ([#132](https://github.com/OpenCCU/OpenCCU-Base/pull/132); refs
  [OpenCCU/OpenCCU#1400](https://github.com/OpenCCU/OpenCCU/issues/1400),
  [OpenCCU/OpenCCU#4375](https://github.com/OpenCCU/OpenCCU/issues/4375),
  [OpenCCU/OpenCCU#4376](https://github.com/OpenCCU/OpenCCU/issues/4376),
  [OpenCCU/OpenCCU#4377](https://github.com/OpenCCU/OpenCCU/issues/4377),
  @jens-maus)
* fix system variables with a line break in their description or value list,
  which broke the system variable selection and the system variable list.
  The system variable list was corrected to show these line breaks as line
  breaks instead of visible "<br/>" text (OpenCCU patch 0035).
  ([#130](https://github.com/OpenCCU/OpenCCU-Base/pull/130); refs
  [OpenCCU/OpenCCU#4368](https://github.com/OpenCCU/OpenCCU/issues/4368),
  [OpenCCU/OpenCCU#4370](https://github.com/OpenCCU/OpenCCU/issues/4370),
  @jens-maus)
* show the LED colour of the HM-OU-LED16 keys next to the keys on the
  control page. The layout of other devices with short-press-only keys
  (HM-Sen-DB) was fixed, which had lost a table cell (OpenCCU patch 0091).
  ([#129](https://github.com/OpenCCU/OpenCCU-Base/pull/129); refs
  [OpenCCU/OpenCCU#985](https://github.com/OpenCCU/OpenCCU/issues/985),
  [OpenCCU/OpenCCU#4366](https://github.com/OpenCCU/OpenCCU/issues/4366),
  [OpenCCU/OpenCCU#4367](https://github.com/OpenCCU/OpenCCU/issues/4367),
  @jens-maus)
* store install wizard and firewall hint acknowledgements per user ID, so
  accounts with similar names no longer share them and a session can only
  acknowledge for its own account. Existing acknowledgements are taken over.
  ([#128](https://github.com/OpenCCU/OpenCCU-Base/pull/128); refs
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  @jens-maus)
* show the role and UI mode of the logged-on user in the header; users and
  administrators can click their name to open their account settings. This
  was fixed so that the install wizard or firewall hint no longer reappears
  whenever the user's role, UI mode or language changes (OpenCCU patch
  0141). ([#127](https://github.com/OpenCCU/OpenCCU-Base/pull/127); refs
  [OpenCCU/OpenCCU#1463](https://github.com/OpenCCU/OpenCCU/issues/1463),
  [OpenCCU/OpenCCU#4360](https://github.com/OpenCCU/OpenCCU/issues/4360),
  [OpenCCU/OpenCCU#4361](https://github.com/OpenCCU/OpenCCU/issues/4361),
  [OpenCCU/OpenCCU#4364](https://github.com/OpenCCU/OpenCCU/issues/4364),
  [#126](https://github.com/OpenCCU/OpenCCU-Base/issues/126), @jens-maus)
* stop the install wizard or the firewall hint from appearing on every login
  for user names with several blanks, slashes or umlauts. The WebUI was
  corrected to also remove all other special characters, so it always finds
  the confirmation the CCU saved for the user (OpenCCU patch 0055).
  ([#126](https://github.com/OpenCCU/OpenCCU-Base/pull/126); refs
  [OpenCCU/OpenCCU#600](https://github.com/OpenCCU/OpenCCU/issues/600),
  [OpenCCU/OpenCCU#4362](https://github.com/OpenCCU/OpenCCU/issues/4362),
  [OpenCCU/OpenCCU#4363](https://github.com/OpenCCU/OpenCCU/issues/4363),
  @jens-maus)
* add a "Save changes" button to the header that saves the configuration
  permanently. A script error was fixed that guest users got on every page
  update and when clicking the header logo (OpenCCU patch 0090).
  ([#125](https://github.com/OpenCCU/OpenCCU-Base/pull/125); refs
  [OpenCCU/OpenCCU#979](https://github.com/OpenCCU/OpenCCU/issues/979),
  [OpenCCU/OpenCCU#4354](https://github.com/OpenCCU/OpenCCU/issues/4354),
  [OpenCCU/OpenCCU#4357](https://github.com/OpenCCU/OpenCCU/issues/4357),
  @jens-maus)
* allow device firmware updates on systems with HmIP-RF only, by checking
  the duty cycle of the HmIP-RF interface when there is no BidCos-RF
  interface. The check was corrected to also use HmIP-RF when BidCos-RF
  reports no interface, instead of starting the update without a duty cycle
  check (OpenCCU patch 0068).
  ([#124](https://github.com/OpenCCU/OpenCCU-Base/pull/124); refs
  [OpenCCU/OpenCCU#770](https://github.com/OpenCCU/OpenCCU/issues/770),
  [OpenCCU/OpenCCU#4349](https://github.com/OpenCCU/OpenCCU/issues/4349),
  [OpenCCU/OpenCCU#4351](https://github.com/OpenCCU/OpenCCU/issues/4351),
  @jens-maus)
* report failed add-on operations such as an uninstall correctly instead of
  "Failure Success", point to an error log, and refresh the add-on list
  afterwards. A failed uninstall was changed to keep the add-on so it can be
  retried, error output of a successful script no longer counts as a
  failure, and a failure to write the error log is reported (OpenCCU patch
  0147). ([#123](https://github.com/OpenCCU/OpenCCU-Base/pull/123); refs
  [OpenCCU/OpenCCU#1500](https://github.com/OpenCCU/OpenCCU/issues/1500),
  [OpenCCU/OpenCCU#4347](https://github.com/OpenCCU/OpenCCU/issues/4347),
  [OpenCCU/OpenCCU#4348](https://github.com/OpenCCU/OpenCCU/issues/4348),
  [OpenCCU/OpenCCU#4350](https://github.com/OpenCCU/OpenCCU/issues/4350),
  @jens-maus)
* show the extended channel descriptions of HmIP channels again, for example
  in the device settings. A script error was fixed for HmIP-WGT/WGTC
  channels that are not loaded yet; they now get the generic description
  instead (OpenCCU patch 0190).
  ([#121](https://github.com/OpenCCU/OpenCCU-Base/pull/121); refs
  [OpenCCU/OpenCCU#2898](https://github.com/OpenCCU/OpenCCU/issues/2898),
  [OpenCCU/OpenCCU#2902](https://github.com/OpenCCU/OpenCCU/issues/2902),
  [OpenCCU/OpenCCU#4343](https://github.com/OpenCCU/OpenCCU/issues/4343),
  [OpenCCU/OpenCCU#4344](https://github.com/OpenCCU/OpenCCU/issues/4344),
  @jens-maus)
* keep the arrow, page and space keys scrolling the page after a page
  change, without clicking into the page first. The content area was taken
  out of the tab order, so the Tab key again moves through the text fields
  of the e-paper status display dialog in the right order (OpenCCU patch
  0114). ([#118](https://github.com/OpenCCU/OpenCCU-Base/pull/118); refs
  [OpenCCU/OpenCCU#1144](https://github.com/OpenCCU/OpenCCU/issues/1144),
  [OpenCCU/OpenCCU#4337](https://github.com/OpenCCU/OpenCCU/issues/4337),
  [OpenCCU/OpenCCU#4338](https://github.com/OpenCCU/OpenCCU/issues/4338),
  @jens-maus)
* allow renaming and deleting the built-in Admin user in the user
  management. The built-in Admin was made to always stay an administrator,
  also after a rename, so it can no longer be downgraded by mistake and lock
  everyone out of the user management. Only administrators can now change
  access levels (OpenCCU patch 0093).
  ([#114](https://github.com/OpenCCU/OpenCCU-Base/pull/114); refs
  [OpenCCU/OpenCCU#987](https://github.com/OpenCCU/OpenCCU/issues/987),
  [OpenCCU/OpenCCU#988](https://github.com/OpenCCU/OpenCCU/issues/988),
  [OpenCCU/OpenCCU#4331](https://github.com/OpenCCU/OpenCCU/issues/4331),
  [OpenCCU/OpenCCU#4332](https://github.com/OpenCCU/OpenCCU/issues/4332),
  [OpenCCU/OpenCCU#4333](https://github.com/OpenCCU/OpenCCU/issues/4333),
  @jens-maus)
* include all log files in /var/log in the log file download of the
  maintenance page, not only the system and HMServer logs. Hidden log files
  were added and the files are now sorted, and an empty /var/log gives an
  empty download instead of an error (OpenCCU patch 0175).
  ([#108](https://github.com/OpenCCU/OpenCCU-Base/pull/108); refs
  [OpenCCU/OpenCCU#2186](https://github.com/OpenCCU/OpenCCU/issues/2186),
  [OpenCCU/OpenCCU#4323](https://github.com/OpenCCU/OpenCCU/issues/4323),
  [OpenCCU/OpenCCU#4324](https://github.com/OpenCCU/OpenCCU/issues/4324),
  @jens-maus)
* sort the add-ons in the additional software dialog by their name, ignoring
  case. The sorting was corrected so that add-ons with the same name are all
  listed, and an add-on whose information cannot be read no longer breaks
  the whole page (OpenCCU patch 0096).
  ([#105](https://github.com/OpenCCU/OpenCCU-Base/pull/105); refs
  [OpenCCU/OpenCCU#1003](https://github.com/OpenCCU/OpenCCU/issues/1003),
  [OpenCCU/OpenCCU#4319](https://github.com/OpenCCU/OpenCCU/issues/4319),
  [OpenCCU/OpenCCU#4320](https://github.com/OpenCCU/OpenCCU/issues/4320),
  @jens-maus)
* show a progress bar and disable the logout button while logging out, and
  save the configuration in the background so logging out is faster. The
  order of the steps was corrected so that the configuration is actually
  saved on logout; before, the save was rejected because the session had
  already ended (OpenCCU patch 0168).
  ([#93](https://github.com/OpenCCU/OpenCCU-Base/pull/93); refs
  [OpenCCU/OpenCCU#1858](https://github.com/OpenCCU/OpenCCU/issues/1858),
  [OpenCCU/OpenCCU#1859](https://github.com/OpenCCU/OpenCCU/issues/1859),
  [OpenCCU/OpenCCU#4297](https://github.com/OpenCCU/OpenCCU/issues/4297),
  [OpenCCU/OpenCCU#4298](https://github.com/OpenCCU/OpenCCU/issues/4298),
  @jens-maus)
* fix the script error logged when the general channel settings are closed
  with "OK", by passing the device ID when a device is set visible. The fix
  was corrected so that the "visible" checkbox of new devices in the inbox
  works again instead of failing with a script error (OpenCCU patch 0082).
  ([#85](https://github.com/OpenCCU/OpenCCU-Base/pull/85); refs
  [OpenCCU/OpenCCU#919](https://github.com/OpenCCU/OpenCCU/issues/919),
  [OpenCCU/OpenCCU#936](https://github.com/OpenCCU/OpenCCU/issues/936),
  [OpenCCU/OpenCCU#4283](https://github.com/OpenCCU/OpenCCU/issues/4283),
  [OpenCCU/OpenCCU#4284](https://github.com/OpenCCU/OpenCCU/issues/4284),
  @jens-maus)
* add German and English descriptions for 51 channel types that had none.
  Two German typos of the previous patch are corrected ("Helligkeitssensor",
  "Türschlossantrieb") (OpenCCU patch 0194).
  ([#68](https://github.com/OpenCCU/OpenCCU-Base/pull/68); refs
  [OpenCCU/OpenCCU#2922](https://github.com/OpenCCU/OpenCCU/issues/2922),
  @jens-maus)
* fix the comfort/eco temperature check of HM-CC-RT-DN, HM-TC-IT-WM-W-EU and
  HM-CC-VG-1 ignoring decimals and, since the previous patch, not working at
  all and writing NaN into the dialog. The check now compares the full
  decimal values and rejects an eco temperature above the comfort
  temperature again (OpenCCU patch 0117).
  ([#66](https://github.com/OpenCCU/OpenCCU-Base/pull/66), @jens-maus)
* fix the system protocol breaking when a string system variable value
  contains a single quote. Backslashes in such values are now also escaped
  (OpenCCU patch 0158).
  ([#58](https://github.com/OpenCCU/OpenCCU-Base/pull/58), @jens-maus)
* show the currently assigned IP address, netmask, gateway and DNS servers
  in the network settings when DHCP is used. Saving the settings in DHCP
  mode no longer overwrites the stored manual network settings with these
  values (OpenCCU patch 0180).
  ([#54](https://github.com/OpenCCU/OpenCCU-Base/pull/54), @jens-maus)
* fix ReGa script injection through crafted favorite IDs in the PDA (mobile)
  favorites pages and repair their broken "execute program" command. In
  addition, a program favorite can now only be executed if it is a usable
  program (OpenCCU patch 0207).
  ([#46](https://github.com/OpenCCU/OpenCCU-Base/pull/46), @jens-maus)

### Migrated from OpenCCU patches

These changes were OpenCCU rootfs patches of the WebUI before and are part
of the WebUI sources now, without changes. For OpenCCU users nothing
changes; other users of OpenCCU-Base get them for the first time.

* patch 0002: add the Bootstrap 5 framework to the WebUI pages as a base for
  upcoming WebUI changes.
  ([#155](https://github.com/OpenCCU/OpenCCU-Base/pull/155), @jens-maus)
* patch 0004: restart ReGaHss, sshd, lighttpd and syslogd from the WebUI
  through monit. ([#65](https://github.com/OpenCCU/OpenCCU-Base/pull/65),
  @jens-maus)
* patch 0007: run firmware updates through the OpenCCU recovery system
  (accepting .tar.gz, .zip and SD card/partition images) and create and
  restore backups with the OpenCCU backup tools.
  ([#202](https://github.com/OpenCCU/OpenCCU-Base/pull/202); refs
  [OpenCCU/OpenCCU#1336](https://github.com/OpenCCU/OpenCCU/issues/1336),
  @jens-maus)
* patch 0010: remove the obsolete hint in the time module dialog that
  discouraged intervals shorter than 15 minutes.
  ([#61](https://github.com/OpenCCU/OpenCCU-Base/pull/61), @jens-maus)
* patch 0013: show the security key change counter on the security settings
  page. ([#64](https://github.com/OpenCCU/OpenCCU-Base/pull/64), @jens-maus)
* patch 0014: fix the control panel layout when the security wizard or
  coupling entries are not shown.
  ([#62](https://github.com/OpenCCU/OpenCCU-Base/pull/62), @jens-maus)
* patch 0019: add a "not equal" value condition to programs.
  ([#181](https://github.com/OpenCCU/OpenCCU-Base/pull/181), @jens-maus)
* patch 0021: remove the HomeMatic logo from the login and logout pages.
  ([#77](https://github.com/OpenCCU/OpenCCU-Base/pull/77), @jens-maus)
* patch 0026: rely on the HTTP cache headers instead of version parameters
  and no-cache tags, and no longer ask to clear the browser cache after a
  firmware update.
  ([#157](https://github.com/OpenCCU/OpenCCU-Base/pull/157); refs
  [OpenCCU/OpenCCU#245](https://github.com/OpenCCU/OpenCCU/issues/245),
  @jens-maus)
* patch 0027: do not show an empty system variable row for CUxD channels in
  the status and control view.
  ([#72](https://github.com/OpenCCU/OpenCCU-Base/pull/72), @jens-maus)
* patch 0033: fix missing direct link/program hints in the device parameters
  for channels without a ReGa name.
  ([#73](https://github.com/OpenCCU/OpenCCU-Base/pull/73), @jens-maus)
* patch 0037: improve the devconfig tool: separate address/type columns,
  more delete options, firmware update only when a newer one exists, RSSI
  values for Homematic IP devices and a more detailed status view.
  ([#69](https://github.com/OpenCCU/OpenCCU-Base/pull/69), @jens-maus)
* patch 0038: show a status column in the device list with pending
  config/updates, low battery, unreachable and other alarms, operating
  voltage, color-coded RSSI and duty cycle.
  ([#270](https://github.com/OpenCCU/OpenCCU-Base/pull/270), @jens-maus)
* patch 0040: install add-ons without a forced reboot; the CCU only restarts
  when the add-on requests it, and installation errors are reported.
  ([#160](https://github.com/OpenCCU/OpenCCU-Base/pull/160), @jens-maus)
* patch 0041: add CUxD and CCU-Jack to the interface filter of the device
  list. ([#88](https://github.com/OpenCCU/OpenCCU-Base/pull/88); refs
  [OpenCCU/OpenCCU#2445](https://github.com/OpenCCU/OpenCCU/issues/2445),
  @jens-maus)
* patch 0043: add a button to sort a user's system variable favorites
  alphabetically in the user account dialog.
  ([#144](https://github.com/OpenCCU/OpenCCU-Base/pull/144), @jens-maus)
* patch 0045: show system variables that are assigned to a channel in the
  system variable lists and selection popups, so they can be used like
  global ones. ([#106](https://github.com/OpenCCU/OpenCCU-Base/pull/106),
  @jens-maus)
* patch 0059: fix a deactivated program being reactivated when it is edited
  and saved. ([#75](https://github.com/OpenCCU/OpenCCU-Base/pull/75),
  @jens-maus)
* patch 0060: end the session and save the ReGa object model on logout.
  ([#92](https://github.com/OpenCCU/OpenCCU-Base/pull/92), @jens-maus)
* patch 0063: open add-on update downloads in a new tab instead of leaving
  the WebUI. ([#78](https://github.com/OpenCCU/OpenCCU-Base/pull/78),
  @jens-maus)
* patch 0064: update the URL parser (purl) to 2.3.1 to fix URLs with @ in
  parameter values being parsed incorrectly.
  ([#70](https://github.com/OpenCCU/OpenCCU-Base/pull/70); refs
  [OpenCCU/OpenCCU#742](https://github.com/OpenCCU/OpenCCU/issues/742),
  @jens-maus)
* patch 0065: avoid needless link and program usage counts that could stall
  ReGaHss on the rooms and functions lists of large systems.
  ([#131](https://github.com/OpenCCU/OpenCCU-Base/pull/131); refs
  [OpenCCU/OpenCCU#751](https://github.com/OpenCCU/OpenCCU/issues/751),
  @jens-maus)
* patch 0066: always show expert channels in the program channel chooser
  dialogs, fixing missing or wrong channels after renaming.
  ([#113](https://github.com/OpenCCU/OpenCCU-Base/pull/113); refs
  [OpenCCU/OpenCCU#810](https://github.com/OpenCCU/OpenCCU/issues/810),
  @jens-maus)
* patch 0067: fix the direct link list showing a Tcl error instead of the
  list for broken links, and fix its "Programs and connections" path link.
  ([#107](https://github.com/OpenCCU/OpenCCU-Base/pull/107), @jens-maus)
* patch 0070: show inactive programs grayed out in the program list, updated
  right away when the "active" checkbox changes.
  ([#153](https://github.com/OpenCCU/OpenCCU-Base/pull/153); refs
  [OpenCCU/OpenCCU#779](https://github.com/OpenCCU/OpenCCU/issues/779),
  @jens-maus)
* patch 0071: show the program name in the confirmation before deleting a
  program. ([#134](https://github.com/OpenCCU/OpenCCU-Base/pull/134); refs
  [OpenCCU/OpenCCU#785](https://github.com/OpenCCU/OpenCCU/issues/785),
  @jens-maus)
* patch 0074: fix a JavaScript error that could freeze the WebUI when a new
  room or function name was confirmed with Return.
  ([#95](https://github.com/OpenCCU/OpenCCU-Base/pull/95); refs
  [OpenCCU/OpenCCU#855](https://github.com/OpenCCU/OpenCCU/issues/855),
  @jens-maus)
* patch 0076: allow ^ characters in scripts of the script editor; such
  scripts failed the syntax check and could not be saved.
  ([#91](https://github.com/OpenCCU/OpenCCU-Base/pull/91); refs
  [OpenCCU/OpenCCU#225](https://github.com/OpenCCU/OpenCCU/issues/225),
  @jens-maus)
* patch 0083: fix ReGaHss logging a script error every 3 seconds while a
  function test runs for a whole device.
  ([#83](https://github.com/OpenCCU/OpenCCU-Base/pull/83); refs
  [OpenCCU/OpenCCU#939](https://github.com/OpenCCU/OpenCCU/issues/939),
  @jens-maus)
* patch 0084: fix the room list and room channel list failing to display
  when a room contains a channel without a valid device.
  ([#104](https://github.com/OpenCCU/OpenCCU-Base/pull/104); refs
  [OpenCCU/OpenCCU#944](https://github.com/OpenCCU/OpenCCU/issues/944),
  @jens-maus)
* patch 0085: make the content of the control panel dialogs (maintenance,
  network, security, additional software) scrollable so their buttons stay
  reachable. ([#111](https://github.com/OpenCCU/OpenCCU-Base/pull/111); refs
  [OpenCCU/OpenCCU#592](https://github.com/OpenCCU/OpenCCU/issues/592),
  @jens-maus)
* patch 0086: fix "500 Internal Server Error" responses of the reboot,
  shutdown and update actions.
  ([#109](https://github.com/OpenCCU/OpenCCU-Base/pull/109); refs
  [OpenCCU/OpenCCU#959](https://github.com/OpenCCU/OpenCCU/issues/959),
  @jens-maus)
* patch 0087: fix message box content requests with a doubled session id
  that could fail with HTTP error 500.
  ([#122](https://github.com/OpenCCU/OpenCCU-Base/pull/122); refs
  [OpenCCU/OpenCCU#959](https://github.com/OpenCCU/OpenCCU/issues/959),
  @jens-maus)
* patch 0092: fix the "Konfigurationsdaten werden geladen" message staying
  on screen forever at the end of a device replacement.
  ([#117](https://github.com/OpenCCU/OpenCCU-Base/pull/117); refs
  [OpenCCU/OpenCCU#879](https://github.com/OpenCCU/OpenCCU/issues/879),
  @jens-maus)
* patch 0094: wrap long names, descriptions and values in the system
  variable table of the user settings instead of widening the dialog.
  ([#89](https://github.com/OpenCCU/OpenCCU-Base/pull/89), @jens-maus)
* patch 0100: fix the translated group description not being filled in when
  creating a direct link.
  ([#76](https://github.com/OpenCCU/OpenCCU-Base/pull/76), @jens-maus)
* patch 0101: fix the JSON-RPC method system.describe returning invalid JSON
  (trailing commas).
  ([#42](https://github.com/OpenCCU/OpenCCU-Base/pull/42); refs
  [OpenCCU/OpenCCU#1037](https://github.com/OpenCCU/OpenCCU/issues/1037),
  @jens-maus)
* patch 0102: fix the firewall configuration not saving when the allowed IP
  list ends with a line break.
  ([#84](https://github.com/OpenCCU/OpenCCU-Base/pull/84); refs
  [OpenCCU/OpenCCU#1036](https://github.com/OpenCCU/OpenCCU/issues/1036),
  @jens-maus)
* patch 0105: fix an error message about the BidCos-RF install mode when
  opening the inbox on systems with only an HmIP radio module.
  ([#100](https://github.com/OpenCCU/OpenCCU-Base/pull/100); refs
  [OpenCCU/OpenCCU#372](https://github.com/OpenCCU/OpenCCU/issues/372),
  @jens-maus)
* patch 0106: show an unset condition operator (OPERATOR_NONE) as "OR"
  instead of "?" in the program editor.
  ([#43](https://github.com/OpenCCU/OpenCCU-Base/pull/43), @jens-maus)
* patch 0107: fix a ReGa script error when opening the RGBW color picker of
  a device without DEFAULT metadata.
  ([#39](https://github.com/OpenCCU/OpenCCU-Base/pull/39); refs
  [OpenCCU/OpenCCU#1052](https://github.com/OpenCCU/OpenCCU/issues/1052),
  @jens-maus)
* patch 0108: offer HmIP channels set to invisible in program conditions and
  activities. ([#120](https://github.com/OpenCCU/OpenCCU-Base/pull/120),
  @jens-maus)
* patch 0110: fix the Configure button missing in the additional software
  dialog for add-ons that only declare a Config-Url.
  ([#38](https://github.com/OpenCCU/OpenCCU-Base/pull/38); refs
  [eq-3/occu#110](https://github.com/eq-3/occu/issues/110), @jens-maus)
* patch 0112: fix broken hidden comfort/eco temperature fields in the HmIP
  heating channel dialog that could turn the previous value into NaN when
  invalid input was reverted.
  ([#40](https://github.com/OpenCCU/OpenCCU-Base/pull/40); refs
  [OpenCCU/OpenCCU#1058](https://github.com/OpenCCU/OpenCCU/issues/1058),
  @jens-maus)
* patch 0115: sort the device list of the LAN gateway configuration by name.
  ([#87](https://github.com/OpenCCU/OpenCCU-Base/pull/87), @jens-maus)
* patch 0116: fix the header cell of the sorted column being taller than the
  others in Chrome. ([#82](https://github.com/OpenCCU/OpenCCU-Base/pull/82);
  refs
  [OpenCCU/OpenCCU#1152](https://github.com/OpenCCU/OpenCCU/issues/1152),
  @jens-maus)
* patch 0118: fix backup, log file download and the time settings dialog
  failing with an internal error because of the new date -Iseconds time zone
  format (+02:00). ([#97](https://github.com/OpenCCU/OpenCCU-Base/pull/97),
  @jens-maus)
* patch 0120: mark the user name and password fields of the login, account
  and install wizard forms so password managers can fill them reliably.
  ([#135](https://github.com/OpenCCU/OpenCCU-Base/pull/135), @jens-maus)
* patch 0122: offer two start buttons for programs: "Execute" with condition
  check and "Execute (only 1st THEN)".
  ([#238](https://github.com/OpenCCU/OpenCCU-Base/pull/238); refs
  [OpenCCU/OpenCCU#1291](https://github.com/OpenCCU/OpenCCU/issues/1291),
  @jens-maus)
* patch 0123: fix program channel actions with a duration of 31 seconds or
  31 minutes switching on permanently; only 31 hours means permanent.
  ([#86](https://github.com/OpenCCU/OpenCCU-Base/pull/86); refs
  [OpenCCU/OpenCCU#1306](https://github.com/OpenCCU/OpenCCU/issues/1306),
  @jens-maus)
* patch 0124: fix several consecutive spaces in string system variable
  values being shown as one.
  ([#204](https://github.com/OpenCCU/OpenCCU-Base/pull/204); refs
  [OpenCCU/OpenCCU#1310](https://github.com/OpenCCU/OpenCCU/issues/1310),
  [OpenCCU/OpenCCU#1311](https://github.com/OpenCCU/OpenCCU/issues/1311),
  @jens-maus)
* patch 0127: sort the device firmware overview on the start page by device
  type and address.
  ([#119](https://github.com/OpenCCU/OpenCCU-Base/pull/119), @jens-maus)
* patch 0128: fix the line break in the English "Update Access Points with
  incompatible FW" button label.
  ([#81](https://github.com/OpenCCU/OpenCCU-Base/pull/81), @jens-maus)
* patch 0133: add a button to show internal device channels (e.g. the
  maintenance channel) in Status and control, and include enabled internal
  channels in rooms, functions, alarms and favorites.
  ([#198](https://github.com/OpenCCU/OpenCCU-Base/pull/198); refs
  [OpenCCU/OpenCCU#1475](https://github.com/OpenCCU/OpenCCU/issues/1475),
  [OpenCCU/OpenCCU#1477](https://github.com/OpenCCU/OpenCCU/issues/1477),
  [OpenCCU/OpenCCU#1751](https://github.com/OpenCCU/OpenCCU/issues/1751),
  @jens-maus)
* patch 0137: warn in the teach-in dialog when no radio module or LAN
  gateway is available, and fix HmIP blind channels failing to load without
  an HmIP-RF interface.
  ([#151](https://github.com/OpenCCU/OpenCCU-Base/pull/151), @jens-maus)
* patch 0139: fix the device view only showing the first channel and logging
  a ReGa script error for devices with unknown data points.
  ([#80](https://github.com/OpenCCU/OpenCCU-Base/pull/80); refs
  [OpenCCU/OpenCCU#1456](https://github.com/OpenCCU/OpenCCU/issues/1456),
  @jens-maus)
* patch 0140: show channel pictures in the channel lists of Status and
  control (by device, room and function).
  ([#183](https://github.com/OpenCCU/OpenCCU-Base/pull/183), @jens-maus)
* patch 0142: fix guest users seeing the untranslated placeholder
  ${lblWithoutDescription} for favorites without a description on the start
  page. ([#41](https://github.com/OpenCCU/OpenCCU-Base/pull/41); refs
  [OpenCCU/OpenCCU#1470](https://github.com/OpenCCU/OpenCCU/issues/1470),
  @jens-maus)
* patch 0143: translate the wind direction abbreviations and fix several
  German and English translations, typos and energy counter labels.
  ([#197](https://github.com/OpenCCU/OpenCCU-Base/pull/197); refs
  [OpenCCU/OpenCCU#3009](https://github.com/OpenCCU/OpenCCU/issues/3009),
  [OpenCCU/OpenCCU#3072](https://github.com/OpenCCU/OpenCCU/issues/3072),
  @jens-maus)
* patch 0146: show date and time, sunrise and sunset, and an available
  firmware update in one line each on the start page; administrators open
  the time settings by clicking the date.
  ([#248](https://github.com/OpenCCU/OpenCCU-Base/pull/248); refs
  [OpenCCU/OpenCCU#1495](https://github.com/OpenCCU/OpenCCU/issues/1495),
  [OpenCCU/OpenCCU#4541](https://github.com/OpenCCU/OpenCCU/issues/4541),
  @jens-maus)
* patch 0148: reboot and shut down in the background after answering the
  request, so the WebUI can show the result (e.g. after installing an
  add-on). ([#110](https://github.com/OpenCCU/OpenCCU-Base/pull/110); refs
  [OpenCCU/OpenCCU#1496](https://github.com/OpenCCU/OpenCCU/issues/1496),
  @jens-maus)
* patch 0152: add a button to show or hide internal system variables in the
  system variable chooser; all choosers now start with internal variables
  hidden. ([#133](https://github.com/OpenCCU/OpenCCU-Base/pull/133),
  @jens-maus)
* patch 0153: update the blinking header logo and the alarm/service message
  counters only when they change, instead of reloading the images on every
  refresh. ([#212](https://github.com/OpenCCU/OpenCCU-Base/pull/212),
  @jens-maus)
* patch 0157: fix the SysVar JSON-RPC methods returning invalid JSON and
  resolving objects outside the system variables.
  ([#57](https://github.com/OpenCCU/OpenCCU-Base/pull/57), @jens-maus)
* patch 0159: fix several JSON-RPC methods (e.g. restarting ReGa or sshd,
  SSH and sticky-unreach settings) returning an empty response, and encode
  JSON-RPC requests and responses as UTF-8.
  ([#67](https://github.com/OpenCCU/OpenCCU-Base/pull/67); refs
  [OpenCCU/OpenCCU#1593](https://github.com/OpenCCU/OpenCCU/issues/1593),
  @jens-maus)
* patch 0162: fix EVENT_DELAY_UNIT of acceleration sensor channels (e.g.
  HmIP-SAM) not being saved.
  ([#71](https://github.com/OpenCCU/OpenCCU-Base/pull/71); refs
  [OpenCCU/OpenCCU#1654](https://github.com/OpenCCU/OpenCCU/issues/1654),
  [OpenCCU/OpenCCU#1656](https://github.com/OpenCCU/OpenCCU/issues/1656),
  @jens-maus)
* patch 0166: fix the IEC power meter control breaking when the device does
  not provide all electricity/gas data points, and show gas power in m³/h.
  ([#50](https://github.com/OpenCCU/OpenCCU-Base/pull/50), @jens-maus)
* patch 0167: fix internal Tcl session requests (login, renewal, session
  creation) using URLs without the http:// scheme.
  ([#45](https://github.com/OpenCCU/OpenCCU-Base/pull/45), @jens-maus)
* patch 0172: fix a script error when the additional software dialog is
  closed before the add-on version check returns.
  ([#79](https://github.com/OpenCCU/OpenCCU-Base/pull/79), @jens-maus)
* patch 0173: send device, link and easy mode configuration commands by
  POST, so long parameter sets are no longer cut off by the URL length and
  values containing "&" or "=" stay intact.
  ([#243](https://github.com/OpenCCU/OpenCCU-Base/pull/243); refs
  [OpenCCU/OpenCCU#4520](https://github.com/OpenCCU/OpenCCU/issues/4520),
  @jens-maus)
* patch 0176: fix the valve state of several wall thermostat (BWTH) heating
  controls on one page updating the wrong control.
  ([#51](https://github.com/OpenCCU/OpenCCU-Base/pull/51), @jens-maus)
* patch 0177: fix empty text fields in device settings (e.g. the HmIP-WKP
  PIN) being filled with two quote characters ('').
  ([#52](https://github.com/OpenCCU/OpenCCU-Base/pull/52), @jens-maus)
* patch 0178: reject forbidden characters in the name and description when
  saving a system variable.
  ([#53](https://github.com/OpenCCU/OpenCCU-Base/pull/53), @jens-maus)
* patch 0181: always use HTTPS (without port 8443) for firmware update
  checks and downloads.
  ([#55](https://github.com/OpenCCU/OpenCCU-Base/pull/55), @jens-maus)
* patch 0182: fix dimmer controls always highlighting the same On/Off button
  instead of the one matching the current level.
  ([#49](https://github.com/OpenCCU/OpenCCU-Base/pull/49), @jens-maus)
* patch 0184: fix the channel behaviour of HM-MOD-EM8 input channels staying
  locked although the channel has no direct link or program.
  ([#103](https://github.com/OpenCCU/OpenCCU-Base/pull/103), @jens-maus)
* patch 0185: fix the time/position and additional software dialogs no
  longer being centered on the screen.
  ([#94](https://github.com/OpenCCU/OpenCCU-Base/pull/94); refs
  [OpenCCU/OpenCCU#2775](https://github.com/OpenCCU/OpenCCU/issues/2775),
  @jens-maus)
* patch 0187: fix saving a group while assignable devices are still in the
  inbox, show device images for inbox devices and fix the group's
  "Einstellen" button.
  ([#101](https://github.com/OpenCCU/OpenCCU-Base/pull/101), @jens-maus)
* patch 0191: sort lists by name in natural order ("Room 2" before "Room
  10") in administration, status and control, and favorites.
  ([#141](https://github.com/OpenCCU/OpenCCU-Base/pull/141), @jens-maus)
* patch 0197: keep the "device acts as router" option available for HmIP
  devices such as the HmIP-PSM.
  ([#99](https://github.com/OpenCCU/OpenCCU-Base/pull/99); refs
  [OpenCCU/OpenCCU#3067](https://github.com/OpenCCU/OpenCCU/issues/3067),
  @jens-maus)
* patch 0198: fix special-value options of direct link parameters showing a
  markup fragment instead of the translated value in newer Chromium-based
  browsers. ([#98](https://github.com/OpenCCU/OpenCCU-Base/pull/98),
  @jens-maus)
* patch 0199: show the date before the time in the system protocol.
  ([#137](https://github.com/OpenCCU/OpenCCU-Base/pull/137), @jens-maus)
* patch 0200: fix the WebUI briefly freezing on every refresh because of a
  synchronous request for the showAPITools tweak.
  ([#90](https://github.com/OpenCCU/OpenCCU-Base/pull/90); refs
  [OpenCCU/OpenCCU#3385](https://github.com/OpenCCU/OpenCCU/issues/3385),
  @jens-maus)
* patch 0201: fix syntax errors in system.fn.
  ([#96](https://github.com/OpenCCU/OpenCCU-Base/pull/96), @jens-maus)
* patch 0204: allow setting ACTIVE_PROFILE of a thermostat from a system
  variable in program actions.
  ([#44](https://github.com/OpenCCU/OpenCCU-Base/pull/44), @jens-maus)
* patch 0208: fix a ReGa script error for door lock channels in program
  actions and only offer the target lock level (LOCK_TARGET_LEVEL) there.
  ([#47](https://github.com/OpenCCU/OpenCCU-Base/pull/47), @jens-maus)

## Older versions

Changes of earlier versions are only recorded in the git history of this
repository.
