#include <fbxsdk.h>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include "assert.h"

#define EPSLOOSE 0.1
#define EPS 1e-8
#define TOLERATE 2.0
class Vector{
private:
	double a[4];
	int _size;
public:

	Vector(double x, double y, double z) {
		_size = 3;
		a[0] = x;
		a[1] = y;
		a[2] = z;
	}

	Vector() { _size = 0; }
	Vector(int n) { _size = n; }
	Vector(int n, double x) {
		_size = n;
		for (int i = 0; i < n; i++)
			a[i] = x;
	}

	void push_back(double x) {
		a[_size] = x;
		_size += 1;
	}
	int size()const { return _size; }//没有const，上述代码将无法编译，因为您不能在const对象上调用可能修改对象状态的成员函数。

	double& operator[](int x) { return a[x]; }
	const double& operator [] (int x) const { return a[x]; }

	bool operator==(const Vector& other) {
		if (this->a[0] == other.a[0] && this->a[1] == other.a[1] && this->a[2] == other.a[2]) {
			return true;
		}
		else
			return false;
	}

	void swap(Vector& b) {
		assert(b.size() == _size);
		for (int i = 0; i < _size; i++)
			std::swap(a[i], b.a[i]);
	}
	void pop_back() { _size--; }
	void clear() { _size = 0; }
};

typedef std::pair<int, int> Edge;
typedef std::vector<Vector> Matrix;

class compareDate {
public:
	Edge edge;        //旧边
	Vector vertex;           //新点
	double cost;

	compareDate(const Edge& e, Vector v, double c) {
		edge = e;
		vertex = v;
		cost = c;
	}

	bool operator==(const compareDate& a) {
		if (this->cost == a.cost)
			return true;
		else
		{
			return false;
		}
	}

	bool operator<(const compareDate& a) const {
		return this->cost > a.cost;//确保创建的是一个小根堆；因为按照默认创建的是一个大根堆；
	}
};

//重新定义网格数据结构
class Mesh {
public:
	std::vector<Vector> vertex; //点
	std::vector<bool> removed;
	std::set<Edge> edge;        //边
	std::vector<std::set<Edge>> face;//面
	std::priority_queue<compareDate> heap;
	int faceN;

	void clear() {
		vertex.clear();
		edge.clear();
		faceN=0;
	}
};

Vector operator + (const Vector& a, const Vector& b) {
	assert(a.size() == b.size());
	Vector c(a.size());
	for (int i = 0; i < a.size(); i++)
		c[i] = a[i] + b[i];
	return c;
}

Matrix operator + (const Matrix& a, const Matrix& b) {
	assert(a.size() == b.size());
	Matrix c(a.size());
	for (int i = 0; i < a.size(); i++)
		c[i] = a[i] + b[i];
	return c;
}

Vector operator - (const Vector& a, const Vector& b) {
	assert(a.size() == b.size());
	Vector c(a.size());
	for (int i = 0; i < a.size(); i++)
		c[i] = a[i] - b[i];
	return c;
}

Vector operator * (const double& a, const Vector& b) {
	Vector c(b.size());
	for (int i = 0; i < b.size(); i++)
		c[i] = a * b[i];
	return c;
}

Vector operator / (const Vector& a, const double& b) {
	assert(b != 0);
	Vector c(a.size());
	for (int i = 0; i < a.size(); i++)
		c[i] = a[i] / b;
	return c;
}

Vector innerProduct1(const Vector& a, const Matrix& b) {
	assert(a.size() == b.size());
	if (a.size() == 0) return Vector();
	Vector c(b[0].size(), 0);
	for (int i = 0; i < b.size(); i++)
		for (int j = 0; j < b[0].size(); j++)
			c[j] += a[i] * b[i][j];
	return c;
}

void printVector(const Vector& v) {
	for (int i = 0; i < v.size(); i++)
		printf("%.4lf\t", v[i]);
	printf("\n");
}

void printMatrx(const Matrix& m) {
	for (auto v : m)
		printVector(v);
}

