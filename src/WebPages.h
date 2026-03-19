#ifndef WebPages_h
#define WebPages_h

#include <Arduino.h>

/* * ============================================================
 * Karoro WiFi Manager - Web Assets
 * ============================================================
 */

// --- HTML Page ---
const char wifi_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Karoro WiFi Setup</title>
    <link rel="stylesheet" href="/wifi.css">
</head>
<body>
    <div class="container">
        <h2>Karoro Setup</h2>
        <p style="text-align:center; color:#666; font-size:14px;">กรุณาเลือกเครือข่าย WiFi</p>
        <div id="wifi-list" class="wifi-list">
            <div style="text-align:center; padding:20px;">กำลังค้นหาสัญญาณ...</div>
        </div>
    </div>
    <script src="/wifi.js"></script>
</body>
</html>
)rawliteral";

// --- CSS Styles ---
const char wifi_css[] PROGMEM = R"rawliteral(
body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    margin: 0; padding: 0;
    background-color: #f0f2f5;
    display: flex; justify-content: center; align-items: flex-start;
    min-height: 100vh;
}

.container {
    padding: 20px;
    width: 100%;
    max-width: 380px;
    margin-top: 30px;
}

h2 { text-align: center; color: #1a1a1a; margin-bottom: 5px; }

.wifi-list {
    display: flex;
    flex-direction: column;
    gap: 10px;
    margin-top: 20px;
}

.wifi-item {
    background-color: white;
    border-radius: 12px;
    box-shadow: 0 4px 6px rgba(0,0,0,0.05);
    transition: transform 0.2s;
    overflow: hidden;
}

.wifi-item:active { transform: scale(0.98); }

.connect-btn {
    display: flex;
    justify-content: space-between;
    align-items: center;
    width: 100%;
    padding: 16px;
    border: none;
    background: none;
    cursor: pointer;
    font-size: 15px;
    color: #333;
}

.wifi-ssid { font-weight: 600; display: flex; align-items: center; gap: 8px; }

.signal-bars { display: flex; align-items: flex-end; gap: 2px; height: 15px; }
.bar { width: 3px; background: #ddd; border-radius: 1px; }
.bar.active { background: #2ecc71; }
.bar:nth-child(1) { height: 6px; }
.bar:nth-child(2) { height: 9px; }
.bar:nth-child(3) { height: 12px; }
.bar:nth-child(4) { height: 15px; }
)rawliteral";

// --- JavaScript Logic ---
const char wifi_js[] PROGMEM = R"rawliteral(
document.addEventListener("DOMContentLoaded", async () => {
    try {
        const response = await fetch("/scan");
        if (!response.ok) throw new Error();
        const data = await response.json();
        const wifiList = document.getElementById("wifi-list");
        wifiList.innerHTML = "";

        data.forEach(net => {
            const level = getLevel(net.rssi);
            const bars = [1,2,3,4].map(i => `<div class="bar ${i<=level?'active':''}"></div>`).join("");
            
            const item = document.createElement("div");
            item.className = "wifi-item";
            item.innerHTML = `
                <button class="connect-btn" onclick="connect('${net.ssid}')">
                    <div class="wifi-ssid">
                        <div class="signal-bars">${bars}</div>
                        ${net.ssid}
                    </div>
                    <span>${net.encryption === 'open' ? '🔓' : '🔐'}</span>
                </button>
            `;
            wifiList.appendChild(item);
        });
    } catch (e) {
        document.getElementById("wifi-list").innerHTML = "<p style='text-align:center; color:red;'>ไม่สามารถดึงข้อมูลได้</p>";
    }
});

function getLevel(rssi) {
    if (rssi >= -50) return 4;
    if (rssi >= -65) return 3;
    if (rssi >= -80) return 2;
    return 1;
}

async function connect(ssid) {
    const pass = prompt(`ใส่รหัสผ่านสำหรับ: ${ssid}`);
    if (pass === null) return;

    const params = new URLSearchParams();
    params.append("ssid", ssid);
    params.append("pass", pass);

    try {
        const res = await fetch("/connect", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: params.toString()
        });
        if (res.ok) {
            alert("บันทึกสำเร็จ! อุปกรณ์กำลังรีสตาร์ทเพื่อเชื่อมต่อ...");
        }
    } catch (e) { alert("เกิดข้อผิดพลาดในการส่งข้อมูล"); }
}
)rawliteral";

#endif