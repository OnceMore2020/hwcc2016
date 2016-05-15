Graph Generator
===============

## Used For

Testcase generator.

## Usage

```bash
$ ./generate_graph release/topo0.csv release/demand0.csv release/reference.csv
```

|Parameter|File|
|:--------|:---|
|`sys.argv[1]`|Topo file of the graph|
|`sys.argv[2]`|Demand file of the testcase|
|`sys.argv[3]`|Reference Result file|
