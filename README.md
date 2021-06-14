# Embedded System HW4

## (1) how to setup and run my program

## 1. XBee Controlled BB Car

### Define calibration table

首先，我先幫車子建立了 calibration table，因為在這次作業中我的速度全部都是由 `set_speed_by_cm` 這個 function 來控制，這樣可以透過直接量測距離來看看車子要走幾秒，比較好算。

![](https://i.imgur.com/3NdFEgb.png)

### BB car function

接下來我先定義了倒車時用來轉彎的函式 `turn_my`，因為我希望他能在軸心（內輪）幾乎不動的情況下轉 90 度，所以當轉彎時我就會將內輪的速度幾乎設為0，不過助教提醒我說還是給那個輪子一點速度會比較順，所以我將內輪的速度設為 1cm/s。

![](https://i.imgur.com/FtqMPmx.png)

### RPC function

我這部份的作業只要知道題目規定的 `d1`、`d2` 及方位，並輸入剛剛的三個參數便可以完成倒車的動作。首先我定義了 `turn_speed` 這個 variable，因為車子兩輪之間的距離是 10.5 cm，換算一下之後，要讓車子轉 90 度的話等於在內輪不動的情況下外輪要走 16.5 cm 左右，也就是用這個速度走一秒，因為考量慣性之類的一些物理上的問題，所以我將這個速度設為 -16（因為要倒車所以是負的）。

在吃進 `d1`、`d2` 及 `pos`（方位）參數後，我會先把 `d1`、`d2` 除以 `turn_speed`，看看車子在 x 方向及 y 方向應該要移動幾秒，在兩個 while 迴圈內便是用算秒數的方式來進行 x、y方向的直線移動。

而在兩個 while 迴圈之間，也就是要轉彎的地方，就呼叫了剛剛提到的 `turn_my` 函式，用 `turn_speed` 轉一秒，並給他 `factor` 參數，看是要往左轉還是往右轉。

![](https://i.imgur.com/dRaLE99.png)


## 2. Line Following BB Car

### Line detection

首先，我是利用 OpenMV 內建的 regression 方法找到一條直線（助教跟我說的），發現滿好用的，只是環境還是要比較乾淨才能去除雜訊，有了這條線之後，我就將他的 `length`、`theta`、`rho` 三個數值傳到我的車子中。

![](https://i.imgur.com/TFlKx7I.jpg)

### Concept Explanation

在這邊我要解釋一下怎麼利用 `rho` 跟 `theta` 這兩個參數來校正車子，使他跟著線的方向移動。

在 OpenMV 的官網中給出了下面這張圖，第一象限內就是 OpenMV 相機可以拍到的範圍，也就是車子看過去的畫面，左上角是原點。

![](https://i.imgur.com/Z9kQglF.png)

`rho` 代表的就是從原點找一條到那條線的「垂直線」的「長度」，當 `rho` 在第四象限時，他的值會是負的，反之當 `rho` 在第一象限時，他的值會是正的。而 `theta` 代表的是從 `rho` 逆時針轉碰到 x 軸時所經過的角度，所以當 `rho` 在第四象限時， 90 < `theta` < 180，而 `rho` 在第一象限時，0 < `theta` < 90。因此我就可以用 `rho` 跟 `theta` 來判斷車子的位置及該調整的角度。

### Function definition

首先我先定義了 `turn_my_forward` 函式，其實就只是剛剛的
 `turn_my` 函式的往前走版本，其實應該可以透過傳入相反的 speed 值就好，不用再定義一個。

![](https://i.imgur.com/gEOPzDa.png)

接下來我定義了 `RPC_go_line` 這個 RPC function，所有動作都會在這裡完成，上面是吃進參數的部分，`turn_speed` 就和上一題一樣。

![](https://i.imgur.com/epmMU1s.png)

接下來我就拿一個方向（`rho` < 0，代表線是往左斜的）來舉例，我在兩個方向裡面都定義了他們的 `dis_time`、`len_time`、`turn_time`，一開始有提到 `rho` 是原點到那條線的「長度」，而我們要先做的是讓車子趨近那條線，所以 `dis_time` 定義了車子要走幾秒才會到那條線。

靠近了那條線後，就是要轉彎讓車子做調整了，所以我利用上一題的概念定義了 `turn_time`，也定義了 `degree_by_x_axis`，因為在 `rho` 小於 0 時，90 < `theta` < 180，但其實車子只要把頭往回擺 180 - `theta` 度就好，所以在此 `degree_by_x_axis = 180 - theta`。

由於車輪畫一圈大約 65.493 cm，所以當我要轉 `degree_by_x_axis` 度時，就要轉 `65.493 x degree_by_x_axis / 360` 公分，再除上 `turn_speed` 代表車子要轉幾秒，而前面的 4 是一個經測試之後訂出來的調整的參數。

經過前面兩個 while 迴圈之後，車子就與線平行了，接下來我們就看看線是多長來決定要走多久。`len_time` 就是線的 `length` 除以 `turn_speed`，就是他要在線上走的時間，跑完這個 while 迴圈之後，車子就漂亮的沿著線校正完，並走到線的盡頭了。

![](https://i.imgur.com/nSdv64p.png)

## 3. BB Car Position Calibration

在這裡我先講解一下我的概念，首先是 AprilTag detection 的部分，我發現當 AprilTag 在鏡頭相對左邊時，他的 y_rotation 值換算成角度後會大概在 2~15 度之間，而當 AprilTag 在相對鏡頭右邊時，他的 y_rotation 值換算成角度後會大概在 345~359 度之間，所以我就用這些區間來判斷車子應該往哪個方向校正，如果介於 {2, 1, 0, 359, 358} 度的話我就判定為車子跟 AprilTag 是垂直的。

在判定好方位後，我就會用 uart 傳一個對應的字元給車子那邊吃，而且為了怕傳輸的頻率太高，導致車子像做小碎步一樣不停的校正，所以我兩秒才傳送一次，讓車子有時間可以往前進一點。

![](https://i.imgur.com/Lb5vLr6.png)

### Car behavior definition

首先我先定義了 `turn_my_carlib` 函式，這也是用來微調車子角度的函式，跟之前的那些 `turn_my` 函式的差別是他內輪的速度較快，其實也是可以透過調整參數來完成，不用重新定義，但這樣在意義上比較分明。

![](https://i.imgur.com/gG8XOoC.png)


在吃進 uart 傳來的訊號之後，便可以判斷車子該往哪轉，或是已經跟 AprilTag 垂直了要直走。若是傳進來的是 `l` 或 `r`，代表車子的角度要微調，所以就呼叫 `turn_my_carlib` 函式進行調整，當收到 `c` 的訊號時，則代表車子已經跟 AprilTag 垂直了，我就讓他再用 5cm/s 的速度走 0.5 秒就停。

![](https://i.imgur.com/zu1H2Tz.png)

### Ping Detection

我定義了 `openmv_detect` 這個讓 ping 用來偵測車子行走前後距離的函式，依據傳入的 `state` 值判定是車子走之前還是之後。

![](https://i.imgur.com/kaYFZMY.png)

所以在車子行走前，我先先做一次測量，而在車子偵測到與 AprilTag 垂直並停止後，我會在偵測一次距離。

![](https://i.imgur.com/PZgaNzc.png)

---

## (2) what are the results

### 1. XBee Controlled BB Car

下圖是我設計的車格，最後車子也有順利停進去，影片已和助教 demo。

![](https://i.imgur.com/imrskou.png)

---

### 2. Line Following BB Car

中間那條垂直牆壁的線就是我要 detect 和 follow 的線，最後也順利完成，影片有 demo 給助教看了。

![](https://i.imgur.com/mjG5nsb.jpg)

### 3. BB Car Position Calibration

從圖中可以看出 ping 在車子移動前會先偵測一次距離，然後車子收到 OpenMV 傳過來的信號並行走，最後 ping 再量一次新的距離。

![](https://i.imgur.com/SoiybnZ.png)
