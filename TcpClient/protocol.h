#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed:name existed"
#define LOGIN_OK "login_ok"
#define LOGIN_FAILED "login failed:the user logged in or name/password error!"
#define CANCEL_OK "cancel ok"
#define CANCEL_FAILED "cancel failed:no find the user!"
#define SEARCH_USR_NO "no such people"
#define SEARCH_USR_ONLINE "online"
#define SEARCH_USR_OFFLINE "offline"
#define ALREADY_FRIEND "friend existed"
#define UNKNOW_ERR "unknow error!please check input!"
#define DELETE_ERR "delete friend error!!"
#define DELETE_OK "delete friend successful!"
#define DIR_NOT_EXIST "dir is not exist!"
#define FILE_EXIST "filename is exists!"
#define DIR_CREATE_OK "dir create successful!"
#define DEL_DIR_OK "delete dir successful!"
#define DEL_DIR_FAILURED "delete dir failured"
#define RENAME_OK "file or directory rename successful!"
#define RENAME_FAILURED "file or directory rename failured!"
#define ENTER_DIR_FAILURED "enter dir unseccessful!"
#define UPLOAD_FILE_ERR "upload file error!"
#define UPLOAD_FILE_OK "upload file successful!"
#define DEL_FILE_OK "delete file successful!"
#define DEL_FILE_FAILURED "delete file failured"
#define DOWNLOAD_FILE_ERR "download file error!"
#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILURED "move file failured:is reguler file"
#define COMMON_ERR "operate failed: system is busy"

enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN=0,
    ENUM_MSG_TYPE_REGIST_REQUEST,//注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,//注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST,//登入请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,//登入回复
    ENUM_MSG_TYPE_CANCEL_REQUEST,//注销请求
    ENUM_MSG_TYPE_CANCEL_RESPOND,//注销回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,//在线用户回复
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,//搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,//搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_AGGREE,//同意添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝添加好友
    ENUM_MSG_TYPE_ADD_FRIEND_UNKNOW_ERR,//添加好友不知名错误
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//刷新好友列表请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//刷新好友列表回复
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//删除好友回复
    ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_REQUEST,//私聊好友请求
    ENUM_MSG_TYPE_PRIVATECHAT_FRIEND_RESPOND,//私聊好友回复
    ENUM_MSG_TYPE_MASSTEXT_FRIEND_REQUEST,//群发消息请求
    ENUM_MSG_TYPE_MASSTEXT_FRIEND_RESPOND,//群发消息回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//创建文件夹回复

    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//刷新文件列表请求
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//刷新文件列表回复

    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//删除文件夹请求
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//删除文件夹回复

    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//重命名文件请求
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//重命名文件回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,//进入目录请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,//进入目录回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//文件上传请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//文件上传回复

    ENUM_MSG_TYPE_DEL_FILE_REQUEST,//删除文件请求
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,//删除文件回复

    ENUM_MSG_TYPE_DOWMLOAD_FILE_REQUEST,//下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//下载文件回复

    ENUM_MSG_TYPE_ONLINE_FRIEND_REQUEST,//在线好友请求
    ENUM_MSG_TYPE_ONLINE_FRIEND_RESPOND,//在线好友回复

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//分享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//分享文件回复

    ENUM_MSG_TYPE_SHARE_FILE_NOTE_REQUEST,//分享文件通知请求
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,//分享文件通知回复
    ENUM_MSG_TYPE_SHARE_FILE_SUCCESS,//成功收到文件

    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,//移动文件请求
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,//移动文件回复

    ENUM_MSG_TYPE_MAX=0x00ffffff,
};

struct FileInfo{
    char fileName[32];
    int fileType;
};

struct PDU
{
    uint uiPDULen; //总的协议数据单元大小
    uint uiMsgType; //消息类型
    char data[64];
    uint uiMsgLen;//实际消息长度
    int caMsg[];//实际消息
};

PDU *mkPDU(uint uiMsgLen);
#endif // PROTOCOL_H
