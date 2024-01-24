###########################################################################
## Authors: Costa Filippo
## Description: Build information for the ADM project
###########################################################################

# Use the bci2000_include macro if you need to link with frameworks from /src/extlib:
# bci2000_include( MATLAB )

bci2000_add_signal_processing_module(
  ADM NOBUNDLE
  PipeDefinition.cpp
  ADMFilter.cpp
  FIRFilter.cpp
  TransmissionFilterDIFF.cpp
)
