
// word导出软件Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "word导出软件.h"
#include "word导出软件Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <shellapi.h>


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#include "msword.h"//调用word2003
#include <ATLBASE.H>
//声明用到的word中的类对象
_Application Wordapp;
Documents WordDocs;
_Document WordDoc;
Selection sel;
Find find;//查找对象
Replacement replace;//替换对象
Range aRange;

CEdit m_Edit;  
int m_item = 0;  //行
int m_Subitem = 0; //列
int rowCount = 0;  //总行数
bool  needSave = false; //用于保存标志,如果有过编写则为TURE,当下次编辑时会在原来的位置输入原来输入的内容
#define  IDC_EDIT 0xffff 	

CString m_FilePath;//文件路径设置相关
int m_iPosIndex;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordDlg dialog

CWordDlg::CWordDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWordDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWordDlg)
	DDX_Control(pDX, IDC_LIST2, m_list2);
	DDX_Control(pDX, IDC_LIST1, m_list1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWordDlg, CDialog)
//{{AFX_MSG_MAP(CWordDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK2, OnOutPutWord)
ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickList2)
ON_BN_CLICKED(IDC_BUTTON1, OnOnRefresh)
ON_BN_CLICKED(IDOK3, OnOpenFileLocation)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWordDlg message handlers

BOOL CWordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Add "About..." menu item to system menu.
	
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here


	RECT m_rect;
	m_list1.GetClientRect(&m_rect);//获取list的客户区
	m_list1.SetExtendedStyle(LVS_EX_GRIDLINES/*以网格形式显示*/|LVS_EX_FULLROWSELECT/*当单机一行时整行都会选中*/);//设置list风格
	
	
	/***************输入框*************/
	//设置表头控制
	m_list1.InsertColumn(0, TEXT("代替字段"));
	m_list1.InsertColumn(1, TEXT("输入选项"));
	m_list1.InsertColumn(2, TEXT("输入内容"));
	
//	m_list1.InsertItem(0, _T("@#&1"));/*插入行*/
//	m_list1.SetItemText(0, 1, _T("姓名"));/*设置单元格内容*/
//	m_list1.SetItemText(0, 2, _T(""));/*设置单元格内容*/
	
//	m_list1.InsertItem(1, _T("@#&2"));
//	m_list1.SetItemText(1, 1, _T("年龄"));
//	m_list1.SetItemText(1, 2, _T(""));
	
	//设置表头控制宽度
	m_list1.SetColumnWidth(0,70);
	m_list1.SetColumnWidth(1,100);
	m_list1.SetColumnWidth(2,337);
	
	/****************选择框***************/
	m_list2.InsertColumn(0, TEXT("序号"));
	m_list2.InsertColumn(1, TEXT("文件名"));
	//	m_list2.InsertColumn(1, TEXT("文件类型"));
	//设置表头控制宽度
	m_list2.SetColumnWidth(0,43);
	m_list2.SetColumnWidth(1,462);
	//	m_list2.SetColumnWidth(1,73);
	
	DWORD dwStyle2 = m_list1.GetExtendedStyle();  
	dwStyle2 = LVS_EX_GRIDLINES;
	//网格线（只适用与report风格的listctrl）      
	m_list2.SetExtendedStyle(dwStyle2); 
	
	GetModuleFileName(NULL, m_FilePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);//获取程序自身路径，此获得的路径不含最后的应用程序名
	m_FilePath.ReleaseBuffer();
	m_iPosIndex = m_FilePath.ReverseFind('\\'); 
	m_FilePath = m_FilePath.Left(m_iPosIndex);
//	MessageBox(m_FilePath);	
	
/////////////////////////删除缓存文件夹内文件办法
	CFileFind m_ff;
	CString path = m_FilePath + "\\缓存文件";
	
	if (path.Right(1)!=_T("\\"))
		path+=_T("\\");
	
	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	
	while(res)
	{
		res=m_ff.FindNextFile();
		//是文件直接删除
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			DeleteFile(m_ff.GetFilePath());
		}
		else if(m_ff.IsDots())
			continue;
		else if (m_ff.IsDirectory())
		{
			path=m_ff.GetFilePath();
			//是目录时继续递归，删除该目录下的文件
		//	OnDeleteDirectory(path);
			//目录为空后删除目录
		//	RemoveDirectory(path);
		}
	}

	m_ff.Close();
////////////////////////////////////////////