class QEM {

public:
	Mesh* lMesh;
	void DisplayDate(FbxNode* lNode, Mesh* lMesh) {
		//std::set<std::pair<double, std::pair<double, double>>> uniqueVertices;//用set去除控制点中重复坐标
		//初始化面
		FbxMesh* cMesh = (FbxMesh*)lNode->GetNodeAttribute();
		FBXSDK_printf("Mesh name:%s\n", (char*)lNode->GetName());
		//读取顶点
		int i, l, lControlPointsCount = cMesh->GetControlPointsCount();
		FbxVector4* lControlPoints = cMesh->GetControlPoints();
		for (i = 0; i < lControlPointsCount; i++)
		{
			//std::pair<double, std::pair<double, double>> p(lControlPoints[i][0], std::make_pair(lControlPoints[i][1], lControlPoints[i][2]));
			//uniqueVertices.insert(p);
			Vector vertex(lControlPoints[i][0], lControlPoints[i][1], lControlPoints[i][2]);
			lMesh->vertex.push_back(vertex);
		}
		/*for (const auto& p : uniqueVertices)
		{
			Vector vertex(p.first, p.second.first, p.second.second);
			lMesh->vertex.push_back(vertex);
		}
		uniqueVertices.clear();*/
		int vertexN = lControlPointsCount;
		lMesh->removed.resize(vertexN, false);
		lMesh->face.resize(cMesh->GetControlPointsCount());
		//std::cout << "poly number" << cMesh->GetPolygonCount() << "\n";
		//读取面
		for (int i = 0; i < cMesh->GetPolygonCount(); i++)
		{

			int temp[3];
			for (int j = 0; j < cMesh->GetPolygonSize(i); j++)
			{
				//std::cout << "PolygonSize "<<"-" << i << "-" << cMesh->GetPolygonSize(i) << "\n";

				int ControlPointIndex = cMesh->GetPolygonVertex(i, j);
				temp[j] = ControlPointIndex;
			}

			lMesh->face[temp[0]].insert(std::make_pair(temp[1], temp[2]));
			lMesh->face[temp[1]].insert(std::make_pair(temp[0], temp[2]));
			lMesh->face[temp[2]].insert(std::make_pair(temp[0], temp[1]));
			std::sort(temp,temp+3);
			lMesh->edge.insert(std::make_pair(temp[0], temp[1]));
			lMesh->edge.insert(std::make_pair(temp[1], temp[2]));
			lMesh->edge.insert(std::make_pair(temp[0], temp[2]));
		}
		lMesh->faceN = cMesh->GetPolygonCount();
	}

	void DisplayMesh(FbxNode* lNode, Mesh* lMesh) {
		int i;
		if (lNode->GetNodeAttribute() == NULL)
		{
			FBXSDK_printf("NULL Node Attribute\n\n");
		}
		else
		{
			if (lNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				DisplayDate(lNode, lMesh);
			}
		}
		for (i = 0; i < lNode->GetChildCount(); i++) {
			DisplayMesh(lNode->GetChild(i),lMesh);
		}
	}

	void DisplayScene(FbxScene* lScene,Mesh* lMesh) {
		FbxNode* lNode = lScene->GetRootNode();
		int i;
		if (lNode) 
		{
			for (i = 0; i < lNode->GetChildCount(); i++) 
			{
				DisplayMesh(lNode->GetChild(i), lMesh);
			}
		}
	}

