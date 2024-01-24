////////////////////////////////////////////////////////////////////////////////
// Authors: Omen@LAPTOP-BUKIE434
// Description: Pipe definition for the ADM module.
////////////////////////////////////////////////////////////////////////////////

// This file defines which filters will be used, and the sequence in which they are
// applied. Each Filter() entry consists of the name of a filter and a location token:
// the BCI2000 framework will then determine the order of the filters by string-sorting
// these location tokens.  By convention:
//   - filters locations for SignalSource modules begin with "1."
//   - filters locations for SignalProcessing modules begin with "2."
//   - filters locations Application modules begin with "3."
//
// Locations defined here using the Filter() macro override the defaults set by
// RegisterFilter() in the individual filters' implementation files.

// Uncomment/add the lines for the filters you need in addition to your own filter(s):

#include "FIRFilter.h"
Filter( FIRFilter, 2.B );

#include "TransmissionFilterDIFF.h"
Filter(TransmissionFilterDIFF,1.3);

//#include "ARFilter.h"
//Filter( ARFilter, 2.C );

//#include "LinearClassifier.h"
//Filter( LinearClassifier, 2.D );

//#include "LPFilter.h"
//Filter( LPFilter, 2.D1 );

//#include "ExpressionFilter.h"
//Filter( ExpressionFilter, 2.D2 );

//#include "Normalizer.h"
//Filter( Normalizer, 2.C );


#include "ADMFilter.h"
Filter( ADMFilter, 2.C );