nbphp 1.0
=====
nbphp.so-php日志扩展
由强哥一手打造

Linux 安装方式
phpize
./configure --with-php-config=xxxx(php-config的路径)
make
make install

ps:mac系统需要加sudo你懂的

功能介绍
支持PHP各类型变量记录日志(字符串、数组、对象、资源等)

支持函数列表
nl(log,cat,logpath) 日志,分类,日志目录(后两个参数是可选参数)