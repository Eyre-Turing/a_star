#include "a_star.h"
#include <math.h>

using namespace std;

bool AStar::cmp::operator()(const MapPos &a, const MapPos &b) const
{
    return a.lowerBound > b.lowerBound;
}

AStar::AStar()
{
    sqrt2 = sqrt(2);
    handleAliveNodeCallBack = NULL;
    matrix = NULL;
}

AStar::~AStar()
{

}

void AStar::setHandleAliveNodeCallBack(void (*callBack)(AStar *, MapPos))
{
    handleAliveNodeCallBack = callBack;
}

bool AStar::moveToTarget(MapNode *beg, MapNode *tag, int tr, int tc)
{
    if(tag->isWall || beg->isEnd || tag->condition<0)
    {
        return false;
    }

    double distance = 1*(tag->condition);

    if(isVar1BestThanVar2(beg->cost+distance, tag->cost))
    {
        tag->cost = beg->cost+distance;
        tag->prior = beg;
        tag->isUsed = true;

        //到达终点，刷新当前最优值
        if(tag->isEnd)
        {
            if(isBestThanCurBest(tag->cost))
            {
                curBestCost = tag->cost;
            }
        }
        else if(!(tag->isInAliveNodes))    //否则把该结点加入到活结点队列
        {
            MapPos mapPos;
            mapPos.r = tr;
            mapPos.c = tc;
            mapPos.lowerBound = lowerBoundFunction(tr, tc);
            if(isUsePriorityQueue)
            {
                aliveNodeP.push(mapPos);
            }
            else
            {
                aliveNode.push(mapPos);
            }
            tag->isInAliveNodes = true;
        }

        return true;
    }

    return false;
}

bool AStar::moveToTargetWhenDiag(MapNode *tOffsetR, MapNode *tOffsetC, MapNode *beg, MapNode *tag, int tr, int tc)
{
    if(tag->isWall || (isObliqueEasy?(tOffsetR->isWall&&tOffsetC->isWall):(tOffsetR->isWall||tOffsetC->isWall)) || beg->isEnd || tag->condition<0)
    {
        return false;
    }

    double distance = sqrt2*(tag->condition);

    if(isVar1BestThanVar2(beg->cost+distance, tag->cost))
    {
        tag->cost = beg->cost+distance;
        tag->prior = beg;
        tag->isUsed = true;

        //到达终点，刷新当前最优值
        if(tag->isEnd)
        {
            if(isBestThanCurBest(tag->cost))
            {
                curBestCost = tag->cost;
            }
        }
        else if(!(tag->isInAliveNodes))   //否则把该结点加入活结点队列
        {
            MapPos mapPos;
            mapPos.r = tr;
            mapPos.c = tc;
            mapPos.lowerBound = lowerBoundFunction(tr, tc);
            if(isUsePriorityQueue)
            {
                aliveNodeP.push(mapPos);
            }
            else
            {
                aliveNode.push(mapPos);
            }
            tag->isInAliveNodes = true;
        }

        return true;
    }

    return false;
}

