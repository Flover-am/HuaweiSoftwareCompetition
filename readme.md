# 华为软件精英大赛项目代码

## 目录结构：

- `maps`：地图文件
- `Robot`：两个robot.exe
- `gui_run.bat` :lds 暂时写的运行文件
- `CodeCraft_zip.sh` :应该是打包工具，打包后上传手动上传到网站跑分

## 类：

- `Robot`：机器人
- `Station`：工作台
- `dataTable`:数据存储共享类（包含机器人，工作台，frame，钱）
- `Logger`：日志类


- gitignore：
    - `/reply` ：本地生成的回放文件，可以用官方提供的播放器播放
        - 注意：播放器文件过大，不要把他放到git中提交，会炸库
    - ` /CodeCraft-2023.zip`:打包提交的代码文件

###  *Learned*:

- 静态成员变量不能通过对象调用，否则会报错