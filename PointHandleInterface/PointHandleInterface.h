// PointHandleInterface.h : PointHandleInterface DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPointHandleInterfaceApp
// �йش���ʵ�ֵ���Ϣ������� PointHandleInterface.cpp
//

class CPointHandleInterfaceApp : public CWinApp
{
public:
	CPointHandleInterfaceApp();
	void ReadPartPara(const char* dataname,int BroderAngle);
	int IWidth;
	int IHeight;
	float IPersion;

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
