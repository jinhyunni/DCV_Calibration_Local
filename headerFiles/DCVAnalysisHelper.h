#ifndef DCVANALYSISHELPER
#define DCVANALYSISHELPER

#include "NxChannelMatching.h"

// Making histogram keys for Single MPPC channel
TString makeHistogramKey
(
	TString cutInfo,
	int trackID,
	int modID,
	int MCOnOff,
	int mainSub
)
{

	TString histKey;

	if( MCOnOff == 0){
		if( mainSub ==0 ){
			histKey	= Form("DCVPeak_%s_trk%02d_Mod%02d", cutInfo.Data(), trackID, modID);
		} else {
			histKey	= Form("DCVSubPeak_%s_trk%02d_Mod%02d", cutInfo.Data(), trackID, modID);
		}
	} else {
		if( mainSub ==0 ){	// 0 for main channel
			histKey	= Form("DCVChannelEne_%s_trk%02d_Mod%02d", cutInfo.Data(), trackID, modID);
		} else {			// 1 for sub channel
			histKey	= Form("DCVSubChannelEne_%s_trk%02d_Mod%02d", cutInfo.Data(), trackID, modID);
		}
	}
	return histKey;
}

// Making histogram keys for MPPC pair
TString makeHistogramKey( TString cutInfo, int trackID, int mainModID, int subModID )
{
	TString histKey = Form("DCVPairEne_%s_trk%02d_MainMod%02d_SubMod%02d", cutInfo.Data(), trackID, mainModID, subModID);
	return histKey;
}

// Making histtogram keys for DCV Module
TString makeHistogramKey( TString cutInfo, int trackID, int upMainModID, int upSubModID, int downMainModID, int downSubModID )
{
	TString histKey = Form("DCVModuleEne_%s_trk%02d_Upstream_Main%02d_Sub%02d_Downstream_Main%02d_Sub%02d",
			cutInfo.Data(),
			trackID,
			upMainModID,
			upSubModID,
			downMainModID,
			downSubModID);
	return histKey;
}


// Making legend for histogram
TString makeObjInfo( int trackID, int modID )
{
	TString legend = Form("trk%02d_Mod%02d", trackID, modID);
	return legend;
}

TString makeObjInfo( int trackID, int mainModID, int subModID )
{
	TString legend = Form("trk%02d_MainMod%02d_SubMod%02d", trackID, mainModID, subModID);
	return legend;
}

TString makeObjInfo( int trackID, int upmain, int upsub, int downmain, int downsub)
{
	TString legend = Form("trk%02d_Upstream_Main%02d_Sub%02d_Downstream_Main%02d_Sub%02d", trackID, upmain, upsub, downmain, downsub);
	return legend;
}

// Method for storing histogram informaitons to vectors
// -> This method is used for making inputs
void fnStoreInputHistos(vector<vector<TString>>& outputHisName, vector<vector<TString>>& outputHisInfo,  TString inputHisSuffix, int calObj, int McOnOff, int verbose)
{
	switch( calObj )
	{
		case 0:
			for( auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackMPPC.begin(), trackIt);
				auto const& mods = *trackIt;
				cout << trackId << endl;

				for( auto modIt = mods.begin(); modIt != mods.end(); modIt++)
				{
					int hisIndex = std::distance(mods.begin(), modIt);
					auto const& mod = *modIt;

					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, mod, McOnOff, 0) );
					outputHisInfo[trackId].push_back( makeObjInfo(trackId, mod) );

					if( verbose )
						cout << outputHisName[trackId][hisIndex] << " " << outputHisInfo[trackId][hisIndex] << endl;
				}
			}
			break;

		case 1:
			for( auto trackIt = TrackPairMPPC.begin(); trackIt != TrackPairMPPC.end(); trackIt++)
			{
				int trackId = std::distance(TrackPairMPPC.begin(), trackIt);
				auto const pairs = *trackIt;
				for( auto pairIt = pairs.begin(); pairIt != pairs.end(); pairIt++ )
				{
					int hisIndex = std::distance(pairs.begin(), pairIt);
					auto const& pair = *pairIt;

					int mainModId = pair.mainMod;
					int subModId = pair.subMod;
					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, mainModId, subModId) );
					outputHisInfo[trackId].push_back( makeObjInfo(trackId, mainModId, subModId) );

					if( verbose )
						cout << outputHisName[trackId][hisIndex] << " " << outputHisInfo[trackId][hisIndex] << endl;
				}
			}
			break;

		case 2:
			for( auto trackIt = TrackModuleMPPC.begin(); trackIt != TrackModuleMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackModuleMPPC.begin(), trackIt);
				auto const modules = *trackIt;

				for( auto moduleIt = modules.begin(); moduleIt != modules.end(); moduleIt++ )
				{
					int hisIndex = std::distance(modules.begin(), moduleIt);
					auto const& mod = *moduleIt;

					auto upstream = mod.upstream;
					int upMainMod = upstream.mainMod;
					int upSubMod = upstream.subMod;

					auto downstream = mod.downstream;
					int downMainMod = downstream.mainMod;
					int downSubMod = downstream.subMod;

					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, upMainMod, upSubMod, downMainMod, downSubMod) );
					outputHisInfo[trackId].push_back( makeObjInfo(trackId, upMainMod, upSubMod, downMainMod, downSubMod) );

					if( verbose )
						cout << outputHisName[trackId][hisIndex] << " " << outputHisInfo[trackId][hisIndex] << endl;
				}
			}
			break;

		default:
			cerr << "No viable option! Break out!" << endl;
			return;
	}
}

