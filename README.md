Simplicity
======

A simple event driven simulation framework.

## How to

Download Simplicity framework and include the Makefile.inc in your project
Makefile:

```
#include SIMPLICITY_PATH/Makefile.inc
```

Check the Makefile.inc to know what variables are used.

Include the framework in your header files:

```
#include "Simplicity.hh"
```

Then, inherit from Module and implement the following pure virtual methods:

```

class MyModule : public Module {

  ...

  virtual void handleEvent(Event *ev);

  virtual void initialize(int stage);

  virtual void finalize(void);

  ...
};

```

Also register the modules in your source files:

```
RegisterModule(MyModule);
```

Finally, configure the simulation. Write the required config.json and init.json
files in your project's folder: define the simulation domain in the config.json, then initialize module parameters in the init.json.

To run the simulation:

```
./build/target -c config.json
```

Check the examples for available parameters and details on how to configure and use the framework.
