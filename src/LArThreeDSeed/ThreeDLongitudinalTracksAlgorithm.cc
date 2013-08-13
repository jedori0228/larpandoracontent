/**
 *  @file   LArContent/src/LArThreeDSeed/ThreeDLongitudinalTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the three dimensional tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LArHelpers/LArClusterHelper.h"
#include "LArHelpers/LArGeometryHelper.h"
#include "LArHelpers/LArVertexHelper.h"

#include "LArThreeDSeed/ThreeDLongitudinalTracksAlgorithm.h"

using namespace pandora;

namespace lar
{

void ThreeDLongitudinalTracksAlgorithm::CalculateOverlapResult(Cluster *pClusterU, Cluster *pClusterV, Cluster *pClusterW)
{
    LArClusterHelper::TwoDSlidingFitResult slidingFitResultU, slidingFitResultV, slidingFitResultW;

    LArClusterHelper::LArTwoDSlidingFit(pClusterU, 20, slidingFitResultU);
    LArClusterHelper::LArTwoDSlidingFit(pClusterV, 20, slidingFitResultV);
    LArClusterHelper::LArTwoDSlidingFit(pClusterW, 20, slidingFitResultW);

    /*
    unsigned int nMultiValuedInX(0);

    if ( slidingFitResultU.IsMultivaluedInX() ) ++nMultiValuedInX;
    if ( slidingFitResultV.IsMultivaluedInX() ) ++nMultiValuedInX;
    if ( slidingFitResultW.IsMultivaluedInX() ) ++nMultiValuedInX;

    if ( nMultiValuedInX < 2 )
        return;
    */

   
    
    const float m_maxChi2 = 5.f;
    const float m_minCosOpeningAngle = 0.5;
  
            
    TrackOverlapResult bestOverlapResult(0,1);


    for (unsigned int nU=0; nU<2; ++nU)
    {
        const bool isForwardU = (0==nU) ? true : false;

        for (unsigned int nV=0; nV<2; ++nV)
	{
            const bool isForwardV = (0==nV) ? true : false;

            for (unsigned int nW=0; nW<2; ++nW)
	    {
                const bool isForwardW = (0==nW) ? true : false;

		
                // Check consistency of directions with reconstructed vertex 
                if (LArVertexHelper::DoesCurrentVertexExist())
		{
		    if ( true  == isForwardU && true == LArVertexHelper::IsBackwardInZ( slidingFitResultU.GetCluster() )
                      || false == isForwardU && true == LArVertexHelper::IsForwardInZ( slidingFitResultU.GetCluster() )
                      || true  == isForwardV && true == LArVertexHelper::IsBackwardInZ( slidingFitResultV.GetCluster() )
                      || false == isForwardV && true == LArVertexHelper::IsForwardInZ( slidingFitResultV.GetCluster() )
                      || true  == isForwardW && true == LArVertexHelper::IsBackwardInZ( slidingFitResultW.GetCluster() )
		      || false == isForwardW && true == LArVertexHelper::IsForwardInZ( slidingFitResultW.GetCluster() ) )
		        continue;
                }

                // Get 2D start and end positions for each sliding window fit
                const CartesianVector vtxU = (isForwardU==true)  ? slidingFitResultU.GetGlobalMinLayerPosition() : slidingFitResultU.GetGlobalMaxLayerPosition();
                const CartesianVector endU = (isForwardU==false) ? slidingFitResultU.GetGlobalMinLayerPosition() : slidingFitResultU.GetGlobalMaxLayerPosition();    

                const CartesianVector vtxV = (isForwardV==true)  ? slidingFitResultV.GetGlobalMinLayerPosition() : slidingFitResultV.GetGlobalMaxLayerPosition();
                const CartesianVector endV = (isForwardV==false) ? slidingFitResultV.GetGlobalMinLayerPosition() : slidingFitResultV.GetGlobalMaxLayerPosition();    

                const CartesianVector vtxW = (isForwardW==true)  ? slidingFitResultW.GetGlobalMinLayerPosition() : slidingFitResultW.GetGlobalMaxLayerPosition();
                const CartesianVector endW = (isForwardW==false) ? slidingFitResultW.GetGlobalMinLayerPosition() : slidingFitResultW.GetGlobalMaxLayerPosition();    


                float chi2(0.f);
                CartesianVector position3D(0.f,0.f,0.f);
                CartesianPointList vtxList3D, endList3D;


                // Calculate possible 3D start positions
                LArGeometryHelper::MergeTwoPositions3D(VIEW_U, VIEW_V, vtxU, vtxV, position3D, chi2);
                if (chi2<m_maxChi2) vtxList3D.push_back(position3D);

                LArGeometryHelper::MergeTwoPositions3D(VIEW_V, VIEW_W, vtxV, vtxW, position3D, chi2);
                if (chi2<m_maxChi2) vtxList3D.push_back(position3D);

                LArGeometryHelper::MergeTwoPositions3D(VIEW_W, VIEW_U, vtxW, vtxU, position3D, chi2);
                if (chi2<m_maxChi2) vtxList3D.push_back(position3D);


                // Calculate possible 3D end positions
                LArGeometryHelper::MergeTwoPositions3D(VIEW_U, VIEW_V, endU, endV, position3D, chi2);
                if (chi2<m_maxChi2) endList3D.push_back(position3D);

                LArGeometryHelper::MergeTwoPositions3D(VIEW_V, VIEW_W, endV, endW, position3D, chi2);
                if (chi2<m_maxChi2) endList3D.push_back(position3D);

                LArGeometryHelper::MergeTwoPositions3D(VIEW_W, VIEW_U, endW, endU, position3D, chi2);
                if (chi2<m_maxChi2) endList3D.push_back(position3D);


                // Find best matched 3D trajactory
                for ( CartesianPointList::const_iterator iterI = vtxList3D.begin(), iterEndI = vtxList3D.end(); iterI != iterEndI; ++iterI )
                {
                    const CartesianVector vtxMerged3D = *iterI;

                    const CartesianVector vtxMergedU = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_U);
                    const CartesianVector vtxMergedV = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_V);
                    const CartesianVector vtxMergedW = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_W);

                    for ( CartesianPointList::const_iterator iterJ = endList3D.begin(), iterEndJ = endList3D.end(); iterJ != iterEndJ; ++iterJ )
                    {
                        const CartesianVector endMerged3D = *iterJ;

                        const CartesianVector endMergedU = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_U);
                        const CartesianVector endMergedV = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_V); 
                        const CartesianVector endMergedW = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_W);

                        if ( (endMergedU - vtxMergedU).GetCosOpeningAngle(endU - vtxU) < m_minCosOpeningAngle
                          || (endMergedV - vtxMergedV).GetCosOpeningAngle(endV - vtxV) < m_minCosOpeningAngle
			  || (endMergedW - vtxMergedW).GetCosOpeningAngle(endW - vtxW) < m_minCosOpeningAngle )
			    continue;

                        if ( (vtxMergedU - vtxU).GetMagnitudeSquared() > (vtxMergedU - endU).GetMagnitudeSquared()
                          || (vtxMergedV - vtxV).GetMagnitudeSquared() > (vtxMergedV - endV).GetMagnitudeSquared()
                          || (vtxMergedW - vtxW).GetMagnitudeSquared() > (vtxMergedW - endW).GetMagnitudeSquared()
                          || (endMergedU - endU).GetMagnitudeSquared() > (endMergedU - vtxU).GetMagnitudeSquared()
                          || (endMergedV - endV).GetMagnitudeSquared() > (endMergedV - vtxV).GetMagnitudeSquared()
			  || (endMergedW - endW).GetMagnitudeSquared() > (endMergedW - vtxW).GetMagnitudeSquared() 
                          || (vtxMergedU - vtxU).GetMagnitudeSquared() > (endMergedU - vtxU).GetMagnitudeSquared()
                          || (vtxMergedV - vtxV).GetMagnitudeSquared() > (endMergedV - vtxV).GetMagnitudeSquared()
                          || (vtxMergedW - vtxW).GetMagnitudeSquared() > (endMergedW - vtxW).GetMagnitudeSquared()
                          || (endMergedU - endU).GetMagnitudeSquared() > (vtxMergedU - endU).GetMagnitudeSquared()
                          || (endMergedV - endV).GetMagnitudeSquared() > (vtxMergedV - endV).GetMagnitudeSquared()
			  || (endMergedW - endW).GetMagnitudeSquared() > (vtxMergedW - endW).GetMagnitudeSquared() )
			    continue;

                        TrackOverlapResult thisOverlapResult(0,1);
                        
                        this->CalculateOverlapResult(slidingFitResultU, slidingFitResultV, slidingFitResultW, 
                                                     vtxMerged3D, endMerged3D, thisOverlapResult);

                        if (thisOverlapResult.GetNMatchedSamplingPoints() > bestOverlapResult.GetNMatchedSamplingPoints())
                            bestOverlapResult = thisOverlapResult;
		    }
		}

	    }
	}
    }

    if (bestOverlapResult.GetNMatchedSamplingPoints() > 0)
        m_overlapTensor.SetOverlapResult(pClusterU, pClusterV, pClusterW, bestOverlapResult);

}


