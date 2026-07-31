# API Reference — PIFKID 2026 Smart Desk Clock

**Base URL:** `http://<esp32-ip>:80`
**Format:** JSON
**Content-Type:** `application/json`

---

## Response Format

All responses follow this structure:

```json
{
  "success": true,
  "code": 200,
  "message": "Description",
  "data": {},
  "timestamp": 1234567890
}
```

### Error Response
```json
{
  "success": false,
  "code": 400,
  "message": "Invalid request",
  "data": null,
  "timestamp": 1234567890
}
```

---

## Endpoints

### System

#### GET /api/status
Returns system status and free memory.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Status retrieved",
  "data": {
    "wifi": { "connected": true, "ssid": "MyWiFi", "rssi": -45 },
    "memory": { "freeHeap": 180000, "minFreeHeap": 150000 },
    "uptime": 123456,
    "firmware": "1.0.0",
    "wifiMode": 1
  },
  "timestamp": 1234567890
}
```

---

#### GET /api/time
Returns current time.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Time retrieved",
  "data": {
    "year": 2026,
    "month": 7,
    "day": 28,
    "hour": 14,
    "minute": 30,
    "second": 45,
    "ntpSynced": true,
    "ntpServer": "pool.ntp.org"
  },
  "timestamp": 1234567890
}
```

---

#### POST /api/time
Set manual time.

**Request:**
```json
{
  "year": 2026,
  "month": 7,
  "day": 28,
  "hour": 14,
  "minute": 30,
  "second": 0
}
```

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Time set successfully",
  "data": null,
  "timestamp": 1234567890
}
```

---

### Todo

#### GET /api/todo
List all todos.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Todos retrieved",
  "data": [
    {
      "id": 1,
      "title": "Buy groceries",
      "description": "Milk, eggs, bread",
      "color": "#FF5733",
      "completed": false,
      "createdAt": 1234567890
    }
  ],
  "timestamp": 1234567890
}
```

---

#### POST /api/todo
Create a new todo.

**Request:**
```json
{
  "title": "Buy groceries",
  "description": "Milk, eggs, bread",
  "color": "#FF5733"
}
```

**Response:**
```json
{
  "success": true,
  "code": 201,
  "message": "Todo created",
  "data": {
    "id": 1,
    "title": "Buy groceries",
    "description": "Milk, eggs, bread",
    "color": "#FF5733",
    "completed": false,
    "createdAt": 1234567890
  },
  "timestamp": 1234567890
}
```

---

#### GET /api/todo/:id
Get a specific todo.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Todo retrieved",
  "data": {
    "id": 1,
    "title": "Buy groceries",
    "description": "Milk, eggs, bread",
    "color": "#FF5733",
    "completed": false,
    "createdAt": 1234567890
  },
  "timestamp": 1234567890
}
```

---

#### PUT /api/todo/:id
Update a todo.

**Request:**
```json
{
  "title": "Buy groceries (updated)",
  "completed": true
}
```

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Todo updated",
  "data": {
    "id": 1,
    "title": "Buy groceries (updated)",
    "description": "Milk, eggs, bread",
    "color": "#FF5733",
    "completed": true,
    "createdAt": 1234567890
  },
  "timestamp": 1234567890
}
```

---

#### DELETE /api/todo/:id
Delete a todo.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Todo deleted",
  "data": null,
  "timestamp": 1234567890
}
```

---

### Alarm

#### GET /api/alarm
List all alarms.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Alarms retrieved",
  "data": [
    {
      "id": 1,
      "hour": 7,
      "minute": 30,
      "repeatDays": [false, true, true, true, true, true, false],
      "enabled": true,
      "sound": "beep",
      "volume": 80
    }
  ],
  "timestamp": 1234567890
}
```

---

#### POST /api/alarm
Create a new alarm.

**Request:**
```json
{
  "hour": 7,
  "minute": 30,
  "repeatDays": [false, true, true, true, true, true, false],
  "enabled": true,
  "sound": "beep",
  "volume": 80
}
```

**Response:**
```json
{
  "success": true,
  "code": 201,
  "message": "Alarm created",
  "data": {
    "id": 1,
    "hour": 7,
    "minute": 30,
    "repeatDays": [false, true, true, true, true, true, false],
    "enabled": true,
    "sound": "beep",
    "volume": 80
  },
  "timestamp": 1234567890
}
```

---

#### GET /api/alarm/:id
Get a specific alarm.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Alarm retrieved",
  "data": {
    "id": 1,
    "hour": 7,
    "minute": 30,
    "repeatDays": [false, true, true, true, true, true, false],
    "enabled": true,
    "sound": "beep",
    "volume": 80
  },
  "timestamp": 1234567890
}
```

---

#### PUT /api/alarm/:id
Update an alarm.

**Request:**
```json
{
  "hour": 8,
  "minute": 0,
  "enabled": false
}
```

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Alarm updated",
  "data": {
    "id": 1,
    "hour": 8,
    "minute": 0,
    "repeatDays": [false, true, true, true, true, true, false],
    "enabled": false,
    "sound": "beep",
    "volume": 80
  },
  "timestamp": 1234567890
}
```

---

#### DELETE /api/alarm/:id
Delete an alarm.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Alarm deleted",
  "data": null,
  "timestamp": 1234567890
}
```

---

### Schedule

#### GET /api/schedule
List all schedule entries.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Schedule retrieved",
  "data": [
    {
      "id": 1,
      "day": 1,
      "startTime": "09:00",
      "endTime": "17:00",
      "title": "Work",
      "color": "#3498DB"
    }
  ],
  "timestamp": 1234567890
}
```

---

#### POST /api/schedule
Create a schedule entry.

**Request:**
```json
{
  "day": 1,
  "startTime": "09:00",
  "endTime": "17:00",
  "title": "Work",
  "color": "#3498DB"
}
```

**Response:**
```json
{
  "success": true,
  "code": 201,
  "message": "Schedule entry created",
  "data": {
    "id": 1,
    "day": 1,
    "startTime": "09:00",
    "endTime": "17:00",
    "title": "Work",
    "color": "#3498DB"
  },
  "timestamp": 1234567890
}
```

---

#### GET /api/schedule/:id
Get a specific schedule entry.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Schedule entry retrieved",
  "data": {
    "id": 1,
    "day": 1,
    "startTime": "09:00",
    "endTime": "17:00",
    "title": "Work",
    "color": "#3498DB"
  },
  "timestamp": 1234567890
}
```

---

#### PUT /api/schedule/:id
Update a schedule entry.

**Request:**
```json
{
  "title": "Work (updated)",
  "endTime": "18:00"
}
```

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Schedule entry updated",
  "data": {
    "id": 1,
    "day": 1,
    "startTime": "09:00",
    "endTime": "18:00",
    "title": "Work (updated)",
    "color": "#3498DB"
  },
  "timestamp": 1234567890
}
```

---

#### DELETE /api/schedule/:id
Delete a schedule entry.

**Response:**
```json
{
  "success": true,
  "code": 200,
  "message": "Schedule entry deleted",
  "data": null,
  "timestamp": 1234567890
}
```

---

## Error Codes

| Code | Description |
|------|-------------|
| 200 | Success |
| 201 | Created |
| 400 | Bad request (invalid JSON, missing fields) |
| 404 | Resource not found |
| 500 | Internal server error |

---

## Frontend API Client

```javascript
// Base client usage
const response = await fetch('/api/todo', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ title: 'New todo' })
});

const data = await response.json();
if (data.success) {
  // Use data.data
} else {
  // Handle error: data.message
}
```

---

*End of API Reference*