/////////////////////////////////以下是初始化读取母版文件内文件信息
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//	FindFileData.cAlternateFileName = {"doc"};
//	char cs[100] = {".\\Debug\\母版文件\\*.*"};
	CString cs = m_FilePath + "\\母版文件\\*.*";
    hFind = FindFirstFile(cs, &FindFileData);
	
    if(hFind == INVALID_HANDLE_VALUE)
	{
        return 0;
    }
	else
	{
        int count = -1;//从-1开始否则list control第一行会显示错误信息
		
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			char *pd = NULL;
			char as[100];
			
			sprintf(as, "%d", count);//将行号转换为字符串
			
			pd = (char *)malloc(sizeof(FindFileData.cFileName));
			
			strcpy(pd, FindFileData.cFileName);
			LPCTSTR lpszItem; 
			lpszItem = (LPCTSTR)pd;
			
			m_list2.InsertItem(count, as);//插入每一行的列号到list列表
			m_list2.SetItemText(count, 1, lpszItem);/*设置单元格内容*/
			
			free(pd);
			count = count + 1;
		} 
    }
    // 查找停止
    FindClose(hFind);


//////////////////////////////////


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWordDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWordDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWordDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWordDlg::OnOutPutWord() 
{
	// TODO: Add your control notification handler code here
	CString cstext = m_list1.GetItemText(0, 0);
	if (strcmp(cstext, "") == 0)
	{
		MessageBox("对不起，查找不到替换字段");
		return;
	}


	MessageBox("开始导出word文档请勿关闭软件。");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//	FindFileData.cAlternateFileName = {"doc"};
	//	char cs[100] = {".\\Debug\\原始文件\\*.*"};
	CString cs = m_FilePath + "\\缓存文件\\*.*";
				
	hFind = FindFirstFile(cs, &FindFileData);
				
	if(hFind != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			if (strstr(FindFileData.cFileName, ".doc") != NULL || strstr(FindFileData.cFileName, ".docx") != NULL)//缓存文件夹找到的文件必须是doc文件
			{
//				AfxMessageBox(_T(FindFileData.cFileName));
				
				if (Wordapp.m_lpDispatch == NULL)
				{
					if (!Wordapp.CreateDispatch(_T("Word.Application")))//启动word
					{
						AfxMessageBox(_T("没有安装word，请先安装word！"));
						return;
					}
					Wordapp.SetVisible(false);//word程序界面可见
					Wordapp.SetWindowState(1);
					WordDocs = Wordapp.GetDocuments();				
				}

				CString docfilepath = m_FilePath + "\\缓存文件\\" + FindFileData.cFileName;//文件路径
				//参数设定，主要有文件名称，确认转换，只读，添加到最近文档，文档口令等
				COleVariant vFileName(docfilepath), vStrNull(_T(""));
				COleVariant vFalse(short(0), VT_BOOL), vTrue(short(1), VT_BOOL);
				COleVariant vOpt((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
				//打开文档
				WordDoc.AttachDispatch(WordDocs.Open(vFileName, vOpt, vFalse,
					vOpt, vStrNull, vStrNull, vOpt, vStrNull, vStrNull, vOpt,
					vOpt, vTrue, vOpt, vOpt, vOpt, vOpt));
				
				//查找项对应的替换项
				//替换函数的参数设置
				/*	CString findstr = "桃源";
				CString replacestr = "文理学院";
				*/
				int is = 0;
				CString findstr;
				CString replacestr;
				
				while (strcmp(m_list1.GetItemText(is, 0), "") != 0)
				{

					//list1数据不为空，以及同一行两个列的数据不能相同（这个很重要，否则word替换函数不能够替换）
					findstr = m_list1.GetItemText(is, 0);
//					AfxMessageBox(_T(findstr));
					replacestr = m_list1.GetItemText(is, 2);//findstr，replacestr两个字符不能相同，否则就会出问题！
//					AfxMessageBox(_T(replacestr));
					is++;//行数自加1
					
					//参数设置
					CComVariant FindText(findstr), MatchCase(true), MathWholeWord(false);
					CComVariant MatchWildcards(false), MatchSoundsLike(false);
					CComVariant MatchAllWordForms(false), Forward(true), Wrap(1), Format(false);
					CComVariant ReplaceWith(replacestr), Replace(true), MatchKashida(false);
					CComVariant MatchDiacritics(false), MathAlefHamza(false), MathControl(false);
					
					sel = Wordapp.GetSelection();
					sel.WholeStory();//选中全文
					find = sel.GetFind();//查找对象
					replace = find.GetReplacement();//替换对象
					find.ClearFormatting();
					replace.ClearFormatting();
//						AfxMessageBox(_T("qqqq"));
					//flag为查找替换标志，通过while循环对全文所有需要替换内容进行查找替换
					int flag = 1;
					while (flag)
					{
						//主要参数有查找内容，区分大小写，全字匹配，使用通配符等
						flag = find.Execute(&FindText, &MatchCase, &MathWholeWord,
							&MatchWildcards, &MatchSoundsLike, &MatchAllWordForms,
							&Forward, &Wrap, &Format, &ReplaceWith, &Replace,
							&MatchKashida, &MatchDiacritics, &MathAlefHamza, &MathControl);
					}
					

				}
				//保存文件部分
				CComVariant FileName(_T(docfilepath));///docfilepath中是用户指定的报告存储路径
				CComVariant FileFormat(11);//初始化SaveAs函数的参数
				CComVariant LockComments(false), Password(_T(""));
				CComVariant AddToRecentFiles(true), WritePassword(_T("")),
					ReadOnlyRecommended(false), EmbedTrueTypeFonts(false),
					SaveNativePictureFormat(false), SaveFormsData(false);
				CComVariant SaveAsAOCELetter(false), vOptional;
				
				//调用SaveAs函数保存文档
				WordDoc.SaveAs(&FileName, &FileFormat, &LockComments,
					&Password, &AddToRecentFiles, &WritePassword, 
					&ReadOnlyRecommended, &EmbedTrueTypeFonts, &SaveNativePictureFormat,
					&SaveFormsData, &SaveAsAOCELetter, &vOptional, &vOptional,
					&vOptional, &vOptional, &vOptional);
				
				CComVariant SaveChanges(false), OriginalFormat, RouteDocument;
				//调用Quit函数退出Word应用程序
				Wordapp.Quit(&SaveChanges, &OriginalFormat, &RouteDocument);
				//释放所有用到的资源
				replace.ReleaseDispatch();
				sel.ReleaseDispatch();
				find.ReleaseDispatch();
				WordDoc.ReleaseDispatch();
				WordDocs.ReleaseDispatch();
				Wordapp.ReleaseDispatch();				
				
			}
			
		}         
	}
				
	// 查找停止
	FindClose(hFind);

	/////////////////////////复制缓存文件夹内文件到导出文件办法
	CTime time=CTime::GetCurrentTime(); 
	CString strtime;  
	strtime.Format("%02d-%02d-%02d-%02d-%02d-%02d",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
	
	CFileFind m_ff;
	CString path = m_FilePath + "\\缓存文件";
	CString Dest = m_FilePath + "\\导出文件\\" + strtime;

	CreateDirectory(Dest, NULL);//创建导出文件下的具体导出目录

	if (path.Right(1)!=_T("\\"))
		path+=_T("\\");
	
	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	
	while(res)
	{
		res=m_ff.FindNextFile();
		//是文件直接复制过去
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			CString strFileName = m_ff.GetFileName();
//			MessageBox(strFileName);	
//MessageBox(m_FilePath + "\\缓存文件\\" + strFileName);	
//MessageBox(Dest + "\\" + strFileName);	
			CopyFile(m_FilePath + "\\缓存文件\\" + strFileName, Dest + "\\" + strFileName, true);/////////////
		}
		else
		{
			continue;	
		}
	}

	m_ff.Close();
////////////////////////////////////////////
		MessageBox("word文档已导出完毕!请点击“打开文件夹”按钮查看导出文件");
}

void CWordDlg::OnOK() 
{
	// TODO: Add extra validation here
/////////////////////////删除文件夹内文件办法
	CFileFind m_ff;
	CString path = m_FilePath + "\\缓存文件";
	
	if (path.Right(1)!=_T("\\"))
		path+=_T("\\");
	
	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	
	while(res)
	{
		res=m_ff.FindNextFile();
		//是文件直接删除
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			DeleteFile(m_ff.GetFilePath());
		}
		else if(m_ff.IsDots())
			continue;
		else if (m_ff.IsDirectory())
		{
			path=m_ff.GetFilePath();
			//是目录时继续递归，删除该目录下的文件
		//	OnDeleteDirectory(path);
			//目录为空后删除目录
		//	RemoveDirectory(path);
		}
	}

	m_ff.Close();
////////////////////////////////////////////


	m_list1.DeleteAllItems();//按下确定选项按钮后同时刷新m_list1的所有信息
	//先打开要将word内容写入的缓冲txt文件，不存在则新建一个
	FILE *fp = NULL;
	fp = fopen(m_FilePath + "\\缓冲文本文件.txt", "w");//只读打开一个文本文件，只允许读数据打不开则证明不存在这样的文件
	
	WIN32_FIND_DATA FindFileDataStore;
	HANDLE hFinds;
	//	FindFileData.cAlternateFileName = {"doc"};
	CString css = m_FilePath + "\\缓存文件\\*.*";
	
    hFinds = FindFirstFile(css, &FindFileDataStore);
	
    if(hFinds != INVALID_HANDLE_VALUE)
	{
		while (FindNextFile(hFinds, &FindFileDataStore) != 0)
		{
//			MessageBox(FindFileDataStore.cFileName);
			//	char *pd = NULL;
			
			//	pd = (char *)malloc(sizeof(FindFileDataStore.cFileName));
			
			/*		strcpy(pd, FindFileDataStore.cFileName);
			LPCTSTR lpszItem; 
			lpszItem = (LPCTSTR)pd;*/
			
			CString lpszItem = m_FilePath + "\\" + FindFileDataStore.cFileName;
			//删除文件的函数忘记了！！！要添加上来！
			DeleteFile(lpszItem);
			//	free(pd);
		}         
    }
	
    // 查找停止
    FindClose(hFinds);
	
	
	int nId;
	//首先得到点击的位置
	POSITION pos=m_list2.GetFirstSelectedItemPosition();
	
	if(pos==NULL)
	{
		MessageBox("请至少选择一项","提示",MB_ICONEXCLAMATION);
		return;
	}
	else
	{
		while (pos)
		{
			nId = m_list2.GetNextSelectedItem(pos);//得到所在行的行号
			//	sprintf(s1, "%d", nId);
			
			CString strText =  m_list2.GetItemText(nId, 1);//读取所在行的内容即文件名
			CString docfilepath = m_FilePath + "\\母版文件\\" + strText;//要打开的文件路径	
//			MessageBox(_T(strText));
			
			CopyFile(m_FilePath + "\\母版文件\\" + strText, m_FilePath + "\\缓存文件\\" + strText, false);
			
			//	CopyFile(docfilepath, LPCTSTR lpNewFileName,BOOL bFailIfExists );
			
			//DeleteFile();删除文件函数
			COleVariant  vTrue((short)TRUE), vFalse((short)FALSE), vOpt((long)DISP_E_PARAMNOTFOUND,  VT_ERROR);    
			
			if (Wordapp.m_lpDispatch == NULL)
			{
				if (!Wordapp.CreateDispatch(_T("Word.Application")))//启动word
				{
					AfxMessageBox(_T("没有安装word，请先安装word！"));
					return;
				}
			}
			
			Wordapp.SetVisible(false);  
			WordDocs=Wordapp.GetDocuments();  
			WordDoc=WordDocs.Open(COleVariant(docfilepath),vFalse,vTrue,vFalse,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt,vOpt);  
			aRange=WordDoc.Range(vOpt,vOpt);  
			
			//		AfxMessageBox(aRange.GetText());//这里GetText得到的就是word文件的纯文本了,你可以将其写到txt文件中  
			CString InputStr = aRange.GetText();
			fp = fopen(m_FilePath + "\\缓冲文本文件.txt", "a");
			
			if(fp != NULL)
			{
				fseek(fp, 0, SEEK_END);
				fputs(InputStr, fp);//这句应该可以正确执行，如果不行这样改：fputs((const char*)str, fp);
				//	fputs("\r\n",fp);//或fputs("\r\n");
				
				fclose(fp);
			}
			
			
			//退出文件部分
			CComVariant SaveChanges(false), OriginalFormat, RouteDocument;
			//调用Quit函数退出Word应用程序
			Wordapp.Quit(&SaveChanges, &OriginalFormat, &RouteDocument);
			//释放所有用到的资源
			WordDoc.ReleaseDispatch();
			WordDocs.ReleaseDispatch();
			Wordapp.ReleaseDispatch();
			
			//		CopyFile(ExitFile, PutOutFile, false);//指定文件从“原始文件”移动到“缓存文件夹”
		}
		
	}	
//	MessageBox("输入完毕！");
	//	CDialog::OnOK();
	//以下添加缓冲文件txt与配置文件ini的对比查找
	
	//哈哈终于找到方法啦！！！
	FILE *pFile = fopen(m_FilePath + "\\缓冲文本文件.txt", "r");
	char *pBuf;
	fseek(pFile, 0, SEEK_END);//移动文件指针到文件末尾
	int len = ftell(pFile);//获取当前文件指针在文件中的偏移量，Gets the current position of a file pointer.offset
	pBuf = new char[len];
	rewind(pFile);//将指针移动到文件头，Repositions the file pointer to the beginning of a file
	//也可以用fseek(pFile,0,SEEK_SET);
	fread(pBuf, 1, len, pFile);
	pBuf[len] = 0;
	
	fclose(pFile); 
//	MessageBox(pBuf);	
	CString str(pBuf);
//	MessageBox(str);		
	
	int i = 1;
	int cow = 0;
	char buffer[100];
	char ReturnString[100];//查找子项对应的值
	
	
	while (1)//循环读取Ini文件信息和字符串对比
	{
		_itoa(i, buffer, 10);
		CString stri(buffer);
		CString Search = "@" + stri + "#";//查找ini文件的子项
		MessageBox(Search);
		GetPrivateProfileString("配置信息", Search, "%^F%$F$GGHhf", ReturnString, sizeof(ReturnString), m_FilePath + "\\配置文件.ini");
		MessageBox(ReturnString);
		MessageBox(buffer);	
		
		if (strcmp(ReturnString, "%^F%$F$GGHhf") == 0)
		{
			MessageBox("没找到信息");
			break;
		}
		
		if (str.Find(Search) != -1)//字符串里找到代替字段(即子项)了
		{
			m_list1.InsertItem(cow, Search);//插入行
			m_list1.SetItemText(cow, 1, ReturnString);//设置单元格内容
			m_list1.SetItemText(cow, 2, "");//设置单元格内容
			cow = cow + 1;
		}

		i = i + 1;		
		
	}
	
	m_list2.DeleteAllItems();//清空列表2的所有信息
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//	FindFileData.cAlternateFileName = {"doc"};
	CString cs = m_FilePath + "\\母版文件\\*.*";
	
    hFind = FindFirstFile(cs, &FindFileData);
	
    if(hFind == INVALID_HANDLE_VALUE)
	{
        return;
    }
	else
	{
        int count = -1;//从-1开始否则list control第一行会显示错误信息
		
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			char *pd = NULL;
			char as[100];
			
			sprintf(as, "%d", count);//将行号转换为字符串
			
			pd = (char *)malloc(sizeof(FindFileData.cFileName));
			
			strcpy(pd, FindFileData.cFileName);
			LPCTSTR lpszItem; 
			lpszItem = (LPCTSTR)pd;
			
			m_list2.InsertItem(count, as);//插入每一行的列号到list列表
			m_list2.SetItemText(count, 1, lpszItem);/*设置单元格内容*/
			
			free(pd);
			count = count + 1;
		} 
    }
    // 查找停止
    FindClose(hFind);
	MessageBox("信息读取完毕，请继续在“输入内容”文本框内输入信息。");
}

void CWordDlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	//我这里有两种办法可以实现   
	
	NM_LISTVIEW  *pEditCtrl = (NM_LISTVIEW *)pNMHDR;  
	CRect  EditRect;  
	
	//CPoint  point;   
    //GetCursorPos(&point);   
    //m_list.ScreenToClient(&point);   
    //LVHITTESTINFO info;   
	//info.pt = point;   
	//info.flags = LVHT_ABOVE   
    //m_Item = m_list.SubItemHitTest(&info);   
	
    if (pEditCtrl->iItem == -1)  
    {   
		//当点击其他区域时,显示内容   
		if (needSave)  
        {    
            //needSave == TURE   
			CString  EditStr;  
            m_Edit.GetWindowText(EditStr); //获取输入的内容   
			m_list1.SetItemText(m_item,m_Subitem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值   
			m_Edit.ShowWindow(SW_HIDE);  
			m_Edit.ShowWindow(SW_HIDE);  
        }  
		
		return;  
    }  
	//m_SubItem = info.iSubItem;   
	
	
	//在第2列实现编辑   
    if (pEditCtrl->iSubItem == 2 /*&& pEditCtrl->iSubItem != 1*/)  
	{  
        if (needSave)  
		{    
			//needSave == TURE   
            CString  EditStr;  
            m_Edit.GetWindowText(EditStr); //获取输入的内容   
            m_list1.SetItemText(m_item,m_Subitem,EditStr); //m_Item,m_SubItem这两个变量的值是原来的单元格的值   
			
			//		CWnd *pWndCtrl = GetFocus();
			
        }  
		
		m_item = pEditCtrl->iItem; //行数   
        m_Subitem = pEditCtrl->iSubItem; //列数   
		
        m_list1.GetSubItemRect(m_item,m_Subitem,LVIR_LABEL,EditRect); //获取单元格的空间   
		
        if (m_Edit.m_hWnd == NULL)  
        {    //m_Edit.m_hWnd == NULL说明还没创建   
			//RECT rect;   
			
			needSave = true;  
			m_Edit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,  
				CRect(0,0,0,0),this,IDC_EDIT);  
			
            //IDC_EDIT已近在头文件中定义,这个很重要,很多时候会忽略,   
            //网上找到的实现编辑办法中这个都没说明   
			//我定义为#define  IDC_EDIT 0xffff   
			
			m_Edit.ShowWindow(SW_HIDE); //Edit创建完后隐藏   
			m_Edit.SetFont(this->GetFont(),FALSE);//设置字体 
		}  
		m_Edit.SetParent(&m_list1); //将list control设置为父窗口,生成的Edit才能正确定位,这个也很重要,   
		
		EditRect.SetRect(EditRect.left,EditRect.top,EditRect.left+m_list1.GetColumnWidth(m_Subitem),EditRect.bottom);  
		//m_list.GetColumnWidth(m_SubItem)获取列的宽度   
		
		CString strItem = m_list1.GetItemText(m_item,m_Subitem);  
		
		m_Edit.MoveWindow(&EditRect);  
		m_Edit.ShowWindow(SW_SHOW);  
		m_Edit.SetWindowText(strItem);  
		m_Edit.SetFocus();//设置为焦点   
		m_Edit.SetSel(0,-1);  //0,-1表示单元格内容全选中   
	}  
	
	*pResult = 0;
}

