# Phase 5 — Stress Testing Procedures

## Prerequisites
- ESP32 flashed with firmware
- WiFi connected (STA mode)
- USB serial monitor at 115200 baud
- `curl` or REST client available

---

## ST-1: REST API Stress Test — 1000 Todo Operations

### Create 1000 Todos
```bash
#!/bin/bash
IP="192.168.1.100"
for i in $(seq 1 1000); do
  curl -s -X POST "http://$IP/api/todo" \
    -H "Content-Type: application/json" \
    -d "{\"title\":\"Todo $i\",\"description\":\"Stress test $i\",\"color\":\"#ff0000\"}" \
    > /dev/null
  if [ $((i % 100)) -eq 0 ]; then
    echo "Created $i todos"
    # Log heap
    curl -s "http://$IP/api/status" | grep -o '"heap":"[^"]*"'
  fi
done
echo "ST-1 COMPLETE"
```

**Measure:** Heap after each batch, free heap at end, any crashes
**Pass criteria:** No crash, heap >50KB free at end, all todos created

### Read 1000 Todos
```bash
for i in $(seq 1 1000); do
  curl -s "http://$IP/api/todo" > /dev/null
done
echo "ST-1 READ COMPLETE"
```

**Measure:** Average response time, heap stability

### Update 1000 Todos
```bash
for i in $(seq 1 1000); do
  curl -s -X PUT "http://$IP/api/todo/$i" \
    -H "Content-Type: application/json" \
    -d "{\"completed\":true}" \
    > /dev/null
done
echo "ST-1 UPDATE COMPLETE"
```

### Delete All Todos
```bash
for i in $(seq 1 1000); do
  curl -s -X DELETE "http://$IP/api/todo/$i" > /dev/null
done
echo "ST-1 DELETE COMPLETE"
```

---

## ST-2: REST API Stress Test — 1000 Alarm Operations

```bash
#!/bin/bash
IP="192.168.1.100"
# Create 10 alarms (max), then update 1000 times
for i in $(seq 1 10); do
  curl -s -X POST "http://$IP/api/alarm" \
    -H "Content-Type: application/json" \
    -d "{\"hour\":$((i % 24)),\"minute\":0,\"repeat\":[0,1,2,3,4,5,6]}" \
    > /dev/null
done

# Update each alarm 100 times
for i in $(seq 1 100); do
  for j in $(seq 1 10); do
    curl -s -X PUT "http://$IP/api/alarm/$j" \
      -H "Content-Type: application/json" \
      -d "{\"enabled\":$([ $((i % 2)) -eq 0 ] && echo 'true' || echo 'false')}" \
      > /dev/null
  done
  if [ $((i % 10)) -eq 0 ]; then
    echo "Alarm updates: $((i * 10))"
    curl -s "http://$IP/api/status" | grep -o '"heap":"[^"]*"'
  fi
done
echo "ST-2 COMPLETE"
```

---

## ST-3: REST API Stress Test — 1000 Schedule Operations

```bash
#!/bin/bash
IP="192.168.1.100"
# Create 100 schedule entries
for i in $(seq 1 100); do
  curl -s -X POST "http://$IP/api/schedule" \
    -H "Content-Type: application/json" \
    -d "{\"day\":$((i % 7)),\"start\":\"09:00\",\"end\":\"17:00\",\"title\":\"Event $i\"}" \
    > /dev/null
done

# Update 1000 times across entries
for i in $(seq 1 1000); do
  idx=$(( (i % 100) + 1 ))
  curl -s -X PUT "http://$IP/api/schedule/$idx" \
    -H "Content-Type: application/json" \
    -d "{\"title\":\"Updated $i\"}" \
    > /dev/null
  if [ $((i % 100)) -eq 0 ]; then
    echo "Schedule updates: $i"
    curl -s "http$IP/api/status" | grep -o '"heap":"[^"]*"'
  fi
done
echo "ST-3 COMPLETE"
```

---

## ST-4: WiFi Reconnect Stress — 500 Cycles