bool AStar::searchOneUp(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r >= 1)
    {
        MapNode *tag = matrix->get(r-1, c);
        if(moveToTarget(beg, tag, r-1, c))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneDown(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r <= rows-2)
    {
        MapNode *tag = matrix->get(r+1, c);
        if(moveToTarget(beg, tag, r+1, c))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneLeft(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(c >= 1)
    {
        MapNode *tag = matrix->get(r, c-1);
        if(moveToTarget(beg, tag, r, c-1))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneRight(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(c <= cols-2)
    {
        MapNode *tag = matrix->get(r, c+1);
        if(moveToTarget(beg, tag, r, c+1))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneLeftUp(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r>=1 && c>=1)
    {
        MapNode *tag = matrix->get(r-1, c-1);
        MapNode *tOffsetR = matrix->get(r-1, c);
        MapNode *tOffsetC = matrix->get(r, c-1);
        if(moveToTargetWhenDiag(tOffsetR, tOffsetC, beg, tag, r-1, c-1))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneRightUp(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r>=1 && c<=cols-2)
    {
        MapNode *tag = matrix->get(r-1, c+1);
        MapNode *tOffsetR = matrix->get(r-1, c);
        MapNode *tOffsetC = matrix->get(r, c+1);
        if(moveToTargetWhenDiag(tOffsetR, tOffsetC, beg, tag, r-1, c+1))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneLeftDown(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r<=rows-2 && c>=1)
    {
        MapNode *tag = matrix->get(r+1, c-1);
        MapNode *tOffsetR = matrix->get(r+1, c);
        MapNode *tOffsetC = matrix->get(r, c-1);
        if(moveToTargetWhenDiag(tOffsetR, tOffsetC, beg, tag, r+1, c-1))
        {
            return true;
        }
    }
    return false;
}

bool AStar::searchOneRightDown(int r, int c)
{
    MapNode *beg = matrix->get(r, c);
    if(r<=rows-2 && c<=cols-2)
    {
        MapNode *tag = matrix->get(r+1, c+1);
        MapNode *tOffsetR = matrix->get(r+1, c);
        MapNode *tOffsetC = matrix->get(r, c+1);
        if(moveToTargetWhenDiag(tOffsetR, tOffsetC, beg, tag, r+1, c+1))
        {
            return true;
        }
    }
    return false;
}

/*
 * 有得走返回true
 * 没得走返回false
 * 往上、下、左、右、左上、右上、左下、右下，这八个方向的展开
 * 格子之间，平的方向距离是1*condition；斜的方向，距离是(根号2)*condition
 */
bool AStar::searchOne(int r, int c, double lowerBound)
{
    if(r<0 || c<0)
    {
        return false;
    }
    MapNode *beg = matrix->get(r, c);
    if(!beg || !(beg->isUsed))
    {
        return false;
    }

    if(isBestThanCurBest(lowerBound /*lowerBoundFunction(r, c)*/))
    {
        if(isObliqueEnable)
        {
            return (searchOneUp(r, c) | searchOneDown(r, c) | searchOneLeft(r, c) | searchOneRight(r, c) |
                    searchOneLeftUp(r, c) | searchOneRightUp(r, c) |
                    searchOneLeftDown(r, c) | searchOneRightDown(r, c));
        }
        else
        {
            return (searchOneUp(r, c) | searchOneDown(r, c) | searchOneLeft(r, c) | searchOneRight(r, c));
        }
    }

    return false;
}

void AStar::searchAll()
{
    if(isUsePriorityQueue)
    {
        while(isRunnable && !aliveNodeP.empty())
        {
            MapPos handleAliveNode = aliveNodeP.top();
            aliveNodeP.pop();

            if(handleAliveNodeCallBack)
            {
                handleAliveNodeCallBack(this, handleAliveNode);
            }
            searchOne(handleAliveNode.r, handleAliveNode.c, handleAliveNode.lowerBound);

            matrix->get(handleAliveNode.r, handleAliveNode.c)->isInAliveNodes = false;
        }
        while(!aliveNodeP.empty())
        {
            MapPos handleAliveNode = aliveNodeP.top();
            aliveNodeP.pop();
            matrix->get(handleAliveNode.r, handleAliveNode.c)->isInAliveNodes = false;
        }
    }
    else
    {
        while(isRunnable && !aliveNode.empty())
        {
            MapPos handleAliveNode = aliveNode.front();
            aliveNode.pop();

            if(handleAliveNodeCallBack)
            {
                handleAliveNodeCallBack(this, handleAliveNode);
            }
            searchOne(handleAliveNode.r, handleAliveNode.c, handleAliveNode.lowerBound);

            matrix->get(handleAliveNode.r, handleAliveNode.c)->isInAliveNodes = false;
        }
        while(!aliveNode.empty())
        {
            MapPos handleAliveNode = aliveNode.front();
            aliveNode.pop();
            matrix->get(handleAliveNode.r, handleAliveNode.c)->isInAliveNodes = false;
        }
    }
}

/*
 * 返回一个栈，出栈顺序即从起点到终点的路径
 */
stack<MapPos> AStar::getPath(const Matrix *mapMatrix, bool obliqueEnable, bool obliqueEasy, bool usePriorityQueue, const GetPathConditionSet *settings)
{
    isRunnable = true;
    isObliqueEnable = obliqueEnable;
    isObliqueEasy = obliqueEasy;
    isUsePriorityQueue = usePriorityQueue;

    matrix = new Matrix(*mapMatrix);
    stack<MapPos> result;
    curBestCost = -1;

    //获取地图大小
    MapPos mapSize = matrix->size();
    rows = mapSize.r;
    cols = mapSize.c;

    endR = endC = beginR = beginC = -1;
    minCondition = -1;

    if(settings)
    {
        endR = settings->endR;
        endC = settings->endC;
        beginR = settings->beginR;
        beginC = settings->beginC;
        minCondition = settings->minCondition;
        matrix->get(endR, endC)->isEnd = true;
        matrix->get(beginR, beginC)->isUsed = true;
    }
    else
    {
        //寻找起点、终点坐标，以及最小路况值
        for(int ri=0; ri<rows; ri++)
        {
            for(int ci=0; ci<cols; ci++)
            {
                MapNode *mapNode = matrix->get(ri, ci);
                if(mapNode->isEnd)
                {
                    endR = ri;
                    endC = ci;
                }
                if(mapNode->isUsed)
                {
                    beginR = ri;
                    beginC = ci;
                }
                if(isVar1BestThanVar2(mapNode->condition, minCondition))
                {
                    minCondition = mapNode->condition;
                }
            }
        }
    }

    //找不到起点或终点坐标，直接返回空栈
    if(beginR==-1 || beginC==-1 || endR==-1 || endC==-1)
    {
        delete matrix;
        matrix = NULL;
        isRunnable = false;
        return result;
    }

    matrix->get(beginR, beginC)->cost = 0;

    MapPos beginPos;
    beginPos.r = beginR;
    beginPos.c = beginC;
    beginPos.lowerBound = lowerBoundFunction(beginR, beginC);
    if(isUsePriorityQueue)
    {
        aliveNodeP.push(beginPos);
    }
    else
    {
        aliveNode.push(beginPos);
    }

    matrix->get(beginR, beginC)->isInAliveNodes = true;

    searchAll();

    //处理路线
    MapNode *p = matrix->get(endR, endC);
    while(p)
    {
        MapPos pos;
        matrix->nodePos(p, pos.r, pos.c);
        pos.cost = p->cost;
        pos.lowerBound = curBestCost;
        result.push(pos);
        p = p->prior;
    }

    delete matrix;
    matrix = NULL;
    isRunnable = false;
    return result;
}

/*
 * 从起点经过当前点[r][c]，再到终点，路径长度的一个下界
 * 如果这个下界小于当前已经得到的解，认为是有希望比当前已经得到的解更优的
 * 如果这个下界大于或等于已经得到的解，则这条路不必再展开搜索了
 * -1认为是正无穷（或者是负数认为是正无穷）
 */
double AStar::lowerBoundFunction(int r, int c) const
{
    if(minCondition < 0)
    {
        return -1;
    }
    if(isObliqueEnable)
    {
        return matrix->get(r, c)->cost+sqrt(pow(r-endR, 2)+pow(c-endC, 2))*minCondition;    //下界为：已走路程+最小路况下当前点到终点的欧氏距离
    }
    else
    {
        return matrix->get(r, c)->cost+(fabs(r-endR)+fabs(c-endC))*minCondition;            //下界为：已走路程+最小路况下当前点到终点的曼哈顿距离
    }
}

/*
 * -1认为是正无穷（或者是负数认为是正无穷）
 */
bool AStar::isBestThanCurBest(double value) const
{
    return isVar1BestThanVar2(value, curBestCost);
}

/*
 * -1认为是正无穷（或者是负数认为是正无穷）
 */
bool AStar::isVar1BestThanVar2(double var1, double var2) const
{
    if(var1 < 0)
    {
        return false;
    }
    if(var2 < 0)
    {
        return true;
    }
    if(var1 < var2)
    {
        return true;
    }
    return false;
}

double AStar::getCurBestCost() const
{
    return curBestCost;
}

double AStar::getPosCost(MapPos mapPos) const
{
    if(!matrix)
    {
        return -1;
    }
    const MapNode *mapNode = matrix->getConst(mapPos.r, mapPos.c);
    return mapNode->cost;
}

void AStar::stopGetPath()
{
    isRunnable = false;
}

int AStar::aliveNodeSize() const
{
    if(isUsePriorityQueue)
    {
        return aliveNodeP.size();
    }
    else
    {
        return aliveNode.size();
    }
}

bool AStar::running() const
{
    return isRunnable;
}
