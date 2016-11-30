set title "Offloading (axpy_kernel) Profile on 6 Devices"
set yrange [0:72.000000]
set xlabel "execution time in ms"
set xrange [0:158.400000]
set style fill pattern 2 bo 1
set style rect fs solid 1 noborder
set border 15 lw 0.2
set xtics out nomirror
unset key
set ytics out nomirror ("dev 0(sysid:0,type:HOSTCPU)" 5,"dev 1(sysid:1,type:HOSTCPU)" 15,"dev 2(sysid:0,type:THSIM)" 25,"dev 3(sysid:1,type:THSIM)" 35,"dev 4(sysid:2,type:THSIM)" 45,"dev 5(sysid:3,type:THSIM)" 55)
set object 1 rect from 4, 65 to 17, 68 fc rgb "#FF0000"
set label "ACCU_TOTAL" at 4,63 font "Helvetica,8'"

set object 2 rect from 21, 65 to 34, 68 fc rgb "#00FF00"
set label "INIT_0" at 21,63 font "Helvetica,8'"

set object 3 rect from 38, 65 to 51, 68 fc rgb "#0000FF"
set label "INIT_0.1" at 38,63 font "Helvetica,8'"

set object 4 rect from 55, 65 to 68, 68 fc rgb "#FFFF00"
set label "INIT_1" at 55,63 font "Helvetica,8'"

set object 5 rect from 72, 65 to 85, 68 fc rgb "#00FFFF"
set label "MODELING" at 72,63 font "Helvetica,8'"

set object 6 rect from 89, 65 to 102, 68 fc rgb "#FF00FF"
set label "ACC_MAPTO" at 89,63 font "Helvetica,8'"

set object 7 rect from 106, 65 to 119, 68 fc rgb "#808080"
set label "KERN" at 106,63 font "Helvetica,8'"

set object 8 rect from 123, 65 to 136, 68 fc rgb "#800000"
set label "PRE_BAR_X" at 123,63 font "Helvetica,8'"

set object 9 rect from 140, 65 to 153, 68 fc rgb "#808000"
set label "DATA_X" at 140,63 font "Helvetica,8'"

set object 10 rect from 157, 65 to 170, 68 fc rgb "#008000"
set label "POST_BAR_X" at 157,63 font "Helvetica,8'"

set object 11 rect from 174, 65 to 187, 68 fc rgb "#800080"
set label "ACC_MAPFROM" at 174,63 font "Helvetica,8'"

set object 12 rect from 191, 65 to 204, 68 fc rgb "#008080"
set label "FINI_1" at 191,63 font "Helvetica,8'"

set object 13 rect from 208, 65 to 221, 68 fc rgb "#000080"
set label "BAR_FINI_2" at 208,63 font "Helvetica,8'"

set object 14 rect from 225, 65 to 238, 68 fc rgb "(null)"
set label "PROF_BAR" at 225,63 font "Helvetica,8'"

