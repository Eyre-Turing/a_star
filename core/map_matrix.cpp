#include "map_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;

Matrix::Matrix(int height, int width)
{
    mapMatrix.clear();
    vector<struct MapNode *> mapNodeVector;
    struct MapNode *mapNode;
    for(int ri=0; ri<height; ri++)
    {
        mapNodeVector.clear();
        for(int ci=0; ci<width; ci++)
        {
            mapNode = (struct MapNode *) malloc(sizeof(struct MapNode));
            mapNode->isWall = false;
            mapNode->isUsed = false;
            mapNode->isEnd = false;
            mapNode->condition = 1;
            mapNode->cost = -1;
            mapNode->prior = NULL;
            if(ri)
            {
                mapNode->reserve = (void *) mapMatrix[ri-1][ci];
            }
            else
            {
                mapNode->reserve = NULL;
            }
            mapNode->isInAliveNodes = false;
            mapNodeVector.push_back(mapNode);
        }
        mapMatrix.push_back(mapNodeVector);
    }
}

Matrix::Matrix(const Matrix &matrix)
{
    vector<vector<struct MapNode *> > nodes = matrix.mapMatrix;
    mapMatrix.clear();
    vector<struct MapNode *> mapNodeVector;
    struct MapNode *mapNode;
    int rows = nodes.size();
    for(int ri=0; ri<rows; ri++)
    {
        int cols = nodes[ri].size();
        mapNodeVector.clear();
        for(int ci=0; ci<cols; ci++)
        {
            mapNode = (struct MapNode *) malloc(sizeof(struct MapNode));
            mapNode->isWall = nodes[ri][ci]->isWall;
            mapNode->isUsed = nodes[ri][ci]->isUsed;
            mapNode->isEnd = nodes[ri][ci]->isEnd;
            mapNode->condition = nodes[ri][ci]->condition;
            mapNode->cost = nodes[ri][ci]->cost;
            //mapNode->prior = nodes[ri][ci]->prior;
            if(ri)
            {
                mapNode->reserve = (void *) mapMatrix[ri-1][ci];
            }
            else
            {
                mapNode->reserve = NULL;
            }
            mapNode->isInAliveNodes = false;
            mapNodeVector.push_back(mapNode);
        }
        mapMatrix.push_back(mapNodeVector);
    }

    //设置prior
    for(int ri=0; ri<rows; ri++)
    {
        int cols = nodes[ri].size();
        for(int ci=0; ci<cols; ci++)
        {
            if(nodes[ri][ci]->prior)
            {
                int priorPosR, priorPosC;
                matrix.nodePos(nodes[ri][ci]->prior, priorPosR, priorPosC);
                if(priorPosR!=-1 && priorPosC!=-1)
                {
                    mapMatrix[ri][ci]->prior = mapMatrix[priorPosR][priorPosC];
                }
                else
                {
                    mapMatrix[ri][ci]->prior = NULL;
                }
            }
            else
            {
                mapMatrix[ri][ci]->prior = NULL;
            }
        }
    }
}

Matrix::~Matrix()
{
    int rows = mapMatrix.size();
    for(int ri=0; ri<rows; ri++)
    {
        int cols = mapMatrix[ri].size();
        for(int ci=0; ci<cols; ci++)
        {
            free(mapMatrix[ri][ci]);
            mapMatrix[ri][ci] = NULL;
        }
        mapMatrix[ri].clear();
    }
    mapMatrix.clear();
}

