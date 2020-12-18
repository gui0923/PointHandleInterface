// PointHandleInterface.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "PointHandleInterface.h"
#include "DllInterfacePkg_PointHandleInterface.h"
#include <math.h>
#include <fstream>
#include <vector>
using std::vector;
using namespace std;

#define PI 3.14159265358979323846 //跟JAVA一样吧

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//jstring类型的字符串转换成Windows下的字符串，此版本可转换中文，且无乱码。
char* jstringToWindows( JNIEnv *env, jstring jstr )
{
  int length = env->GetStringLength(jstr);
  const jchar* jcstr = env->GetStringChars(jstr, 0);
  char* rtn = (char*)malloc(length*2+1);
  int size = 0;
  size = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, rtn,(length*2+1), NULL, NULL);
  if( size <= 0 )
	return NULL;
  env->ReleaseStringChars(jstr, jcstr);
  rtn[size] = 0;
  return rtn;
}
//Windows下的字符串转换成jstring类型的字符串，此版本可转换中文，且无乱码。
jstring WindowsTojstring( JNIEnv *env, const char* str )
{
  jstring rtn = 0;
  int slen = strlen(str);
  unsigned short* buffer = 0;
  if( slen == 0 )
    rtn = env->NewStringUTF(str); 
  else
  {
    int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, NULL, 0);
    buffer = (unsigned short*)malloc(length*2 + 1);
    if( MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, (LPWSTR)buffer, length) >0)
      rtn = env->NewString((jchar*)buffer, length);
  }
  if(buffer)
  free(buffer);
  return rtn;
}

//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CPointHandleInterfaceApp

BEGIN_MESSAGE_MAP(CPointHandleInterfaceApp, CWinApp)
END_MESSAGE_MAP()


// CPointHandleInterfaceApp 构造

CPointHandleInterfaceApp::CPointHandleInterfaceApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPointHandleInterfaceApp 对象

CPointHandleInterfaceApp theApp;


// CPointHandleInterfaceApp 初始化

BOOL CPointHandleInterfaceApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

