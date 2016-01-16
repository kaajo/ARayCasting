#ifndef ARMARKER_H
#define ARMARKER_H

#include <AR/ar.h>
#include <AR/arosg.h>
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/gsub_lite.h>

#include <fbxsdk.h>

#include <QVector>
#include <QObject>
#include <QMatrix4x4>

enum ShadingMode
{
    SHADING_MODE_WIREFRAME,
    SHADING_MODE_SHADED,
};


class ARMarker : public QObject
{
    Q_OBJECT
public:

    enum class ARMarkerType
    {
        UNKNOWN,
        LIGHTSOURCE,
        OBJECT
    };

    ARMarker(FbxManager * sdkManager, const char *modelFile, const char *pattName, ARMarkerType type = ARMarkerType::UNKNOWN);
    ~ARMarker();

    ARdouble makeTransformationMatrix(ARMarkerInfo *markerInfo, AR3DHandle *gAR3DHandle);
    void draw() const;

    ARMarkerType type() const;
    int id() const;
    int found() const;
    void setFound(int found);

    float modelSize();
    void setModelSize(const float &modelSize);

    static ARPattHandle *ARPatternHandle;
    ARdouble gPatt_trans[3][4];
    QMatrix4x4 transformation;
    FbxImporter * mImporter;
    FbxManager *m_sdkManager;

    QVector<QVector4D> m_vertexes;

signals:
    void meshForCheck(FbxMesh* scene,FbxVector4* vertexes, FbxAMatrix matrix) const;

private:
    //type of marker
    ARMarkerType m_type = ARMarkerType::UNKNOWN;

    //draw functions based on m_type
    void drawLightSource() const;
    void drawObject() const;

    bool LoadFile();

    ARdouble gPatt_width = 80.0;
    int	m_patternId = -1;

    bool m_found = false;

    float m_modelSize = 1.0;
    FbxScene* m_fbxScene  = nullptr;

    void checkNodeRecursive(FbxNode* pNode);
    void checkNode(FbxNode* pNode);

    void DrawNode(FbxNode* pNode,
                  FbxTime& lTime,
                  FbxAnimLayer * pAnimLayer,
                  FbxAMatrix& pParentGlobalPosition,
                  FbxAMatrix& pGlobalPosition,
                  FbxPose* pPose,
                  ShadingMode pShadingMode) const;
    void DrawMarker(FbxAMatrix& pGlobalPosition) const;
    void DrawSkeleton(FbxNode* pNode,
                      FbxAMatrix& pParentGlobalPosition,
                      FbxAMatrix& pGlobalPosition) const;
    void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
                  FbxAMatrix& pGlobalPosition, FbxPose* pPose, ShadingMode pShadingMode) const;
    void ComputeShapeDeformation(FbxMesh* pMesh,
                                 FbxTime& pTime,
                                 FbxAnimLayer * pAnimLayer,
                                 FbxVector4* pVertexArray) const;
    void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition,
                                   FbxMesh* pMesh,
                                   FbxCluster* pCluster,
                                   FbxAMatrix& pVertexTransformMatrix,
                                   FbxTime pTime,
                                   FbxPose* pPose) const;
    void ComputeLinearDeformation(FbxAMatrix& pGlobalPosition,
                                  FbxMesh* pMesh,
                                  FbxTime& pTime,
                                  FbxVector4* pVertexArray,
                                  FbxPose* pPose) const;
    void ComputeDualQuaternionDeformation(FbxAMatrix& pGlobalPosition,
                                          FbxMesh* pMesh,
                                          FbxTime& pTime,
                                          FbxVector4* pVertexArray,
                                          FbxPose* pPose) const;
    void ComputeSkinDeformation(FbxAMatrix& pGlobalPosition,
                                FbxMesh* pMesh,
                                FbxTime& pTime,
                                FbxVector4* pVertexArray,
                                FbxPose* pPose) const;
    void ReadVertexCacheData(FbxMesh* pMesh,
                             FbxTime& pTime,
                             FbxVector4* pVertexArray) const;
    void DrawCamera(FbxNode* pNode,
                    FbxTime& pTime,
                    FbxAnimLayer* pAnimLayer,
                    FbxAMatrix& pGlobalPosition) const;
    void DrawLight(FbxNode* pNode, const FbxTime& pTime, const FbxAMatrix& pGlobalPosition) const;
    void DrawNull(FbxAMatrix& pGlobalPosition) const;
    void MatrixScale(FbxAMatrix& pMatrix, double pValue) const;
    void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue) const;
    void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix) const;

    void InitializeLights(FbxScene* pScene, const FbxTime & pTime, FbxPose* pPose) const;
    void DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
                           FbxAMatrix& pParentGlobalPosition, FbxPose* pPose,
                           ShadingMode pShadingMode) const;
};

#endif // ARMARKER_H
