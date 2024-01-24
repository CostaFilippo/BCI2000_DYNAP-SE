////////////////////////////////////////////////////////////////////////////////
// $Id: TransmissionFilter.cpp 6484 2022-01-03 16:59:03Z mellinger $
// Author: juergen.mellinger@uni-tuebingen.de (custom change for bipolar montage: Filippo Costa)
// Description: A filter that returns a subset of input channels in its output
//   signal.
//
// $BEGIN_BCI2000_LICENSE$
//
// This file is part of BCI2000, a platform for real-time bio-signal research.
// [ Copyright (C) 2000-2022: BCI2000 team and many external contributors ]
//
// BCI2000 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// BCI2000 is distributed in the hope that it will be useful, but
//                         WITHOUT ANY WARRANTY
// - without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//
// $END_BCI2000_LICENSE$
////////////////////////////////////////////////////////////////////////////////
#include "TransmissionFilterDIFF.h"

#include "BCIStream.h"
#include "IndexList.h"

#include <limits>

RegisterFilter(TransmissionFilterDIFF, 1.3);

TransmissionFilterDIFF::TransmissionFilterDIFF() : mCopy(false)
{
  BEGIN_PARAMETER_DEFINITIONS
    "Source:Montage list ChList1= 1 * % % % "
        "// list of transmitted channels: Channel names may contain * and ? wildcards, "
        " and character ranges enclosed in []; wildcard patterns may be negated by prepending an exclamation mark. "
        " Ranges of channels may be specified using : or - to separate begin from end.",
    "Source:Montage list ChList2= 1 * % % % "
      "// list of channels subtracted to ChList1. Bipolar Montage.",
  END_PARAMETER_DEFINITIONS
}

void TransmissionFilterDIFF::AutoConfig(const SignalProperties&)
{
    Parameter("ChList1")->SetNumValues(1);
    Parameter("ChList1")(0) = "*";

    Parameter("ChList2")->SetNumValues(1);
    Parameter("ChList2")(0) = "*";
}

void TransmissionFilterDIFF::Preflight(const SignalProperties &Input, SignalProperties &Output) const
{
    std::string ChList1 = FlatParameter("ChList1");
    IndexList idx(ChList1, Input.ChannelLabels());
    if (!idx.Errors().empty())
        bcierr << "Invalid ChList1: " << ChList1 << ": " << idx.Errors();
    else if (idx.Empty() && !Input.IsEmpty())
        bciwarn << "ChList1 \"" << ChList1 << "\" does not match any channel";

    // DIFF
    std::string ChList2 = FlatParameter("ChList2");
    IndexList idxDIFF(ChList2, Input.ChannelLabels());
    if (!idxDIFF.Errors().empty())
        bcierr << "Invalid ChList2: " << ChList2 << ": " << idxDIFF.Errors();
    else if (idxDIFF.Empty() && !Input.IsEmpty())
        bciwarn << "ChList2 \"" << ChList2 << "\" does not match any channel";


    Output = Input;
    Output.SetName("Transmitted Channels");
    Output.SetChannels(idx.Size());
    for (int idxOut = 0; idxOut < idx.Size(); ++idxOut)
        if (Input.ChannelLabels()[idx[idxOut]] == Input.ChannelLabels()[idxDIFF[idxOut]]) {
            Output.ChannelLabels()[idxOut] = Input.ChannelLabels()[idx[idxOut]];
        }
        else{
            Output.ChannelLabels()[idxOut] = Input.ChannelLabels()[idx[idxOut]] + "_" + Input.ChannelLabels()[idxDIFF[idxOut]];
        }
}   

void TransmissionFilterDIFF::Initialize(const SignalProperties &Input, const SignalProperties &Output)
{
    mChannelList.clear();
    mChannelListDIFF.clear();
    IndexList idx(FlatParameter("ChList1"), Input.ChannelLabels());
    IndexList idxDIFF(FlatParameter("ChList2"), Input.ChannelLabels());
    mCopy = (Input.Channels() == Output.Channels());
    for (int i = 0; i < idx.Size(); ++i)
    {
        mCopy = mCopy && idx[i] == i;
        mChannelList.push_back(idx[i]);
        mChannelListDIFF.push_back(idxDIFF[i]);
        bciwarn << idx[i] << idxDIFF[i];
    }

}

void TransmissionFilterDIFF::Process(const GenericSignal &Input, GenericSignal &Output)
{
    //if (mCopy)
    //    Output = Input;
    //else
    for (size_t i = 0; i < mChannelList.size(); ++i) {
        //bciwarn << mChannelList[i] << mChannelListDIFF[i];
        for (int j = 0; j < Input.Elements(); ++j) {

            if (mChannelList[i] == mChannelListDIFF[i]) {
                Output(i, j) = Input(mChannelList[i], j);
            }
            else {
                Output(i, j) = Input(mChannelList[i], j) - Input(mChannelListDIFF[i], j);
            }
        }
    }

}

std::string TransmissionFilterDIFF::FlatParameter(const std::string &inName) const
{
    ParamRef p = Parameter(inName);
    std::string flat;
    for (int i = 0; i < p->NumValues(); ++i)
        flat += " " + std::string(p(i));
    return flat.empty() ? flat : flat.substr(1);
}
