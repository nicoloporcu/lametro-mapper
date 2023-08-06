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
