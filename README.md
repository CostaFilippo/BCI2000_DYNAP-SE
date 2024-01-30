# BCI2000_DYNAP-SE
Code related to 'Robust compression and detection of epileptiform patterns in ECoG using a real-time spiking neural network hardware framework' https://doi.org/10.21203/rs.3.rs-3591328/v1

### System requirements

Windows (for BCI2000 code)

- BCI2000: 3.6.7010 
- Python: 3.10.11
  - numpy: 1.20.1
  - scipy: 1.6.0
  - mne: 0.24.1
  - notebook: 6.5.2	

  

Linux (for DYNAP-SE code)

- Python: 3.8.10
  - numpy: 1.21.0
  - pandas: 1.4.3
  - matplotlib: 3.4.2
  - notebook: 6.4.6

DYNAP-SE is not required to run the demo. 

### Installation guide

Install BCI2000 from: https://www.bci2000.org/mediawiki/index.php/DownloadBCI2000

Estimated install time: 2 hours


### Demo

Download the dataset: https://openneuro.org/datasets/ds004944/versions/1.0.0

**BCI2000 code/ setBCI2000params.ipynb**
 - Add the ADM module to BCI2000 following one of these two approaches:
   - Move 'BCI2000 code/ADM.exe' into the 'prog' folder in BCI2000
   - Move the 'BCI2000 code/ADM' folder into the 'src/custom' folder in BCI2000 and compile the code following: https://www.bci2000.org/mediawiki/index.php/Programming_Howto:Create_a_custom_Signal_Processing_Module

 - Move 'setBCI2000params.ipynb' and 'demo.prm' into the 'tools/python' folder in BCI2000
 - After running the notebook, you will obtain an ADM data encoding in a file called 'UPDN_demo.txt',identical to the one provided in 'BCI2000 code'

Expected run time: 7 min

**DYNAP-SE code/ processECoG.ipynb**
 - You will obtain HFO and IED-HFO rates of the demo patient.
 - You will visualize HFO and IED-HFO events of the demo patient, together with DYNAP-SE activity.

Expected run time: 1 min

### Instruction for use

- You can run 'setBCI2000params.ipynb' on custom data replacing **BCI2000.Parameters['PlaybackFileName'].Value**
- You can run BCI2000 with custom parameters:
 - changing parameter values in 'setBCI2000params.ipynb'
 - using the Config button in BCI2000 Operator