```bash
#!/bin/bash
IP="192.168.1.100"
for i in $(seq 1 500); do
  # Disconnect via API (if available) or router
  curl -s -X POST "http://$IP/api/time" \
    -H "Content-Type: application/json" \
    -d '{"mode":"ntp"}' > /dev/null
  sleep 2
  # Check connectivity
  STATUS=$(curl -s -o /dev/null -w "%{http_code}" "http://$IP/api/status")
  echo "Cycle $i: HTTP $STATUS"
  if [ "$STATUS" != "200" ]; then
    echo "FAIL at cycle $i"
    break
  fi
done
echo "ST-4 COMPLETE"
```

---

## ST-5: Reboot Cycle Stress — 100 Reboots

```bash
#!/bin/bash
IP="192.168.1.100"
for i in $(seq 1 100); do
  # Trigger reboot via watchdog or power cycle
  # If using RST pin:
  # python -c "import serial; s=serial.Serial('COM3',115200); s.write(b'AT+RST\r\n')"
  
  # Or via API if reboot endpoint exists:
  # curl -s -X POST "http://$IP/api/device/reboot"
  
  echo "Reboot $i — waiting for boot..."
  sleep 10
  
  # Check if device is up
  STATUS=$(curl -s -o /dev/null -w "%{http_code}" "http://$IP/api/status")
  if [ "$STATUS" != "200" ]; then
    echo "FAIL: Device not responding after reboot $i"
    break
  fi
  
  # Check filesystem integrity
  curl -s "http://$IP/api/todo" > /dev/null
  curl -s "http$IP/api/alarm" > /dev/null
  curl -s "http://$IP/api/schedule" > /dev/null
  
  echo "Reboot $i: OK"
done
echo "ST-5 COMPLETE"
```

---

## ST-6: 24-Hour Runtime Test

```bash
#!/bin/bash
IP="192.168.1.100"
START=$(date +%s)
END=$((START + 86400))  # 24 hours

while [ $(date +%s) -lt $END ]; do
  # Query status every 60 seconds
  STATUS=$(curl -s "http://$IP/api/status")
  HEAP=$(echo $STATUS | grep -o '"heap":"[^"]*"' | cut -d'"' -f4)
  UPTIME=$(echo $STATUS | grep -o '"uptime":[0-9]*' | cut -d: -f2)
  echo "$(date): heap=$HEAP uptime=${UPTIME}s"
  
  # Random API calls
  curl -s "http://$IP/api/todo" > /dev/null
  curl -s "http://$IP/api/time" > /dev/null
  
  sleep 60
done
echo "ST-6 COMPLETE — 24 hours"
```

**Measure:** Heap trend over 24h, uptime accuracy, any crashes

---

## ST-7: 72-Hour Runtime Test

Same as ST-6 but for 72 hours. Monitor:
- Heap trend (should be stable, not decreasing)
- Uptime accuracy
- Filesystem integrity (create/read/delete cycle every hour)
- WiFi stability (reconnect count)
- Display (no artifacts after 72h)

---

## ST-8: Concurrent Access Stress

```bash
#!/bin/bash
IP="192.168.1.100"
# Run 10 parallel clients making random API calls
for client in $(seq 1 10); do
  (
    for i in $(seq 1 100); do
      ENDPOINT=$(shuf -e /api/todo /api/alarm /api/schedule /api/status /api/time -n 1)
      curl -s "http://$IP$ENDPOINT" > /dev/null
    done
  ) &
done
wait
echo "ST-8 COMPLETE"
```

---

## Measurement Points

For all stress tests, record:

| Metric | How to Measure |
|--------|---------------|
| Heap (free) | `curl /api/status` → `heap` field |
| Heap (min free) | `ESP.getMinFreeHeap()` via logger |
| Fragmentation | `ESP.getHeapFragmentation()` via logger |
| Uptime | `/api/status` → `uptime` field |
| Watchdog resets | Count unexpected reboots in serial log |
| CPU load | Estimate from loop() cycle time |
| Frame time | `tftManager.getAverageFrameTime()` |
| API latency | Time curl requests |
| WiFi latency | Ping response time |
| Filesystem integrity | Read/write cycle after each test |
| Boot time | Time from power-on to first API response |

---

*End of Stress Testing Procedures*
