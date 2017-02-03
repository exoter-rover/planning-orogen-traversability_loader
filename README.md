# Traversability Loader

## A component for loading of a traversability cost map from a file.

This component loads a traversability cost map from a file. 
It assumes that each cell contains a SBPL-compatible cost. 
The values are converted to drivability and reprsented using the Envire library.
The whole map is loaded at once and outputted during a single update.

An examle of the data format can be found in `data/`.  
Details on input, output and triggering can be found in [`.orogen`](https://github.com/exoter-rover/planning-orogen-traversability_loader/blob/master/traversability_loader.orogen) file.


**Author: [Jan Filip](mailto:jan.filip2@gmail.com "Contact the author"),  
Contact: [Martin Azkarate](mailto:Martin.Azkarate@esa.int "Contact the maintainer"),  
Affiliation: Automation and Robotics Laboratories, ESTEC, ESA**

