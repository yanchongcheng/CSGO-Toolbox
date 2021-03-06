﻿#include "mainwindow.h"
#include "ui_mainwindow.h"

//引入库文件，位置必须是绝对地址，有改变务必改动
#pragma comment(lib, "E:/QtProject/CSGO Toolbox/CSGO-Toolbox/libShareCodeToURLcs.lib" )
__declspec(dllimport) int api_Urlstring(const char* a);

//全局变量
QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";
QString steamID = "";
QString userName = "";
bool autoClip = false;
bool autoDownload = false;
bool backupdataZipped = false;

//TODO: 加载过程的文字显示在Loading界面上
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUI();
    readSetting();
    getSteamPath();
    getCsgoPath();
    getSteamID();
    refreshBackup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
  * 程序的核心模块，是完成其他实际功能的基础
  * 1. 程序启动/关闭时调用
  * 2. 获取各种路径
  * 3. 获取SteamID
  */

/*  1. 程序启动/关闭时调用   */
//设置UI样式
void MainWindow::setupUI()
{
    QFont font, font1, font2, font6, font60;
    font.setFamily("微软雅黑");
    qApp->setFont(font);
    font1.setPixelSize(14);
    font2.setPixelSize(17);
    font6.setPixelSize(20);
    font60.setPixelSize(60);
    ui->centralwidget->setFont(font1);
    ui->tabWidget->setFont(font2);
    ui->TabMain->setFont(font2);
    ui->backupSetting->setFont(font2);
    ui->backupdata->setFont(font1);
    ui->checkCsgoPath->setFont(font1);
    ui->checkLauncherPath->setFont(font1);
    ui->checkSteamID->setFont(font1);
    ui->checkSteamPath->setFont(font1);
    ui->getLauncherPathBtn->setFont(font2);
    ui->getUserDataBtn->setFont(font2);
    ui->manualBtn->setFont(font2);
    ui->openBackupLoc->setFont(font2);
    ui->openCNlocalcfg->setFont(font2);
    ui->opencsgoDir->setFont(font2);
    ui->opencsgocfg->setFont(font2);
    ui->openlocalcfg->setFont(font2);
    ui->userdata->setFont(font2);
    ui->zipBackupdata->setFont(font2);
    ui->TabSetting->setFont(font2);
    ui->ManualSteamID->setFont(font2);
    ui->antiHarmony->setFont(font2);
    ui->debug->setFont(font2);
    ui->reloadHarmony->setFont(font2);
    ui->solveVAC->setFont(font2);
    ui->textID->setFont(font6);
    ui->TabDemo->setFont(font2);
    ui->autoClip->setFont(font2);
    ui->autoDownload->setFont(font2);
    ui->clipURL->setFont(font2);
    ui->dispURL->setFont(font2);
    ui->downURL->setFont(font2);
    //ui->dragArea->setFont(font40);
    ui->labelDispURL->setFont(font2);
    ui->labelDragArea->setFont(font60);
    ui->labelSharecode->setFont(font2);
    ui->textEdit->setFont(font6);
    ui->labelDragArea->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkSteamPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkCsgoPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkLauncherPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
}

//读取设置
void MainWindow::readSetting()
{
    QSettings *iniRead = new QSettings("./config.ini", QSettings::IniFormat);
    iniRead->setIniCodec("utf-8");     //解决乱码问题

    iniRead->beginGroup("Paths");
    steamPath = iniRead->value("steamPath").toString();
    launcherPath = iniRead->value("launcherPath").toString();
    csgoPath = iniRead->value("csgoPath").toString();
    iniRead->endGroup();

    iniRead->beginGroup("user");
    userName = iniRead->value("userName").toString();
    steamID = iniRead->value("steamID").toString();
    iniRead->endGroup();

    iniRead->beginGroup("auto");
    autoClip = iniRead->value("autoClip").toBool();
    autoDownload = iniRead->value("autoDownload").toBool();
    iniRead->endGroup();
    //读入完成后删除指针
    delete iniRead;

    //检查如果路径不存在则置空
    if( !isSteamExisted() )
        steamPath = "";
    if( !isLauncherExisted() )
        launcherPath = "";
    if( !isCsgoExisted() )
        csgoPath = "";

    //改变路径、iD相关的checkbox状态
    onSteamPathChanged();
    onCsgoPathChanged();
    onLauncherPathChanged();
    onSteamIDChanged();

    //设置ui中的checkbox状态
    if(autoClip == true)
        ui->autoClip->setCheckState(Qt::Checked);
    else
        ui->autoClip->setCheckState(Qt::Unchecked);
    if(autoDownload == true)
        ui->autoDownload->setCheckState(Qt::Checked);
    else
        ui->autoDownload->setCheckState(Qt::Unchecked);
}

