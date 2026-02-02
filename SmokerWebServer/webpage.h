/*
 * Smoker Web Server - Web Page HTML
 *
 * This file contains the HTML/CSS/JavaScript for the smoker controller
 * web interface. Separated from main code for better organization.
 */

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

// HTML page stored in PROGMEM
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smoker Controller</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #ff6b6b 0%, #ff8e53 25%, #ffa047 50%, #ffb347 75%, #ffc837 100%);
            background-attachment: fixed;
            color: #2c3e50;
            min-height: 100vh;
            padding: 20px;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            animation: fadeIn 0.6s ease-in;
        }
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }
        h1 {
            text-align: center;
            color: #ffffff;
            margin-bottom: 20px;
            font-size: 2.4em;
            font-weight: 900;
            text-shadow:
                0 0 10px rgba(0,0,0,0.5),
                0 0 20px rgba(0,0,0,0.4),
                3px 3px 6px rgba(0,0,0,0.6),
                0 0 30px rgba(139, 0, 0, 0.5);
            letter-spacing: 1px;
            background: rgba(0,0,0,0.15);
            padding: 15px 20px;
            border-radius: 16px;
            backdrop-filter: blur(5px);
            border: 2px solid rgba(255,255,255,0.3);
        }
        .status {
            text-align: center;
            padding: 12px;
            border-radius: 12px;
            margin-bottom: 20px;
            font-size: 0.95em;
            font-weight: 700;
            backdrop-filter: blur(10px);
            animation: slideDown 0.4s ease-out;
        }
        @keyframes slideDown {
            from { opacity: 0; transform: translateY(-10px); }
            to { opacity: 1; transform: translateY(0); }
        }
        .status.connected {
            background: rgba(212, 237, 218, 0.95);
            color: #155724;
            border: 2px solid #28a745;
            box-shadow: 0 4px 15px rgba(40, 167, 69, 0.3);
        }
        .status.disconnected {
            background: rgba(248, 215, 218, 0.95);
            color: #721c24;
            border: 2px solid #dc3545;
            box-shadow: 0 4px 15px rgba(220, 53, 69, 0.3);
        }
        .card {
            background: rgba(255, 255, 255, 0.98);
            border-radius: 20px;
            padding: 20px;
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(0,0,0,0.15), 0 3px 8px rgba(0,0,0,0.1);
            border: 1px solid rgba(255,255,255,0.8);
            backdrop-filter: blur(10px);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
        }
        .card:hover {
            transform: translateY(-2px);
            box-shadow: 0 15px 40px rgba(0,0,0,0.2), 0 5px 12px rgba(0,0,0,0.15);
        }
        .card-title {
            font-size: 1.2em;
            background: linear-gradient(135deg, #ff6b35 0%, #ff8c42 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            margin-bottom: 18px;
            border-bottom: 3px solid;
            border-image: linear-gradient(90deg, #ff6b35, #ffa552, transparent) 1;
            padding-bottom: 10px;
            font-weight: 700;
            letter-spacing: 0.3px;
        }
        .temp-display {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
            gap: 18px;
        }
        .temp-box {
            background: linear-gradient(135deg, #fff5f0 0%, #ffe8dd 100%);
            border-radius: 16px;
            padding: 20px;
            text-align: center;
            border: 2px solid #ffd7c9;
            box-shadow: 0 4px 12px rgba(255, 107, 53, 0.15);
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }
        .temp-box::before {
            content: '';
            position: absolute;
            top: -50%;
            left: -50%;
            width: 200%;
            height: 200%;
            background: radial-gradient(circle, rgba(255,255,255,0.3) 0%, transparent 70%);
            opacity: 0;
            transition: opacity 0.3s ease;
        }
        .temp-box:hover::before {
            opacity: 1;
        }
        .temp-box:hover {
            transform: translateY(-4px) scale(1.02);
            box-shadow: 0 8px 20px rgba(255, 107, 53, 0.25);
        }
        .temp-label {
            font-size: 0.85em;
            color: #666;
            margin-bottom: 8px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .temp-value {
            font-size: 2.2em;
            font-weight: 800;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.05);
        }
        .temp-value.smoker { color: #e74c3c; }
        .temp-value.food { color: #f39c12; }
        .temp-value.set { color: #27ae60; }
        .controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
        }
        .control-group { margin-bottom: 1px; }
        .control-label {
            display: block;
            margin-bottom: 8px;
            color: #555;
            font-size: 0.9em;
            font-weight: 600;
        }
        input[type="number"], input[type="text"], input[type="password"] {
            width: 100%;
            padding: 12px;
            border: 2px solid #e0e6ed;
            border-radius: 8px;
            background: #f8f9fa;
            color: #2c3e50;
            font-size: 1em;
            transition: border-color 0.2s;
        }
        input[type="number"]:focus, input[type="text"]:focus, input[type="password"]:focus {
            outline: none;
            border-color: #ff6b35;
        }
        .temp-control-wrapper {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-bottom: 10px;
        }
        .temp-display-large {
            flex: 1;
            background: linear-gradient(135deg, #fff5f0 0%, #ffe8dd 100%);
            border-radius: 12px;
            padding: 18px;
            text-align: center;
            font-size: 2.2em;
            font-weight: 800;
            color: #ff6b35;
            border: 2px solid #ffd7c9;
            box-shadow: inset 0 2px 8px rgba(255, 107, 53, 0.1), 0 4px 12px rgba(255, 107, 53, 0.15);
            transition: all 0.3s ease;
        }
        .temp-display-large:hover {
            box-shadow: inset 0 2px 8px rgba(255, 107, 53, 0.15), 0 6px 16px rgba(255, 107, 53, 0.25);
        }
        .temp-arrows {
            display: flex;
            flex-direction: column;
            gap: 8px;
        }
        .btn-arrow {
            padding: 10px 18px;
            border: none;
            border-radius: 10px;
            font-size: 1.3em;
            cursor: pointer;
            background: linear-gradient(135deg, #ff6b35 0%, #ff8555 100%);
            color: #fff;
            transition: all 0.2s ease;
            min-width: 55px;
            box-shadow: 0 4px 12px rgba(255, 107, 53, 0.3);
            font-weight: 700;
        }
        .btn-arrow:active {
            transform: scale(0.95);
            box-shadow: 0 2px 6px rgba(255, 107, 53, 0.3);
        }
        .btn-arrow:hover {
            background: linear-gradient(135deg, #ff8555 0%, #ffa066 100%);
            transform: translateY(-2px);
            box-shadow: 0 6px 16px rgba(255, 107, 53, 0.4);
        }
        .fan-section {
            border: 2px solid #e0e6ed;
            border-radius: 12px;
            padding: 16px;
            margin-bottom: 15px;
            background: linear-gradient(135deg, #ffffff 0%, #f8f9fa 100%);
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            transition: all 0.3s ease;
        }
        .fan-section:hover {
            box-shadow: 0 4px 12px rgba(0,0,0,0.12);
        }
        .bar-graph {
            margin-top: 15px;
        }
        .bar-label {
            font-size: 0.85em;
            color: #555;
            margin-bottom: 5px;
            display: flex;
            justify-content: space-between;
            font-weight: 600;
        }
        .bar-container {
            width: 100%;
            height: 28px;
            background: linear-gradient(135deg, #e8ecf1 0%, #d1d8e0 100%);
            border-radius: 14px;
            overflow: hidden;
            position: relative;
            margin-bottom: 15px;
            border: 2px solid #d1d8e0;
            box-shadow: inset 0 2px 4px rgba(0,0,0,0.06);
        }
        .bar-fill {
            height: 100%;
            transition: width 0.3s ease;
            border-radius: 14px;
            position: relative;
            overflow: hidden;
        }
        .bar-fill::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, transparent 0%, rgba(255,255,255,0.3) 50%, transparent 100%);
            animation: shimmer 2s infinite;
        }
        @keyframes shimmer {
            0% { transform: translateX(-100%); }
            100% { transform: translateX(100%); }
        }
        .bar-fill.rpm {
            background: linear-gradient(90deg, #27ae60 0%, #f39c12 50%, #e74c3c 100%);
            box-shadow: 0 0 15px rgba(243, 156, 18, 0.5);
        }
        .bar-fill.pwm {
            background: linear-gradient(90deg, #3498db 0%, #27ae60 100%);
            box-shadow: 0 0 15px rgba(52, 152, 219, 0.5);
        }
        .bar-value {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            font-size: 0.85em;
            font-weight: bold;
            color: #2c3e50;
            text-shadow: 1px 1px 2px rgba(255,255,255,0.8);
        }
        input[type="range"] {
            width: 100%;
            margin: 10px 0;
        }
        .btn {
            padding: 14px 28px;
            border: none;
            border-radius: 12px;
            font-size: 1em;
            cursor: pointer;
            transition: all 0.3s ease;
            width: 100%;
            font-weight: 700;
            box-shadow: 0 4px 12px rgba(0,0,0,0.15);
            position: relative;
            overflow: hidden;
        }
        .btn::before {
            content: '';
            position: absolute;
            top: 50%;
            left: 50%;
            width: 0;
            height: 0;
            border-radius: 50%;
            background: rgba(255,255,255,0.3);
            transform: translate(-50%, -50%);
            transition: width 0.6s, height 0.6s;
        }
        .btn:active::before {
            width: 300px;
            height: 300px;
        }
        .btn:active { transform: scale(0.97); }
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 18px rgba(0,0,0,0.2);
        }
        .btn-primary {
            background: linear-gradient(135deg, #ff6b35 0%, #ff8555 100%);
            color: #fff;
        }
        .btn-primary:hover {
            background: linear-gradient(135deg, #ff8555 0%, #ffa066 100%);
        }
        .btn-danger {
            background: linear-gradient(135deg, #e74c3c 0%, #ec7063 100%);
            color: #fff;
        }
        .btn-danger:hover {
            background: linear-gradient(135deg, #ec7063 0%, #f1948a 100%);
        }
        .btn-success {
            background: linear-gradient(135deg, #27ae60 0%, #2ecc71 100%);
            color: #fff;
        }
        .btn-success:hover {
            background: linear-gradient(135deg, #2ecc71 0%, #58d68d 100%);
        }
        .btn-secondary {
            background: linear-gradient(135deg, #95a5a6 0%, #b2babb 100%);
            color: #fff;
        }
        .btn-secondary:hover {
            background: linear-gradient(135deg, #b2babb 0%, #d5dbdb 100%);
        }
        .btn:disabled {
            opacity: 0.5;
            cursor: not-allowed;
            transform: none !important;
        }
        .btn-group {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }
        .switch-container {
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 8px;
            border-radius: 8px;
            transition: background-color 0.3s ease;
        }
        .switch-container:hover {
            background-color: rgba(255, 107, 53, 0.05);
        }
        .switch {
            position: relative;
            width: 60px;
            height: 34px;
        }
        .switch input { opacity: 0; width: 0; height: 0; }
        .slider {
            position: absolute;
            cursor: pointer;
            top: 0; left: 0; right: 0; bottom: 0;
            background-color: #bdc3c7;
            transition: all 0.3s ease;
            border-radius: 34px;
            box-shadow: inset 0 2px 4px rgba(0,0,0,0.1), 0 2px 6px rgba(0,0,0,0.1);
        }
        .slider:before {
            position: absolute;
            content: "";
            height: 26px;
            width: 26px;
            left: 4px;
            bottom: 4px;
            background: linear-gradient(135deg, #ffffff 0%, #f8f9fa 100%);
            transition: all 0.3s cubic-bezier(0.68, -0.55, 0.265, 1.55);
            border-radius: 50%;
            box-shadow: 0 2px 6px rgba(0,0,0,0.25);
        }
        input:checked + .slider {
            background: linear-gradient(135deg, #27ae60 0%, #2ecc71 100%);
            box-shadow: inset 0 2px 4px rgba(0,0,0,0.1), 0 0 15px rgba(39, 174, 96, 0.5);
        }
        input:checked + .slider:before {
            transform: translateX(26px) rotate(360deg);
        }
        .timer-display {
            font-size: 3em;
            text-align: center;
            font-family: 'Courier New', monospace;
            background: linear-gradient(135deg, #ff6b35 0%, #ff8c42 50%, #ffa552 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            margin: 20px 0;
            font-weight: 800;
            letter-spacing: 4px;
            text-shadow: 2px 2px 8px rgba(255, 107, 53, 0.3);
            padding: 15px;
            background-color: rgba(255, 245, 240, 0.5);
            border-radius: 12px;
        }
        .info-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }
        .info-item {
            background: #f8f9fa;
            padding: 10px;
            border-radius: 8px;
            text-align: center;
            border: 2px solid #e0e6ed;
        }
        .info-label { font-size: 0.75em; color: #666; font-weight: 600; }
        .info-value { font-size: 1.2em; color: #2c3e50; font-weight: bold; }
        .settings-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 10px;
        }
        .collapsible {
            cursor: pointer;
            user-select: none;
        }
        .collapsible:after {
            content: ' +';
            float: right;
            color: #ff6b35;
            font-weight: bold;
        }
        .collapsible.active:after { content: ' -'; }
        .collapse-content {
            max-height: 0;
            overflow: hidden;
            transition: max-height 0.3s ease-out;
        }
        .collapse-content.show { max-height: 1000px; }
        .chart-container {
            position: relative;
            width: 100%;
            height: 250px;
            background: linear-gradient(135deg, #ffffff 0%, #f8f9fa 100%);
            border-radius: 16px;
            padding: 15px;
            border: 2px solid #d1d8e0;
            box-shadow: inset 0 2px 8px rgba(0,0,0,0.05), 0 4px 12px rgba(0,0,0,0.08);
        }
        .chart-canvas {
            width: 100%;
            height: 100%;
        }
        .chart-legend {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin-top: 10px;
            font-size: 0.85em;
            font-weight: 600;
        }
        .legend-item {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        .legend-color {
            width: 12px;
            height: 12px;
            border-radius: 2px;
        }
        .legend-smoker { background: #e74c3c; }
        .legend-food { background: #f39c12; }
        .wifi-setup-banner {
            background: linear-gradient(135deg, #ffeaa7 0%, #fdcb6e 100%);
            border-radius: 16px;
            padding: 25px;
            margin-bottom: 20px;
            text-align: center;
            box-shadow: 0 8px 20px rgba(253, 203, 110, 0.4);
            border: 2px solid #fdcb6e;
            animation: pulse 2s ease-in-out infinite;
        }
        @keyframes pulse {
            0%, 100% { box-shadow: 0 8px 20px rgba(253, 203, 110, 0.4); }
            50% { box-shadow: 0 8px 25px rgba(253, 203, 110, 0.6); }
        }
        .wifi-setup-banner h2 {
            margin: 0 0 12px 0;
            font-size: 1.6em;
            color: #2c3e50;
            font-weight: 800;
        }
        .wifi-setup-banner p {
            margin: 0 0 18px 0;
            color: #555;
            font-size: 1em;
        }
        .wifi-setup-banner .btn {
            background: linear-gradient(135deg, #ff6b35 0%, #ff8555 100%);
            color: #fff;
            font-weight: 800;
            max-width: 300px;
            margin: 0 auto;
            box-shadow: 0 4px 15px rgba(255, 107, 53, 0.4);
        }
        .wifi-setup-banner .btn:hover {
            background: linear-gradient(135deg, #ff8555 0%, #ffa066 100%);
        }
        .wifi-info-banner {
            background: linear-gradient(135deg, #d4edda 0%, #c3e6cb 100%);
            border-radius: 12px;
            padding: 15px;
            margin-bottom: 20px;
            text-align: center;
            font-size: 0.95em;
            border: 2px solid #28a745;
            box-shadow: 0 4px 12px rgba(40, 167, 69, 0.2);
        }
        .wifi-info-banner .wifi-ssid {
            color: #155724;
            font-weight: 800;
        }
        @media (max-width: 480px) {
            .temp-value { font-size: 1.6em; }
            .timer-display { font-size: 2em; }
            .chart-container { height: 200px; }
            .wifi-setup-banner h2 { font-size: 1.2em; }
        }
        /* Session Recording Styles */
        .recording-status {
            display: flex;
            align-items: center;
            gap: 10px;
            padding: 12px;
            margin-bottom: 15px;
            background: #f8f9fa;
            border-radius: 12px;
            border: 2px solid #e0e6ed;
        }
        .recording-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #95a5a6;
            flex-shrink: 0;
        }
        .recording-dot.active {
            background: #e74c3c;
            animation: pulse-recording 1s infinite;
        }
        @keyframes pulse-recording {
            0%, 100% { opacity: 1; box-shadow: 0 0 0 0 rgba(231, 76, 60, 0.7); }
            50% { opacity: 0.8; box-shadow: 0 0 0 8px rgba(231, 76, 60, 0); }
        }
        .session-list {
            max-height: 300px;
            overflow-y: auto;
        }
        .session-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 12px;
            margin-bottom: 8px;
            background: linear-gradient(135deg, #fff5f0 0%, #ffe8dd 100%);
            border-radius: 10px;
            border: 2px solid #ffd7c9;
            cursor: pointer;
            transition: all 0.2s;
        }
        .session-item:hover {
            transform: translateX(5px);
            border-color: #ff6b35;
        }
        .session-name {
            font-weight: 700;
            color: #2c3e50;
        }
        .session-meta {
            font-size: 0.85em;
            color: #666;
        }
        .modal {
            position: fixed;
            z-index: 1000;
            left: 0;
            top: 0;
            width: 100%;
            height: 100%;
            background: rgba(0,0,0,0.5);
            backdrop-filter: blur(5px);
            display: none;
        }
        .modal-content {
            background: #fff;
            margin: 5% auto;
            padding: 25px;
            border-radius: 20px;
            max-width: 500px;
            max-height: 80vh;
            overflow-y: auto;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            animation: modalSlideIn 0.3s ease-out;
        }
        @keyframes modalSlideIn {
            from { opacity: 0; transform: translateY(-50px); }
            to { opacity: 1; transform: translateY(0); }
        }
        .modal-close {
            float: right;
            font-size: 28px;
            font-weight: bold;
            color: #aaa;
            cursor: pointer;
            line-height: 1;
        }
        .modal-close:hover { color: #e74c3c; }
        .modal-title {
            margin: 0 0 15px 0;
            color: #2c3e50;
            font-size: 1.4em;
        }
        .modal-section {
            margin: 15px 0;
            padding: 12px;
            background: #f8f9fa;
            border-radius: 10px;
        }
        .modal-section-title {
            font-weight: 700;
            color: #ff6b35;
            margin-bottom: 8px;
            font-size: 0.9em;
        }
        .event-item {
            padding: 8px;
            margin: 5px 0;
            background: #fff;
            border-radius: 6px;
            font-size: 0.9em;
            border-left: 3px solid #ff6b35;
        }
        .note-item {
            padding: 8px;
            margin: 5px 0;
            background: #fff5f0;
            border-radius: 6px;
            font-size: 0.9em;
            border-left: 3px solid #27ae60;
        }
        .modal-actions {
            display: flex;
            gap: 10px;
            margin-top: 20px;
        }
        .modal-actions .btn { flex: 1; }
        select {
            width: 100%;
            padding: 12px;
            border: 2px solid #e0e6ed;
            border-radius: 8px;
            background: #f8f9fa;
            color: #2c3e50;
            font-size: 1em;
            cursor: pointer;
        }
        select:focus {
            outline: none;
            border-color: #ff6b35;
        }
        .custom-meat-input {
            margin-top: 10px;
            display: none;
        }
        .note-input-wrapper {
            display: flex;
            gap: 10px;
            margin-top: 10px;
        }
        .note-input-wrapper input { flex: 1; }
        .note-input-wrapper .btn { width: auto; padding: 12px 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔥 Smoker Controller 🔥</h1>

        <!-- WiFi Setup Required Banner (shown only in AP mode) -->
        <div id="wifiSetupBanner" class="wifi-setup-banner" style="display: none;">
            <h2>⚠️ WiFi Setup Required</h2>
            <p>Your smoker controller is currently in setup mode. Please configure your WiFi network to enable remote access.</p>
            <button class="btn" onclick="scrollToWiFiSettings()">Configure WiFi Now</button>
        </div>

        <!-- WiFi Connected Info (shown when connected) -->
        <div id="wifiInfoBanner" class="wifi-info-banner" style="display: none;">
            Connected to: <span class="wifi-ssid" id="connectedSSID"></span>
        </div>

        <div id="connectionStatus" class="status disconnected">
            Connecting...
        </div>

        <!-- Temperature Display -->
        <div class="card">
            <div class="card-title">Temperatures</div>
            <div class="temp-display">
                <div class="temp-box">
                    <div class="temp-label">Smoker</div>
                    <div class="temp-value smoker" id="smokerTemp">--</div>
                </div>
                <div class="temp-box">
                    <div class="temp-label">Food</div>
                    <div class="temp-value food" id="foodTemp">--</div>
                </div>
                <div class="temp-box">
                    <div class="temp-label">Set Temp</div>
                    <div class="temp-value set" id="setTempDisplay">--</div>
                </div>
            </div>
        </div>

        <!-- Controls -->
        <div class="card">
            <div class="card-title">Controls</div>
            <div class="controls">
                <div class="control-group">
                    <label class="control-label">Set Temperature (°F)</label>
                    <div class="temp-control-wrapper">
                        <div class="temp-display-large" id="setTempInput">225</div>
                        <div class="temp-arrows">
                            <button class="btn-arrow" onclick="adjustTemperature(5)">▲</button>
                            <button class="btn-arrow" onclick="adjustTemperature(-5)">▼</button>
                        </div>
                    </div>
                    <button class="btn btn-primary" onclick="setTemperature()">Set Temp</button>
                </div>
                <div class="control-group">
                    <!-- Fan Control Switch -->
                    <div class="fan-section">
                        <div class="switch-container">
                            <span>Fan Control</span>
                            <label class="switch">
                                <input type="checkbox" id="fanSwitch" onchange="toggleFan()">
                                <span class="slider"></span>
                            </label>
                        </div>
                    </div>

                    <!-- Fan RPM Bar Graph -->
                    <div class="fan-section">
                        <div class="bar-label">
                            <span>Fan RPM</span>
                            <span id="fanRPMValue">0</span>
                        </div>
                        <div class="bar-container">
                            <div class="bar-fill rpm" id="rpmBar" style="width: 0%"></div>
                            <div class="bar-value" id="fanRPM">0 RPM</div>
                        </div>
                    </div>

                    <!-- PWM Output Bar Graph -->
                    <div class="fan-section">
                        <div class="bar-label">
                            <span>PWM Output</span>
                            <span id="fanPWMValue">0%</span>
                        </div>
                        <div class="bar-container">
                            <div class="bar-fill pwm" id="pwmBar" style="width: 0%"></div>
                            <div class="bar-value" id="fanPWM">0%</div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- Timer -->
        <div class="card">
            <div class="card-title">Timer</div>
            <div class="timer-display" id="timerDisplay">00:00:00</div>
            <div class="btn-group">
                <button class="btn btn-success" id="timerStartBtn" onclick="timerControl('START')">Start</button>
                <button class="btn btn-danger" onclick="timerControl('RESET')">Reset</button>
            </div>
        </div>

        <!-- Session Recording -->
        <div class="card">
            <div class="card-title">Session Recording</div>

            <!-- Recording Status -->
            <div class="recording-status">
                <span id="recordingIndicator" class="recording-dot"></span>
                <span id="recordingText">Not Recording</span>
            </div>

            <!-- Start New Session Form (shown when not recording) -->
            <div id="startSessionForm">
                <div class="control-group">
                    <label class="control-label">Session Name</label>
                    <input type="text" id="sessionName" placeholder="e.g., Sunday Brisket">
                </div>
                <div class="control-group">
                    <label class="control-label">Type of Meat</label>
                    <select id="meatType" onchange="handleMeatTypeChange()">
                        <option value="Beef Brisket">Beef Brisket</option>
                        <option value="Pork Shoulder">Pork Shoulder</option>
                        <option value="Pork Ribs">Pork Ribs</option>
                        <option value="Beef Ribs">Beef Ribs</option>
                        <option value="Chicken">Chicken</option>
                        <option value="Turkey">Turkey</option>
                        <option value="Salmon">Salmon</option>
                        <option value="Pork Belly">Pork Belly</option>
                        <option value="Other">Other...</option>
                    </select>
                    <div id="customMeatWrapper" class="custom-meat-input">
                        <input type="text" id="customMeatType" placeholder="Enter meat type">
                    </div>
                </div>
                <button class="btn btn-success" onclick="startSession()">Start Recording</button>
            </div>

            <!-- Active Session Controls (shown when recording) -->
            <div id="activeSessionControls" style="display:none;">
                <div style="padding:12px; background:#fff5f0; border-radius:10px; margin-bottom:15px; border:2px solid #ffd7c9;">
                    <div style="font-weight:700; color:#2c3e50;" id="activeSessionName">Session Name</div>
                    <div style="font-size:0.9em; color:#666;">
                        Duration: <span id="activeSessionDuration">00:00:00</span>
                    </div>
                </div>
                <div class="control-group">
                    <label class="control-label">Add Note</label>
                    <div class="note-input-wrapper">
                        <input type="text" id="sessionNote" placeholder="e.g., Spritzed with apple juice">
                        <button class="btn btn-primary" onclick="addSessionNote(this)">Add</button>
                    </div>
                </div>
                <button class="btn btn-danger" onclick="stopSession()">Stop Recording</button>
            </div>
        </div>

        <!-- Session History (Collapsible) -->
        <div class="card">
            <div class="card-title collapsible" onclick="toggleCollapse(this)">
                Session History
            </div>
            <div class="collapse-content">
                <div id="sessionList" class="session-list" style="margin-top:15px;">
                    <p style="color:#666; text-align:center;">Loading sessions...</p>
                </div>
                <button class="btn btn-secondary" onclick="loadSessions()" style="margin-top:15px;">Refresh List</button>
            </div>
        </div>

        <!-- Temperature History Chart -->
        <div class="card">
            <div class="card-title">Temperature History (Last 2 Hours)</div>
            <div class="chart-container">
                <canvas id="tempChart" class="chart-canvas"></canvas>
            </div>
            <div class="chart-legend">
                <div class="legend-item">
                    <div class="legend-color legend-smoker"></div>
                    <span>Smoker</span>
                </div>
                <div class="legend-item">
                    <div class="legend-color legend-food"></div>
                    <span>Food</span>
                </div>
            </div>
        </div>

        <!-- Advanced Settings (Collapsible) -->
        <div class="card">
            <div class="card-title collapsible" onclick="toggleCollapse(this)">
                Advanced Settings
            </div>
            <div class="collapse-content">
                <!-- Temperature Calibration Section -->
                <div style="margin-top:15px; padding:15px; background:#fff5f0; border-radius:12px; border:2px solid #ffd7c9;">
                    <div style="font-weight:700; color:#ff6b35; margin-bottom:12px;">Temperature Calibration</div>
                    <p style="font-size:0.85em; color:#666; margin-bottom:15px;">
                        Adjust these values to calibrate your temperature probes. Positive values add degrees, negative values subtract.
                    </p>
                    <div class="settings-grid">
                        <div class="control-group">
                            <label class="control-label">Smoker Offset (°F)</label>
                            <input type="number" id="smokerOffsetInput" step="1" value="0" min="-50" max="50">
                        </div>
                        <div class="control-group">
                            <label class="control-label">Food Offset (°F)</label>
                            <input type="number" id="foodOffsetInput" step="1" value="0" min="-50" max="50">
                        </div>
                    </div>
                    <button class="btn btn-primary" onclick="saveCalibration()" style="margin-top:10px">Save Calibration</button>
                </div>

                <!-- PID Settings Section -->
                <div style="margin-top:20px; padding:15px; background:#f8f9fa; border-radius:12px; border:2px solid #e0e6ed;">
                    <div style="font-weight:700; color:#555; margin-bottom:12px;">PID Settings</div>
                    <div class="settings-grid">
                        <div class="control-group">
                            <label class="control-label">Kp</label>
                            <input type="number" id="kpInput" step="0.1" value="7.0">
                        </div>
                        <div class="control-group">
                            <label class="control-label">Ki</label>
                            <input type="number" id="kiInput" step="0.1" value="0">
                        </div>
                        <div class="control-group">
                            <label class="control-label">Kd</label>
                            <input type="number" id="kdInput" step="0.1" value="0.2">
                        </div>
                    </div>
                    <button class="btn btn-primary" onclick="setPID()" style="margin-top:10px">Update PID</button>
                </div>

                <div style="margin-top:20px">
                    <button class="btn btn-secondary" onclick="getConfig()">Load Current Config</button>
                </div>
            </div>
        </div>

        <!-- WiFi Settings (Collapsible) -->
        <div class="card">
            <div class="card-title collapsible" onclick="toggleCollapse(this)">
                WiFi Settings
            </div>
            <div class="collapse-content">
                <div class="control-group" style="margin-top:15px">
                    <label class="control-label">SSID</label>
                    <input type="text" id="wifiSSID" placeholder="WiFi Network Name">
                </div>
                <div class="control-group">
                    <label class="control-label">Password</label>
                    <input type="password" id="wifiPassword" placeholder="WiFi Password">
                </div>
                <div class="control-group">
                    <label class="control-label">Hostname (for smoker.local)</label>
                    <input type="text" id="wifiHostname" placeholder="smoker" value="smoker">
                </div>
                <button class="btn btn-primary" onclick="saveWiFi()">Save & Connect</button>
            </div>
        </div>
    </div>

    <!-- Session Detail Modal -->
    <div id="sessionModal" class="modal">
        <div class="modal-content">
            <span class="modal-close" onclick="closeSessionModal()">&times;</span>
            <h2 class="modal-title" id="modalSessionName">Session Details</h2>

            <div class="modal-section">
                <div style="display:grid; grid-template-columns:1fr 1fr; gap:10px;">
                    <div><strong>Meat:</strong> <span id="modalMeatType">-</span></div>
                    <div><strong>Set Temp:</strong> <span id="modalSetTemp">-</span>&deg;F</div>
                    <div><strong>Duration:</strong> <span id="modalDuration">-</span></div>
                    <div><strong>Date:</strong> <span id="modalDate">-</span></div>
                </div>
            </div>

            <div class="modal-section">
                <div class="modal-section-title">Temperature Changes</div>
                <div id="modalTempChanges">
                    <p style="color:#666; font-size:0.9em;">No temperature changes recorded</p>
                </div>
            </div>

            <div class="modal-section">
                <div class="modal-section-title">Notes</div>
                <div id="modalNotes">
                    <p style="color:#666; font-size:0.9em;">No notes recorded</p>
                </div>
            </div>

            <div class="modal-actions">
                <button class="btn btn-primary" onclick="loadSessionSettings()">Load Settings</button>
                <button class="btn btn-danger" onclick="deleteSession()">Delete</button>
            </div>
        </div>
    </div>

    <script>
        let ws;
        let timerRunning = false;
        let smokerHistory = [];
        let foodHistory = [];
        let chartCanvas, chartCtx;
        let userSetTemp = 225;  // Track user's desired set temperature locally
        let userSetTempPending = false;  // Track if user has unsent temperature changes
        let controllerConnected = false;  // Track smoker controller connection status
        let calibrationLoaded = false;  // Track if calibration values have been loaded initially
        let pidLoaded = false;  // Track if PID values have been loaded initially

        // Session recording state
        let sessionRecording = false;
        let activeSessionId = null;
        let currentViewSessionId = null;
        let sessionStartTime = null;

        function initChart() {
            chartCanvas = document.getElementById('tempChart');
            chartCtx = chartCanvas.getContext('2d');
            // Set canvas resolution for sharp rendering
            const rect = chartCanvas.getBoundingClientRect();
            chartCanvas.width = rect.width * 2;
            chartCanvas.height = rect.height * 2;
            chartCtx.scale(2, 2);
        }

        function drawChart() {
            if (!chartCtx) return;

            const rect = chartCanvas.getBoundingClientRect();
            const width = rect.width;
            const height = rect.height;
            const padding = { top: 20, right: 20, bottom: 30, left: 45 };
            const chartWidth = width - padding.left - padding.right;
            const chartHeight = height - padding.top - padding.bottom;

            // Clear canvas
            chartCtx.clearRect(0, 0, width, height);

            // Find min/max for Y axis
            let allTemps = [...smokerHistory, ...foodHistory].filter(t => t > 0);
            if (allTemps.length === 0) {
                // No data yet - draw empty chart
                chartCtx.fillStyle = '#95a5a6';
                chartCtx.font = '14px sans-serif';
                chartCtx.textAlign = 'center';
                chartCtx.fillText('Waiting for data...', width / 2, height / 2);
                return;
            }

            let minTemp = Math.min(...allTemps);
            let maxTemp = Math.max(...allTemps);

            // Add padding to range
            const range = maxTemp - minTemp;
            minTemp = Math.max(0, minTemp - range * 0.1 - 10);
            maxTemp = maxTemp + range * 0.1 + 10;

            // Draw grid lines and Y axis labels
            chartCtx.strokeStyle = '#d1d8e0';
            chartCtx.lineWidth = 1;
            chartCtx.fillStyle = '#666';
            chartCtx.font = '10px sans-serif';
            chartCtx.textAlign = 'right';

            const ySteps = 5;
            for (let i = 0; i <= ySteps; i++) {
                const y = padding.top + (chartHeight * i / ySteps);
                const temp = Math.round(maxTemp - ((maxTemp - minTemp) * i / ySteps));

                chartCtx.beginPath();
                chartCtx.moveTo(padding.left, y);
                chartCtx.lineTo(width - padding.right, y);
                chartCtx.stroke();

                chartCtx.fillText(temp + '°', padding.left - 5, y + 3);
            }

            // Draw X axis labels (time)
            chartCtx.textAlign = 'center';
            const dataPoints = Math.max(smokerHistory.length, 1);
            const xLabels = ['2h ago', '1.5h', '1h', '30m', 'Now'];
            for (let i = 0; i < xLabels.length; i++) {
                const x = padding.left + (chartWidth * i / (xLabels.length - 1));
                chartCtx.fillText(xLabels[i], x, height - padding.bottom + 15);
            }

            // Helper function to draw line
            function drawLine(data, color) {
                if (data.length < 2) return;

                chartCtx.strokeStyle = color;
                chartCtx.lineWidth = 2;
                chartCtx.beginPath();

                let started = false;
                for (let i = 0; i < data.length; i++) {
                    if (data[i] === 0) continue; // Skip zero values

                    const x = padding.left + (chartWidth * i / (data.length - 1));
                    const y = padding.top + chartHeight - ((data[i] - minTemp) / (maxTemp - minTemp) * chartHeight);

                    if (!started) {
                        chartCtx.moveTo(x, y);
                        started = true;
                    } else {
                        chartCtx.lineTo(x, y);
                    }
                }
                chartCtx.stroke();
            }

            // Draw smoker temperature line (red)
            drawLine(smokerHistory, '#e74c3c');

            // Draw food temperature line (orange)
            drawLine(foodHistory, '#f39c12');
        }

        function connect() {
            const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
            ws = new WebSocket(`${protocol}//${window.location.host}/ws`);

            ws.onopen = () => {
                // WebSocket connected to ESP32, but wait for data to confirm controller connection
                updateConnectionStatus();
            };

            ws.onclose = () => {
                controllerConnected = false;
                document.getElementById('connectionStatus').className = 'status disconnected';
                document.getElementById('connectionStatus').textContent = 'Disconnected from Web Server - Reconnecting...';
                setTimeout(connect, 2000);
            };

            ws.onerror = (error) => {
                console.error('WebSocket error:', error);
            };

            ws.onmessage = (event) => {
                try {
                    const data = JSON.parse(event.data);
                    updateDisplay(data);
                } catch (e) {
                    console.error('Parse error:', e);
                }
            };
        }

        function updateConnectionStatus() {
            const statusEl = document.getElementById('connectionStatus');
            if (controllerConnected) {
                statusEl.className = 'status connected';
                statusEl.textContent = 'Connected to Smoker Controller';
            } else {
                statusEl.className = 'status disconnected';
                statusEl.textContent = 'Not Connected to Smoker Controller';
            }
        }

        function updateDisplay(data) {
            // Update controller connection status
            if (data.controllerConnected !== undefined) {
                controllerConnected = data.controllerConnected;
                updateConnectionStatus();
            }

            if (data.smokerTemp !== undefined) {
                document.getElementById('smokerTemp').textContent = data.smokerTemp + '°F';
                document.getElementById('foodTemp').textContent = data.foodTemp + '°F';
                document.getElementById('setTempDisplay').textContent = data.setTemp + '°F';

                // Update the Controls set temp display from controller unless user has pending changes
                // This allows the display to reflect changes from the controller while preserving
                // user edits that haven't been sent yet
                if (!userSetTempPending) {
                    userSetTemp = data.setTemp;
                    document.getElementById('setTempInput').textContent = data.setTemp;
                }

                document.getElementById('fanSwitch').checked = data.fanOn;

                // Update fan RPM bar graph (max 3000 RPM)
                const rpmPercent = Math.min((data.rpm / 3000) * 100, 100);
                document.getElementById('rpmBar').style.width = rpmPercent + '%';
                document.getElementById('fanRPM').textContent = data.rpm + ' RPM';
                document.getElementById('fanRPMValue').textContent = data.rpm;

                // Update PWM bar graph
                const pwmPercent = Math.round(data.pwm / 255 * 100);
                document.getElementById('pwmBar').style.width = pwmPercent + '%';
                document.getElementById('fanPWM').textContent = pwmPercent + '%';
                document.getElementById('fanPWMValue').textContent = pwmPercent + '%';

                // Update timer display and button state
                const totalSec = Math.floor(data.timerMs / 1000);
                const hrs = Math.floor(totalSec / 3600);
                const mins = Math.floor((totalSec % 3600) / 60);
                const secs = totalSec % 60;
                document.getElementById('timerDisplay').textContent =
                    `${hrs.toString().padStart(2,'0')}:${mins.toString().padStart(2,'0')}:${secs.toString().padStart(2,'0')}`;

                // Update timer button state based on actual timer status
                // Timer is running if we have a non-zero timer value or if it's incrementing
                const isRunning = totalSec > 0;
                timerRunning = isRunning;
                document.getElementById('timerStartBtn').textContent = timerRunning ? 'Stop' : 'Start';

                // Update PID values only on initial load (not every update)
                if (!pidLoaded && data.Kp !== undefined) {
                    document.getElementById('kpInput').value = data.Kp;
                    document.getElementById('kiInput').value = data.Ki;
                    document.getElementById('kdInput').value = data.Kd;
                    pidLoaded = true;
                }

                // Update calibration offset values only on initial load (not every update)
                if (!calibrationLoaded && data.smokerOffset !== undefined && data.foodOffset !== undefined) {
                    document.getElementById('smokerOffsetInput').value = data.smokerOffset;
                    document.getElementById('foodOffsetInput').value = data.foodOffset;
                    calibrationLoaded = true;
                }

                // Update temperature history and redraw chart
                if (data.smokerHistory && data.foodHistory) {
                    smokerHistory = data.smokerHistory;
                    foodHistory = data.foodHistory;
                    drawChart();
                }

                // Update session recording state from server
                if (data.sessionRecording !== undefined) {
                    const wasRecording = sessionRecording;
                    sessionRecording = data.sessionRecording;

                    if (sessionRecording && !wasRecording) {
                        // Just started recording - sync UI
                        sessionStartTime = Date.now();
                        activeSessionId = data.activeSessionId;
                        updateRecordingUI(true, data.activeSessionName || 'Active Session');
                    } else if (!sessionRecording && wasRecording) {
                        // Just stopped recording - sync UI
                        updateRecordingUI(false);
                        loadSessions();  // Refresh session list
                    }
                }
            }

            // Handle config response
            if (data.config) {
                document.getElementById('kpInput').value = data.config.Kp;
                document.getElementById('kiInput').value = data.config.Ki;
                document.getElementById('kdInput').value = data.config.Kd;
            }
        }

        function sendCommand(cmd) {
            if (ws && ws.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({command: cmd}));
            }
        }

        function adjustTemperature(delta) {
            userSetTemp += delta;
            // Clamp between reasonable limits
            if (userSetTemp < 150) userSetTemp = 150;
            if (userSetTemp > 500) userSetTemp = 500;
            document.getElementById('setTempInput').textContent = userSetTemp;
            // Mark that user has pending changes (not yet sent to controller)
            userSetTempPending = true;
        }

        function setTemperature() {
            sendCommand(`SET_TEMP:${userSetTemp}`);
            // Clear pending flag - changes have been sent
            userSetTempPending = false;
        }

        function toggleFan() {
            const fanOn = document.getElementById('fanSwitch').checked;
            sendCommand(`FAN:${fanOn ? 'ON' : 'OFF'}`);
        }

        function timerControl(action) {
            sendCommand(`TIMER:${action}`);
            if (action === 'START') {
                timerRunning = !timerRunning;
                document.getElementById('timerStartBtn').textContent = timerRunning ? 'Stop' : 'Start';
                if (!timerRunning) {
                    sendCommand('TIMER:STOP');
                }
            }
        }

        function setPID() {
            const kp = document.getElementById('kpInput').value;
            const ki = document.getElementById('kiInput').value;
            const kd = document.getElementById('kdInput').value;
            sendCommand(`PID:${kp}:${ki}:${kd}`);
        }

        function getConfig() {
            sendCommand('GET_CONFIG');
        }

        function saveCalibration() {
            const smokerOffset = parseInt(document.getElementById('smokerOffsetInput').value) || 0;
            const foodOffset = parseInt(document.getElementById('foodOffsetInput').value) || 0;

            fetch('/api/calibration', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({smokerOffset: smokerOffset, foodOffset: foodOffset})
            })
            .then(response => response.json())
            .then(data => {
                if (data.status === 'ok') {
                    alert('Calibration saved! Smoker: ' + data.smokerOffset + '°F, Food: ' + data.foodOffset + '°F');
                } else {
                    alert('Error saving calibration');
                }
            })
            .catch(err => {
                alert('Error saving calibration: ' + err);
            });
        }

        function saveWiFi() {
            const ssid = document.getElementById('wifiSSID').value;
            const password = document.getElementById('wifiPassword').value;
            const hostname = document.getElementById('wifiHostname').value;

            fetch('/wifi', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({ssid: ssid, password: password, hostname: hostname})
            })
            .then(response => response.json())
            .then(data => {
                alert(data.message || 'WiFi settings saved');
            })
            .catch(err => {
                alert('Error saving WiFi settings');
            });
        }

        function toggleCollapse(element) {
            element.classList.toggle('active');
            const content = element.nextElementSibling;
            content.classList.toggle('show');
        }

        function scrollToWiFiSettings() {
            const wifiCard = document.querySelector('.card:has(.collapsible)');
            const wifiCollapsible = document.querySelector('.card .collapsible');

            // Expand the WiFi settings section if not already expanded
            if (!wifiCollapsible.classList.contains('active')) {
                toggleCollapse(wifiCollapsible);
            }

            // Scroll to the WiFi settings card
            wifiCard.scrollIntoView({ behavior: 'smooth', block: 'start' });
        }

        function checkWiFiStatus() {
            fetch('/wifi/status')
                .then(response => response.json())
                .then(data => {
                    // Populate hostname field if available
                    if (data.hostname) {
                        document.getElementById('wifiHostname').value = data.hostname;
                    }

                    if (data.apMode) {
                        // Show WiFi setup banner
                        document.getElementById('wifiSetupBanner').style.display = 'block';
                        document.getElementById('wifiInfoBanner').style.display = 'none';
                    } else if (data.connected) {
                        // Show connected info banner
                        document.getElementById('wifiSetupBanner').style.display = 'none';
                        document.getElementById('wifiInfoBanner').style.display = 'block';
                        document.getElementById('connectedSSID').textContent = data.ssid;
                    }
                })
                .catch(err => {
                    console.error('Error checking WiFi status:', err);
                });
        }

        // ==================== Session Recording Functions ====================

        function handleMeatTypeChange() {
            const select = document.getElementById('meatType');
            const customWrapper = document.getElementById('customMeatWrapper');
            customWrapper.style.display = select.value === 'Other' ? 'block' : 'none';
        }

        function getMeatType() {
            const select = document.getElementById('meatType');
            if (select.value === 'Other') {
                return document.getElementById('customMeatType').value || 'Other';
            }
            return select.value;
        }

        function loadSessions() {
            fetch('/api/sessions')
                .then(response => response.json())
                .then(data => {
                    renderSessionList(data.sessions || []);
                })
                .catch(err => {
                    console.error('Failed to load sessions:', err);
                    document.getElementById('sessionList').innerHTML =
                        '<p style="color:#e74c3c; text-align:center;">Failed to load sessions</p>';
                });
        }

        function renderSessionList(sessions) {
            const container = document.getElementById('sessionList');
            if (sessions.length === 0) {
                container.innerHTML = '<p style="color:#666; text-align:center;">No recorded sessions yet</p>';
                return;
            }

            // Sort by start time descending (newest first)
            sessions.sort((a, b) => b.start - a.start);

            container.innerHTML = sessions.map(s => `
                <div class="session-item" onclick="openSessionDetail(${s.id})">
                    <div>
                        <div class="session-name">${escapeHtml(s.name)}${s.active ? ' <span style="color:#e74c3c;">(Recording)</span>' : ''}</div>
                        <div class="session-meta">${escapeHtml(s.meat)} &bull; ${formatDuration(s.duration)}</div>
                    </div>
                    <div style="text-align:right;">
                        <div style="font-weight:600;">${s.setTemp}&deg;F</div>
                        <div class="session-meta">${formatDate(s.start)}</div>
                    </div>
                </div>
            `).join('');
        }

        async function startSession() {
            const name = document.getElementById('sessionName').value || 'Unnamed Session';
            const meat = getMeatType();

            try {
                const response = await fetch('/api/sessions', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({
                        name: name,
                        meat: meat,
                        setTemp: userSetTemp,
                        timestamp: Math.floor(Date.now() / 1000)
                    })
                });

                if (response.ok) {
                    sessionStartTime = Date.now();
                    updateRecordingUI(true, name);
                    document.getElementById('sessionName').value = '';
                    document.getElementById('meatType').value = 'Beef Brisket';
                    document.getElementById('customMeatWrapper').style.display = 'none';
                    document.getElementById('customMeatType').value = '';
                } else {
                    const err = await response.json();
                    alert(err.error || 'Failed to start session');
                }
            } catch (err) {
                alert('Error: ' + err.message);
            }
        }

        async function stopSession() {
            if (!confirm('Stop recording this session?')) return;

            try {
                const response = await fetch('/api/sessions/active', {method: 'DELETE'});
                if (response.ok) {
                    updateRecordingUI(false);
                    loadSessions();
                } else {
                    const err = await response.json();
                    alert(err.error || 'Failed to stop session');
                }
            } catch (err) {
                alert('Error stopping session: ' + err.message);
            }
        }

        async function addSessionNote(btn) {
            const noteText = document.getElementById('sessionNote').value.trim();
            if (!noteText) return;

            try {
                const response = await fetch('/api/note', {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({note: noteText})
                });

                if (response.ok) {
                    document.getElementById('sessionNote').value = '';
                    // Brief visual feedback
                    const origText = btn.textContent;
                    btn.textContent = 'Added!';
                    setTimeout(() => btn.textContent = origText, 1000);
                } else {
                    const err = await response.json();
                    alert(err.error || 'Failed to add note');
                }
            } catch (err) {
                alert('Error adding note: ' + err.message);
            }
        }

        async function openSessionDetail(id) {
            currentViewSessionId = id;

            try {
                const response = await fetch(`/api/session/details?id=${id}`);
                const session = await response.json();

                if (session.error) {
                    alert(session.error);
                    return;
                }

                // Populate modal
                document.getElementById('modalSessionName').textContent = session.name;
                document.getElementById('modalMeatType').textContent = session.meat;
                document.getElementById('modalSetTemp').textContent = session.setTemp;
                document.getElementById('modalDuration').textContent = formatDuration(session.duration);
                document.getElementById('modalDate').textContent = formatDate(session.start);

                // Temperature changes
                const tempChanges = (session.events || []).filter(e => e.e === 1);
                if (tempChanges.length > 0) {
                    document.getElementById('modalTempChanges').innerHTML = tempChanges.map(e =>
                        `<div class="event-item">Changed to ${e.st}&deg;F at ${formatTime(e.t)}</div>`
                    ).join('');
                } else {
                    document.getElementById('modalTempChanges').innerHTML =
                        '<p style="color:#666; font-size:0.9em;">No temperature changes recorded</p>';
                }

                // Notes
                if (session.notes && session.notes.length > 0) {
                    document.getElementById('modalNotes').innerHTML = session.notes.map(n =>
                        `<div class="note-item"><strong>${formatTime(n.t)}:</strong> ${escapeHtml(n.text)}</div>`
                    ).join('');
                } else {
                    document.getElementById('modalNotes').innerHTML =
                        '<p style="color:#666; font-size:0.9em;">No notes recorded</p>';
                }

                // Show modal
                document.getElementById('sessionModal').style.display = 'block';

            } catch (err) {
                alert('Error loading session: ' + err.message);
            }
        }

        function closeSessionModal() {
            document.getElementById('sessionModal').style.display = 'none';
            currentViewSessionId = null;
        }

        async function loadSessionSettings() {
            if (!currentViewSessionId) return;

            try {
                const response = await fetch(`/api/session/load?id=${currentViewSessionId}`, {
                    method: 'POST'
                });

                if (response.ok) {
                    const data = await response.json();
                    alert(`Loaded set temperature: ${data.setTemp}\u00B0F`);
                    closeSessionModal();
                } else {
                    const err = await response.json();
                    alert(err.error || 'Failed to load session');
                }
            } catch (err) {
                alert('Error loading session: ' + err.message);
            }
        }

        async function deleteSession() {
            if (!currentViewSessionId) return;
            if (!confirm('Delete this session permanently?')) return;

            try {
                const response = await fetch(`/api/session/delete?id=${currentViewSessionId}`, {
                    method: 'DELETE'
                });

                if (response.ok) {
                    closeSessionModal();
                    loadSessions();
                } else {
                    const err = await response.json();
                    alert(err.error || 'Failed to delete session');
                }
            } catch (err) {
                alert('Error deleting session: ' + err.message);
            }
        }

        function updateRecordingUI(isRecording, sessionName = '') {
            sessionRecording = isRecording;
            document.getElementById('startSessionForm').style.display = isRecording ? 'none' : 'block';
            document.getElementById('activeSessionControls').style.display = isRecording ? 'block' : 'none';
            document.getElementById('recordingIndicator').classList.toggle('active', isRecording);
            document.getElementById('recordingText').textContent = isRecording ? 'Recording' : 'Not Recording';

            if (isRecording && sessionName) {
                document.getElementById('activeSessionName').textContent = sessionName;
            }
        }

        function updateSessionDuration() {
            if (!sessionRecording || !sessionStartTime) return;
            const elapsed = Math.floor((Date.now() - sessionStartTime) / 1000);
            document.getElementById('activeSessionDuration').textContent = formatDurationHMS(elapsed);
        }

        // Helper functions
        function formatDuration(seconds) {
            if (!seconds || seconds === 0) return '0m';
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            return h > 0 ? `${h}h ${m}m` : `${m}m`;
        }

        function formatDurationHMS(seconds) {
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            const s = seconds % 60;
            return `${h.toString().padStart(2, '0')}:${m.toString().padStart(2, '0')}:${s.toString().padStart(2, '0')}`;
        }

        function formatTime(seconds) {
            const h = Math.floor(seconds / 3600);
            const m = Math.floor((seconds % 3600) / 60);
            if (h > 0) return `${h}h ${m}m`;
            return `${m}m`;
        }

        function formatDate(timestamp) {
            if (!timestamp) return '-';
            return new Date(timestamp * 1000).toLocaleDateString();
        }

        function escapeHtml(text) {
            if (!text) return '';
            const div = document.createElement('div');
            div.textContent = text;
            return div.innerHTML;
        }

        // Close modal when clicking outside
        window.addEventListener('click', (e) => {
            const modal = document.getElementById('sessionModal');
            if (e.target === modal) {
                closeSessionModal();
            }
        });

        // Update session duration every second
        setInterval(updateSessionDuration, 1000);

        // Initialize on page load
        window.addEventListener('load', () => {
            initChart();
            connect();
            checkWiFiStatus();
            loadSessions();
        });

        // Handle window resize
        window.addEventListener('resize', () => {
            initChart();
            drawChart();
        });
    </script>
</body>
</html>
)rawliteral";

#endif // WEBPAGE_H
