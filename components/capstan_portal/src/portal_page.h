#pragma once

/*
 * The setup page, inline.
 *
 * Served from flash as one string with no external requests: a phone on
 * the setup AP has NO route to the internet, so any CDN font, framework
 * or stylesheet would hang until it timed out and then render unstyled.
 * Everything here is self-contained for that reason.
 *
 * Colours are the TrailCurrent tokens, hardcoded rather than referenced,
 * because this page is not an LVGL screen and cannot read the palette.
 * If the brand colours change, these change with them.
 */
static const char PORTAL_HTML[] =
"<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
"<title>Capstan Setup</title><style>"
":root{--bg:#12141a;--card:#1c1f27;--line:#2c313c;--fg:#f2f4f8;"
"--muted:#98a1b3;--accent:#52a441}"
"*{box-sizing:border-box}"
"body{margin:0;padding:20px;background:var(--bg);color:var(--fg);"
"font:16px/1.5 system-ui,-apple-system,sans-serif}"
"h1{font-size:20px;margin:0 0 4px}"
"p.sub{color:var(--muted);margin:0 0 20px;font-size:14px}"
"fieldset{border:1px solid var(--line);border-radius:10px;padding:14px;"
"margin:0 0 16px;background:var(--card)}"
"legend{padding:0 6px;color:var(--accent);font-weight:600;font-size:14px}"
"label{display:block;margin:10px 0 4px;font-size:13px;color:var(--muted)}"
"input,select{width:100%;padding:11px;border-radius:8px;font-size:16px;"
"border:1px solid var(--line);background:#0e1015;color:var(--fg)}"
"button{width:100%;padding:14px;margin-top:8px;border:0;border-radius:8px;"
"background:var(--accent);color:#08130a;font-size:16px;font-weight:700}"
"button.sec{background:transparent;color:var(--fg);"
"border:1px solid var(--line);font-weight:500}"
"#nets div{padding:11px;border-bottom:1px solid var(--line);"
"display:flex;justify-content:space-between;cursor:pointer}"
"#nets div:last-child{border-bottom:0}"
".bars{color:var(--muted);font-variant-numeric:tabular-nums}"
"#msg{padding:12px;border-radius:8px;margin-top:12px;display:none}"
".ok{background:#16351b;color:#9be8a5}.err{background:#3a1b1b;color:#ffb3b3}"
"</style></head><body>"
"<h1>Capstan Setup</h1>"
"<p class=\"sub\">Connect this display to your network and to Headwaters.</p>"
"<fieldset><legend>Wi-Fi</legend>"
"<button class=\"sec\" onclick=\"scan()\" id=\"sb\">Scan for networks</button>"
"<div id=\"nets\"></div>"
"<label>Network name</label><input id=\"ssid\" autocapitalize=\"off\">"
"<label>Security</label><select id=\"sec\">"
"<option value=\"4\">WPA / WPA2</option>"
"<option value=\"5\">WPA3</option>"
"<option value=\"1\">WEP</option>"
"<option value=\"0\">Open (no password)</option></select>"
"<label>Password</label><input id=\"pw\" type=\"password\">"
"</fieldset>"
"<fieldset><legend>Headwaters</legend>"
/* A real VALUE, not a placeholder.
 *
 * This was a placeholder, which renders as grey text that looks
 * pre-filled. Leaving it untouched submitted an empty host, the save
 * silently skipped the broker, and the panel came back up with MQTT
 * "Not set" having shown no error -- the one field the user was most
 * likely to think they had already filled in.
 *
 * headwaters.local is right for every deployment of this platform, so
 * it is a default rather than a hint. */
"<label>Broker host</label>"
"<input id=\"mh\" value=\"headwaters.local\" autocapitalize=\"off\">"
"<label>Port</label><input id=\"mp\" type=\"number\" value=\"8883\">"
"<label>Username</label><input id=\"mu\" autocapitalize=\"off\">"
"<label>Password</label><input id=\"mpw\" type=\"password\">"
"</fieldset>"
"<button onclick=\"save()\">Save and connect</button>"
"<div id=\"msg\"></div>"
"<script>"
"function show(t,c){var m=document.getElementById('msg');"
"m.textContent=t;m.className=c;m.style.display='block'}"
/* Poll, do not wait.
 *
 * /scan returns immediately with whatever is cached and a `scanning`
 * flag; results arrive on a later poll. Holding one long request open
 * worked from a laptop and failed on an iPhone, because iOS's
 * captive-portal browser abandons a slow fetch. Several short requests
 * suit it far better, and the list fills in as the radio finds things
 * rather than appearing all at once after a stall. */
"var polls=0;"
"function render(l){var n=document.getElementById('nets');n.innerHTML='';"
"l.forEach(function(a){var d=document.createElement('div');"
"d.innerHTML='<span>'+a.ssid+'</span><span class=\"bars\">'+a.bars+'</span>';"
"d.onclick=function(){document.getElementById('ssid').value=a.ssid;"
"document.getElementById('sec').value=a.sec;"
"document.getElementById('pw').focus()};n.appendChild(d)})}"
"function poll(){fetch('/scan').then(r=>r.json()).then(function(r){"
"render(r.nets||[]);"
"var b=document.getElementById('sb');"
"if(r.scanning&&polls<15){polls++;b.textContent='Scanning...';"
"setTimeout(poll,1500);return}"
"b.textContent='Scan again';b.disabled=false;"
"if(!(r.nets||[]).length){show('No networks found. Move the display "
"closer to the router and scan again.','err')}})"
".catch(function(){var b=document.getElementById('sb');"
"b.textContent='Scan again';b.disabled=false;"
"show('Scan failed. Try again.','err')})}"
"function scan(){var b=document.getElementById('sb');"
"b.textContent='Scanning...';b.disabled=true;polls=0;poll()}"
"function save(){var b={ssid:document.getElementById('ssid').value,"
"pw:document.getElementById('pw').value,"
"sec:parseInt(document.getElementById('sec').value,10),"
"mh:document.getElementById('mh').value,"
"mp:parseInt(document.getElementById('mp').value,10)||8883,"
"mu:document.getElementById('mu').value,"
"mpw:document.getElementById('mpw').value};"
"if(!b.ssid){show('Choose a network first.','err');return}"
"fetch('/save',{method:'POST',body:JSON.stringify(b)})"
".then(r=>r.json()).then(function(r){"
"if(r.ok){show('Checking...','ok');setTimeout(chk,1200)}"
"else{show(r.error||'Could not save.','err')}})"
".catch(function(){show('Could not reach the display.','err')})}"
/* Nothing is saved until this reports ok. The page has to keep asking,
 * because associating and then completing a TLS handshake takes far longer
 * than an iOS captive-portal browser will hold a request open. */
"function chk(){fetch('/status').then(r=>r.json()).then(function(r){"
"if(r.state=='checking'){show(r.msg||'Checking...','ok');"
"setTimeout(chk,1200);return}"
"if(r.state=='ok'){show(r.msg||'Saved.','ok');return}"
"if(r.state=='fail'){show((r.msg||'Could not verify.')+' Nothing was saved -- "
"correct it and try again.','err');return}"
"setTimeout(chk,1200)})"
".catch(function(){setTimeout(chk,1500)})}"
"scan();"
"</script></body></html>";
