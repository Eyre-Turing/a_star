#ifndef A_STAR_H
#define A_STAR_H

#include "map_matrix.h"
#include <stack>
#include <vector>
#include <queue>

struct GetPathConditionSet
{
    int beginR;
    int beginC;
    int endR;
    int endC;
    double minCondition;
};

class AStar
{
public:
    AStar();
    ~AStar();

    std::stack<MapPos> getPath(const Matrix *mapMatrix, bool obliqueEnable = true, bool obliqueEasy = false, bool usePriorityQueue = true,
                          const struct GetPathConditionSet *settings = NULL);  //obliqueEnable为真是八方向的，为假是四方向的
    void setHandleAliveNodeCallBack(void (*callBack)(AStar *aStar, MapPos mapPos));

    double getCurBestCost() const;
    double getPosCost(MapPos mapPos) const;

    void stopGetPath();

    int aliveNodeSize() const;
    bool running() const;

private:
    double sqrt2;
    Matrix *matrix;

    bool searchOneUp(int r, int c);
    bool searchOneDown(int r, int c);
    bool searchOneLeft(int r, int c);
    bool searchOneRight(int r, int c);

    bool searchOneLeftUp(int r, int c);
    bool searchOneRightUp(int r, int c);
    bool searchOneLeftDown(int r, int c);
    bool searchOneRightDown(int r, int c);

    bool searchOne(int r, int c, double lowerBound);
    void searchAll();
    bool moveToTarget(MapNode *beg, MapNode *tag, int tr, int tc);
    bool moveToTargetWhenDiag(MapNode *tOffsetR, MapNode *tOffsetC, MapNode *beg, MapNode *tag, int tr, int tc);

    int rows, cols;
    int endR, endC;
    int beginR, beginC;
    double minCondition;

    double lowerBoundFunction(int r, int c) const;
    double curBestCost;
    bool isBestThanCurBest(double value) const;
    bool isVar1BestThanVar2(double var1, double var2) const;

    class cmp
    {
    public:
        bool operator()(const MapPos &a, const MapPos &b) const;
    };

    std::priority_queue<MapPos, std::vector<MapPos>, cmp> aliveNodeP;
    std::queue<MapPos> aliveNode;

    void (*handleAliveNodeCallBack)(AStar *aStar, MapPos mapPos);   //回调参数：当前AStar对象、当前扩展结点位置

    bool isRunnable;
    bool isObliqueEnable;
    bool isObliqueEasy;
    bool isUsePriorityQueue;
};

#endif // A_STAR_H