//写入设置
void MainWindow::writeSetting()
{
    //判断当前路径变量是否正确，否则置空
    if( !isSteamExisted() )
        steamPath = "";
    if( !isLauncherExisted() )
        launcherPath = "";
    if( !isCsgoExisted() )
        csgoPath = "";

    //写设置
    QSettings *IniWrite = new QSettings("./config.ini", QSettings::IniFormat);
    IniWrite->setIniCodec("utf-8");     //解决乱码问题

    IniWrite->beginGroup("AppInfo");
    IniWrite->setValue("Name", qAppName());
    IniWrite->setValue("Version", APP_VERSION);
    IniWrite->endGroup();

    IniWrite->beginGroup("Paths");
    IniWrite->setValue("steamPath", steamPath);
    IniWrite->setValue("launcherPath", launcherPath);
    IniWrite->setValue("csgoPath", csgoPath);
    IniWrite->endGroup();

    IniWrite->beginGroup("user");
    IniWrite->setValue("userName", userName);
    IniWrite->setValue("steamID", steamID);
    IniWrite->endGroup();

    IniWrite->beginGroup("auto");
    IniWrite->setValue("autoClip", autoClip);
    IniWrite->setValue("autoDownload", autoDownload);
    IniWrite->endGroup();
    //写入完成后删除指针
    delete IniWrite;
}

//关闭主界面时保存设置
void MainWindow::closeEvent(QCloseEvent *e)
{
    writeSetting();
    e->accept();
}