set object 15 rect from 0.138988, 0 to 155.439301, 10 fc rgb "#FF0000"
set object 16 rect from 0.091122, 0 to 102.016776, 10 fc rgb "#00FF00"
set object 17 rect from 0.093370, 0 to 102.761696, 10 fc rgb "#0000FF"
set object 18 rect from 0.093784, 0 to 103.580121, 10 fc rgb "#FFFF00"
set object 19 rect from 0.094532, 0 to 104.747420, 10 fc rgb "#FF00FF"
set object 20 rect from 0.095603, 0 to 105.195029, 10 fc rgb "#808080"
set object 21 rect from 0.096005, 0 to 105.787454, 10 fc rgb "#800080"
set object 22 rect from 0.096797, 0 to 106.374395, 10 fc rgb "#008080"
set object 23 rect from 0.097080, 0 to 151.803561, 10 fc rgb "#000080"
set arrow from  0,0 to 158.400000,0 nohead
set object 24 rect from 0.137291, 10 to 153.968108, 20 fc rgb "#FF0000"
set object 25 rect from 0.075436, 10 to 85.045979, 20 fc rgb "#00FF00"
set object 26 rect from 0.077909, 10 to 85.863305, 20 fc rgb "#0000FF"
set object 27 rect from 0.078380, 10 to 86.760721, 20 fc rgb "#FFFF00"
set object 28 rect from 0.079228, 10 to 88.089291, 20 fc rgb "#FF00FF"
set object 29 rect from 0.080445, 10 to 88.629055, 20 fc rgb "#808080"
set object 30 rect from 0.080941, 10 to 89.315830, 20 fc rgb "#800080"
set object 31 rect from 0.081825, 10 to 90.032226, 20 fc rgb "#008080"
set object 32 rect from 0.082184, 10 to 149.847460, 20 fc rgb "#000080"
set arrow from  0,10 to 158.400000,10 nohead
set object 33 rect from 0.133466, 20 to 152.834819, 30 fc rgb "#FF0000"
set object 34 rect from 0.036128, 20 to 44.820284, 30 fc rgb "#00FF00"
set object 35 rect from 0.041498, 20 to 46.752248, 30 fc rgb "#0000FF"
set object 36 rect from 0.042757, 20 to 49.180097, 30 fc rgb "#FFFF00"
set object 37 rect from 0.045002, 20 to 50.901421, 30 fc rgb "#FF00FF"
set object 38 rect from 0.046529, 20 to 51.648535, 30 fc rgb "#808080"
set object 39 rect from 0.047219, 20 to 52.434049, 30 fc rgb "#800080"
set object 40 rect from 0.048549, 20 to 53.785657, 30 fc rgb "#008080"
set object 41 rect from 0.049164, 20 to 145.189239, 30 fc rgb "#000080"
set arrow from  0,20 to 158.400000,20 nohead
set object 42 rect from 0.142100, 30 to 159.078326, 40 fc rgb "#FF0000"
set object 43 rect from 0.119731, 30 to 133.166293, 40 fc rgb "#00FF00"
set object 44 rect from 0.121755, 30 to 133.983619, 40 fc rgb "#0000FF"
set object 45 rect from 0.122241, 30 to 134.922724, 40 fc rgb "#FFFF00"
set object 46 rect from 0.123097, 30 to 136.076856, 40 fc rgb "#FF00FF"
set object 47 rect from 0.124151, 30 to 136.560670, 40 fc rgb "#808080"
set object 48 rect from 0.124597, 30 to 137.141029, 40 fc rgb "#800080"
set object 49 rect from 0.125377, 30 to 137.776240, 40 fc rgb "#008080"
set object 50 rect from 0.125703, 30 to 155.358113, 40 fc rgb "#000080"
set arrow from  0,30 to 158.400000,30 nohead
set object 51 rect from 0.140578, 40 to 157.409662, 50 fc rgb "#FF0000"
set object 52 rect from 0.104888, 40 to 116.982104, 50 fc rgb "#00FF00"
set object 53 rect from 0.106994, 40 to 117.758840, 50 fc rgb "#0000FF"
set object 54 rect from 0.107452, 40 to 118.669418, 50 fc rgb "#FFFF00"
set object 55 rect from 0.108281, 40 to 119.848786, 50 fc rgb "#FF00FF"
set object 56 rect from 0.109372, 40 to 120.312852, 50 fc rgb "#808080"
set object 57 rect from 0.109822, 40 to 120.999626, 50 fc rgb "#800080"
set object 58 rect from 0.110668, 40 to 121.815857, 50 fc rgb "#008080"
set object 59 rect from 0.111153, 40 to 153.617041, 50 fc rgb "#000080"
set arrow from  0,40 to 158.400000,40 nohead
set object 60 rect from 0.135377, 50 to 151.938503, 60 fc rgb "#FF0000"
set object 61 rect from 0.060555, 50 to 68.735625, 60 fc rgb "#00FF00"
set object 62 rect from 0.063018, 50 to 69.625360, 60 fc rgb "#0000FF"
set object 63 rect from 0.063594, 50 to 70.661007, 60 fc rgb "#FFFF00"
set object 64 rect from 0.064527, 50 to 71.867800, 60 fc rgb "#FF00FF"
set object 65 rect from 0.065645, 50 to 72.347227, 60 fc rgb "#808080"
set object 66 rect from 0.066073, 50 to 72.970371, 60 fc rgb "#800080"
set object 67 rect from 0.066893, 50 to 73.673602, 60 fc rgb "#008080"
set object 68 rect from 0.067283, 50 to 147.777260, 60 fc rgb "#000080"
set arrow from  0,50 to 158.400000,50 nohead
plot 0
