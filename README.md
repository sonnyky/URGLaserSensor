# UrgLaserSensor
A module to read distance data from URG 10-lx sensor and convert them into touch points. Used to convert walls, tables and other planes into touch screens. Currently supports up to 4 touch points.

## 使い方
ビルドしたらbuild 以下にEXEができるので、ターミナルで実行する。そのときに必要なパラメーターは：
* MinX：X軸上の最小値。これより小さい値は無視される。
* MinY：Y軸上の最小値。これより小さい値は無視される。
* MaxX：X軸上の最大値。これより大きい値は無視される。
* MaxY：Y軸上の最大値。これより大きい値は無視される。
* MaxRange：検知される最大値。遠い物体は無視される。センサは扇形でスキャンしているのでMaxRange はMaxX、MaxYより大きい数字でなければ四角い壁やスクリーンに反応しないエリアが出てくる。
* Ip：距離データの転送先IPアドレス。
* Port：距離データーの転送先ポート。
* Mode：タッチポイントを計算する方法指定。０：一番小さい（近い）点を返す、１：平均を計算する。
* Connection : Ethernet の場合は -e つける。

例：4mx2m の壁、センサーは投影面の上辺から20㎝ 上にある。左右から見ると中央に位置している。
この場合、コマンドとしては、
urglaserrange.exe 200 -2000 2200 2000 2600 192.168.1.x 5001 0 -e

ここで、MaxRange は十分に大きい数字であれば問題ない。
