# esp32-heart-pair
All the files for the esp32 heart pair project.

At any given time, we can assume that at most only 1 pair is connected to the server. Thus, when one device connects to a server, the server will automatically pair it with the next device that connects to the server. We assume that device-level programs have no lag, but that device-server messages will.

### Components
* Heart Device A
  * Display paired BPM & mood with pulses
  * Replicate beat with one high intensity, one low intensity
  * Beat color is pink by default, changes based on opposing mood
  * Timed based on known latency and last beat time
* Heart Device B
* Trigger App
  * Detect two volume-up as STOP
  * Detect two volume-down as START
* Configuration App A
* Configuration App B
* Handler Server
  * Get ping from device return messages, filter with weighted average
    * Keep a hashmap of sent update UUIDs and their respective send times, throw out any that are too old
    * When a GOT_UPDATE message is received, find the UUID and the send time and use receive time to update the ping

### Messages
* Trigger: Trigger -> Server
  * Message Type (TRIGGER)
  * Message Data (None)
* Stop: Trigger -> Server
  * Message Type (STOP)
  * Message Data (None)

* Server Update: Server -> Device
  * Message Type (SERVER_UPDATE)
  * Message Data
    * Update UUID
    * Measured BPM (optional)
    * Time from last beat peak
    * Trigger State (optional)
    * Estimated total latency

* Client Verification Ping: Device -> Server
  * Message Type (GOT_UPDATE)
  * Message Data
    * Update Message UUID
* Client Update: Device -> Server
  * Message Type (CLIENT_UPDATE)
  * Message Data
    * Measured BPM (optional)
    * Time from last beat peak
    