void CPointHandleInterfaceApp::ReadPartPara(const char* dataname,int BroderAngle)
{//读取分辨率

	ifstream infile;
	infile.open(dataname,ifstream::binary);
	if (infile.fail())
	{
		fprintf(stderr, "bad file\n");_flushall();
		return;
	}
	float m = 0;
	float n = 0;
	int i;
	infile.read((char *)&m,4);
	for( i = 0; i < 4; i ++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	IPersion = n;

	infile.read((char *)&m,4);
	for( i = 0; i < 4; i ++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	IWidth =(int)n;

	infile.read((char *)&m,4);
	for( i = 0; i < 4; i ++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	IHeight =(int)n;
	double* OriginBuf = new double[IWidth * IHeight * 3];

	int num = IWidth*IHeight;
	int counter = 0;
	//读zdat
	double coor[3];
	while(counter<num)
	{
		
		infile.read((char *)&m,4);
		for(int i=0; i<4; i++)
			*((char*)&n + i) = *((char*)&m + 4 - i -1);
		coor[2] = n;
		coor[0] = (counter%IWidth)*IPersion;
		coor[1] = (counter/IWidth)*IPersion;
		OriginBuf[counter * 3] = coor[0];
		OriginBuf[counter * 3 + 1] = coor[1];
		OriginBuf[(counter++) * 3 + 2] = coor[2];
		
	}
	infile.close();
              double Tmp1,Tmp2,radian;
              double Anglerow,Anglecol,Anglerightup,Anglerightdown;
              double vectorax,vectoray,vectoraz;
              double vectorbx,vectorby,vectorbz;
              
              int sumFeiDian=0;
              int count=0;//计算执行次数，否则会出现一个死循环
			  int *ValidFlags=new int[IWidth * IHeight];
			  vector<int> PointPosVec;

			  int lastsumFeiDian=10;//上一次飞点的数量，防止不必要的循环
			  int countcircle=0;//统计上次和这次飞点相等的次数
    do{
		sumFeiDian=0;
        
        for(int ii=0;ii<IHeight;ii++)
        {
            for(int j=0;j<IWidth;j++)
            {
                       ValidFlags[ii*IWidth+j]=1;//默认时，所有的点都应该是合法的，这样，初始时，所有的坐标都为合法的0。图片上点的三维坐标就与三维点云图统一了。
             }
        }

		for(int j=1;j<IHeight-1;j++)
              {
                  for(int k=1;k<IWidth-1;k++)
                  {
                      vectorax=OriginBuf[(j*IWidth+k)*3+0]-OriginBuf[(j*IWidth+k-1)*3+0];
                      vectoray=OriginBuf[(j*IWidth+k)*3+1]-OriginBuf[(j*IWidth+k-1)*3+1];
                      vectoraz=OriginBuf[(j*IWidth+k)*3+2]-OriginBuf[(j*IWidth+k-1)*3+2];
                      vectorbx=OriginBuf[(j*IWidth+k+1)*3+0]-OriginBuf[(j*IWidth+k)*3+0];
                      vectorby=OriginBuf[(j*IWidth+k+1)*3+1]-OriginBuf[(j*IWidth+k)*3+1];
                      vectorbz=OriginBuf[(j*IWidth+k+1)*3+2]-OriginBuf[(j*IWidth+k)*3+2];

                       Tmp1=(vectorax*vectorbx+vectoray*vectorby+vectoraz*vectorbz);
                        Tmp2=sqrt(vectorax*vectorax+0*0+vectoraz*vectoraz)*
                        sqrt(vectorbx*vectorbx+0*0+vectorbz*vectorbz);

                         radian=Tmp1/Tmp2;
                         Anglerow=acos(radian)*180/PI;//求角度

                      vectorax=OriginBuf[(j*IWidth+k)*3+0]-OriginBuf[((j-1)*IWidth+k)*3+0];
                      vectoray=OriginBuf[(j*IWidth+k)*3+1]-OriginBuf[((j-1)*IWidth+k)*3+1];
                      vectoraz=OriginBuf[(j*IWidth+k)*3+2]-OriginBuf[((j-1)*IWidth+k)*3+2];
                      vectorbx=OriginBuf[((j+1)*IWidth+k)*3+0]-OriginBuf[(j*IWidth+k)*3+0];
                      vectorby=OriginBuf[((j+1)*IWidth+k)*3+1]-OriginBuf[(j*IWidth+k)*3+1];
                      vectorbz=OriginBuf[((j+1)*IWidth+k)*3+2]-OriginBuf[(j*IWidth+k)*3+2];

                      Tmp1=(vectorax*vectorbx+vectoray*vectorby+vectoraz*vectorbz);
                        Tmp2=sqrt(vectorax*vectorax+vectoray*vectoray+vectoraz*vectoraz)*
                        sqrt(vectorbx*vectorbx+vectorby*vectorby+vectorbz*vectorbz);

                      radian=Tmp1/Tmp2;
                      Anglecol=acos(radian)*180/PI;//求角度


                      vectorax=OriginBuf[(j*IWidth+k)*3+0]-OriginBuf[((j-1)*IWidth+k-1)*3+0];
                      vectoray=OriginBuf[(j*IWidth+k)*3+1]-OriginBuf[((j-1)*IWidth+k-1)*3+1];
                      vectoraz=OriginBuf[(j*IWidth+k)*3+2]-OriginBuf[((j-1)*IWidth+k-1)*3+2];
                      vectorbx=OriginBuf[((j+1)*IWidth+k+1)*3+0]-OriginBuf[(j*IWidth+k)*3+0];
                      vectorby=OriginBuf[((j+1)*IWidth+k+1)*3+1]-OriginBuf[(j*IWidth+k)*3+1];
                      vectorbz=OriginBuf[((j+1)*IWidth+k+1)*3+2]-OriginBuf[(j*IWidth+k)*3+2];

                      Tmp1=(vectorax*vectorbx+vectoray*vectorby+vectoraz*vectorbz);
                        Tmp2=sqrt(vectorax*vectorax+vectoray*vectoray+vectoraz*vectoraz)*
                        sqrt(vectorbx*vectorbx+vectorby*vectorby+vectorbz*vectorbz);

                      radian=Tmp1/Tmp2;
                      Anglerightup=acos(radian)*180/PI;//求角度
                      
                      vectorax=OriginBuf[(j*IWidth+k)*3+0]-OriginBuf[((j-1)*IWidth+k+1)*3+0];
                      vectoray=OriginBuf[(j*IWidth+k)*3+1]-OriginBuf[((j-1)*IWidth+k+1)*3+1];
                      vectoraz=OriginBuf[(j*IWidth+k)*3+2]-OriginBuf[((j-1)*IWidth+k+1)*3+2];
                      vectorbx=OriginBuf[((j+1)*IWidth+k-1)*3+0]-OriginBuf[(j*IWidth+k)*3+0];
                      vectorby=OriginBuf[((j+1)*IWidth+k-1)*3+1]-OriginBuf[(j*IWidth+k)*3+1];
                      vectorbz=OriginBuf[((j+1)*IWidth+k-1)*3+2]-OriginBuf[(j*IWidth+k)*3+2];
                      Tmp1=(vectorax*vectorbx+vectoray*vectorby+vectoraz*vectorbz);
                        Tmp2=sqrt(vectorax*vectorax+vectoray*vectoray+vectoraz*vectoraz)*
                        sqrt(vectorbx*vectorbx+vectorby*vectorby+vectorbz*vectorbz);

                      radian=Tmp1/Tmp2;
                      Anglerightdown=acos(radian)*180/PI;//求角度

                      if(Anglerow>BroderAngle||Anglecol>BroderAngle||Anglerightup>BroderAngle||Anglerightdown>BroderAngle)//视这种点为飞点吧
                      {
                          sumFeiDian++;
						  ValidFlags[j*IWidth+k]=0;
                      }/*else if(Anglerow>BroderAngle)
                      {
                          sumFeiDian++;sumFeiDian++;
                          ValidFlags[(j-1)*IWidth+k]=0;
                          ValidFlags[(j+1)*IWidth+k]=0;
                      }else if(Anglecol>BroderAngle)
                      {
                          sumFeiDian++;sumFeiDian++;
                          ValidFlags[j*IWidth+k+1]=0;
                          ValidFlags[j*IWidth+k-1]=0;
                      }else if(Anglerightup>BroderAngle)
                      {
                          sumFeiDian++;sumFeiDian++;
                          ValidFlags[(j+1)*IWidth+k+1]=0;
                          ValidFlags[(j-1)*IWidth+k-1]=0;
                      }else if(Anglerightdown>BroderAngle)
                      {
                          sumFeiDian++;sumFeiDian++;
                          ValidFlags[(j-1)*IWidth+k+1]=0;
                          ValidFlags[(j+1)*IWidth+k-1]=0;
                      }*/

                  }
              }
                       printf("%s飞点总共有%d个\n",dataname,sumFeiDian);_flushall();
					   PointPosVec.clear();
					   double v=0;
                       int vCount=0;
					   int i,j,k,l,m;
					   for(j=0;j<IHeight;j++)
					   {
						   for(k=0;k<IWidth;k++)
						   {
							   if(ValidFlags[j*IWidth+k]==0||OriginBuf[(j*IWidth+k)*3+2]==9999.0)
							   {
								   v=0;vCount=0;
                                  for(l=j-1;l<=j+1;l++)
								  {
                                     for(m=k-1;m<=k+1;m++)
									 {//用一个点周围8个点的平均值来代替当前点的值！
										 if(l>=0 && l<IHeight && m>=0 &&
                                           m<IWidth &&
                                       ValidFlags[l*IWidth+m]!=0 &&
									   OriginBuf[(l*IWidth+m)*3+2]!=9999.0
                                           )
										 {
											 vCount++;
                                             v+=OriginBuf[(l*IWidth+m)*3+2];
										 }

									 }
							   }
						if(vCount==0)
                       {//有可能周围的点全是飞点，这是角落里有可能发生！此时，需要将这些点放在一个向量中，从后向前再拟合一次，应该就能解决。
                            //PointPosVec.add(j*IWidth+k);
							PointPosVec.push_back(j*IWidth+k);
                       }else
                       {
                            OriginBuf[(j*IWidth+k)*3+2]=v/(double)vCount;
                            ValidFlags[j*IWidth+k]=1;
                       }
							   }

						   }
					   }

     for(i=PointPosVec.size()-1;i>=0;i--)
        {
            j=PointPosVec[i]/IWidth;
            k=PointPosVec[i]%IWidth;
             v=0;vCount=0;
                       for(l=j-1;l<=j+1;l++)
                       {
                           for(m=k-1;m<=k+1;m++)
                           {//用一个点周围8个点的平均值来代替当前点的值！
                               if(l>=0 && l<IHeight && m>=0 &&
                                  m<IWidth &&
                                  ValidFlags[l*IWidth+m]!=0 &&
                                  OriginBuf[(l*IWidth+m)*3+2]!=9999.0
                               )
                               {
                                   vCount++;
                                   v+=OriginBuf[(l*IWidth+m)*3+2];
                               }
                           }
                       }
                       if(vCount==0)
                       {//此时，不再可能发生周围点全是飞点的情况了！除非整个靶面全部飞点，那就没有办法了。这种情况可以不考虑。

                       }else
                       {
                            OriginBuf[(j*IWidth+k)*3+2]=v/(double)vCount;
                            ValidFlags[j*IWidth+k]=1;
                       }
        }
	 PointPosVec.clear();
	 if(abs(lastsumFeiDian-sumFeiDian)<=5)//如果上次飞点的数量不等于这次飞点数量的上下浮动范围
	 {
		 countcircle++;
	 }else{
		 lastsumFeiDian=sumFeiDian;
		 countcircle=0;
	 }
					   count++;

	}while(sumFeiDian>10&&count<100&&countcircle<=5);//先5次吧
	count=0;

	//以上是飞点拟合的过程，下面该是保存数据了

		ofstream outfile;
	outfile.open(dataname,ifstream::binary);
	if (outfile.fail())
	{
		fprintf(stderr, "bad file\n");_flushall();
		return;
	}
	m = IPersion;
	//n = 0;
	for(int i=0; i<4; i++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	outfile.write((char *)&n,4);

	m =(float)IWidth;
	for(int i=0; i<4; i++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	outfile.write((char *)&n,4);

	m =(float)IHeight;
	for(int i=0; i<4; i++)
		*((char*)&n + i) = *((char*)&m + 4 - i -1);
	outfile.write((char *)&n,4);

	counter = 0;

	while(counter<num)
	{
		m =(float)OriginBuf[(counter++) * 3 + 2];
		for(int i=0; i<4; i++)
			*((char*)&n + i) = *((char*)&m + 4 - i -1);
		outfile.write((char *)&n,4);
	}	
	delete[] OriginBuf;
	OriginBuf=NULL;

	delete[] ValidFlags;
	ValidFlags=NULL;
	
	outfile.close();

}
JNIEXPORT void JNICALL Java_DllInterfacePkg_PointHandleInterface_PointHandleByC
  (JNIEnv* env, jclass jObject, jstring InputDatFileName, jint Angle)
{

	char *S_InputDatFileName=jstringToWindows(env,InputDatFileName);_flushall();
	theApp.ReadPartPara(S_InputDatFileName,Angle);

}