nbphp.so/php_nbphp.dll-1.1
=====
github：<a target="_blank" rel="nofollow" href="https://github.com/oasmichael/nbphp">https://github.com/oasmichael/nbphp</a><p>nbphp.so-php日志扩展<br>
由强哥一手打造<br></p>

<p>Linux 安装方式<br>
phpize<br>
./configure --with-php-config=xxxx(php-config的路径)<br>
make<br>
make install<br>ps:mac系统需要加sudo你懂的</p>
<p>Windows 系统</p><p>dll文件在windll目录内</p><p>nl的目前支持的是wamp server 2.5(php5.5.12)记录日志的目录是在c:\wamp\logs内</p><p>wamp server 2.5 官网地址：<a target="_blank" rel="nofollow" href="http://www.wampserver.com/en/">http://www.wampserver.com/en/</a></p>
<p>功能介绍<br></p><p>支持Linux和Windows系统<br></p><p>支持PHP各类型变量记录日志(字符串、数组、对象、资源等)<br></p>超强的加密函数md6<br><br>支持函数列表
<br>nl(log,cat,logpath) 日志,分类,日志目录（后两个参数是可选参数）<br>md6(sss,salt) 第一个参数是要加密的字符串 第二个是盐（此方法比md5要难破解）
