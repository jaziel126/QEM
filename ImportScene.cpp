#include <fbxsdk.h>

//读取控制点
void DisplayControlsPoints(FbxMesh* lMesh) {
	//通过给定函数获取控制点数量以及坐标数组
	int i,l, lControlPointsCount = lMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = lMesh->GetControlPoints();
	FBXSDK_printf("    Control Points\n");
	//通过循环遍历打印  Control Point 0
	for (i = 0; i < lControlPointsCount; i++)
	{
		FBXSDK_printf("        Control Points %d\n", i);
		FBXSDK_printf("            Coordinates: %f, %f, %f\n", (float)lControlPoints[i][0], (float)lControlPoints[i][1], (float)lControlPoints[i][2]);
		for (l = 0; l < lMesh->GetElementNormalCount(); l++)
		{
			FbxGeometryElementNormal* lNormal = lMesh->GetElementNormal(l);
			if (lNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{
				if (lNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
				{
					FBXSDK_printf("            Normal: %f,%f,%f\n", lNormal->GetDirectArray().GetAt(i)[0], lNormal->GetDirectArray().GetAt(i)[1], lNormal->GetDirectArray().GetAt(i)[2]);
				}
			}
		}
	}
}


//面
void DisplayPolygons(FbxMesh* lMesh) {
	//获取面数以及控制点坐标
	int i, j, lPolygonCount = lMesh->GetPolygonCount();
	FbxVector4* lControlPoints = lMesh->GetControlPoints();
	FBXSDK_printf("\n    Polygons\n");
	//遍历每个面 Polygon 0
	int vertexId = 0;
	for (i = 0; i < lPolygonCount; i++)
	{
		FBXSDK_printf("        Polygons %d\n",i);
		int l;
		//遍历面每个控制点
		for (j = 0; j < lMesh->GetPolygonSize(i); j++) {
			//Coordinates
			int ControlPointIndex = lMesh->GetPolygonVertex(i, j);
			FBXSDK_printf("            Coordinates: %f, %f, %f\n", (float)lControlPoints[ControlPointIndex][0], (float)lControlPoints[ControlPointIndex][1], (float)lControlPoints[ControlPointIndex][2]);

			//Texture UV
			for (l = 0; l < lMesh->GetElementUVCount(); ++l)
			{
				//FBXSDK_printf("UV number:%d\n", lMesh->GetElementUVCount());
				FbxGeometryElementUV* leUV = lMesh->GetElementUV(l);
				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					FBXSDK_printf("eByControlPoint\n");
				case FbxGeometryElement::eByPolygonVertex:
					//每个顶点在其所属的每个多边形中都有一个映射坐标。
					// 这意味着一个顶点属于多少个多边形，就有多少个映射坐标。
					if (leUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{

					}
					int lTextureUVIndex = lMesh->GetTextureUVIndex(i, j);
					FBXSDK_printf("            Texture UV: %f, %f\n", leUV->GetDirectArray().GetAt(lTextureUVIndex)[0], leUV->GetDirectArray().GetAt(lTextureUVIndex)[1]);
				}
			}

			//normal
			//FBXSDK_printf("normal number: %d\n", lMesh->GetElementNormalCount());
			for (l = 0; l < lMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = lMesh->GetElementNormal();
				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						FBXSDK_printf("            Normal: %f, %f, %f\n", leNormal->GetDirectArray().GetAt(vertexId)[0], leNormal->GetDirectArray().GetAt(vertexId)[1], leNormal->GetDirectArray().GetAt(vertexId)[2]);
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						FBXSDK_printf("            Normal: %f, %f, %f\n", leNormal->GetDirectArray().GetAt(id)[0], leNormal->GetDirectArray().GetAt(id)[1], leNormal->GetDirectArray().GetAt(id)[2]);
						break;
					}
					default:
						break;
					}
				}
			}
			vertexId++;
		}

	}
}

//读取网格的节点
void DisplayMesh(FbxNode* lNode) {
	FbxMesh* lMesh = (FbxMesh*)lNode->GetNodeAttribute();
	FBXSDK_printf("Mesh name:%s\n", (char*)lNode->GetName());
	DisplayControlsPoints(lMesh);
	DisplayPolygons(lMesh);
}


//读取场景下的节点
void DisplayContent(FbxNode* lNode) {
	FbxNodeAttribute::EType lAttributeType;
	int i;

	if (lNode->GetNodeAttribute() == NULL)
	{	
		FBXSDK_printf("NULL Node Attribute\n\n");
	}
	else
	{
		lAttributeType = (lNode->GetNodeAttribute()->GetAttributeType());

		if (lAttributeType == FbxNodeAttribute::eMesh)
		{
			DisplayMesh(lNode);
		}
	}
	for (i = 0; i < lNode->GetChildCount(); i++)
	{
		DisplayContent(lNode->GetChild(i));
	}
}

//读取场景图函数
void DisplayContent(FbxScene* lScene) {

	FbxNode* lNode = lScene->GetRootNode();

	int i;
	if (lNode) {
		for (i = 0; i < lNode->GetChildCount(); i++) {
			DisplayContent(lNode->GetChild(i));
		}
	}
}

int main(int argc, char** argv) {
	//管理器对象
	FbxManager* lSdkManager = FbxManager::Create();

	//IO设置
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	//导入器
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	//const char* lFilename = "E:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\samples\\Camera\\Camera.fbx";
    //const char* lFilename = "E:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\samples\\ExportDocument\\ExportDocument.fbx";
    const char* lFilename = "E:\\Users\\mr\\source\\repos\\pbx_sample\\pbx_sample\\ExportScene03.fbx";
	bool lImportStatus = lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings());

	//新建场景对象
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	if (!lImportStatus) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	//get version number
	int lFileMajor, lFileMinor, FileRevision;
	lImporter->GetFileVersion(lFileMajor, lFileMinor, FileRevision);

	FBXSDK_printf("fileVersion:---------- %d.%d.%d\n\n", lFileMajor, lFileMinor, FileRevision);

	//读取节点内容，包括控制点、面、法线、UV
	DisplayContent(lScene);
}