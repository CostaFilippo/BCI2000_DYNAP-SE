////////////////////////////////////////////////////////////////////////////////
// Authors: Costa Filippo
// Description: ADMFilter implementation
////////////////////////////////////////////////////////////////////////////////

#include "ADMFilter.h"
#include "BCIStream.h"
#include <iostream>
#include <fstream>

RegisterFilter( ADMFilter, 2.C );
     // Change the location as appropriate, to determine where your filter gets
     // sorted into the chain. By convention:
     //  - filter locations within SignalSource modules begin with "1."
     //  - filter locations witin SignalProcessing modules begin with "2."
     //       (NB: Filter() commands in a separate PipeDefinition.cpp file may override the default location set here with RegisterFilter)
     //  - filter locations within Application modules begin with "3."

ADMFilter::ADMFilter()
{
}

ADMFilter::~ADMFilter()
{
  Halt();
}

void
ADMFilter::Publish()
{
  // Define filter parameters

BEGIN_PARAMETER_DEFINITIONS
"ADM float RefractoryPeriod= 0.0003 0.0003 % %"
"// ADM refractory period parameter",
"ADM int LengthTuning= 10 10 0 60"
"// Lenght of the tuning phase (seconds)",
"ADM intlist SelectedChannel= 2 0 1 % 0 300"
"// ID of the channel to process",
"ADM string OutputA= ../../ADM_Events/UPDN_a.txt % % %"
"// 1st output file",
"ADM string OutputB= ../../ADM_Events/UPDN_b.txt % % %"
"// 2nd output file",
"ADM floatlist LenWindow= 2 0.01 0.01 % 0.001 1"
"// Window length for each block",
"ADM intlist percentileDelta= 2 40 50 % 10 90"
"Percentile of the amplitude range distribution",
 END_PARAMETER_DEFINITIONS


  // Define state variables

 BEGIN_STATE_DEFINITIONS
   "Refresh      8 1 1 1",
 END_STATE_DEFINITIONS

}

void
ADMFilter::Preflight( const SignalProperties& Input, SignalProperties& Output ) const
{
    if ((float)Parameter("RefractoryPeriod") <= 0)
        bcierr << "Refractory Period has to be positive";

    Parameter("SelectedChannel");

    if ((float)Parameter("LengthTuning") <= 0)
        bcierr << "Tuning Length has to be positive";

    Parameter("OutputA");
    Parameter("OutputB");

    State("Refresh");

  // The user has pressed "Set Config" and we need to sanity-check everything.
  
  // Check that the values of any parameters are sane:
  // Errors issued in this way, during Preflight, still allow the user to open
  // the Config dialog box, fix bad parameters and re-try.  By contrast, errors
  // and C++ exceptions at any other stage (outside Preflight) will make the
  // system stop, such that BCI2000 will need to be relaunched entirely.

  Output = Input; // this passes information through about SampleBlock dimensions, etc....

  // Note that the ADMFilter instance itself, and its members, are read-only during
  // this phase, due to the "const" at the end of the Preflight prototype above.
  // Any methods called by Preflight must also be "const" in the same way.
  
}


void
ADMFilter::Initialize( const SignalProperties& Input, const SignalProperties& Output )
{
  // The user has pressed "Set Config" and all Preflight checks have passed.

  chId.clear();
  DeltaVec.clear();
  lastEv.clear();
  dcVoltage.clear();
  minS.clear();
  maxS.clear();
  peakToPeak.clear();
  evSample.clear();
  lenW.clear();
  percentile.clear();
  tuning.clear();

  ref     = (int)(Parameter("RefractoryPeriod")*Input.SamplingRate());
  LTuning = (int)(Parameter("LengthTuning") * Input.SamplingRate());
  
  ParamRef lenWList = Parameter("LenWindow");
  bciwarn << Parameter("LenWindow");
  for (int i = 0; i < Input.Channels(); i++) {
      bciwarn << Input.Channels();
      lenW.push_back((int)(lenWList(i) * Input.SamplingRate()));
      bciwarn << "len W for ch" << i << ": " << lenW[i];
  }

  ParamRef percList = Parameter("percentileDelta");
  bciwarn << Parameter("percentileDelta");
  for (int i = 0; i < Input.Channels(); i++) {
      percentile.push_back((int)(percList(i)));
      bciwarn << "percentile for ch" << i << ": " << percentile[i];
  }


  ParamRef chList = Parameter("SelectedChannel");
  bciwarn << Parameter("SelectedChannel")->NumValues();
  for (int c = 0; c < Parameter("SelectedChannel")->NumValues(); c++) {
      chId.push_back(chList(c));
  }


  for (int c = 0; c <= Input.Channels(); c++) {
      lastEv.push_back(ref);
      dcVoltage.push_back(0);
      DeltaVec.push_back(0);
      minS.push_back(+10000);
      maxS.push_back(-10000);
      peakToPeak.push_back({});
      evSample.push_back(0);
      tuning.push_back(true);
  }
}

