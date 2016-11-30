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

set object 15 rect from 0.126507, 0 to 160.819971, 10 fc rgb "#FF0000"
set object 16 rect from 0.105570, 0 to 132.952095, 10 fc rgb "#00FF00"
set object 17 rect from 0.107539, 0 to 133.711271, 10 fc rgb "#0000FF"
set object 18 rect from 0.107909, 0 to 134.551097, 10 fc rgb "#FFFF00"
set object 19 rect from 0.108609, 0 to 135.822620, 10 fc rgb "#FF00FF"
set object 20 rect from 0.109609, 0 to 137.094124, 10 fc rgb "#808080"
set object 21 rect from 0.110636, 0 to 137.767715, 10 fc rgb "#800080"
set object 22 rect from 0.111457, 0 to 138.469846, 10 fc rgb "#008080"
set object 23 rect from 0.111747, 0 to 156.268575, 10 fc rgb "#000080"
set arrow from  0,0 to 158.400000,0 nohead
set object 24 rect from 0.124961, 10 to 159.431827, 20 fc rgb "#FF0000"
set object 25 rect from 0.090319, 10 to 114.024510, 20 fc rgb "#00FF00"
set object 26 rect from 0.092295, 10 to 114.876721, 20 fc rgb "#0000FF"
set object 27 rect from 0.092727, 10 to 115.799655, 20 fc rgb "#FFFF00"
set object 28 rect from 0.093499, 10 to 117.314310, 20 fc rgb "#FF00FF"
set object 29 rect from 0.094718, 10 to 118.697481, 20 fc rgb "#808080"
set object 30 rect from 0.095837, 10 to 119.469061, 20 fc rgb "#800080"
set object 31 rect from 0.096740, 10 to 120.264227, 20 fc rgb "#008080"
set object 32 rect from 0.097072, 10 to 154.208097, 20 fc rgb "#000080"
set arrow from  0,10 to 158.400000,10 nohead
set object 33 rect from 0.123236, 20 to 157.056310, 30 fc rgb "#FF0000"
set object 34 rect from 0.076589, 20 to 96.819979, 30 fc rgb "#00FF00"
set object 35 rect from 0.078439, 20 to 97.550633, 30 fc rgb "#0000FF"
set object 36 rect from 0.078758, 20 to 98.603811, 30 fc rgb "#FFFF00"
set object 37 rect from 0.079607, 20 to 100.102347, 30 fc rgb "#FF00FF"
set object 38 rect from 0.080817, 20 to 101.238653, 30 fc rgb "#808080"
set object 39 rect from 0.081746, 20 to 101.954408, 30 fc rgb "#800080"
set object 40 rect from 0.082570, 20 to 102.707391, 30 fc rgb "#008080"
set object 41 rect from 0.082938, 20 to 152.336184, 30 fc rgb "#000080"
set arrow from  0,20 to 158.400000,20 nohead
set object 42 rect from 0.121801, 30 to 155.211663, 40 fc rgb "#FF0000"
set object 43 rect from 0.061934, 30 to 78.706156, 40 fc rgb "#00FF00"
set object 44 rect from 0.063818, 30 to 79.470304, 40 fc rgb "#0000FF"
set object 45 rect from 0.064181, 30 to 80.436659, 40 fc rgb "#FFFF00"
set object 46 rect from 0.064965, 30 to 81.920296, 40 fc rgb "#FF00FF"
set object 47 rect from 0.066154, 30 to 83.042960, 40 fc rgb "#808080"
set object 48 rect from 0.067077, 30 to 83.746311, 40 fc rgb "#800080"
set object 49 rect from 0.067886, 30 to 84.535285, 40 fc rgb "#008080"
set object 50 rect from 0.068268, 30 to 150.389832, 40 fc rgb "#000080"
set arrow from  0,30 to 158.400000,30 nohead
set object 51 rect from 0.120277, 40 to 153.551901, 50 fc rgb "#FF0000"
set object 52 rect from 0.046209, 40 to 59.653280, 50 fc rgb "#00FF00"
set object 53 rect from 0.048458, 40 to 60.439758, 50 fc rgb "#0000FF"
set object 54 rect from 0.048843, 40 to 61.363930, 50 fc rgb "#FFFF00"
set object 55 rect from 0.049604, 40 to 62.887273, 50 fc rgb "#FF00FF"
set object 56 rect from 0.050844, 40 to 64.186080, 50 fc rgb "#808080"
set object 57 rect from 0.051876, 40 to 64.983723, 50 fc rgb "#800080"
set object 58 rect from 0.052804, 40 to 65.885565, 50 fc rgb "#008080"
set object 59 rect from 0.053248, 40 to 148.396344, 50 fc rgb "#000080"
set arrow from  0,40 to 158.400000,40 nohead
set object 60 rect from 0.118295, 50 to 153.916553, 60 fc rgb "#FF0000"
set object 61 rect from 0.025490, 50 to 34.799815, 60 fc rgb "#00FF00"
set object 62 rect from 0.028546, 50 to 36.236316, 60 fc rgb "#0000FF"
set object 63 rect from 0.029337, 50 to 38.811599, 60 fc rgb "#FFFF00"
set object 64 rect from 0.031449, 50 to 40.821206, 60 fc rgb "#FF00FF"
set object 65 rect from 0.033040, 50 to 42.220497, 60 fc rgb "#808080"
set object 66 rect from 0.034179, 50 to 43.116147, 60 fc rgb "#800080"
set object 67 rect from 0.035318, 50 to 44.764780, 60 fc rgb "#008080"
set object 68 rect from 0.036303, 50 to 145.379438, 60 fc rgb "#000080"
set arrow from  0,50 to 158.400000,50 nohead
plot 0
