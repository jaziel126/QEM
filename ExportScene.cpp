#include <fbxsdk.h>

//Function prototypes.

bool CreateScene(FbxScene* lScene);

FbxNode* CreateCubeWithTexture(FbxScene* lScene, const char* CubeName);
FbxNode* CreatePyramidWithMaterials(FbxScene* lScene, const char* PyramidName);

static const char* gDiffuseElementName = "DiffuseUV";
//主函数
// 
//1.创建管理器对象
//2.IoSetting
//3.创建场景
//4.保存场景

int main(int argc, char** argv)
{
	//准备sdk
	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	//创建场景
	FbxScene* lScene = FbxScene::Create(lSdkManager, "My Scene");
	bool lResult;
	lResult = CreateScene(lScene);

	//保存场景
	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");
	const char* FilePath = "E:\\Users\\mr\\source\\repos\\pbx_sample\\pbx_sample\\ExportScene03.fbx";
	bool ExportStatus = lExporter->Initialize(FilePath, -1, lSdkManager->GetIOSettings());

	if (ExportStatus == false)
	{
		FBXSDK_printf("导入失败");
	}
	lExporter->Export(lScene);

	return 0;
}

//创建场景函数
// 
// 1.创建节点
// 2.创建节点姿态
// 3.构建节点树
//
bool CreateScene(FbxScene* lScene)
{

	FbxNode* lCube = CreateCubeWithTexture(lScene, "Cube");
	FbxNode* pyramid = CreatePyramidWithMaterials(lScene, "Pyramid");

	//build the node tree
	FbxNode* lRootNode = lScene->GetRootNode();
	lRootNode->AddChild(lCube);
	lRootNode->AddChild(pyramid);

	return true;
}

//创建立方体网格并带有纹理
FbxNode* CreateCubeWithTexture(FbxScene* lScene, const char* CubeName)
{
	//FbxMesh->
	// lMesh->GetControlPoints()
	// lMesh->createElementNormal()
	// lMesh->CreateElementUV(gDiffuseElementName)
	// lMesh->AddPolygon
	// 
	//初始化
	int i, j;
	FbxMesh* lMesh = FbxMesh::Create(lScene, CubeName);

	// 检查网格是否创建成功
	if (!lMesh)
	{
		// 处理错误，例如输出错误信息或返回nullptr
		FBXSDK_printf("创建对象失败");
	}

	FbxVector4 lControlPoint0(-50, 0, 50);
	FbxVector4 lControlPoint1(50, 0, 50);
	FbxVector4 lControlPoint2(50, 100, 50);
	FbxVector4 lControlPoint3(-50, 100, 50);
	FbxVector4 lControlPoint4(-50, 0, -50);
	FbxVector4 lControlPoint5(50, 0, -50);
	FbxVector4 lControlPoint6(50, 100, -50);
	FbxVector4 lControlPoint7(-50, 100, -50);

	FbxVector4 lNormalXPos(1, 0, 0);
	FbxVector4 lNormalXNeg(-1, 0, 0);
	FbxVector4 lNormalYPos(0, 1, 0);
	FbxVector4 lNormalYNeg(0, -1, 0);
	FbxVector4 lNormalZPos(0, 0, 1);
	FbxVector4 lNormalZNeg(0, 0, -1);

	//Create Controls points 立方体，8个定点6个面，24个控制点

	lMesh->InitControlPoints(24);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;

	lControlPoints[4] = lControlPoint1;
	lControlPoints[5] = lControlPoint5;
	lControlPoints[6] = lControlPoint6;
	lControlPoints[7] = lControlPoint2;

	lControlPoints[8] = lControlPoint5;
	lControlPoints[9] = lControlPoint4;
	lControlPoints[10] = lControlPoint7;
	lControlPoints[11] = lControlPoint6;

	lControlPoints[12] = lControlPoint4;
	lControlPoints[13] = lControlPoint0;
	lControlPoints[14] = lControlPoint3;
	lControlPoints[15] = lControlPoint7;

	lControlPoints[16] = lControlPoint3;
	lControlPoints[17] = lControlPoint2;
	lControlPoints[18] = lControlPoint6;
	lControlPoints[19] = lControlPoint7;

	lControlPoints[20] = lControlPoint1;
	lControlPoints[21] = lControlPoint0;
	lControlPoints[22] = lControlPoint4;
	lControlPoints[23] = lControlPoint5;

	//设置法线映射 mapping model：eByControlPoint,每个点只有一个法线，适用于光滑曲面

	FbxGeometryElementNormal* lGeometryElementNormal = lMesh->CreateElementNormal();
	lGeometryElementNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);

	//法线值设置模式 通过索引或者直接通过点本身
	lGeometryElementNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalZNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalXNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYPos);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);
	lGeometryElementNormal->GetDirectArray().Add(lNormalYNeg);

	//面顶点数组
	int lPolygonVertices[] = {
		0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };

	//Create UV for Diffuse channel
	//新建UV对象，设置映射以及查找方式，赋值
	FbxGeometryElementUV* lUVDiffuseElement = lMesh->CreateElementUV(gDiffuseElementName);
	lUVDiffuseElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVDiffuseElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	FbxVector2 lVectors0(0, 0);
	FbxVector2 lVectors1(1, 0);
	FbxVector2 lVectors2(1, 1);
	FbxVector2 lVectors3(0, 1);

	lUVDiffuseElement->GetDirectArray().Add(lVectors0);
	lUVDiffuseElement->GetDirectArray().Add(lVectors1);
	lUVDiffuseElement->GetDirectArray().Add(lVectors2);
	lUVDiffuseElement->GetDirectArray().Add(lVectors3);

	//Now we have set the UVs as eIndexToDirect reference and in eByPolygonVertex  mapping mode
	//we must update the size of the index array.
	lUVDiffuseElement->GetIndexArray().SetCount(24);

	//create polygons.
	for (i = 0; i < 6; i++)
	{
		lMesh->BeginPolygon(-1, -1, false);

		for (j = 0; j < 4; j++)
		{
			lMesh->AddPolygon(lPolygonVertices[i * 4 + j]);
			lUVDiffuseElement->GetIndexArray().SetAt(i * 4 + j, j);
		}
		lMesh->EndPolygon();
	}

	//创建节点，把mesh赋值给节点
	FbxNode* lNode = FbxNode::Create(lScene, CubeName);

	lNode->SetNodeAttribute(lMesh);

	return lNode;
}

