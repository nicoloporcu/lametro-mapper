# lametro-mapper

Collaborators: @nicoloporcu @maylisw

API documentation: [swiftly](https://swiftly-inc.stoplight.io/docs/realtime-standalone/YXBpOjI4NDM2MDU3-swiftly-api-reference)

## GTFS

[About GTFS](https://gtfs.org/realtime/)

[Google GTFS Specs](https://developers.google.com/transit/gtfs-realtime/)

[gtfs-realtime.proto](https://github.com/google/transit/blob/master/gtfs-realtime/proto/gtfs-realtime.proto)

`brew install protobuf`

```bash
protoc --proto_path=gtfs-realtime.proto --cpp_out=.
```

PCB / Visuals
- 1 LED per station per direction
- At NS & EW stations => 4
- 101 stations => ~220?

Scale?
- yes but no A line extension

What future projects are supported?
- D line extension
- K line

API Endpoints
- `GTFS-rt-alerts`: alerts for the system
- `GTFS-rt-vehicle-positions`: all vehicles in the system and their locations
- `GTFS-rt-trip-updates`: all stops and when the next vehicle is arriving/departing
  - probably best one to use


Strech Goals
- Use `GTFS-rt-alerts` to display system alerts
