////////////////////////////////////////////////////////////////////////////////
// Authors:     Costa Filippo
// Description: ADMFilter header
////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDED_ADMFILTER_H
#define INCLUDED_ADMFILTER_H

#include "GenericFilter.h"
#include <vector>

class ADMFilter : public GenericFilter
{
 public:
  ADMFilter();
  ~ADMFilter();
  void Publish() override;
  void Preflight( const SignalProperties& Input, SignalProperties& Output ) const override;
  void Initialize( const SignalProperties& Input, const SignalProperties& Output ) override;
  void StartRun() override;
  void Process( const GenericSignal& Input, GenericSignal& Output ) override;
  void StopRun() override;
  void Halt() override;

 private:
   // Use this space to declare any ADMFilter-specific methods and member variables you'll need
   int    LTuning;                  // # samples for the ADM tuning.        Initialized in ADMFilter::Initialize
   int    ref;                      // # samples for the refractory period. Initialized in ADMFilter::Initialize

   bool   notOpen1    = true;		               // flag. true if file 1 is not open 
   bool   notOpen2    = true;                      // flag. true if file 2 is not open
   std::ofstream ADM_EventFile;					   // output file
  
   std::vector<bool>   tuning;		               // True if the filter is in the tuning mode. One per channel 
   std::vector<int>    lenW;	                   // Length of the non-overlapping window for each channel (# samples)				    
   std::vector<int>    percentile;                 // Percentile of the peak-to-peak distribution for each channel
   std::vector<float>  minS;                       // Minimum value inside the window for each channel
   std::vector<float>  maxS;                       // Maximum value inside the window for each channel
   std::vector<std::vector<float>>  peakToPeak;    // vector of peak-to-peak values, one vector for each channel
   std::vector<int>    lastEv;                     // # samples after last ADM events
   std::vector<int>    chId;					   // vector of channel indices
   std::vector<int>    evSample;                   // sample number, since the start of the filter, for each channel
   std::vector<float>  dcVoltage;				   // reconstruction values. OUTPUT of the filter
   std::vector<float>  DeltaVec;                   // delta value for each channel
   int    deltaPos;								   // peak-to-peak percentile position


   float  ADM_Convert(float inputEv, int ref, float &dcVoltage, float delta, int& lastEv, int ch, int el);
};

#endif // INCLUDED_ADMFILTER_H
