// PointHandleInterface.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "PointHandleInterface.h"
#include "DllInterfacePkg_PointHandleInterface.h"
#include <math.h>
#include <fstream>
#include <vector>
using std::vector;
using namespace std;

#define PI 3.14159265358979323846 //��JAVAһ����

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//jstring���͵��ַ���ת����Windows�µ��ַ������˰汾��ת�����ģ��������롣
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
//Windows�µ��ַ���ת����jstring���͵��ַ������˰汾��ת�����ģ��������롣
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
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CPointHandleInterfaceApp

BEGIN_MESSAGE_MAP(CPointHandleInterfaceApp, CWinApp)
END_MESSAGE_MAP()


// CPointHandleInterfaceApp ����

CPointHandleInterfaceApp::CPointHandleInterfaceApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPointHandleInterfaceApp ����

CPointHandleInterfaceApp theApp;


// CPointHandleInterfaceApp ��ʼ��

BOOL CPointHandleInterfaceApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

void CPointHandleInterfaceApp::ReadPartPara(const char* dataname,int BroderAngle)
{//��ȡ�ֱ���

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
	//��zdat
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
              int count=0;//����ִ�д�������������һ����ѭ��
			  int *ValidFlags=new int[IWidth * IHeight];
			  vector<int> PointPosVec;

			  int lastsumFeiDian=10;//��һ�ηɵ����������ֹ����Ҫ��ѭ��
			  int countcircle=0;//ͳ���ϴκ���ηɵ���ȵĴ���
    do{
		sumFeiDian=0;
        
        for(int ii=0;ii<IHeight;ii++)
        {
            for(int j=0;j<IWidth;j++)
            {
                       ValidFlags[ii*IWidth+j]=1;//Ĭ��ʱ�����еĵ㶼Ӧ���ǺϷ��ģ���������ʼʱ�����е����궼Ϊ�Ϸ���0��ͼƬ�ϵ����ά���������ά����ͼͳһ�ˡ�
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
                         Anglerow=acos(radian)*180/PI;//��Ƕ�

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
                      Anglecol=acos(radian)*180/PI;//��Ƕ�


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
                      Anglerightup=acos(radian)*180/PI;//��Ƕ�
                      
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
                      Anglerightdown=acos(radian)*180/PI;//��Ƕ�

                      if(Anglerow>BroderAngle||Anglecol>BroderAngle||Anglerightup>BroderAngle||Anglerightdown>BroderAngle)//�����ֵ�Ϊ�ɵ��
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
                       printf("%s�ɵ��ܹ���%d��\n",dataname,sumFeiDian);_flushall();
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
									 {//��һ������Χ8�����ƽ��ֵ�����浱ǰ���ֵ��
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
                       {//�п�����Χ�ĵ�ȫ�Ƿɵ㣬���ǽ������п��ܷ�������ʱ����Ҫ����Щ�����һ�������У��Ӻ���ǰ�����һ�Σ�Ӧ�þ��ܽ����
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
                           {//��һ������Χ8�����ƽ��ֵ�����浱ǰ���ֵ��
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
                       {//��ʱ�����ٿ��ܷ�����Χ��ȫ�Ƿɵ������ˣ�������������ȫ���ɵ㣬�Ǿ�û�а취�ˡ�����������Բ����ǡ�

                       }else
                       {
                            OriginBuf[(j*IWidth+k)*3+2]=v/(double)vCount;
                            ValidFlags[j*IWidth+k]=1;
                       }
        }
	 PointPosVec.clear();
	 if(abs(lastsumFeiDian-sumFeiDian)<=5)//����ϴηɵ��������������ηɵ����������¸�����Χ
	 {
		 countcircle++;
	 }else{
		 lastsumFeiDian=sumFeiDian;
		 countcircle=0;
	 }
					   count++;

	}while(sumFeiDian>10&&count<100&&countcircle<=5);//��5�ΰ�
	count=0;

	//�����Ƿɵ���ϵĹ��̣�������Ǳ���������

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