# VlanTagger
<p>gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.1)<br>
Файл <strong>vlantaggerd.service</strong> используется для настройки системной службы<br>
<strong>make</strong> собирает объектный и бинарный файл программы<br>
Для запуска программы запустить скрипт <strong>start</strong><br>
Для просмотра статуса системной службы использовать скрипт <strong>status</strong><br>
Посмотреть системный лог можно командой <strong>grep vlan_tagger /var/log/syslog</strong><br>
Скрипт <strong>stop</strong> останавливает службу<br>
Для удаления файлов сборки используется <strong>make clean</strong><br></p>
<strong>change_rules.c</strong>(запускать с sudo) посылает сигнал изменения правил, позже будет интегрирован в CLI