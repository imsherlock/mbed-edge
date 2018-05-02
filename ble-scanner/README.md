## The protocol translator example

<span class="warnings">**Warning:** This is an example to demonstrate the protocol
translator client. Do not use for production implementation.</span>

The `pt-example` is a simple protocol translator implementation. It is
implemented as a generic example to show how to use the protocol translator
client API.

### Operation description

This example creates three mediated devices and registers then to
Mbed Cloud. There are two temperature sensors and one thermometer. The
`cpu-temperature` device is created only if the underlying operating system
provides CPU temperature values from the `/sys/class/thermal/thermalzone[N]` file.
The reset operation is supported for the `cpu-temperature` and it can be
triggered by sending a `POST` operation from Mbed Cloud to that resource. Also
the value resources of the `cpu-temperature` can be observed from Mbed Cloud
and you can see the value updates happening on those resources.

The `thermostat-0` supports the `WRITE` operation on the `Current value`
resource. You can send the value update by `PUT` operation from Mbed Cloud.

| Sensor            | Resource      | Object id | Instance id | Resource id |
|-------------------|---------------|-----------|-------------|-------------|
| cpu-temperature-0 | Min measured  | 3303      | 0           | 5601        |
| cpu-temperature-0 | Max measured  | 3303      | 0           | 5602        |
| cpu-temperature-0 | Reset min/max | 3303      | 0           | 5605        |
| cpu-temperature-0 | Current value | 3303      | 0           | 5700        |
| cpu-temperature-0 | Sensor units  | 3303      | 0           | 5701        |
| thermometer-0     | Current value | 3303      | 0           | 5700        |
| thermometer-0     | Sensor units  | 3303      | 0           | 5701        |
| thermostat-0      | Sensor units  | 3308      | 0           | 5701        |
| thermostat-0      | Current value | 3308      | 0           | 5900        |

### Running

Start edge-core:

```
$ ./edge-core
```

Start the lorapt-example and give the mandatory protocol translator name option:

```
$ ./pt-example -n pt-example
```

On Mbed Cloud, you should see the mediated endpoints appear as new devices and
they should have the sensors as resources, two temperature sensors and
a thermostat sensor.

The `pt-example` supports optional command-line parameters, for example to set the port
and hostname of Edge Core. For help, use:

```
$ ./pt-example --help
```