//------------------------------------------------------------------------------------------------------------------------------------------

void ThreeDLongitudinalTracksAlgorithm::CalculateOverlapResult(const LArClusterHelper::TwoDSlidingFitResult &slidingFitResultU,
        const LArClusterHelper::TwoDSlidingFitResult &slidingFitResultV, const LArClusterHelper::TwoDSlidingFitResult &slidingFitResultW,
	const CartesianVector vtxMerged3D, const CartesianVector endMerged3D, TrackOverlapResult& overlapResult )
{



    // Calculate start and end positions of linear trajectory
    const CartesianVector vtxMergedU = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_U);
    const CartesianVector vtxMergedV = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_V);
    const CartesianVector vtxMergedW = LArGeometryHelper::ProjectPosition(vtxMerged3D,VIEW_W);

    const CartesianVector endMergedU = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_U);
    const CartesianVector endMergedV = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_V); 
    const CartesianVector endMergedW = LArGeometryHelper::ProjectPosition(endMerged3D,VIEW_W);

  


    const float m_maxChi2 = 5.f;
    const float m_samplingPitch = 1.f;

    const unsigned int nTotalSamplingPoints = static_cast<unsigned int>((endMerged3D - vtxMerged3D).GetMagnitude()/ m_samplingPitch);



    float deltaChi2(0.f);
    float totalChi2(0.f);
    float reducedChi2(0.f);

    unsigned int nSamplingPoints(0);
    unsigned int nMatchedSamplingPoints(0);



    for( int n=0; n<nTotalSamplingPoints ; ++n )
    {
        const float alphaU = (0.5 + static_cast<float>(n)) / static_cast<float>(nTotalSamplingPoints);
        const float alphaV = (0.5 + static_cast<float>(n)) / static_cast<float>(nTotalSamplingPoints);
        const float alphaW = (0.5 + static_cast<float>(n)) / static_cast<float>(nTotalSamplingPoints);

        CartesianVector linearU = vtxMergedU + (endMergedU - vtxMergedU) * alphaU;
        CartesianVector linearV = vtxMergedV + (endMergedV - vtxMergedV) * alphaV;
        CartesianVector linearW = vtxMergedW + (endMergedW - vtxMergedW) * alphaW;

        try{
            CartesianVector posU(0.f,0.f,0.f);
            CartesianVector posV(0.f,0.f,0.f);
            CartesianVector posW(0.f,0.f,0.f);

            CartesianVector mergedU(0.f,0.f,0.f);
            CartesianVector mergedV(0.f,0.f,0.f);
            CartesianVector mergedW(0.f,0.f,0.f);

            slidingFitResultU.GetGlobalFitProjection(linearU, posU);
            slidingFitResultV.GetGlobalFitProjection(linearV, posV);
            slidingFitResultW.GetGlobalFitProjection(linearW, posW);

            LArGeometryHelper::MergeThreePositions(posU, posV, posW,
                                                   mergedU, mergedV, mergedW,
                                                   deltaChi2);

            ++nSamplingPoints;
            if( deltaChi2 < m_maxChi2 ) ++nMatchedSamplingPoints;

            totalChi2 += deltaChi2;
	}
        catch (StatusCodeException &)
	{
	}
    }


    if (nSamplingPoints > 0)
    {
        reducedChi2 = totalChi2 / static_cast<float>(nSamplingPoints);

        overlapResult = TrackOverlapResult(nMatchedSamplingPoints,nSamplingPoints);
    }




