# Compiler project

## Team members
- AL JAMOUS Myriam
- BOUSLIMI Roua
- CHEVALIER Mathieu
- GUEDDOUDA Bachir
- MAISON Benjamin 
- SERPINET Gaspard
- SUDLOW James

> Flat and agile management, we all contributed to the project.

## Prerequisites
Before building your  compiler, you should install  antlr using your distribution's  package manager,  or by  running the  provided shell script: `install-antlr.sh`. 

`Makefile` contains the actual build logic. The Makefile includes a .mk file that defines several variables (ANTLR, ANTLRJAR, ANTLRINC and  ANTLRLIB) indicating the location of various parts of the Antlr suite. You can (should) change those values to suit your installation, either by editing the Makefile and/or adding another .mk file.

## User documentation
UPDATE DOC AND PUT HERE

## Developper documention
To generate the developper documentation, you should have `doxygen` installed. See https://www.doxygen.nl/download.html for installation.

Then in the `compiler` folder run : `make doc`

This will open the doxygen documentation in the browser.
