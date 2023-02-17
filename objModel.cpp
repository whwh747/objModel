//
// Created by wh on 23-2-17.
//
#include"myslam/objModel.h"
#include <pangolin/pangolin.h>
#include <opencv2/opencv.hpp>
#include"bits/stdc++.h"
#include <pangolin/geometry/glgeometry.h>
#include <pangolin/gl/glformattraits.h>


using namespace std;
using namespace pangolin;
namespace myslam{
    unsigned char *objModel::LoadFileContent(const char *path, int &filesize) {
        unsigned char*fileContent = nullptr;
        filesize = 0;
        FILE*pFile = fopen(path, "rb");   //二进制方式读取
        if (pFile)
        {
            fseek(pFile, 0, SEEK_END);      //将文件指针移动到文件末尾
            int nLen = ftell(pFile);        //距离文件头部的距离   //这里指文件大小
            if (nLen > 0)
            {
                rewind(pFile);          //移动到文件头部
                fileContent = new unsigned char[nLen + 1];
                //为文件指针开辟空间
                fread(fileContent, sizeof(unsigned char), nLen, pFile);
                //将pFile的内容读入fileContent
                fileContent[nLen] = '\0';         //文件末尾加上\0
                filesize = nLen;                  //为文件大小赋值
            }
            fclose(pFile);
        }
        return fileContent;
    }

    objModel::objModel(const char *objFileName) {
        int nFileSize = 0;
        unsigned char*fileContent = LoadFileContent(objFileName, nFileSize);    //读取文件内容
        if (fileContent == nullptr)return ;     //文件为空
        std::stringstream ssFileContent((char*)fileContent);   //流读取文件内容
        std::string temp;       //接受无关信息
        char szoneLine[256];        //读取一行的数据
        while (!ssFileContent.eof())
        {
            memset(szoneLine, 0, 256);        //  每次循环初始化数组szoneLine
            ssFileContent.getline(szoneLine, 256);      //流读取一行
            if (strlen(szoneLine) > 0)       //该行不为空
            {
                if (szoneLine[0] == 'v')     //v开头的数据
                {
                    std::stringstream ssOneLine(szoneLine);        //数据存储到流中 方便赋值
                    if (szoneLine[1] == 't')       //纹理信息
                    {
                        ssOneLine >> temp;     //接受标识符 vt
                        Float2 tempTexcoord;
                        ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];   //数据存入临时变量中
                        mTexcoord.push_back(tempTexcoord);         //存入容器
                    }
                    if (szoneLine[1] == 'n')            //法线信息
                    {
                        ssOneLine >> temp;      //接收标识符vn
                        Float3 tempNormal;
                        ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
                        mNormal.push_back(tempNormal);
                    }
                    else                          //点的位置信息
                    {
                        ssOneLine >> temp;
                        Float3 tempLocation;
                        ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
                        mLocation.push_back(tempLocation);
                    }
                }
                else if (szoneLine[0] == 'f')          //面信息
                {
                    std::stringstream ssOneLine(szoneLine);     //流读取一行数据
                    ssOneLine >> temp; //接收标识符f
                    //    f信息    exp： f 1/1/1 2/2/2 3/3/3      位置索引/纹理索引/法线索引   三角面片 三个点构成一个面
                    std::string vertexStr;   //接收流的内容
                    Face tempFace;
                    for (int i = 0; i < 3; ++i)         //每个面三个点
                    {
                        ssOneLine >> vertexStr;           //从流中读取点的索引信息
                        size_t pos = vertexStr.find_first_of('/');       //找到第一个/的位置      //即找到点的位置信息
                        std::string locIndexStr = vertexStr.substr(0, pos);       //赋值点的位置信息
                        size_t pos2 = vertexStr.find_first_of('/', pos + 1);   //找到第二个/   即找到点的纹理坐标信息
                        std::string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);       //赋值点的纹理坐标信息
                        string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);   //赋值点的法线信息
                        tempFace.vertex[i][0] = atoi(locIndexStr.c_str());        //将索引信息从 srting转换为 int     //位置索引信息赋值
                        tempFace.vertex[i][1] = atoi(texIndexSrt.c_str());         //纹理坐标索引信息赋值
                        tempFace.vertex[i][2] = atoi(norIndexSrt.c_str());         //法线信息赋值
                    }
                    mFace.push_back(tempFace);
                }
            }   //end 非0行
        }  //end while
        delete fileContent;
    }

    void objModel::filter() {
        int cnt=0;
        for(int i=0;i<mFace.size();i++,cnt++)
        {
            if(cnt%3)continue;
            filter_Face.push_back(mFace[i]);
        }
        cout<<"face size = "<<mFace.size()<<endl;
        cout<<"filter face size = "<<filter_Face.size()<<endl;
    }
    void objModel::objDraw() {
        //glBindTexture(GL_TEXTURE_2D, mTexture);
        glBegin(GL_TRIANGLES);
        for (auto faceIndex = mFace.begin(); faceIndex != mFace.end(); ++faceIndex)         //循环遍历face信息
        {
            ///只画出面信息即可
            //第一个点的法线，纹理，位置信息
            //glNormal3fv(mNormal[faceIndex->vertex[0][2]-1].Data);
            //glTexCoord2fv(mTexcoord[faceIndex->vertex[0][1]-1].Data);
            //glColor3f ( 0.8f,0.f,0.f );
            glVertex3fv(mLocation[faceIndex->vertex[0][0]-1].Data);
            //第二个点的法线，纹理，位置信息
            //glNormal3fv(mNormal[faceIndex->vertex[1][2]-1].Data);
            //glTexCoord2fv(mTexcoord[faceIndex->vertex[1][1]-1].Data);
            //glColor3f ( 0.8f,0.f,0.f );
            glVertex3fv(mLocation[faceIndex->vertex[1][0]-1].Data);
            //第三个点的法线，纹理，位置信息
            //glNormal3fv(mNormal[faceIndex->vertex[2][2]-1].Data);
            //glTexCoord2fv(mTexcoord[faceIndex->vertex[2][1]-1].Data);
            //glColor3f ( 0.8f,0.f,0.f );
            glVertex3fv(mLocation[faceIndex->vertex[2][0]-1].Data);
        }
        ///如果要过滤请使用下面的代码
//    for (auto faceIndex = filter_Face.begin(); faceIndex != filter_Face.end(); ++faceIndex)         //循环遍历face信息
//    {
//        ///只画出面信息即可
//        glVertex3fv(mLocation[faceIndex->vertex[0][0]-1].Data);
//        glVertex3fv(mLocation[faceIndex->vertex[1][0]-1].Data);
//        glVertex3fv(mLocation[faceIndex->vertex[2][0]-1].Data);
//    }
        glEnd();
    }
}