	void loadFile(const char* lFilename) {
		//lMesh->clear();

		// 管理器对象
		FbxManager* lSdkManager = FbxManager::Create();
		//IO设置
		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);
		//导入器
		FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
		bool lImportStatus = lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings());
		//新建场景对象
		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		lImporter->Import(lScene);
		if (!lImportStatus) {
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
			exit(-1);
		}
		lMesh = new Mesh();
		DisplayScene(lScene, lMesh);
	}

	void saveToFile(std::string filename) {
		FILE* file = fopen(filename.c_str(), "w");
		size_t vertexN = lMesh->vertex.size();
		std::vector<int> vertexID(vertexN, 0);
		int vertexReal = 0;

		for (int i = 0; i < vertexN; i++) {
			if (lMesh->removed[i]) continue;
			vertexID[i] = ++vertexReal;
			fprintf(file, "v %.8lf %.8lf %.8lf\n", lMesh->vertex[i][0], lMesh->vertex[i][1], lMesh->vertex[i][2]);
		}

		for (int i = 0; i < vertexN; i++) {
			if (lMesh->removed[i]) continue;
			for (const auto& f : lMesh->face[i]) {
				assert(!lMesh->removed[f.first] && !lMesh->removed[f.second]);
				assert(vertexID[f.first] && vertexID[f.second] && vertexID[i]);
				if (i < f.first && i < f.second) {
					fprintf(file, "f %d %d %d\n", vertexID[i], vertexID[f.first], vertexID[f.second]);
				}
			}
		}
	}

	Vector crossProduct(const Vector &a,const Vector &b) {
		assert(a.size() == 3 && b.size() == 3);
		Vector c(3);
		c[0] = a[1] * b[2] - a[2] * b[1];
		c[1] = a[2] * b[0] - a[0] * b[2];
		c[2] = a[0] * b[1] - a[1] * b[0];
		return c;
	}

	double norm(const Vector& a) {
		double t=0;
		for (int i = 0; i < a.size(); i++) {
			t += a[i] * a[i];
		}
		return sqrt(t);
	}

	void outProductFast(const Vector& a, const Vector& b, Matrix& q) {
		for (int i = 0; i < a.size(); i++) {
			for (int j = 0; j < b.size(); j++) {
				q[i][j] += a[i] * b[j];
			}
		}
	}

	double innerProduct(const Vector& a, const Vector& b) {
		double temp=0;
		for (int i = 0; i < a.size(); i++) {
			temp += a[i] * b[i];
		}
		return temp;
	}

	Vector solveEquation(Matrix m, int n) {
		assert(m.size() >= n);
		if (m.size() == 0) return Vector();
		assert(m[0].size() > n);
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++)
				if (fabs(m[i][i]) < fabs(m[j][i])) m[i].swap(m[j]);
			if (fabs(m[i][i]) < EPSLOOSE) throw 200;
			m[i] = m[i] / m[i][i];
			for (int j = i + 1; j < n; j++)
				m[j] = m[j] - m[j][i] * m[i];
		}
		Vector v(n);
		for (int i = n - 1; i >= 0; i--) {
			assert(fabs(m[i][i] - 1) < EPS);
			v[i] = -m[i][n];
			for (int j = i + 1; j < n; j++) {
				v[i] -= m[i][j] * v[j];
			}
		}

		return v;
	}

	std::pair<Vector, double> cost(const Edge& e) {
		Matrix q(4, Vector(4, 0));
		for (const auto &f : lMesh->face[e.first]) {
			auto a1 = (lMesh->vertex[f.first] - lMesh->vertex[e.first]);
			auto a2 = (lMesh->vertex[f.second] - lMesh->vertex[e.first]);
			auto n = crossProduct(a1, a2); 
			if (norm(n)==0) continue;
			if(norm(n)==0){
				std::cout << "a1"<<"\n";
				printVector(n);
				printVector(a1);
				printVector(a2);
				std::cout << f.first <<"-" << f.second<<"-" << e.first<<"\n";
			}
			n = n / norm(n);
			n.push_back(-innerProduct(lMesh->vertex[e.first], n));//ax+by+cz+d=0，计算d
			outProductFast(n, n, q);
		}

		for (const auto& f : lMesh->face[e.second]) {
			auto a1 = (lMesh->vertex[f.first] - lMesh->vertex[e.second]);
			auto a2 = (lMesh->vertex[f.second] - lMesh->vertex[e.second]);
			auto n = crossProduct(a1, a2);
			if (norm(n) == 0) continue;
			n = n / norm(n);
			n.push_back(-innerProduct(lMesh->vertex[e.second], n));
			outProductFast(n, n, q);
		}

		Vector v;
		try {
			v = solveEquation(q, 3);
		}
		catch (...) {
			v = (lMesh->vertex[e.first] + lMesh->vertex[e.second]) / 2;
		}
		if (norm(v - lMesh->vertex[e.first]) + norm(v - lMesh->vertex[e.second]) > TOLERATE * norm(lMesh->vertex[e.first] - lMesh->vertex[e.second])) {
			v = (lMesh->vertex[e.first] + lMesh->vertex[e.second]) / 2;
		}
		v.push_back(1);
		double cost = innerProduct(innerProduct1(v, q), v);
		assert(cost > -EPS);
		v.pop_back();
		return std::make_pair(v, cost);
	}

	void addHeap(const Edge& e) {
		std::pair<Vector, double> cost1;
		cost1 = cost(e);
		compareDate tmp(e, cost1.first, cost1.second);
		lMesh->heap.push(tmp);
	}

	void removeEdge( Edge& e,const Vector& v) {
		
		//更新面；收缩边V1-V2；即将V2点合并到V1点，遍历V2点周围每个面；先分别更新三角形与e.second相邻两个点，再更新第三个点V1
		for (const auto& f : lMesh->face[e.second])
		{
			//1.更新三角形与e.second相邻两个点
			//非常重要，这里删除的时候正反都要尝试，因为set是有顺序的，而我写入没有严格按照顺序；否则会导致删除不干净，有些面没有删除，后续计算出现报错；
			//即：我在面里面没有删除，但是实际顶点已经删除，就导致访问错误。
			lMesh->face[f.second].erase(std::make_pair(e.second,f.first));
			lMesh->face[f.second].erase(std::make_pair( f.first, e.second));
			if (f.first != e.first && f.second != e.first)
			{
				lMesh->face[f.second].insert(std::make_pair(e.first,f.first));
			}
			//2.更新三角形与e.second相邻两个点
			lMesh->face[f.first].erase(std::make_pair(f.second, e.second));
			lMesh->face[f.first].erase(std::make_pair(e.second,f.second));
			if (f.first != e.first && f.second != e.first)
			{
				lMesh->face[f.first].insert(std::make_pair(f.second, e.first));
			}
			//3.更新第三个点V1
			if (f.first == e.first || f.second == e.first) {
				lMesh->faceN--;
			}
			else
			{
				lMesh->face[e.first].insert(f);
			}

			//更新边
			auto temp = std::make_pair(std::min(f.first, e.second), std::max(f.first, e.second));
			if (lMesh->edge.find(temp) != lMesh->edge.end())
				lMesh->edge.erase(temp);
			temp = std::make_pair(std::min(f.second, e.second), std::max(f.second, e.second));
			if (lMesh->edge.find(temp) != lMesh->edge.end())
				lMesh->edge.erase(temp);
			if (f.first != e.first && f.second != e.first)
			{
				lMesh->edge.insert(std::make_pair(std::min(f.first, e.first), std::max(f.first, e.first)));
				lMesh->edge.insert(std::make_pair(std::min(f.second, e.first), std::max(f.second, e.first)));
			}
		}
		lMesh->edge.erase(e);
		lMesh->vertex[e.first] = v;
		lMesh->vertex[e.second].clear();
		lMesh->face[e.second].clear();
		lMesh->removed[e.second] = true;

		//局部更新cost
		for (const auto& f : lMesh->face[e.first])
		{
			addHeap(std::make_pair(std::min(f.first, e.first), std::max(f.first, e.first)));
		}
	}
};