void
ADMFilter::StartRun()
{
    // The user has just pressed "Start" (or "Resume")
    bciwarn << "Start ADM Conversion";
    ADM_EventFile.close();
    notOpen1 = true;
    notOpen2 = true;

    //bciwarn << "Refractory period (in #samples): " << ref;
    //bciwarn << "Tuning period (in #samples): " << LTuning;
}


void
ADMFilter::Process( const GenericSignal& Input, GenericSignal& Output )
{

  // Processing a single SampleBlock of data.
  // Remember not to take too much CPU time here, or you will break the real-time constraint.

  // in Output the Reconstructed Signal is stored
    
    
    for (int ch : chId) {
        if(tuning[ch] == true && evSample[ch]>=LTuning) {
        
            std::sort(peakToPeak[ch].begin(), peakToPeak[ch].end(), std::greater<float>());
            if (peakToPeak[ch].size() % 2 == 0) {
                deltaPos = (int)(peakToPeak[ch].size() * (100-percentile[ch]) / 100);
                DeltaVec[ch] = peakToPeak[ch][deltaPos];
                
                bciwarn << "Delta for ch " << ch << " - even ";
            }
            else {
                deltaPos = (int)(peakToPeak[ch].size() * (100-percentile[ch]) / 100);
                DeltaVec[ch] = peakToPeak[ch][deltaPos];
              
                bciwarn << "Delta for ch " << ch << " - odd ";
            }
           
            bciwarn << "Delta for ch " << ch << ": " << DeltaVec[ch];
            
            dcVoltage[ch] = Output(ch, 0);

            tuning[ch] = false;
        }
    }
    

    for (int ch : chId) {
        
        
        for (int el = 0; el < Output.Elements(); el++)
        {
            if (evSample[ch] < LTuning) {

                if (Input(ch, el) < minS[ch]) {
                    minS[ch] = Input(ch, el);
                }
                if (Input(ch, el) > maxS[ch]) {
                    maxS[ch] = Input(ch, el);
                }
            
                if (evSample[ch] % lenW[ch] == 0) {
                    //bciwarn << "PP for ch " << ch << ": " << maxS[ch] - minS[ch];
                    peakToPeak[ch].push_back(maxS[ch] - minS[ch]);
                    maxS[ch] = -10000;
                    minS[ch] = +10000;
                }
            }

            if (tuning[ch]) {
                Output(ch, el) = 0;
                evSample[ch]++;
            }
            else {
                peakToPeak.clear();
                evSample[ch]++;
                Output(ch, el) = ADM_Convert(Input(ch, el), ref, dcVoltage[ch], DeltaVec[ch], lastEv[ch], ch, evSample[ch]);
            }
            
        }
    }

    if (State("Refresh")==0) {
        ADM_EventFile.close();
        notOpen1 = true;
        notOpen2 = true;
    }
    if (State("Refresh") == 1 && notOpen1) {
        ADM_EventFile.close();
        notOpen2 = true;
        ADM_EventFile.open(Parameter("OutputA"), std::ios::out);
        notOpen1 = false;
    }
    if (State("Refresh") == 2 && notOpen2) {
        ADM_EventFile.close();
        notOpen1 = true;
        ADM_EventFile.open(Parameter("OutputB"), std::ios::out);
        notOpen2 = false;
    }

    
}


float ADMFilter::ADM_Convert(float inputEv, int ref, float &voltage, float delta, int &last, int ch, int el)
{
    
    float thrUP = voltage + delta;
    float thrDN = voltage - delta;

    if (last >= ref)
        if (inputEv > thrUP) {
            if (State("Refresh") != 0) {
                ADM_EventFile << 1 << '\t' << el << '\t' << ch << std::endl;
            }
            last = 0;
            voltage = thrUP;
          
        }
        else if (inputEv < thrDN) {
            if (State("Refresh") != 0) {
                ADM_EventFile << -1 << '\t' << el << '\t' << ch << std::endl;
            }
            last = 0;
            voltage = thrDN;

        }

    last++;
    
    return voltage;
}

void
ADMFilter::StopRun()
{
  // The Running state has been set to 0, either because the user has pressed "Suspend",
  // or because the run has reached its natural end.
  
  ADM_EventFile.close();
  bciwarn << "Stop ADM Conversion";
}

void
ADMFilter::Halt()
{
  
  ADM_EventFile.close();
  // Stop any threads or other asynchronous activity.
}