void CWordDlg::OnClickList2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	m_list2.SetExtendedStyle(m_list2.GetExtendedStyle()|LVS_EX_FULLROWSELECT);	//使全行都被选中
	
	*pResult = 0;
}

void CWordDlg::OnOnRefresh() 
{
	// TODO: Add your control notification handler code here


	m_list1.DeleteAllItems();
	m_list2.DeleteAllItems();//清空列表2的所有信息
	
/////////////////////////删除缓存文件夹内文件办法
	CFileFind m_ff;
	CString path = m_FilePath + "\\缓存文件";
	
	if (path.Right(1)!=_T("\\"))
		path+=_T("\\");
	
	path+=_T("*.*");
	BOOL res=m_ff.FindFile(path);
	
	while(res)
	{
		res=m_ff.FindNextFile();
		//是文件直接删除
		if (!m_ff.IsDirectory() && !m_ff.IsDots())
		{
			DeleteFile(m_ff.GetFilePath());
		}
		else if(m_ff.IsDots())
			continue;
		else if (m_ff.IsDirectory())
		{
			path=m_ff.GetFilePath();
			//是目录时继续递归，删除该目录下的文件
		//	OnDeleteDirectory(path);
			//目录为空后删除目录
		//	RemoveDirectory(path);
		}
	}

	m_ff.Close();
////////////////////////////////////////////

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//	FindFileData.cAlternateFileName = {"doc"};
//	char cs[100] = {".\\Debug\\母版文件\\*.*"};
	CString cs = m_FilePath + "\\母版文件\\*.*";
    hFind = FindFirstFile(cs, &FindFileData);
	
    if(hFind == INVALID_HANDLE_VALUE)
	{
        return;
    }
	else
	{
        int count = -1;//从-1开始否则list control第一行会显示错误信息
		
		while (FindNextFile(hFind, &FindFileData) != 0)
		{
			char *pd = NULL;
			char as[100];
			
			sprintf(as, "%d", count);//将行号转换为字符串
			
			pd = (char *)malloc(sizeof(FindFileData.cFileName));
			
			strcpy(pd, FindFileData.cFileName);
			LPCTSTR lpszItem; 
			lpszItem = (LPCTSTR)pd;
			
			m_list2.InsertItem(count, as);//插入每一行的列号到list列表
			m_list2.SetItemText(count, 1, lpszItem);/*设置单元格内容*/
			
			free(pd);
			count = count + 1;
		} 
    }
    // 查找停止
    FindClose(hFind);
	
}

void CWordDlg::OnOpenFileLocation() 
{
	// TODO: Add your control notification handler code here
	CString path = m_FilePath + "\\导出文件";
	ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWNORMAL); 
}
