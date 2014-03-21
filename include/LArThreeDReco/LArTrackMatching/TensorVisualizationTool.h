/**
 *  @file   LArContent/include/LArThreeDReco/LArTrackMatching/TensorVisualizationTool.h
 * 
 *  @brief  Header file for the tensor visualization tool class.
 * 
 *  $Log: $
 */
#ifndef TENSOR_VISUALIZATION_TOOL_H
#define TENSOR_VISUALIZATION_TOOL_H 1

#include "LArThreeDReco/LArTrackMatching/ThreeDTransverseTracksAlgorithm.h"

namespace lar
{

/**
 *  @brief  TensorVisualizationTool class
 */
class TensorVisualizationTool : public TensorManipulationTool
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm tool
     */
    class Factory : public pandora::AlgorithmToolFactory
    {
    public:
        pandora::AlgorithmTool *CreateAlgorithmTool() const;
    };

private:
    pandora::StatusCode Run(ThreeDTransverseTracksAlgorithm *pAlgorithm, TensorType &overlapTensor);
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    unsigned int    m_minClusterConnections;        ///< The minimum number of cluster connections for display
    bool            m_ignoreUnavailableClusters;    ///< Whether to ignore (skip-over) unavailable clusters in the tensor
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::AlgorithmTool *TensorVisualizationTool::Factory::CreateAlgorithmTool() const
{
    return new TensorVisualizationTool();
}

} // namespace lar

#endif // #ifndef TENSOR_VISUALIZATION_TOOL_H