void fnStoreInputHistos(vector<vector<TString>>& outputHisName, TString inputHisSuffix, int calObj, int McOnOff, int verbose)
{
	switch( calObj )
	{
		case 0:
			for( auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackMPPC.begin(), trackIt);
				auto const& mods = *trackIt;
				cout << trackId << endl;

				for( auto modIt = mods.begin(); modIt != mods.end(); modIt++)
				{
					int hisIndex = std::distance(mods.begin(), modIt);
					auto const& mod = *modIt;

					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, mod, McOnOff, 0) );

					if( verbose)
						cout << outputHisName[trackId][hisIndex] << endl;
				}
			}
			break;

		case 1:
			for( auto trackIt = TrackPairMPPC.begin(); trackIt != TrackPairMPPC.end(); trackIt++)
			{
				int trackId = std::distance(TrackPairMPPC.begin(), trackIt);
				auto const pairs = *trackIt;
				for( auto pairIt = pairs.begin(); pairIt != pairs.end(); pairIt++ )
				{
					int hisIndex = std::distance(pairs.begin(), pairIt);
					auto const& pair = *pairIt;

					int mainModId = pair.mainMod;
					int subModId = pair.subMod;
					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, mainModId, subModId) );

					if( verbose)
						cout << outputHisName[trackId][hisIndex] << endl;
				}
			}
			break;

		case 2:
			for( auto trackIt = TrackModuleMPPC.begin(); trackIt != TrackModuleMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackModuleMPPC.begin(), trackIt);
				auto const modules = *trackIt;

				for( auto moduleIt = modules.begin(); moduleIt != modules.end(); moduleIt++ )
				{
					int hisIndex = std::distance(modules.begin(), moduleIt);
					auto const& mod = *moduleIt;

					auto upstream = mod.upstream;
					int upMainMod = upstream.mainMod;
					int upSubMod = upstream.subMod;

					auto downstream = mod.downstream;
					int downMainMod = downstream.mainMod;
					int downSubMod = downstream.subMod;

					outputHisName[trackId].push_back( makeHistogramKey(inputHisSuffix, trackId, upMainMod, upSubMod, downMainMod, downSubMod) );

					if( verbose)
						cout << outputHisName[trackId][hisIndex] << endl;
				}
			}
			break;

		default:
			cerr << "No viable option! Break out!" << endl;
			return;
	}
}

// Storing histogram pointers to vectors
template<typename T>
void fnGetStoredHistos
(
		vector<vector<T*>>& outputHis,
		TFile* inputFile,
		TString inputHisSuffix,
		int calObj,
		int McOnOff,
		int verbose
)
{
	switch( calObj )
	{
		case 0:
			for( auto trackIt = TrackMPPC.begin(); trackIt != TrackMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackMPPC.begin(), trackIt);
				auto const& mods = *trackIt;
				cout << trackId << endl;

				for( auto modIt = mods.begin(); modIt != mods.end(); modIt++)
				{
					int hisIndex = std::distance(mods.begin(), modIt);
					auto const& mod = *modIt;

					outputHis[trackId].push_back( (T*)inputFile -> Get(makeHistogramKey(inputHisSuffix, trackId, mod, McOnOff, 0)) );

					if( !outputHis[trackId][hisIndex] )
					{
						cout << "Histogram not safely stored! Abort!" << endl;
						return;
					}

					if( verbose )
						cout << makeHistogramKey(inputHisSuffix, trackId, mod, McOnOff, 0) << endl;
				}
			}
			break;

		case 1:
			for( auto trackIt = TrackPairMPPC.begin(); trackIt != TrackPairMPPC.end(); trackIt++)
			{
				int trackId = std::distance(TrackPairMPPC.begin(), trackIt);
				auto const pairs = *trackIt;
				for( auto pairIt = pairs.begin(); pairIt != pairs.end(); pairIt++ )
				{
					int hisIndex = std::distance(pairs.begin(), pairIt);
					auto const& pair = *pairIt;

					int mainModId = pair.mainMod;
					int subModId = pair.subMod;
					outputHis[trackId].push_back( (T*)inputFile -> Get(makeHistogramKey(inputHisSuffix, trackId, mainModId, subModId)) );

					if( !outputHis[trackId][hisIndex] )
					{
						cout << "Histogram not safely stored! Abort!" << endl;
						return;
					}

					if( verbose)
						cout << makeHistogramKey(inputHisSuffix, trackId, mainModId, subModId) << endl;
				}
			}
			break;

		case 2:
			for( auto trackIt = TrackModuleMPPC.begin(); trackIt != TrackModuleMPPC.end(); trackIt++ )
			{
				int trackId = std::distance(TrackModuleMPPC.begin(), trackIt);
				auto const modules = *trackIt;

				for( auto moduleIt = modules.begin(); moduleIt != modules.end(); moduleIt++ )
				{
					int hisIndex = std::distance(modules.begin(), moduleIt);
					auto const& mod = *moduleIt;

					auto upstream = mod.upstream;
					int upMainMod = upstream.mainMod;
					int upSubMod = upstream.subMod;

					auto downstream = mod.downstream;
					int downMainMod = downstream.mainMod;
					int downSubMod = downstream.subMod;

					outputHis[trackId].push_back((T*)inputFile -> Get( makeHistogramKey(inputHisSuffix, trackId, upMainMod, upSubMod, downMainMod, downSubMod)) );

					if( !outputHis[trackId][hisIndex] )
					{
						cout << "Histogram not safely stored! Abort!" << endl;
						return;
					}

					if( verbose )
						cout << makeHistogramKey(inputHisSuffix, trackId, upMainMod, upSubMod, downMainMod, downSubMod) << "...Stored" << endl;
				}
			}
			break;

		default:
			cerr << "No viable option! Break out!" << endl;
			return;
	}

}

#endif
