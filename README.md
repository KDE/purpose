# Purpose

Provides an abstraction to provide and leverage actions of a specific kind

## Introduction

Purpose offers the possibility to create integrate services and actions on
any application without having to implement them specifically. Purpose will
offer them mechanisms to list the different alternatives to execute given the
requested action type and will facilitate components so that all the plugins
can receive all the information they need.

## Usage

There's 2 main ways of using Purpose: from C++ and QML/QtQuick.

To import it from QML, import

    import org.kde.people 1.0

It offers different ways of integrating the system in an application. For full
control on the process, one can use the *AlternativesModel* for listing the
different possibilities and *PurposeWizard* for configuring the job and
*RunningJob* for a view that displays the job as it runs. There's no view
to display the result of the job as it depends on the context.

Furthermore, there's the *AlternativesView* component that will integrate all the
process defined below for convenience.


If you want to import right in the C++ application, you can import it on the
cmake scripts by calling:

    find_package(KDEExperimentalPurpose)

Or its qmake counterpart. Then you'll have available the Purpose library if it
needs to be done specifically and PurposeWidgets for convenience.

To integrate on the UI, QtQuick will still be used, as the configuration files
provided by the plugins are written in QML. The recommended way to integrate
on a QtWidgets interface is by using the *Purpose::Menu* class that will allow
us to place the integration wherever pleases us. This class will offer us
a pointer to the used *Purpose::AlternativesModel* so that we can specify what kind of
services we're interested in.

## Plugin Architecture

Every plugin will offer at least a `Purpose::PluginBase`.

There will be 2 files specifying the behavior of the plugin:
* The `*PluginType.json` files.
* The plugin metadata itself, which points to a plugin type.

The plugin type will be identified by the file name. It will specify:
* `X-Purpose-InboundArguments` defines the arguments the application must provide.
* `X-Purpose-OutboundArguments` defines the arguments the plugin must provide by
the end of the execution.

In the plugin metadata we will define:
* `X-Purpose-PluginTypes` defines the purposes tackled by the plugin
* `X-Purpose-Constraints` defines some conditions under the plugin is useful, considering
the provided inboundArguments. For example, the youtube export plugin will specify
`mimeType:video/*` as a constraint, because it's not interested in uploading images.
* `X-Purpose-Configuration` provides a list of extra arguments that the plugin will need.
Ideally everything should be in the plugin type but sometimes we can only wish. This allows
the opportunity to the application to let the user add the missing data.

### Plugin types
The application says what it wants to do, Purpose finds the correct plugins. This
is how we balance decoupling of implementation but keep on top of what the framework
is supposed to do.

An example of such files is the `ExportPluginType.json`:
```json
{
    "KPlugin": {
        "Icon": "edit-paste",
        "Name": "Upload..."
    },
    "X-Purpose-InboundArguments": [ "urls", "mimeType" ],
    "X-Purpose-OutboundArguments": [ "url" ]
}
```

As previously discussed, here we can define the generic tasks that the different
plugins will implement on top, having the inbound arguments as a given and the
outbound as a requirement.

Examples of such plugin types are (hypothetically, not all implemented yet):
* Share: where you can get the different services to share
* GetImage that would list your scanner, camera and also some web services.
* AddContact that would let you add a contact on your address book or
in whichever plugin is offered.