/*  2. 获取各种路径  */
//获取Steam路径
bool MainWindow::getSteamPath()
{
#define STEAM_KEY "HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam" //注册表SteamExe项所在位置
    //第一步 读取设置中的steamPath 已经在main()中调用 readSetting()
    QString tPath = steamPath;

    //第二步 尝试默认路径
    if( !isSteamExisted(tPath) ){
        QStringList defSteamPath;
        defSteamPath
                <<  QStringLiteral("C:/Program Files (x86)/steam/Steam.exe")
                 <<  QStringLiteral("C:/Steam/Steam.exe")
                  <<  QStringLiteral("D:/Steam/Steam.exe")
                   <<  QStringLiteral("E:/Steam/Steam.exe")
                    <<  QStringLiteral("G:/Steam/Steam.exe")
                     <<  QStringLiteral("H:/Steam/Steam.exe")
                         ;

        for(int i = 0; i< defSteamPath.size(); i++)
        {
            if( isSteamExisted( defSteamPath.at(i) ) ){
                tPath = defSteamPath.at(i);
                break;
            }
        }
    }

    //第三步 读取注册表中路径  //HKEY_CURRENT_USER\SOFTWARE\Valve\Steam 查询SteamExe
    if( !isSteamExisted(tPath) ){
        QSettings *pReg = new QSettings(STEAM_KEY, QSettings::NativeFormat);
        tPath = pReg->value("SteamExe").toString(); //读取注册表值
        //pReg->setValue("key", "value");   //设置注册表值
        delete pReg;
    }

    //第四步 读取进程列表 要打开steam
    if( !isSteamExisted(tPath) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("steam.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("steam.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( isSteamExisted(tPath) ){
        steamPath = tPath;
        onSteamPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取Steam路径失败，请手动选择！");
        return false;
    }
}

//获取国服启动器csgoLauncher路径
bool MainWindow::getLauncherPath()
{
#define Launcher_KEY "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam\\NSIS" //注册表CSGO项所在位置 TODO:
    //第一步 读取设置中的launcherPath 已经在main()中调用 readSetting()
    QString tPath = launcherPath;

    //第二步 读取注册表中路径
    if( !isLauncherExisted(tPath) ){
        QSettings *pReg = new QSettings(Launcher_KEY, QSettings::NativeFormat);
        tPath = pReg->value("Path").toString(); //读取注册表值
        delete pReg;
        if( !tPath.isEmpty() )
            tPath = tPath.append("\\csgolauncher.exe");
    }

    //第三步 读取进程列表 要打开国服启动器
    if( !isLauncherExisted(tPath) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgolauncher.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgolauncher.exe");
        }
    }

    //第四步 自动获取路径失败 提示用户手动选择路径
    if( isLauncherExisted(tPath) ){
        launcherPath = tPath;
        onLauncherPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取国服客户端路径失败，请手动选择！");
        tPath = QFileDialog::getOpenFileName(this, "选择国服启动器csgolauncher.exe的位置", tPath, "csgolauncher.exe");
        if( !isLauncherExisted(tPath) ){
            return false;
        }
        launcherPath = tPath;
        return false;
    }
}

//获取国服启动器路径的槽函数
void MainWindow::on_getLauncherPathBtn_clicked()
{
    getLauncherPath();
}

//获取CSGO路径
bool MainWindow::getCsgoPath()
{
    //第一步 读取设置中的csgoPath 已经在main()中调用 readSetting()
    QString tPath = csgoPath;

    //第二步 尝试默认路径
    if( !isCsgoExisted(tPath) ){
        QStringList defCsgoPath;
        defCsgoPath
                <<  QStringLiteral("C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                 <<  QStringLiteral("C:/steam/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                  <<  QStringLiteral("D:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                   <<  QStringLiteral("E:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                    <<  QStringLiteral("F:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                     <<  QStringLiteral("G:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                      <<  QStringLiteral("H:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                          ;

        for(int i = 0; i< defCsgoPath.size(); i++)
        {
            if( QFile::exists( defCsgoPath.at(i) ) ){
                tPath = defCsgoPath.at(i);
                break;
            }
        }
    }

    //第三步 根据Steam或国服启动器获得路径
    if( !isCsgoExisted(tPath) ){
        if( isSteamExisted() ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
        else if( isCsgoExisted() ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
    }

    //第四步 读取进程列表 要打开steam和csgo 会强制关闭CSGO
    if( !isCsgoExisted(tPath) ){
        QUrl url( "steam://rungameid/730" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgo.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgo.exe");
            cmd("taskkill /f /t /im csgo.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( isCsgoExisted(tPath) ){
        csgoPath = tPath;
        onCsgoPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取CSGO路径失败，请手动选择！");
        return false;
    }
}

//手动设置路径
void MainWindow::on_manualBtn_clicked()
{
    QString tPath = "";
    if( !isSteamExisted() ){
        tPath = QFileDialog::getOpenFileName(this, "选择steam.exe的位置", tPath, "steam.exe");
        if( !isSteamExisted(tPath) ){
            return;
        }
        steamPath = tPath;
    }
    if( !isCsgoExisted()){
        tPath = QFileDialog::getOpenFileName(this, "选择csgo.exe的位置", tPath, "csgo.exe");
        if( !isCsgoExisted(tPath) ){
            return;
        }
        csgoPath = tPath;
    }
    //已有路径的处理 TODO: 路径设置好后直接enabled 0
    if( isSteamExisted() && isCsgoExisted() ){
        QMessageBox::warning(this, "提示", "路径已有，无需手动选择！");
    }
}

//手动选择csgo位置
void MainWindow::on_manualCsgoBtn_clicked()
{
    QString tPath = QFileDialog::getOpenFileName(this, "选择csgo.exe的位置", tPath, "csgo.exe");
    if( !isCsgoExisted(tPath) ){
        return;
    }
    csgoPath = tPath;
}

/*  3. 获取SteamID  */
//获取SteamID，一般9位数字 TODO: 错误处理 &
void MainWindow::getSteamID()
{
    QString fileName = "";
    if( isSteamExisted() ){
        fileName = steamPath;
        fileName.replace("steam.exe" ,"config\\loginusers.vdf", Qt::CaseInsensitive );
    }
    else if( isLauncherExisted() ){
        fileName = launcherPath;
        fileName.replace("csgolauncher.exe" ,"config\\loginusers.vdf", Qt::CaseInsensitive );
    }
    else return;

    QFile f( fileName );
    if ( !f.exists() ) //文件不存在
        return;
    if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;
    QTextStream fStream(&f); //用文本流读取文件
    fStream.setCodec("utf-8");  //解决乱码问题
    QString content =  fStream.readAll();
    f.close();
    //去除无用字符
    content.replace("\n", "");
    content.replace("\r", "");
    content.replace("\t", "");
    search_and_cut(content, "users\"{");
    QStringList SteamID,AccountName,PersonaName;
    //获取数据
    for(int i = 0 ; true ; i++){
        search_and_cut(content, "\"");
        //获取SteamID
        QString t = get_until(content, "\"");
        if( !t.isEmpty() ){
            qulonglong temp = t.toULongLong();
            temp -= 76561197960265728;
            SteamID <<  QString::number( temp );
        }
        //获取账号名
        t = getValue(content ,"AccountName");
        if( !t.isEmpty() )
            AccountName <<  t;
        //获取用户昵称
        t = getValue(content ,"PersonaName");
        if( !t.isEmpty() )
            PersonaName <<  t;
        search_and_cut(content, "}");
        if( content.startsWith("}") )
            break;
    }
    //单用户时直接确定ID
    if(SteamID.length() == 1){
        steamID = SteamID.at(0);
        userName = PersonaName.at(0);
        onSteamIDChanged();
        //ui->debug->appendPlainText("检测到只有一个用户ID，已设置好SteamID！\n SteamID -< " + steamID);
    }
    else{   //显示在tabview上 用户按一次按钮完成选择        //TODO: debug
        QFont font;
        font.setPixelSize(14);
        short n = SteamID.length();
        ui->userdata->setColumnCount(3);
        ui->userdata->setRowCount(n);
        ui->userdata->setColumnWidth(0,98);
        ui->userdata->setColumnWidth(1,200);
        ui->userdata->setColumnWidth(2,110);
        //ui->userdata->setColumnWidth(3,83);

        QStringList header;
        header <<  "SteamID" << "昵称" << "";
        ui->userdata->setHorizontalHeaderLabels(header);

        for(short i = 0; i < n; i++){
            ui->userdata->setRowHeight(i, 35);
            ui->userdata->setItem(i, 0, new QTableWidgetItem(SteamID.at(i)));
            ui->userdata->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->userdata->setItem(i, 1, new QTableWidgetItem(PersonaName.at(i)));
            //ui->userdata->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            QPushButton *button = new QPushButton();
            connect(button, SIGNAL(clicked()), this, SLOT(onTableBtnClicked()));
            // 在QTableWidget中添加控件
            ui->userdata->setCellWidget(i, 2, button);
            button->setProperty("id", i);
            button->setProperty("text", "选择");
            button->setProperty("status", "normal");
            button->setFont(font);
        }
        ui->userdata->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

//选择按钮按下时切换SteamID
void MainWindow::onTableBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->userdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    steamID = ui->userdata->item(row, 0)->text();
    userName = ui->userdata->item(row, 1)->text();
    //ui->debug->setPlainText( "SteamID <- " + steamID );
    onSteamIDChanged();
}

//手动输入SteamID TODO: 进一步加强
void MainWindow::on_ManualSteamID_clicked()
{
    QString tID = ui->textID->toPlainText();
    if( tID.isEmpty() )
        return;
    else if( !isDigitStr(tID) || tID.length() < 8 || tID.length() > 10 ){
        ui->debug->appendPlainText("ID格式有误！");
        return;
    }
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("Steam.exe", "userdata/" +tID, Qt::CaseInsensitive);
        if( QFile::exists( tPath ) ){
            steamID = tID;
            onSteamIDChanged();
            ui->debug->appendPlainText("成功！");
            ui->textID->setText("");
            return;
        }
        else
            ui->debug->appendPlainText("userdata目录下检测不到该ID对应的文件夹，注意检测ID正误");
    }
    else if( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" +tID, Qt::CaseInsensitive);
        if( QFile::exists( tPath ) ){
            steamID = tID;
            onSteamIDChanged();
            ui->debug->appendPlainText("成功！");
            ui->textID->setText("");
            return;
        }
        else
            ui->debug->appendPlainText("userdata目录下检测不到该ID对应的文件夹，注意检测ID正误");
    }
    else{
        ui->debug->appendPlainText("无Steam或国服启动器路径，无法识别，注意检测ID正误");
    }
}

//按钮槽函数
void MainWindow::on_getUserDataBtn_clicked()
{
    getSteamID();
}

/**
 *  拓展功能，基于核心模块和封装好的功能模块，完成具体的功能
 *  包括：
 *  1. 转换DEMO分享代码 获得真实下载链接
 *  2. 修复VAC验证问题
 *  3. 备份和还原设置（个人cfg文件夹）
 *  4. 国服反和谐和反和谐恢复
 *  5. 打开各种cfg文件夹和csgo位置
 */

/*------------------------------------------------------------------------------------*/
// 转换DEMO分享代码 获得真实下载链接  注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::sharecodeTransform()
{
    const QString ShareCodePattern = "^CSGO(-?[\\w]{5}){5}$";
    QRegExp reg( ShareCodePattern );     //正则表达式

    //显示提示
    ui->dispURL->setText("正在转换链接。。。");

    //1. 读入分享代码（参照之前的项目）
    QString ShareCode = ui->dragArea->toPlainText();
    if( !QString(ShareCode).isEmpty() )
        ui->dragArea->setText("");
    else
        return;

    //2. 去掉分享代码的无用信息（得到标准格式） "CSGO-xxx"的长度为34
    ShareCode = ShareCode.remove(0, ShareCode.lastIndexOf("CSGO-") ).left(34);

    //3. 正则表达式匹配
    if( !reg.exactMatch( ShareCode )){
        ui->dispURL->setText("分享链接格式不正确！");
        return;     //TODO: 链接格式不正确的处理
    }

    //4. 标准格式转换成获取3个id需要的字符串格式（20个字符）
    ShareCode.replace("CSGO-","");
    ShareCode.replace("-", "");

    //5. 调用@Yellowfisher编译的DLL库（核心代码源于github.com/akiver/CSGO-Demos-Manager）
    //    获得URL（需要DLL+boiler.exe+steam_api.dll+steam_appid.txt）
    //6. 打开Steam 关闭CSGO
    cmd("explorer.exe \"steam://rungame\"");
    cmd("taskkill /f /t /im csgo.exe");
    do{
        stall(100);
    }while( getProcessPath("csgo.exe").endsWith("exe") );

    //调用DLL库中函数 因为限制只能传进去char数组 故此处转换
    QByteArray ba=ShareCode.toLatin1();
    char *c = ba.data();
    QString retValue = QString::number( api_Urlstring( c ) );
    if( retValue != "0" ){
        ui->dispURL->setText( " 转换失败！错误代码：" + retValue );
        return;
    }

    QFile f("URL.txt");
    if ( !f.exists() ) //文件不存在
        return;
    if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;
    QTextStream fStream(&f); //用文本流读取文件
    fStream.setCodec("utf-8");  //解决乱码问题
    QString URL =  fStream.readAll();
    URL.replace("\n", "");
    URL.replace("\r", "");
    f.close();

    if ( QFile::exists("matches.dat") )
        QFile::remove("matches.dat");
    if ( QFile::exists("URL.txt") )
        QFile::remove("URL.txt");

    if( URL.startsWith("http://") )
        ui->dispURL->setText(URL);
    if( autoDownload == true )
        on_downURL_clicked();
    if( autoClip == true )
        on_clipURL_clicked();
}

//开始下载DEMO
void MainWindow::on_downURL_clicked()
{
    QString URL = ui->dispURL->toPlainText();
    if( URL.startsWith("http://") ){
        QUrl url( URL );
        QDesktopServices::openUrl(url);
    }
    else
        return;
}

//复制DEMO下载链接
void MainWindow::on_clipURL_clicked()
{
    QString URL = ui->dispURL->toPlainText();
    if( URL.startsWith("http://") ){
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(URL);                     //设置剪贴板内容</span>
    }
    else
        return;
}

//自动下载checkbox按下
void MainWindow::on_autoDownload_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoDownload = true;
    else
        autoDownload = false;
}

//自动复制checkbox按下
void MainWindow::on_autoClip_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoClip = true;
    else
        autoClip = false;
}

//拖拽区域文字发生改变时调用 注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::on_dragArea_textChanged()
{
    sharecodeTransform();
}

/*------------------------------------------------------------------------------------*/
//修复VAC验证问题 TODO:
void MainWindow::solveVacIssue()
{

    //思路：
    //    1. 关闭steam或者国服启动器
    //    2. 开启 Network Connections
    //        开启 Remote Access Connection Manager
    //        开启 Telephony
    //        开启 Windows Firewall
    //   3. 恢复 Data Execution Prevention 启动设置为默认值
    //   4. 获取steam或国服启动器目录（Done）
    //   5. 重装Steam Services
    //            steamservice  /uninstall
    //            steamservice  /install
    //        # 等待出现"Add firewall exception failed for steamservice.exe"
    //   6. 启动Steam Services服务
    //            start "Steam Client Service"
    //   7. 其他诸如设置快捷方式 打开网吧模式
    //            ? start /high steam -console -cafeapplaunch -forceservice

    //1.
    cmd("taskkill /F /IM Steam.exe");
    cmd("taskkill /F /IM csgolauncher.exe");
    //2.
    cmd("sc config Netman start= AUTO");
    cmd("sc start Netman");
    cmd("sc config RasMan start= AUTO");
    cmd("sc start RasMan");
    cmd("sc config TapiSrv start= AUTO");
    cmd("sc start TapiSrv");
    cmd("sc config MpsSvc start= AUTO");
    cmd("sc start MpsSvc");
    cmd("netsh advfirewall set allprofiles state on");
    //3.
    cmd("bcdedit /deletevalue nointegritychecks");
    cmd("bcdedit /deletevalue loadoptions");
    cmd("bcdedit /debug off");
    cmd("bcdedit /deletevalue nx");
    //4.
    QString tPath;
    if( isSteamExisted() )
        tPath = steamPath;
    else if( isLauncherExisted() )
        tPath = launcherPath;
    else
        return;

    tPath.replace("steam.exe", "bin/", Qt::CaseInsensitive);
    //5.
    cmd_dir("steamservice  /uninstall", tPath);
    stall(100);
    cmd("ping -n 3 127.0.0.1>nul");
    stall(100);
    cmd_dir("steamservice  /install", tPath);
    stall(100);
    //6.
    cmd("sc config \"Steam Client Service\" start= AUTO");
    cmd("sc start \"Steam Client Service\"");
    //debug测试
    QMessageBox::warning(this, "提示", "指令已全部执行，如果不好使请到QQ群反馈，见'关于'");
}

//槽函数
void MainWindow::on_solveVAC_clicked()
{
    solveVacIssue();
}

/*------------------------------------------------------------------------------------*/
//备份设置 TODO:
void MainWindow::on_backupSetting_clicked()
{
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString zipName = current_date_time.toString("yyyy-MM-dd_hh-mm-ss_%1").arg(userName.left(6)) + ".zip";
    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    if (!dir.exists(tPath))   dir.mkpath(tPath);
    //压缩
    JlCompress::compressDir(zipPath + zipName, tPath);
    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "备份成功！");
    refreshBackup();
}

//刷新备份backupdata域
void MainWindow::refreshBackup()
{   //显示在tablewidget上 用户按一次按钮完成选择
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir(zipPath);
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);

    QStringList nameFilters;
    nameFilters << "*.zip";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    short n = files.length();
    ui->backupdata->setColumnCount(3);
    ui->backupdata->setRowCount(n);
    ui->backupdata->setColumnWidth(0,268);
    ui->backupdata->setColumnWidth(1,70);
    ui->backupdata->setColumnWidth(2,70);

    QFont font;
    font.setPixelSize(14);
    QStringList header;
    header <<  "备份文件" << "" << "" ;
    ui->backupdata->setHorizontalHeaderLabels(header);

    for(short i = 0; i < n; i++){
        ui->backupdata->setRowHeight(i, 35);
        ui->backupdata->setItem(i, 0, new QTableWidgetItem(files.at( n - i - 1 )));

        QPushButton *DeleteButton = new QPushButton();
        connect(DeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteBtnClicked()));
        ui->backupdata->setCellWidget(i, 1, DeleteButton);
        DeleteButton->setProperty("id", i);
        DeleteButton->setProperty("text", "删除");
        DeleteButton->setProperty("status", "normal");
        DeleteButton->setFont(font);

        QPushButton *RestoreButton = new QPushButton();
        connect(RestoreButton, SIGNAL(clicked()), this, SLOT(onRestoreBtnClicked()));
        ui->backupdata->setCellWidget(i, 2, RestoreButton);
        RestoreButton->setProperty("id", i);
        RestoreButton->setProperty("text", "还原");
        RestoreButton->setProperty("status", "normal");
        DeleteButton->setFont(font);
    }
    ui->backupdata->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//恢复按钮槽函数
void MainWindow::onRestoreBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->backupdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    //检查ID和Path
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QString zipName = ui->backupdata->item(row, 0)->text();
    QString bakName = "CFG备份.zip";
    QString bakPath = tPath;
    bakPath.replace("cfg", "");
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    if (!dir.exists(tPath))   dir.mkpath(tPath);
    //如果备份文件不存在，报提示
    if( !QFile(zipPath + zipName).exists() ){
        QMessageBox::warning(this, "提示", "该备份已不存在，还原失败！");
        return;
    }

    //把当前cfg文件夹内容备份在local中，删除cfg文件夹内容
    JlCompress::compressDir(bakPath + bakName, tPath);
    dir.setPath(tPath);
    dir.removeRecursively();
    //dir.mkdir(tPath);

    JlCompress::extractDir(zipPath + zipName, tPath);
    QMessageBox::warning(this, "提示", "还原成功！");
    ui->debug->setPlainText( "row = " + QString::number(row) );
}

//删除按钮槽函数
void MainWindow::onDeleteBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->backupdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    //检查ID和Path
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QString zipName = ui->backupdata->item(row, 0)->text();
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    //如果备份文件不存在，报提示
    if( !QFile(zipPath + zipName).exists() ){
        QMessageBox::warning(this, "提示", "该备份已不存在，删除失败！");
        return;
    }

    QFile::remove(zipPath + zipName);
    ui->backupdata->removeRow(row);
    QMessageBox::warning(this, "提示", "删除成功！");
    refreshBackup();
}

//备份按钮槽函数
void MainWindow::on_openBackupLoc_clicked()
{
    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    QUrl url( "file:///" + zipPath);
    QDesktopServices::openUrl(url);
}

//备份区域折叠/展开切换
void MainWindow::on_zipBackupdata_clicked()
{
    if( backupdataZipped == true ){
        ui->backupdata->setGeometry(10, 410, 410, 100);
        ui->zipBackupdata->setText("↑");
        ui->getUserDataBtn->setVisible(true);
        backupdataZipped = false;
    }
    else{
        ui->backupdata->setGeometry(10, 130, 410, 380);
        ui->zipBackupdata->setText("↓");
        ui->getUserDataBtn->setVisible(false);
        backupdataZipped = true;
    }
}

/*------------------------------------------------------------------------------------*/
//国服反和谐
void MainWindow::on_antiHarmony_clicked()
{
    QStringList files;
    QString tPath = "";
    if( isCsgoExisted() ){
        tPath = csgoPath;
        tPath.replace("csgo.exe", "csgo/", Qt::CaseInsensitive);
    }
    else return;
    files << "pakxv_audiochinese_000" << "pakxv_audiochinese_001" << "pakxv_audiochinese_002"
          << "pakxv_audiochinese_003"   << "pakxv_audiochinese_004" <<  "pakxv_audiochinese_dir"
          << "pakxv_lowviolence_000"      << "pakxv_lowviolence_dir"        <<  "pakxv_perfectworld_000"
          <<"pakxv_perfectworld_001"     <<  "pakxv_perfectworld_dir";
    //.vpk->.bak
    for (int i = 0; i < files.length() ; i++) {
        if( QFile::exists( tPath + files.at(i) + ".vpk" ) ){
            //先检测并删除.bak，否则rename不会覆盖
            if( QFile::exists( tPath + files.at(i) + ".bak" ) )
                QFile::remove( tPath + files.at(i) + ".bak" );
            QFile::rename(tPath + files.at(i) + ".vpk", tPath + files.at(i) + ".bak");
        }
    }

    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "反和谐成功！");
}

//恢复反和谐
void MainWindow::on_reloadHarmony_clicked()
{
    QStringList files;
    QString tPath = "";
    if( isCsgoExisted() ){
        tPath = csgoPath;
        tPath.replace("csgo.exe", "csgo/", Qt::CaseInsensitive);
    }
    else return;
    files << "pakxv_audiochinese_000" << "pakxv_audiochinese_001" << "pakxv_audiochinese_002"
          << "pakxv_audiochinese_003"   << "pakxv_audiochinese_004" <<  "pakxv_audiochinese_dir"
          << "pakxv_lowviolence_000"      << "pakxv_lowviolence_dir"        <<  "pakxv_perfectworld_000"
          <<"pakxv_perfectworld_001"     <<  "pakxv_perfectworld_dir";

    //.bak->.vpk
    for (int i = 0; i < files.length() ; i++) {
        if( QFile::exists( tPath + files.at(i) + ".bak" ) ){
            //先检测并删除.vpk，否则rename不会覆盖
            if( QFile::exists( tPath + files.at(i) + ".vpk" ) )
                QFile::remove( tPath + files.at(i) + ".vpk" );
            QFile::rename(tPath + files.at(i) + ".bak", tPath + files.at(i) + ".vpk");
        }
    }

    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "反和谐已恢复！");
}

/*------------------------------------------------------------------------------------*/

//打开local/cfg文件夹
void MainWindow::on_openlocalcfg_clicked()
{
    if( !isSteamExisted() ){
        return;
    }
    QString tPath = steamPath;
    tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);

    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开国服启动器local/cfg文件夹 TODO:
void MainWindow::on_openCNlocalcfg_clicked()
{
    if( !isLauncherExisted() ){
        return;
    }
    QString tPath = launcherPath;
    tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开csgo/cfg文件夹
void MainWindow::on_opencsgocfg_clicked()
{
    if( !isCsgoExisted() ){
        return;
    }

    QString tPath = csgoPath;
    tPath.replace("csgo.exe", "csgo/cfg", Qt::CaseInsensitive);

    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开csgo位置
void MainWindow::on_opencsgoDir_clicked()
{
    if( isCsgoExisted() ){
        QString tPath = csgoPath;
        tPath.replace("csgo.exe", "", Qt::CaseInsensitive);
        QDir dir;
        QUrl url( "file:///" + tPath);
        QDesktopServices::openUrl(url);
    }
    else return;
}

/*------------------------------------------------------------------------------------*/

/**
 *  功能模块封装，给其他功能调用，减少代码冗余
 */

//停顿time(ms)时间
void MainWindow::stall(int time){
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//判断一个字符串是否为纯数字
bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s)
    {
        if(*s>='0' && *s<='9'){}
        else {
            bret = false;
            break;
        }
        s++;
    }
    return bret;
}

/*------------------------------------------------------------------------------------*/
//检查steam路径是否正确，返回布尔值，如果调用时使用参数，则检查该路径
bool MainWindow::isSteamExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = steamPath;
    if( QFile(tPath).exists() && tPath.endsWith("steam.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;

}

bool MainWindow::isLauncherExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = launcherPath;
    if( QFile(tPath).exists() && tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;
}

bool MainWindow::isCsgoExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = csgoPath;
    if( QFile(tPath).exists() && tPath.endsWith("csgo.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;
}

/*------------------------------------------------------------------------------------*/
//获得进程路径，对cmd函数进行封装并调用wmic指令
QString MainWindow::getProcessPath(QString processName)
{   //processid,name
    processName = "wmic process where name='" + processName + "' get executablepath";
    processName = cmd(processName);
    processName = processName.replace("ExecutablePath", "");
    processName = processName.replace("\r", "");
    processName = processName.replace("\n", "");
    processName = processName.simplified();

    return processName;
}

//调用CMD控制台指令并返回结果，正确执行时返回标准输出通道的内容，执行出错时返回错误通道的内容
QString MainWindow::cmd(QString command)
{
    QProcess p;
    QString temp;
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    temp = QString::fromLocal8Bit(p.readAll());
    p.close();
    return temp;
}

//调用CMD指令 并设定工作路径 TODO: 测试是否成功设置工作路径
QString MainWindow::cmd_dir(QString command, QString dir)
{
    QProcess p;
    QString temp;
    p.setWorkingDirectory(dir);
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    temp = QString::fromLocal8Bit(p.readAll());
    p.close();
    return temp;
}

/*------------------------------------------------------------------------------------*/
//剪切掉相应的部分 aaakeybbb -> bbb
QString MainWindow::search_and_cut(QString &input, QString key)
{
    qint32 i = input.indexOf(key, Qt::CaseInsensitive);
    if( i >= 0 )
        input.remove(0, i + key.length() );

    return "";
}

//把第一次出现end之前的串提取出来  xxxendyyy -> xxx
QString MainWindow::get_until(QString input, QString end)
{
    int i = input.indexOf(end, Qt::CaseInsensitive);
    return input.left( i );
}

//封装search_and_cut()和get_until()实现从字符串中提取 "key"  "data"格式的data串
QString MainWindow::getValue(QString input, QString key)
{
    key = "\"" + key + "\"";
    search_and_cut(input, key);
    search_and_cut(input, "\"");
    return get_until(input, "\"");
}

/*------------------------------------------------------------------------------------*/

/**
 *  其他SLOT槽函数
 */

void MainWindow::onSteamPathChanged()
{
    if( isSteamExisted() ){
        ui->checkSteamPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkSteamPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onCsgoPathChanged()
{
    if( isCsgoExisted() ){
        ui->checkCsgoPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkCsgoPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onLauncherPathChanged()
{
    if( isLauncherExisted() ){
        ui->checkLauncherPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkLauncherPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onSteamIDChanged()
{
    if( steamID.isEmpty() ){
        ui->checkSteamID->setText("ID: ×");
    }
    else
        ui->checkSteamID->setText("ID: " + steamID);
}

/*
    //自定义启动项指令 "F:\CSGO国服\csgolauncher.exe" -silent
    // 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Valve\Steam\NSIS  Path
    //国服 计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
    //计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store

    //获得计算机名
    //QString machineName = QHostInfo::localHostName();
    //ui->debug->setPlainText( machineName );
    //ui->debug->setPlainText( QCoreApplication::applicationDirPath() );    //exe所在路径
    //ui->debug->setPlainText( QDir::currentPath() ); //项目路径 当前路径

    if(QFile::exists(steamPath)) ;   //拖拽可能用到的代码
      if( tPath.endsWith("windeployqt.exe",Qt::CaseSensitive) ){
        steamPath.replace("file:///","");        //去除拖拽产生的前缀

        if( !QString(tPath).isEmpty() )

#include <QFile>

void ReadLine()
{

    QFile file("要读的文件路径");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            QString str(line);
            qDebug() << str;
            displayString << str;
        }
        file.close();

    }
}

void WriteLine()
{

    QFile file("要写的文件路径");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.seek(file.size());
        for (auto& i : displayString)
        {
            QString qs;
            qs.append("Content:");
            qs.append(i);
            qs.remove("\n");
            stream << qs << "\n";
        }
        file.close();
    }
}

*/