Cluster* pClusterU = (Cluster*)(slidingFitResultU.GetCluster());
Cluster* pClusterV = (Cluster*)(slidingFitResultV.GetCluster());
Cluster* pClusterW = (Cluster*)(slidingFitResultW.GetCluster());  

ClusterList tempListU; tempListU.insert(pClusterU);
ClusterList tempListV; tempListV.insert(pClusterV);
ClusterList tempListW; tempListW.insert(pClusterW);

PandoraMonitoringApi::SetEveDisplayParameters(0, 0, -1.f, 1.f);
PandoraMonitoringApi::VisualizeClusters(&tempListU, "BestClusterU", RED);
PandoraMonitoringApi::VisualizeClusters(&tempListV, "BestClusterV", GREEN);
PandoraMonitoringApi::VisualizeClusters(&tempListW, "BestClusterW", BLUE);
PandoraMonitoringApi::AddMarkerToVisualization(&vtxMergedU, "vtxMergedU", RED,   3.0);
PandoraMonitoringApi::AddMarkerToVisualization(&vtxMergedV, "vtxMergedV", GREEN, 3.0);
PandoraMonitoringApi::AddMarkerToVisualization(&vtxMergedW, "vtxMergedW", BLUE,  3.0);
PandoraMonitoringApi::AddMarkerToVisualization(&endMergedU, "endMergedU", RED,   3.0);
PandoraMonitoringApi::AddMarkerToVisualization(&endMergedV, "endMergedV", GREEN, 3.0);
PandoraMonitoringApi::AddMarkerToVisualization(&endMergedW, "endMergedW", BLUE,  3.0);
PandoraMonitoringApi::ViewEvent();



}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ThreeDLongitudinalTracksAlgorithm::ExamineTensor()
{
    float bestOverlapResult(0.f);
    Cluster *pBestClusterU(NULL), *pBestClusterV(NULL), *pBestClusterW(NULL);

    const ClusterList &clusterListU(m_overlapTensor.GetClusterListU());
    const ClusterList &clusterListV(m_overlapTensor.GetClusterListV());
    const ClusterList &clusterListW(m_overlapTensor.GetClusterListW());

    for (ClusterList::const_iterator iterU = clusterListU.begin(), iterUEnd = clusterListU.end(); iterU != iterUEnd; ++iterU)
    {
        for (ClusterList::const_iterator iterV = clusterListV.begin(), iterVEnd = clusterListV.end(); iterV != iterVEnd; ++iterV)
        {
            for (ClusterList::const_iterator iterW = clusterListW.begin(), iterWEnd = clusterListW.end(); iterW != iterWEnd; ++iterW)
            {
                try
                {
                    const TrackOverlapResult &overlapResult(m_overlapTensor.GetOverlapResult(*iterU, *iterV, *iterW));

                    if (overlapResult.GetNMatchedSamplingPoints() > bestOverlapResult)
                    {
                        bestOverlapResult = overlapResult.GetNMatchedSamplingPoints();
                        pBestClusterU = *iterU;
                        pBestClusterV = *iterV;
                        pBestClusterW = *iterW;
                    }
                }
                catch (StatusCodeException &)
                {
                }
            }
        }
    }

    if (!pBestClusterU || !pBestClusterV || !pBestClusterW)
        return false;

    ProtoParticle protoParticle;
    protoParticle.m_clusterVectorU.push_back(pBestClusterU);
    protoParticle.m_clusterVectorV.push_back(pBestClusterV);
    protoParticle.m_clusterVectorW.push_back(pBestClusterW);
    m_protoParticleVector.push_back(protoParticle);

    return true;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ThreeDLongitudinalTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
   

    return ThreeDBaseAlgorithm<TrackOverlapResult>::ReadSettings(xmlHandle);
}

} // namespace lar
