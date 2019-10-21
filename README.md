# ROOT View (Working title)
## Description
This is a program which provides a graphical user interface for analyzing data using ROOT. In particular, this is modeled to perform online analysis with a secondary thread managing the dataparsing. The style of the GUI is modeled after NSCL SpecTcl. More work to come on the format of the data parsing (standard will be assumed NSCLDAQ).

## Method
The GUI is built using methods outlined in https://root.cern.ch/root/htmldoc/guides/users-guide/WritingGUI.html

Information linking histograms to pads, histogram name to histogram info, cuts to histograms, etc. is stored locally using std::unordered_map. The unordered version of map is used since order is not required and speed is desired. These maps are then accessed at various points to update, add, and remove information.

### Drawing
Drawing is achieved by locking the threads and calling the draw function of the converter and sending the information stored in the histogram map. The converter then uses this infomation to create a histogram without drawing (since the converter has no access to the embedded pad); this histogram is pulled from the gPad and sent back to the gui where it is drawn to the embedded canvas and the map is updated with a new histogram. There are several problems with this method: tree drawing time and conditional updating. If a large number of cuts are used on a signficantly large tree, the time to draw a new histogram becomes prohibitive using this method. This could be a significant area of improvement (maybe threaded histograms? but then how to make easily editable?). Another area of potential improvement would be developing a method which puts updating on a time limit; if the user is only allowed to perfom an update after a specified time interval, this could avoid some of the draw time problems (but not solve them).

### Cutting
Cuts can be made in the pad for both 1D and 2D histograms. Cuts are then stored as objects by the GUI and sent as TCut objects (regardless of whether the cut is a TCutG or a TCut) to the converter for use at draw time. The reason for this is that drawing from the tree only requires a TCut and not a TCutG so long as the TCutG is created in the same gROOT and branch variables areproperly set for the tree.

### Saving and Loading Info
The information to recreate the configuration of the maps and tree is stored in a .txt file. Using the save button prompts for a name for the .txt file, and then parses the information in the maps. Information is loaded into the maps by using the load button, which prompts the user for a file name. The maps are then wiped (so that the configuration may be changed after a configuration is already loaded) and filled information obtained from the .txt file.

### Handling spectra
Spectra may be created and overwritten, currently deletion is not implemented. A list of the histograms, with the basic info associated with them attached, can be brought up with a list viewer through the view menu. Similarly a list of cuts with their associated spectra via the view menu. Up to 16 spectra can be displayed at given time in the embedded canvas, where the number of spectra is toggled using the rows and columns buttons located on the main window or through the view menu. 

## Use
A Makefile is included for ease of compliation. To build the program use make; to clean the program use make clean. Execution is: ./RootView

## Requirements
ROOT 6 is required along with at least C++ 11.