int main(int argc, char** argv) {
	QEM qem;
	//const char* filepath = "E:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\samples\\ViewScene\\humanoid.fbx";
	//const char* filepath = "C:\\Users\\mr\\Downloads\\airgun\\Air_Gun-FBX.fbx";
	//std::string outputModelFileName = "C:\\Users\\mr\\Downloads\\mesh-simplification-master\\cmake-build-debug\\results\\man0.9.obj";

	//dragon
	/*const char* filepath = "D:\\download\\fbx\\Dragon 2.5_fbx.fbx";
	std::string outputModelFileName = "D:\\download\\fbx\\out\\Dragon0.9.obj";*/

	//car
	const char* filepath = "D:\\download\\32-mercedes-benz-gls-580-2020\\uploads_files_2787791_Mercedes+Benz+GLS+580.fbx";
	std::string outputModelFileName = "D:\\download\\fbx\\out\\Benz.obj";

	qem.loadFile(filepath);

	std::cout << "threshold" << std::endl;
	int threshold = 0.9*qem.lMesh->vertex.size();
	std::cout <<"    " << threshold << "/" << qem.lMesh->vertex.size() << "\n";

	std::cout << "mesh" << std::endl;
	std::cout << "    vertex:" << qem.lMesh->vertex.size()  << "\n";
	std::cout << "    edge:" << qem.lMesh->edge.size() << "\n";
	std::cout << "    face:" << qem.lMesh->faceN << "\n";

	////全局更新算法->每当删除一条边后重新计算所有边的cost
	//for (int i = 0; i < threshold; i++)
	//{
	//	printf("%c%zu ", 13, i);
	//	//std::cout << i << "\n";
	//	//计算每条边cost
	//	std::pair<Vector, double> cost1;
	//	std::vector<compareDate> costRecord;
	//	for (auto const &e:qem.lMesh->edge) {
	//		cost1 = qem.cost(e);
	//		compareDate tmp(e, cost1.first, cost1.second);
	//		costRecord.push_back(tmp);
	//	}
	//	std::sort(costRecord.begin(), costRecord.end(), std::less<compareDate>());
	//	compareDate e_remove = costRecord[0];
	//	qem.removeEdge(e_remove.edge, e_remove.vertex);
	//}

	//局部更新算法->只更新修改部分的cost;利用堆排序算法；
	int i = 0;
	for (const auto& e : qem.lMesh->edge) {
		printf("%c%zu ", 13, i);
		qem.addHeap(e);
		i++;
	}

	int count=0;
	while (true)
	{
		compareDate tmp = qem.lMesh->heap.top();
		qem.lMesh->heap.pop();
		if (!qem.lMesh->removed[tmp.edge.first] && !qem.lMesh->removed[tmp.edge.second])
		{
			if (count < threshold) {
				printf("%c%zu ", 13, count);
				qem.removeEdge(tmp.edge, tmp.vertex);
				count++;
			}
			else
				break;
		}
		//i++;
	}
	qem.saveToFile(outputModelFileName);
}