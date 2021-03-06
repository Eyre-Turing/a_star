#ifndef MAP_MATRIX_H
#define MAP_MATRIX_H

#include <vector>
#include <string>

struct MapPos
{
    int r;
    int c;
    double lowerBound;
    double cost;
};

struct MapNode
{
    bool isWall;            //真则为墙，假则为路
    bool isUsed;            //真则为已经到过的点（包括起点）
    bool isEnd;             //真则为终点
    double condition;       //从其他点到该点的代价比例（1为正常，0为极端通畅，正无穷为极端堵塞，用负数表示正无穷）
    double cost;            //从起点到该点的消耗值，给AStar计算路线时提供存储空间
    struct MapNode *prior;  //记录它的上一个地图节点，用来保存路线
    void *reserve;          //给Matrix保留的指针，现在已用来指向地图中该结点的往上一个格子
    bool isInAliveNodes;    //该结点是否已经在活结点队列里，用于去重复
};

/*
 * 在Matrix里，MapNode的reserve指向它上方的MapNode
 * 比如[i][j]位置的MapNode的reserve指向[i-1][j]位置，[-1][j]认为是NULL
 */
class Matrix
{
public:
    Matrix(int height, int width);
    ~Matrix();

    Matrix(const Matrix &matrix);

    void resize(int height, int width);

    struct MapNode *get(int row, int col);
    const struct MapNode *getConst(int row, int col) const;
    bool set(int row, int col, const struct MapNode &mapNode);    //返回是否设置成功

    void nodePos(struct MapNode *mapNode, int &row, int &col) const;    //根据节点，获取该节点在矩阵的位置

    void readFromText(std::string text, int *height = NULL, int *width = NULL, int *beginX = NULL, int *beginY = NULL, int *endX = NULL, int *endY = NULL, double *minCondition = NULL);

    void readFromFile(std::string filename, int *rows = NULL, int *cols = NULL, int *beginX = NULL, int *beginY = NULL, int *endX = NULL, int *endY = NULL, double *minCondition = NULL);

    std::string toString() const;

    void writeToFile(std::string filename) const;

    struct MapPos size() const;

private:
    std::vector<std::vector<struct MapNode*> > mapMatrix;    //mapMatrix[i][j]表示第i行第j列

    void loadOneFromStr(const char *p, int ri, int ci, int *beginX = NULL, int *beginY = NULL, int *endX = NULL, int *endY = NULL, double *minCondition = NULL);
};

#endif // MAP_MATRIX_H
