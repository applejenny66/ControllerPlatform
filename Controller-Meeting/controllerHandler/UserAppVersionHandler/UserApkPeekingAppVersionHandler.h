#pragma once

#include <memory>
#include "UserAppVersionHandler.h"

class AndroidPackageInfoQuierer;

// 處理使用者裝置 app 版本更新檢查要求的類別 
class UserApkPeekingAppVersionHandler : public UserAppVersionHandler
{
public:
	explicit UserApkPeekingAppVersionHandler(AndroidPackageInfoQuierer *q, 
		std::string pkgName, std::string dir, std::string dlLinkBase);
	virtual ~UserApkPeekingAppVersionHandler();

	virtual std::string getPackageName();
	virtual int getVersionCode();
	virtual std::string getVersionName();
	virtual std::string getDownloadLink();

protected:	
	// 尋找最新版本的 apk，並取得其版本內容，以此更新此類別的成員數值
	virtual void reload();
	virtual bool onInotifyEvent(struct inotify_event *event);

private:
	std::unique_ptr<AndroidPackageInfoQuierer> apkQuierer;
	std::string apkName;
	const std::string downloadLinkBasePath;	
};