//创建金字塔并带有材质
FbxNode* CreatePyramidWithMaterials(FbxScene* lScene, const char* PyramidName)
{
	//1.fbxmesh
	// 2.mesh->getcontrolpoints()
	// 3.mesh->createlementnormal()
	// 4.mesh->createlelmentUV()
	// 5.mesh->addpolygon()
	//
	int i, j;
	FbxMesh* lMesh = FbxMesh::Create(lScene, PyramidName);

	FbxVector4 lControlPoint0(-50, 0, 50);
	FbxVector4 lControlPoint1(50, 0, 50);
	FbxVector4 lControlPoint2(50, 0, -50);
	FbxVector4 lControlPoint3(-50, 0, -50);
	FbxVector4 lControlPoint4(0, 100, 0);

	FbxVector4 lNormalP0(0, 1, 0);
	FbxVector4 lNormalP1(0, 0.447, 0.894);
	FbxVector4 lNormalP2(0.894, 0.447, 0);
	FbxVector4 lNormalP3(0, 0.447, -0.894);
	FbxVector4 lNormalP4(-0.894, 0.447, 0);

	//create controlpoints
	lMesh->InitControlPoints(16);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	lControlPoints[0] = lControlPoint0;
	lControlPoints[1] = lControlPoint1;
	lControlPoints[2] = lControlPoint2;
	lControlPoints[3] = lControlPoint3;

	lControlPoints[4] = lControlPoint0;
	lControlPoints[5] = lControlPoint1;
	lControlPoints[6] = lControlPoint4;

	lControlPoints[7] = lControlPoint1;
	lControlPoints[8] = lControlPoint2;
	lControlPoints[9] = lControlPoint4;

	lControlPoints[10] = lControlPoint2;
	lControlPoints[11] = lControlPoint3;
	lControlPoints[12] = lControlPoint4;

	lControlPoints[13] = lControlPoint3;
	lControlPoints[14] = lControlPoint0;
	lControlPoints[15] = lControlPoint4;

	//set noemal for each controlpoints
	FbxGeometryElementNormal* lNormal = lMesh->CreateElementNormal();
	lNormal->SetMappingMode(FbxGeometryElement::eByControlPoint);
	lNormal->SetReferenceMode(FbxGeometryElement::eDirect);

	lNormal->GetDirectArray().Add(lNormalP0);
	lNormal->GetDirectArray().Add(lNormalP0);
	lNormal->GetDirectArray().Add(lNormalP0);
	lNormal->GetDirectArray().Add(lNormalP0);
	lNormal->GetDirectArray().Add(lNormalP1);
	lNormal->GetDirectArray().Add(lNormalP1);
	lNormal->GetDirectArray().Add(lNormalP1);
	lNormal->GetDirectArray().Add(lNormalP2);
	lNormal->GetDirectArray().Add(lNormalP2);
	lNormal->GetDirectArray().Add(lNormalP2);
	lNormal->GetDirectArray().Add(lNormalP3);
	lNormal->GetDirectArray().Add(lNormalP3);
	lNormal->GetDirectArray().Add(lNormalP3);
	lNormal->GetDirectArray().Add(lNormalP4);
	lNormal->GetDirectArray().Add(lNormalP4);
	lNormal->GetDirectArray().Add(lNormalP4);

	//arry of polygon vertices.
	int lPolygonVertices[] = { 0, 3, 2, 1,
		4, 5, 6,
		7, 8, 9,
		10, 11, 12,
		13, 14, 15 };

	//set material mapping
	FbxGeometryElementMaterial* lMaterial = lMesh->CreateElementMaterial();
	lMaterial->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterial->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	//create polygons.
	lMesh->BeginPolygon(0);
	for (j = 0; j < 4; j++)
	{
		lMesh->AddPolygon(lPolygonVertices[j]);
	}
	lMesh->EndPolygon();

	for (i = 1; i < 5; i++)
	{
		lMesh->BeginPolygon(i);
		for (j = 0; j < 3; j++)
		{
			lMesh->AddPolygon(lPolygonVertices[4 + 3 * (i - 1) + j]);
		}
		lMesh->EndPolygon();
	}

	FbxNode* lNode = FbxNode::Create(lScene, PyramidName);
	lNode->SetNodeAttribute(lMesh);

	return lNode;
}