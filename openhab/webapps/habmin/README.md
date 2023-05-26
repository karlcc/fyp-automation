Overview
--------
HABmin is a web administration console for openHAB. It aims to provide a complete interface to administer openHAB, including the following features -:
* General configuration (openHAB.cfg)
* Configure bindings
* Configure items
* Configure mapping
* Configure sitemaps
* Configure rules and notifications
* Query and graph data from persistence stores
* View OSGi binding status
* View log files

The interface is a modern browser based system providing point and click, and drag and drop input. As features are added, the wiki is being updated - please take a look.


![Item Config Screen](https://raw.github.com/wiki/cdjackson/HABmin/habmin_itemconfig.png)

HABmin interfaces to OpenHAB through the REST interface.  In its current state, the openHAB REST interfaces do not support configuration and as such work with HABmin is inevitably closely linked to the definition of these interfaces within the openHAB project.

![Sitemap Config Screen](https://raw.github.com/wiki/cdjackson/HABmin/habmin_sitemap.png)

In addition to the REST interface, it is imperative to define files that describe the configurable features of OpenHAB. These files describe the configuration required for a binding or an item and are used within openHAB and exposed through the REST interface. The files are defined in XML format and are not directly accessed by HABmin.


![Graph Screen](https://raw.github.com/wiki/cdjackson/HABmin/habmin_graph.png)

While HABmin is a supporting project to OpenHAB,  providing access to OpenHAB's features, since the existing REST interface does not support most of the functionality required by HABmin, HABmin may drive this part of openHAB to some extent. It is also expected that as functionality is added to OpenHAB,  HABmin will need to have its backend modified to reflect the final interfaces implemented in OpenHAB. 

![Binding Config Screen](https://raw.github.com/wiki/cdjackson/HABmin/habmin_bindingconfig.png)


###Status
The project is just getting started. Currently implemented are the following -:
* Graphing of data from the persistence store
* Item editor
* Sitemap editor
* General binding configuration (ie binding configuration in the openhab.cfg file)
* OSGI bundle status viewer
* Item rule library (initial test phase)

Additionally, lot of the initial user interface has been boilerplated and some work has started on the REST interface for configuring bindings.

Technology
----------
HABmin is an open source project. It makes use of a number of libraries under GPL license. The following major libraries are used -:
* ExtJS from Sencha
* Highcharts from Highsoft

![Bundles Screen](https://raw.github.com/wiki/cdjackson/HABmin/habmin_systembundles.png)

Installation
------------
* Download the project zip file from GitHub and unzip files in the directory webapps/habmin (you will need to create this directory).
* Place the org.openhab.io.rest.*.jar file into the addons directory (this is stored in the addons directory in the repository).
* Remove the existing jar file starting with org.openhab.io.rest from the system/plugins directory.

Optionally, in order to get the graphing to work, you need to also install a new core.persistence JAR. This includes a method to return data from a persistence query. This should work with any queryable persistence service, although I've only tried it with mySQL.
* Place the org.openhab.core.persistence.*.jar file into the addons directory (this is stored in the addons directory in the repository).
* Remove the existing jar file starting with org.openhab.core.persistence from the system/plugins directory.

You will probably need to restart OpenHAB for the new REST interface to take affect.

You can then start habmin at the address [http://localhost:8080/habmin/index.html](http://localhost:8080/habmin/index.html) (assuming openHAB is running on your local computer using the default port - if this is not the case, you will need to adjust the address accordingly).


Contributing
------------
If you wish to help with this project, please feel free to clone the repository and work on some features. I would like to maintain a top level TODO/Issues list which lists the main features that require work. Please feel free to add to this list, or discuss implementation issues within the issue. If you are going to work on a feature please make it known so we can avoid duplication.