void Matrix::resize(int height, int width)
{
    if(width<=0 || height<=0)
    {
        return ;
    }

    vector<struct MapNode *> mapNodeVector;
    struct MapNode *mapNode;

    int rows = mapMatrix.size();

    if(height >= rows)    //要加height-rows行
    {
        //处理原始的行
        for(int ri=0; ri<rows; ri++)
        {
            int cols = mapMatrix[ri].size();
            if(width >= cols)   //要加width-cols列
            {
                for(int ci=cols; ci<width; ci++)
                {
                    mapNode = (struct MapNode *) malloc(sizeof(struct MapNode));
                    mapNode->isWall = false;
                    mapNode->isUsed = false;
                    mapNode->isEnd = false;
                    mapNode->condition = 1;
                    mapNode->cost = -1;
                    mapNode->prior = NULL;

                    if(ri)
                    {
                        mapNode->reserve = (void *) mapMatrix[ri-1][ci];
                    }
                    else
                    {
                        mapNode->reserve = NULL;
                    }
                    mapNode->isInAliveNodes = false;
                    mapMatrix[ri].push_back(mapNode);
                }
            }
            else    //要删除cols-width列
            {
                for(int ci=width; ci<cols; ci++)
                {
                    free(mapMatrix[ri][ci]);
                }

                vector<struct MapNode*>::iterator cit = mapMatrix[ri].begin()+width;
                while(cit != mapMatrix[ri].end())
                {
                    cit = mapMatrix[ri].erase(cit);
                }
            }
        }

        //处理新加入行
        for(int ri=rows; ri<height; ri++)
        {
            mapNodeVector.clear();
            for(int ci=0; ci<width; ci++)
            {
                mapNode = (struct MapNode *) malloc(sizeof(struct MapNode));
                mapNode->isWall = false;
                mapNode->isUsed = false;
                mapNode->isEnd = false;
                mapNode->condition = 1;
                mapNode->cost = -1;
                mapNode->prior = NULL;

                if(ri)
                {
                    mapNode->reserve = (void *) mapMatrix[ri-1][ci];
                }
                else
                {
                    mapNode->reserve = NULL;
                }
                mapNode->isInAliveNodes = false;
                mapNodeVector.push_back(mapNode);
            }
            mapMatrix.push_back(mapNodeVector);
        }
    }
    else    //要删除rows-height行
    {
        //处理不需要删除的行
        for(int ri=0; ri<height; ri++)
        {
            int cols = mapMatrix[ri].size();
            if(width >= cols)   //要加width-cols列
            {
                for(int ci=cols; ci<width; ci++)
                {
                    mapNode = (struct MapNode *) malloc(sizeof(struct MapNode));
                    mapNode->isWall = false;
                    mapNode->isUsed = false;
                    mapNode->isEnd = false;
                    mapNode->condition = 1;
                    mapNode->cost = -1;
                    mapNode->prior = NULL;

                    if(ri)
                    {
                        mapNode->reserve = (void *) mapMatrix[ri-1][ci];
                    }
                    else
                    {
                        mapNode->reserve = NULL;
                    }
                    mapNode->isInAliveNodes = false;
                    mapMatrix[ri].push_back(mapNode);
                }
            }
            else    //要删除cols-width列
            {
                for(int ci=width; ci<cols; ci++)
                {
                    free(mapMatrix[ri][ci]);
                }

                vector<struct MapNode*>::iterator cit = mapMatrix[ri].begin()+width;
                while(cit != mapMatrix[ri].end())
                {
                    cit = mapMatrix[ri].erase(cit);
                }
            }
        }

        //处理要删除的行
        for(int ri=height; ri<rows; ri++)
        {
            int cols = mapMatrix[ri].size();
            for(int ci=0; ci<cols; ci++)
            {
                free(mapMatrix[ri][ci]);
            }
            mapMatrix[ri].clear();
        }
        vector<vector<struct MapNode*> >::iterator rit = mapMatrix.begin()+height;
        while(rit != mapMatrix.end())
        {
            rit = mapMatrix.erase(rit);
        }
    }
}

struct MapNode *Matrix::get(int row, int col)
{
    if(row<0 || col<0)
    {
        return NULL;
    }

    int rows = mapMatrix.size();
    if(rows <= row)
    {
        return NULL;
    }
    int cols = mapMatrix[row].size();
    if(cols <= col)
    {
        return NULL;
    }
    return mapMatrix[row][col];
}

const struct MapNode *Matrix::getConst(int row, int col) const
{
    if(row<0 || col<0)
    {
        return NULL;
    }

    int rows = mapMatrix.size();
    if(rows <= row)
    {
        return NULL;
    }
    int cols = mapMatrix[row].size();
    if(cols <= col)
    {
        return NULL;
    }
    return mapMatrix[row][col];
}

bool Matrix::set(int row, int col, const struct MapNode &mapNode)
{
    int rows = mapMatrix.size();
    if(rows <= row)
    {
        return false;
    }
    int cols = mapMatrix[row].size();
    if(cols <= col)
    {
        return false;
    }

    struct MapNode *p = mapMatrix[row][col];
    p->isWall = mapNode.isWall;
    p->isUsed = mapNode.isUsed;
    p->isEnd = mapNode.isEnd;
    p->condition = mapNode.condition;
    p->cost = mapNode.cost;
    p->prior = mapNode.prior;
    p->isInAliveNodes = mapNode.isInAliveNodes;

    return true;
}

void Matrix::nodePos(struct MapNode *mapNode, int &row, int &col) const
{
    row = col = 0;
    struct MapNode *p = mapNode;
    while(p->reserve)
    {
        p = (struct MapNode *) p->reserve;
        row++;
    }

    vector<struct MapNode *> mapNodeVector = mapMatrix[0];
    int len = mapNodeVector.size();
    for(col=0; col<len && mapNodeVector[col]!=p; col++);
    if(col == len)
    {
        row = col = -1;
    }
}

void Matrix::loadOneFromStr(const char *p, int ri, int ci, int *beginX, int *beginY, int *endX, int *endY, double *minCondition)
{
    /*
     * p的格式：
     * [isWall][isUsed][isEnd]_[condition]
     * 其中：[isWall]、[isUsed]、[isEnd]各占1个字节，[condition]占len-4个字节，其中len是p的长度
     */

    int plen = strlen(p);
    if(plen >= 3)
    {
        mapMatrix[ri][ci]->isWall = (int)(p[0]-'0');
        if(mapMatrix[ri][ci]->isUsed = (int)(p[1]-'0'))
        {
            if(beginX)
            {
                *beginX = ci;
            }
            if(beginY)
            {
                *beginY = ri;
            }
        }
        if(mapMatrix[ri][ci]->isEnd = (int)(p[2]-'0'))
        {
            if(endX)
            {
                *endX = ci;
            }
            if(endY)
            {
                *endY = ri;
            }
        }

        if(plen > 4)
        {
            char condition[50];
            strcpy(condition, p+4);
            mapMatrix[ri][ci]->condition = atof(condition);
        }
        else
        {
            mapMatrix[ri][ci]->condition = 1;
        }

        if(minCondition)
        {
            if(*minCondition<0 || (mapMatrix[ri][ci]->condition>=0 && mapMatrix[ri][ci]->condition<*minCondition))
            {
                *minCondition = mapMatrix[ri][ci]->condition;
            }
        }
    }
    mapMatrix[ri][ci]->cost = -1;
    mapMatrix[ri][ci]->prior = NULL;
}

void Matrix::readFromText(string text, int *height, int *width, int *beginX, int *beginY, int *endX, int *endY, double *minCondition)
{
    char *str = (char *) malloc(sizeof(char)*text.length()+1);
    strcpy(str, text.c_str());
    int rows, cols;
    char *p = str;  //如果直接用str来strtok，就会导致不能free了

    p = strtok(p, " ");
    rows = atoi(p);
    p = strtok(NULL, "\n");
    cols = atoi(p);

    resize(rows, cols);

    if(height)
    {
        *height = rows;
    }
    if(width)
    {
        *width = cols;
    }

    if(minCondition)
    {
        *minCondition = -1;
    }

    for(int ri=0; ri<rows; ri++)
    {
        for(int ci=0; ci<cols-1; ci++)
        {
            p = strtok(NULL, " ");
            loadOneFromStr(p, ri, ci, beginX, beginY, endX, endY, minCondition);
        }
        p = strtok(NULL, "\n");
        loadOneFromStr(p, ri, cols-1, beginX, beginY, endX, endY, minCondition);
    }

    free(str);
}

void Matrix::readFromFile(string filename, int *rows, int *cols, int *beginX, int *beginY, int *endX, int *endY, double *minCondition)
{
    ifstream inf;
    inf.open(filename.c_str());

    ostringstream ins;
    ins<<inf.rdbuf();

    string text = ins.str();

    inf.close();

    readFromText(text, rows, cols, beginX, beginY, endX, endY, minCondition);
}

string itos(int value)
{
    char temp[20];
    sprintf(temp, "%d", value);
    string result(temp, temp+strlen(temp));
    return result;
}

string ftos(double value)
{
    char temp[50];
    sprintf(temp, "%g", value);
    string result(temp, temp+strlen(temp));
    return result;
}

string Matrix::toString() const
{
    int rows = mapMatrix.size();
    if(!rows)
    {
        return "0 0\n";
    }
    int cols = mapMatrix[0].size();
    if(!cols)
    {
        return "0 0\n";
    }

    string result = itos(rows)+" "+itos(cols)+"\n";
    for(int ri=0; ri<rows; ri++)
    {
        for(int ci=0; ci<cols; ci++)
        {
            if(ci)
            {
                result += " ";
            }
            MapNode *mapNode = mapMatrix[ri][ci];
            result += itos(mapNode->isWall)+itos(mapNode->isUsed)+itos(mapNode->isEnd);
            if(mapNode->condition != 1)
            {
                result += "_"+ftos(mapNode->condition);
            }
        }
        result += "\n";
    }

    return result;
}

void Matrix::writeToFile(string filename) const
{
    string text = toString();
    ofstream outf;
    outf.open(filename.c_str());
    outf<<text;
    outf.close();
}

struct MapPos Matrix::size() const
{
    MapPos result;

    int rows = mapMatrix.size();
    if(rows)
    {
        result.r = rows;
        result.c = mapMatrix[0].size();
    }
    else
    {
        result.r = 0;
        result.c = 0;
    }

    return result;
}
